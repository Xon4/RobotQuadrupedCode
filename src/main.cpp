#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include <SPI.h>
#include "Leg.h"
#include "Trajectory.h"
#include <ps5Controller.h>
#include <FastLED.h>

// servo number declarations for the servo driver
int FR_abad = 8;
int FR_hip = 7;
int FR_knee = 6;

int FL_abad = 2;
int FL_hip = 1;
int FL_knee = 0;

int BR_abad = 11;
int BR_hip = 10;
int BR_knee = 9;

int BL_abad = 5;
int BL_hip = 4;
int BL_knee = 3;

// RGB LED declarations
#define LED_PIN 2
#define NUM_LEDS 40
CRGBArray<NUM_LEDS> leds;
int RGBval = 0;
int t = 0;

// Speaker declarations
int SPEAKER_PIN = 4;

// speed var declaration
int speed = 100;

// timer declerations
unsigned long current_time = 0;
unsigned long prev_time = 0;
int millis_delay = 10;

// mechanical constants
const float L1 = 120;
const float L2 = 120;
const float L3 = 38.7;

// const float STEP_LENGTH = 50;
// const float STEP_HEIGHT = 50;
// const float BACK_STEP_DEPTH = 10;
// const float SIDE_STEP_LENGTH = 37.8;
// const float SIDE_STEP_HEIGHT = 50;
// const float SIDE_BACK_STEP_DEPTH = 10;
// const float GROUND_DEPTH = 180;

const float STEP_LENGTH = 90;
const float STEP_HEIGHT = 40;
const float BACK_STEP_DEPTH = 5;
const float SIDE_STEP_LENGTH = 50;
const float SIDE_STEP_HEIGHT = 60;
const float SIDE_BACK_STEP_DEPTH = 5;
const float FRONT_GROUND_DEPTH = 200;
const float BACK_GROUND_DEPTH = 200;

// controller input variables
char input_dir = 'S';
float joystick_magnitude = 0;
bool prev_R2_input = false;

// declare class instances
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
const int SERVO_FREQ = 50;

Leg FR_leg(1);
Trajectory FR_trajectory(STEP_LENGTH, STEP_HEIGHT, BACK_STEP_DEPTH, SIDE_STEP_LENGTH, SIDE_STEP_HEIGHT, SIDE_BACK_STEP_DEPTH, 1, FRONT_GROUND_DEPTH);

Leg FL_leg(2);
Trajectory FL_trajectory(STEP_LENGTH, STEP_HEIGHT, BACK_STEP_DEPTH, SIDE_STEP_LENGTH, SIDE_STEP_HEIGHT, SIDE_BACK_STEP_DEPTH, 2, FRONT_GROUND_DEPTH);

Leg BR_leg(3);
Trajectory BR_trajectory(STEP_LENGTH, STEP_HEIGHT, BACK_STEP_DEPTH, SIDE_STEP_LENGTH, SIDE_STEP_HEIGHT, SIDE_BACK_STEP_DEPTH, 3, FRONT_GROUND_DEPTH);

Leg BL_leg(4);
Trajectory BL_trajectory(STEP_LENGTH, STEP_HEIGHT, BACK_STEP_DEPTH, SIDE_STEP_LENGTH, SIDE_STEP_HEIGHT, SIDE_BACK_STEP_DEPTH, 4, FRONT_GROUND_DEPTH);

void setup()
{
  Wire.begin();
  pinMode(2, OUTPUT);
  pwm.begin();
  pwm.setOscillatorFrequency(25000000);
  pwm.setPWMFreq(SERVO_FREQ);
  Serial.begin(9600);

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  leds[0] = CRGB(0, 0, 0);
  leds[1] = CRGB(0, 0, 0);
  FastLED.show();

  pinMode(SPEAKER_PIN, OUTPUT);
  digitalWrite(SPEAKER_PIN, LOW);

  FR_leg.updateAngles(L3, 0, -FRONT_GROUND_DEPTH + 20);
  FL_leg.updateAngles(-L3, 0, -FRONT_GROUND_DEPTH + 20);
  BR_leg.updateAngles(L3, 0, -BACK_GROUND_DEPTH + 20);
  BL_leg.updateAngles(-L3, 0, -BACK_GROUND_DEPTH + 20);

  pwm.writeMicroseconds(FR_abad, map(FR_leg.get_abad_angle(), 0, 180, 1000, 2200));
  pwm.writeMicroseconds(FR_hip, map(FR_leg.get_hip_angle(), 0, 180, 1000, 2200));
  pwm.writeMicroseconds(FR_knee, map(FR_leg.get_knee_angle() - 3, 0, 180, 1000, 2200));

  pwm.writeMicroseconds(FL_abad, map(FL_leg.get_abad_angle() - 4, 0, 180, 1000, 2200));
  pwm.writeMicroseconds(FL_hip, map(FL_leg.get_hip_angle() - 10, 0, 180, 1000, 2200));
  pwm.writeMicroseconds(FL_knee, map(FL_leg.get_knee_angle() - 5, 0, 180, 1000, 2200));

  pwm.writeMicroseconds(BR_abad, map(BR_leg.get_abad_angle() - 5, 0, 180, 1000, 2200));
  pwm.writeMicroseconds(BR_hip, map(BR_leg.get_hip_angle(), 0, 180, 1000, 2200));
  pwm.writeMicroseconds(BR_knee, map(BR_leg.get_knee_angle() - 4, 0, 180, 1000, 2200));

  pwm.writeMicroseconds(BL_abad, map(BL_leg.get_abad_angle(), 0, 180, 1000, 2200));
  pwm.writeMicroseconds(BL_hip, map(BL_leg.get_hip_angle() - 12, 0, 180, 1000, 2200));
  pwm.writeMicroseconds(BL_knee, map(BL_leg.get_knee_angle() + 5, 0, 180, 1000, 2200));

  ps5.begin("7c:66:ef:25:7b:a5"); // replace with MAC address of your controller
}

void loop()
{
  
  current_time = millis();
  while (ps5.isConnected() == false)
  {
    if (fabs(current_time - prev_time) >= 4)
    {
      RGBval = 255 / 2.0 * sin(0.01*t) + 255 / 2.0;
      leds[0] = CRGB(RGBval, RGBval, RGBval);
      leds[1] = CRGB(RGBval, RGBval, RGBval);
      FastLED.show();
      t++;
    }
  }

  if (ps5.RStickX() > 30)
  {
    joystick_magnitude = abs(ps5.RStickX());
    input_dir = 'r';
  }
  else if (ps5.RStickX() < -30)
  {
    joystick_magnitude = abs(ps5.RStickX());
    input_dir = 'l';
  } 
  else if (ps5.LStickY() > 30)
  {
    joystick_magnitude = abs(ps5.LStickY());
    input_dir = 'F';
  }
  else if (ps5.LStickY() < -30)
  {
    joystick_magnitude = abs(ps5.LStickY());
    input_dir = 'B';
  }
  else if (ps5.LStickX() > 30)
  {
    joystick_magnitude = abs(ps5.LStickX());
    input_dir = 'R';
  }
  else if (ps5.LStickX() < -30)
  {
    joystick_magnitude = abs(ps5.LStickX());
    input_dir = 'L';
  }
  else
  {
    joystick_magnitude = 0;
    input_dir = 'S';
  }

  
  // Serial.print(FL_trajectory.getDir());
  // Serial.print(", ");
  // Serial.println(BL_trajectory.getDir());

  if (fabs(current_time - prev_time) >= millis_delay)
  {
    if (ps5.R2()) // orientation control
    {
      FR_trajectory.setDir('N');  //set direction to null since we need to detect a change in direction when going back to gait mode
      FL_trajectory.setDir('N');
      BR_trajectory.setDir('N');
      BL_trajectory.setDir('N');  

      leds[0] = CRGB(5, 255, 22);
      leds[1] = CRGB(5, 255, 22);

      millis_delay = 1;

      float orient[3];
      if (abs(ps5.LStickX()) > 5 || abs(ps5.LStickY()) > 5 || abs(ps5.RStickX()) > 5) // tolerances to account for slight joystick offsets
      {
        orient[0] = float(map(ps5.RStickX(), -127, 127, -100, 100));
        orient[1] = float(map(ps5.LStickY(), -127, 127, -100, 100));
        orient[2] = float(map(ps5.LStickX(), -127, 127, -100, 100));

      } else {
        orient[0] = 0;
        orient[1] = 0;
        orient[2] = 0;
      }
      FR_trajectory.orientControl(orient);
      FL_trajectory.orientControl(orient);
      BR_trajectory.orientControl(orient);
      BL_trajectory.orientControl(orient);

      FR_leg.updateAngles(FR_trajectory.get_x(), FR_trajectory.get_y(), FR_trajectory.get_z());
      FL_leg.updateAngles(-FL_trajectory.get_x(), FL_trajectory.get_y(), FL_trajectory.get_z());
      BR_leg.updateAngles(BR_trajectory.get_x(), BR_trajectory.get_y(), BR_trajectory.get_z());
      BL_leg.updateAngles(-BL_trajectory.get_x(), BL_trajectory.get_y(), BL_trajectory.get_z());
      
    }
    else if (ps5.L2()) // pos control
    {
      FR_trajectory.setDir('N');  //set direction to null since we need to detect a change in direction when going back to gait mode
      FL_trajectory.setDir('N');
      BR_trajectory.setDir('N');
      BL_trajectory.setDir('N');  

      leds[0] = CRGB(245, 7, 43);
      leds[1] = CRGB(245, 7, 43);

      millis_delay = 1;

      float pos[3];
      if (abs(ps5.LStickX()) > 5 || abs(ps5.LStickY()) > 5 || abs(ps5.RStickY()) > 5)
      {
        pos[0] = float(map(ps5.LStickX(), -127, 127, -100, 100));
        pos[1] = float(map(ps5.LStickY(), -127, 127, -100, 100));
        pos[2] = float(map(ps5.RStickY(), -127, 127, -100, 100));
      } else {
        pos[0] = 0;
        pos[1] = 0;
        pos[2] = 0;
      }

      FR_trajectory.posControl(pos);
      FL_trajectory.posControl(pos);
      BR_trajectory.posControl(pos);
      BL_trajectory.posControl(pos);

      FR_leg.updateAngles(FR_trajectory.get_x() + L3, FR_trajectory.get_y(), FR_trajectory.get_z() - FRONT_GROUND_DEPTH);
      FL_leg.updateAngles(FL_trajectory.get_x() - L3, FL_trajectory.get_y(), FL_trajectory.get_z() - FRONT_GROUND_DEPTH);
      BR_leg.updateAngles(BR_trajectory.get_x() + L3, BR_trajectory.get_y(), BR_trajectory.get_z() - FRONT_GROUND_DEPTH);
      BL_leg.updateAngles(BL_trajectory.get_x() - L3, BL_trajectory.get_y(), BL_trajectory.get_z() - FRONT_GROUND_DEPTH);
      
    }
    else if (ps5.Square()) // bark mode
    {
      FR_trajectory.setDir('N');  //set direction to null since we need to detect a change in direction when going back to gait mode
      FL_trajectory.setDir('N');
      BR_trajectory.setDir('N');
      BL_trajectory.setDir('N');  

      leds[0] = CRGB(218, 250, 7);
      leds[1] = CRGB(218, 250, 7);

      digitalWrite(SPEAKER_PIN, HIGH);
      delay(100);
      digitalWrite(SPEAKER_PIN, LOW);
    }
    else // trot gait mode
    {
      leds[0] = CRGB(255, 255, 255);
      leds[1] = CRGB(255, 255, 255);

      if (BL_trajectory.getDir() != input_dir)
      {
        FR_trajectory.setDir(input_dir);
        FL_trajectory.setDir(input_dir);
        BR_trajectory.setDir(input_dir);
        BL_trajectory.setDir(input_dir);
      }
      millis_delay = 10;

      FR_trajectory.interpolateNext(map(joystick_magnitude, 30, 127, 0, 100));
      FL_trajectory.interpolateNext(map(joystick_magnitude, 30, 127, 0, 100));
      BR_trajectory.interpolateNext(map(joystick_magnitude, 30, 127, 0, 100));
      BL_trajectory.interpolateNext(map(joystick_magnitude, 30, 127, 0, 100));

      FR_leg.updateAngles(FR_trajectory.get_x() + L3, FR_trajectory.get_y(), FR_trajectory.get_z() - FRONT_GROUND_DEPTH);
      FL_leg.updateAngles(FL_trajectory.get_x() - L3, FL_trajectory.get_y(), FL_trajectory.get_z() - FRONT_GROUND_DEPTH);
      BR_leg.updateAngles(BR_trajectory.get_x() + L3, BR_trajectory.get_y(), BR_trajectory.get_z() - BACK_GROUND_DEPTH);
      BL_leg.updateAngles(BL_trajectory.get_x() - L3, BL_trajectory.get_y(), BL_trajectory.get_z() - BACK_GROUND_DEPTH);
    }
    FastLED.show();

  pwm.writeMicroseconds(FR_abad, map(FR_leg.get_abad_angle(), 0, 180, 1000, 2200));
  pwm.writeMicroseconds(FR_hip, map(FR_leg.get_hip_angle(), 0, 180, 1000, 2200));
  pwm.writeMicroseconds(FR_knee, map(FR_leg.get_knee_angle() - 3, 0, 180, 1000, 2200));

  pwm.writeMicroseconds(FL_abad, map(FL_leg.get_abad_angle() - 4, 0, 180, 1000, 2200));
  pwm.writeMicroseconds(FL_hip, map(FL_leg.get_hip_angle() - 10, 0, 180, 1000, 2200));
  pwm.writeMicroseconds(FL_knee, map(FL_leg.get_knee_angle() - 5, 0, 180, 1000, 2200));

  pwm.writeMicroseconds(BR_abad, map(BR_leg.get_abad_angle() - 5, 0, 180, 1000, 2200));
  pwm.writeMicroseconds(BR_hip, map(BR_leg.get_hip_angle(), 0, 180, 1000, 2200));
  pwm.writeMicroseconds(BR_knee, map(BR_leg.get_knee_angle() - 4, 0, 180, 1000, 2200));

  pwm.writeMicroseconds(BL_abad, map(BL_leg.get_abad_angle(), 0, 180, 1000, 2200));
  pwm.writeMicroseconds(BL_hip, map(BL_leg.get_hip_angle() - 12, 0, 180, 1000, 2200));
  pwm.writeMicroseconds(BL_knee, map(BL_leg.get_knee_angle() + 5, 0, 180, 1000, 2200));

    prev_time = current_time;
  }
}
