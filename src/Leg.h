#ifndef LEG_H
#define LEG_H
#include <Adafruit_PWMServoDriver.h>

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
  const int L1 = 120;
  const int L2 = 120;
  const int L3 = 20;

public:
  Leg(Adafruit_PWMServoDriver &pwm_init, int abad_servo_init, int hip_servo_init, int knee_servo_init);
  void updateAngles(int x, int y, int z);
  void updateServos();
  float get_abad_angle();
  float get_hip_angle();
  float get_knee_angle();
};

#endif
