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
const float L1 = 120;
const float L2 = 120;
const float L3 = 37.8;

const float STEP_LENGTH = 50;
const float STEP_HEIGHT = 50;
const float BACK_STEP_DEPTH = 10;
const float SIDE_STEP_LENGTH = 37.8;
const float SIDE_STEP_HEIGHT = 50;
const float SIDE_BACK_STEP_DEPTH = 10;
const float GROUND_DEPTH = 180;

// controller input variables
char new_input_dir = 'S';
char prev_input_dir = 'S';
char dir_state = 'S';
float joystick_magnitude = 0;
bool stop_input = false;
bool stop_motion = false;
bool changed_key = false;

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

  BL_leg.updateAngles(L3, 0, -GROUND_DEPTH);
  FL_leg.updateAngles(L3, 0, -GROUND_DEPTH);

  pwm.writeMicroseconds(BL_abad, map(BL_leg.get_abad_angle(), 0, 180, 800, 2200));
  pwm.writeMicroseconds(BL_hip, map(BL_leg.get_hip_angle(), 0, 180, 800, 2200));
  pwm.writeMicroseconds(BL_knee, map(BL_leg.get_knee_angle(), 0, 180, 800, 2200));

  pwm.writeMicroseconds(FL_abad, map(FL_leg.get_abad_angle(), 0, 180, 800, 2200));
  pwm.writeMicroseconds(FL_hip, map(FL_leg.get_hip_angle(), 0, 180, 800, 2200));
  pwm.writeMicroseconds(FL_knee, map(FL_leg.get_knee_angle(), 0, 180, 800, 2200));

  ps5.begin("7c:66:ef:25:7b:a5"); // replace with MAC address of your controller
}

void loop()
{
  current_time = millis();
  while (ps5.isConnected() == false)
  {
    Serial.println("not connected");
  }

  while (ps5.isConnected() == true)
  {
    prev_input_dir = new_input_dir;

    if (ps5.RStickX() > 30)
    {
      joystick_magnitude = abs(ps5.RStickX());
      new_input_dir = 'r';
    }
    else if (ps5.RStickX() < -30)
    {
      joystick_magnitude = abs(ps5.RStickX());
      new_input_dir = 'l';
    }
    else if (ps5.LStickX() > 30)
    {
      joystick_magnitude = abs(ps5.LStickX());
      new_input_dir = 'R';
    }
    else if (ps5.LStickX() < -30)
    {
      joystick_magnitude = abs(ps5.LStickX());
      new_input_dir = 'L';
    }
    else if (ps5.LStickY() > 30)
    {
      joystick_magnitude = abs(ps5.LStickY());
      new_input_dir = 'F';
    }
    else if (ps5.LStickY() < -30)
    {
      joystick_magnitude = abs(ps5.LStickY());
      new_input_dir = 'B';
    }
    else
    {
      joystick_magnitude = 0;
      new_input_dir = 'S';
    }
    Serial.print(FL_trajectory.getDir());
    Serial.print(", ");
    Serial.println(BL_trajectory.getDir());

    if (prev_input_dir != new_input_dir && prev_input_dir != 'S')
    {
      changed_key = true;
    }

    if (changed_key && BL_trajectory.checkGrounded())
    {
      stop_input = true;
      changed_key = false;
    }

    if (new_input_dir == 'F' || new_input_dir == 'B' || new_input_dir == 'L' || new_input_dir == 'R' || new_input_dir == 'r' || new_input_dir == 'l')
    {
      dir_state = new_input_dir;
    }

    if (BL_trajectory.getDir() != dir_state)
    {
      BL_trajectory.setDir(dir_state);
      FL_trajectory.setDir(dir_state);
    }

    if (stop_input && BL_trajectory.checkGrounded())
    {
      stop_motion = true;
    }

    if (fabs(current_time - prev_time) >= 10)
    {
      if (stop_motion)
      {
        if (BL_trajectory.stop() && FL_trajectory.stop())
        {
          dir_state = 'S';
          stop_input = false;
          stop_motion = false;
        }
        BL_trajectory.stop();
        FL_trajectory.stop();
      }
      else
      {
        BL_trajectory.interpolateNext(map(joystick_magnitude, 30, 127, 0, 100));
        FL_trajectory.interpolateNext(map(joystick_magnitude, 30, 127, 0, 100));
      }

      BL_leg.updateAngles(BL_trajectory.get_x() + L3, BL_trajectory.get_y(), BL_trajectory.get_z() - GROUND_DEPTH);
      FL_leg.updateAngles(FL_trajectory.get_x() + L3, FL_trajectory.get_y(), FL_trajectory.get_z() - GROUND_DEPTH);

      pwm.writeMicroseconds(BL_abad, map(BL_leg.get_abad_angle(), 0, 180, 800, 2200));
      pwm.writeMicroseconds(BL_hip, map(BL_leg.get_hip_angle(), 0, 180, 800, 2200));
      pwm.writeMicroseconds(BL_knee, map(BL_leg.get_knee_angle(), 0, 180, 800, 2200));

      pwm.writeMicroseconds(FL_abad, map(FL_leg.get_abad_angle(), 0, 180, 800, 2200));
      pwm.writeMicroseconds(FL_hip, map(FL_leg.get_hip_angle(), 0, 180, 800, 2200));
      pwm.writeMicroseconds(FL_knee, map(FL_leg.get_knee_angle(), 0, 180, 800, 2200));

      prev_time = current_time;
    }
  }
}
