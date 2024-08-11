#include <Arduino.h>
#include <Servo.h>
#include <Adafruit_PWMServoDriver.h>
#include <SPI.h>
#include "Leg.h"
#include "Trajectory.h"

// servo number declarations for the servo driver
int FR_abad = 1;
int FR_hip = 2;
int FR_knee = 3;

int FL_abad = 4;
int FL_hip = 5;
int FL_knee = 6;

int BR_abad = 7;
int BR_hip = 8;
int BR_knee = 9;

int BL_abad = 10;
int BL_hip = 11;
int BL_knee = 12;

//speed var declaration
int speed = 100; 

// timer declerations
unsigned long current_time = 0;
unsigned long prev_time = 0;
int millis_delay = 10;

// mechanical constants
const float STEP_LENGTH = 50;
const float STEP_HEIGHT = 35;
const float BACK_STEP_DEPTH = 10;

// declare class instances
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
const int SERVO_FREQ = 50;

Leg FR_leg(pwm, FR_abad, FR_hip, FR_knee);
Leg FL_leg(pwm, FL_abad, FL_hip, FL_knee);
Leg BR_leg(pwm, BR_abad, BR_hip, BR_knee);
Leg BL_leg(pwm, BL_abad, BL_hip, BL_knee);

Trajectory FR_trajectory(STEP_LENGTH, STEP_HEIGHT, BACK_STEP_DEPTH, 1);
Trajectory FL_trajectory(STEP_LENGTH, STEP_HEIGHT, BACK_STEP_DEPTH, 2);
Trajectory BR_trajectory(STEP_LENGTH, STEP_HEIGHT, BACK_STEP_DEPTH, 1);
Trajectory BL_trajectory(STEP_LENGTH, STEP_HEIGHT, BACK_STEP_DEPTH, 2);

void setup()
{
  pwm.begin();
  pwm.setOscillatorFrequency(25000000);
  pwm.setPWMFreq(SERVO_FREQ);
  Serial.begin(9600);
}

void loop()
{
  current_time = millis();

  if (abs(current_time - prev_time) >= millis_delay)
  {
    FR_trajectory.interpolateNext(speed);
    FL_trajectory.interpolateNext(speed);
    BL_trajectory.interpolateNext(speed);
    BR_trajectory.interpolateNext(speed);

    FR_leg.updateAngles(FR_trajectory.get_x(), FR_trajectory.get_y(), 0);
    FL_leg.updateAngles(FR_trajectory.get_x(), FR_trajectory.get_y(), 0);
    BR_leg.updateAngles(FR_trajectory.get_x(), FR_trajectory.get_y(), 0);
    BL_leg.updateAngles(FR_trajectory.get_x(), FR_trajectory.get_y(), 0);

    FR_leg.updateServos();
    FL_leg.updateServos();
    BR_leg.updateServos();
    BL_leg.updateServos();

    prev_time = current_time;
  }
}
