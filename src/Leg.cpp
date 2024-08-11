#include "Leg.h"

Leg::Leg(Adafruit_PWMServoDriver &pwm_init, int abad_servo_init, int hip_servo_init, int knee_servo_init)
{
    pwm = pwm_init;
    abad_angle = hip_angle = knee_angle = 0;
    abad_servo = abad_servo_init;
    hip_servo = hip_servo_init;
    knee_servo = knee_servo_init;
}

void Leg::updateAngles(int x, int y, int z)
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

void Leg::updateServos()
{
    pwm.writeMicroseconds(abad_servo, map(abad_angle, 0, 180, 500, 2500));
    pwm.writeMicroseconds(hip_servo, map(hip_angle, 0, 180, 500, 2500));
    pwm.writeMicroseconds(knee_servo, map(knee_angle, 0, 180, 500, 2500));
}

float Leg::get_abad_angle()
{
    return abad_angle;
}

float Leg::get_hip_angle()
{
    return hip_angle;
}

float Leg::get_knee_angle()
{
    return knee_angle;
}