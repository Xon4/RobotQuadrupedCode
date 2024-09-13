#ifndef LEG_H
#define LEG_H
#include <Adafruit_PWMServoDriver.h>

class Leg
{
private:
  float abad_angle;
  float hip_angle;
  float knee_angle;
  int type;
  const int L1 = 120;
  const int L2 = 120;
  const int L3 = 20;

public:
  Leg(int type_init); // type: 1 -> FR, 2 -> FL, 3 -> BR, 4 -> BL
  void updateAngles(float x, float y, float z);
  float get_abad_angle();
  float get_hip_angle();
  float get_knee_angle();
};

#endif
