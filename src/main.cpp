#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include <SPI.h>
#include "Leg.h"
#include "Trajectory.h"
#include <ps5Controller.h>

// servo number declarations for the servo driver
int BL_abad = 5;
int BL_hip = 4;
int BL_knee = 3;

int FL_abad = 2;
int FL_hip = 1;
int FL_knee = 0;

// speed var declaration
int speed = 100;

// timer declerations
unsigned long current_time = 0;
unsigned long prev_time = 0;
int millis_delay = 10;

// mechanical constants
const float STEP_LENGTH = 50;
const float STEP_HEIGHT = 50;
const float BACK_STEP_DEPTH = 10;
const float SIDE_STEP_LENGTH = 10;
const float SIDE_STEP_HEIGHT = 20;
const float SIDE_BACK_STEP_DEPTH = 5;
const float GROUND_DEPTH = 180;
// declare class instances
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
const int SERVO_FREQ = 50;

Leg BL_leg(4);
Trajectory BL_trajectory(STEP_LENGTH, STEP_HEIGHT, BACK_STEP_DEPTH, SIDE_STEP_LENGTH, SIDE_STEP_HEIGHT, SIDE_BACK_STEP_DEPTH, 4);

Leg FL_leg(3);
Trajectory FL_trajectory(STEP_LENGTH, STEP_HEIGHT, BACK_STEP_DEPTH, SIDE_STEP_LENGTH, SIDE_STEP_HEIGHT, SIDE_BACK_STEP_DEPTH, 3);

void setup()
{
  Wire.begin();
  pinMode(2, OUTPUT);
  pwm.begin();
  pwm.setOscillatorFrequency(25000000);
  pwm.setPWMFreq(SERVO_FREQ);
  Serial.begin(9600);

  ps5.begin("7c:66:ef:25:7b:a5"); // replace with MAC address of your controller
}

void loop()
{
  while (ps5.isConnected() == false)
  {
    Serial.println("PS5 controller not found");
    delay(300);
  }

  while (ps5.isConnected() == true)
  {
    if (ps5.Right())
      Serial.println("Right Button");
    if (ps5.Down())
      Serial.println("Down Button");
    if (ps5.Up())
      Serial.println("Up Button");
    if (ps5.Left())
      Serial.println("Left Button");

    if (ps5.Square())
      Serial.println("Square Button");
    if (ps5.Cross())
      Serial.println("Cross Button");
    if (ps5.Circle())
      Serial.println("Circle Button");
    if (ps5.Triangle())
      Serial.println("Triangle Button");

    if (ps5.UpRight())
      Serial.println("Up Right");
    if (ps5.DownRight())
      Serial.println("Down Right");
    if (ps5.UpLeft())
      Serial.println("Up Left");
    if (ps5.DownLeft())
      Serial.println("Down Left");

    if (ps5.L1())
      Serial.println("L1 Button");
    if (ps5.R1())
      Serial.println("R1 Button");

    if (ps5.Share())
      Serial.println("Share Button");
    if (ps5.Options())
      Serial.println("Options Button");
    if (ps5.L3())
      Serial.println("L3 Button");
    if (ps5.R3())
      Serial.println("R3 Button");

    if (ps5.PSButton())
      Serial.println("PS Button");
    if (ps5.Touchpad())
      Serial.println("Touch Pad Button");

    if (ps5.L2())
    {
      Serial.printf("L2 button at %d\n", ps5.L2Value());
    }
    if (ps5.R2())
    {
      Serial.printf("R2 button at %d\n", ps5.R2Value());
    }

    if (ps5.LStickX())
    {
      Serial.printf("Left Stick x at %d\n", ps5.LStickX());
    }
    if (ps5.LStickY())
    {
      Serial.printf("Left Stick y at %d\n", ps5.LStickY());
    }
    if (ps5.RStickX())
    {
      Serial.printf("Right Stick x at %d\n", ps5.RStickX());
    }
    if (ps5.RStickY())
    {
      Serial.printf("Right Stick y at %d\n", ps5.RStickY());
    }

    if (ps5.Charging())
      Serial.println("The controller is charging"); // doesn't work
    if (ps5.Audio())
      Serial.println("The controller has headphones attached"); // doesn't work
    if (ps5.Mic())
      Serial.println("The controller has a mic attached"); // doesn't work

    Serial.printf("Battery Level : %d\n", ps5.Battery()); // doesn't work

    Serial.println();
    // This delay is to make the output more human readable
    // Remove it when you're not trying to see the output
    // pwm.writeMicroseconds(BL_hip, map(90-(90),0,180,850,2150));
    // pwm.writeMicroseconds(BL_knee, map((0),0,180,850,2150));
    current_time = millis();

    if (fabs(current_time - prev_time) >= 10)
    {
      BL_trajectory.interpolateNext(speed);
      FL_trajectory.interpolateNext(speed);

      BL_leg.updateAngles(0, map(abs(ps5.LStickX()), -128, 128, 50, -50), map(abs(ps5.LStickY()), -128, 128, 220, 50) - GROUND_DEPTH);
      FL_leg.updateAngles(0, map(abs(ps5.LStickX()), -128, 128, 50, -50), map(abs(ps5.LStickY()), -128, 128, 220, 50) - GROUND_DEPTH);

      // Serial.print(FL_trajectory.get_y());
      // Serial.print(", ");
      // Serial.println(FL_trajectory.get_z() - GROUND_DEPTH);

      pwm.writeMicroseconds(BL_abad, map(95, 0, 180, 800, 2200));
      pwm.writeMicroseconds(BL_hip, map(BL_leg.get_hip_angle(), 0, 180, 800, 2200));
      pwm.writeMicroseconds(BL_knee, map(BL_leg.get_knee_angle(), 0, 180, 800, 2200));

      pwm.writeMicroseconds(FL_abad, map(75, 0, 180, 800, 2200));
      pwm.writeMicroseconds(FL_hip, map(FL_leg.get_hip_angle(), 0, 180, 800, 2200));
      pwm.writeMicroseconds(FL_knee, map(FL_leg.get_knee_angle(), 0, 180, 800, 2200));

      prev_time = current_time;
    }
  }
}
