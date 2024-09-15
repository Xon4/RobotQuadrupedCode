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
  current_time = millis();

  if (fabs(current_time - prev_time) >= 10)
  {
    BL_trajectory.interpolateNext(speed);
    FL_trajectory.interpolateNext(speed);

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