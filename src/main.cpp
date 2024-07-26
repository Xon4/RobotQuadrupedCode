#include <Arduino.h>
#include <Servo.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>

// pin declarations

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

// timer declerations
unsigned long current_time = 0;
unsigned long prev_time = 0;
int millis_delay = 10;

// mechanical constants
const int L1 = 120;
const int L2 = 120;
const int L3 = 20;
const float STEP_LENGTH = 50;
const float STEP_HEIGHT = 35;
const float BACK_STEP_DEPTH = 10;

// declare class instances
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Leg FR_leg(pwm);
Leg FL_leg(pwm);
Leg BR_leg(pwm);
Leg BL_leg(pwm);

Trajectory FR_trajectory(STEP_LENGTH, STEP_HEIGHT, BACK_STEP_DEPTH, 1);
Trajectory FL_trajectory(STEP_LENGTH, STEP_HEIGHT, BACK_STEP_DEPTH, 2);
Trajectory BR_trajectory(STEP_LENGTH, STEP_HEIGHT, BACK_STEP_DEPTH, 1);
Trajectory BL_trajectory(STEP_LENGTH, STEP_HEIGHT, BACK_STEP_DEPTH, 2);

// create a Leg class
class Leg
{
private:
  float abad_angle;
  float hip_angle;
  float knee_angle;
  int abad_servo;
  int hip_servo;
  int knee_servo;
  Adafruit_PWMServoDriver pwm;

public:
  Leg(Adafruit_PWMServoDriver &pwm_init)
  {
    pwm = pwm_init;
    abad_angle = hip_angle = knee_angle = 0;
  }

  void updateAngles(int x, int y, int z)
  {
    float d = sqrt(pow(x, 2) + pow(y, 2));
    float alpha = acos((pow(L1, 2) + pow(L2, 2) - pow(d, 2)) / (2 * L1 * L2));
    float theta = 90 + atan(x / y) - asin(L2 * sin(alpha) / d);
    float phi = alpha - theta;
    float row = asin(L3 / sqrt(pow(z, 2) + pow(y, 2)));
    float omega = row - atan(y / z);
    abad_angle = omega;
    hip_angle = theta;
    knee_angle = phi;
  }

  void updateServos()
  {
    pwm.writeMicroseconds(abad_servo, map(abad_angle, 0, 180, 500, 2500));
    pwm.writeMicroseconds(hip_servo, map(hip_angle, 0, 180, 500, 2500));
    pwm.writeMicroseconds(knee_servo, map(knee_angle, 0, 180, 500, 2500));
  }

  float get_abad_angle()
  {
    return abad_angle;
  }

  float get_hip_angle()
  {
    return hip_angle;
  }

  float get_knee_angle()
  {
    return knee_angle;
  }
};

//create a trajectory class
class Trajectory
{
private:
  float step_length;
  float step_height;
  float back_step_depth;
  float x;
  float y;
  int interpolations = 20;
  bool forward;
  int phase;

public:
  Trajectory(float step_length_init, float step_height_init, float back_step_depth_init, int phase_init)
  {
    phase = phase_init;
    (phase == 1) ? forward = true : forward = false; // legs that are phase 1 will start in forward motion whereas legs that are phase 2 will start in backwards motion
    step_length = step_length_init;
    step_height = step_height_init;
    back_step_depth = back_step_depth_init;
    x = 0;
    y = 0;
  }

  float interpolateNext()
  {
    if (forward)
    {
      x += step_length / interpolations;
      y = step_height * sin(2 * PI / (step_length * 2) * x);
      if (x >= step_length)
      {
        forward = false;
      }
    }
    else
    {
      x -= step_length / interpolations;
      y = -back_step_depth * sin(2 * PI / (step_length * 2) * x);
      if (x <= 0)
      {
        forward = true;
      }
    }

    float get_x()
    {
      return x;
    }

    float get_y()
    {
      return y;
    }
  }
};

void setup()
{

}

void loop()
{
  if (current_time - prev_time >= millis_delay)
  {
    FR_trajectory.updateNext();
    FL_trajectory.updateNext();
    BL_trajectory.updateNext();
    BR_trajectory.updateNext();

    FR_leg.updateAngles(FR_trajectory.get_x(), FR_trajectory.get_y(), 0);
    FL_leg.updateAngles(FR_trajectory.get_x(), FR_trajectory.get_y(), 0);
    BR_leg.updateAngles(FR_trajectory.get_x(), FR_trajectory.get_y(), 0);
    BL_leg.updateAngles(FR_trajectory.get_x(), FR_trajectory.get_y(), 0);

  }
}
