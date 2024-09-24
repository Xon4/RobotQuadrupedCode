#include "Leg.h"

Leg::Leg(int type_init)
{
    type = type_init;
    abad_angle = hip_angle = knee_angle = 0;
}

void Leg::updateAngles(float x, float y, float z)
{
    if (type == 1 || type == 3)
    {
        x = -x;
    }
    float l = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    float d = sqrt(pow(l, 2) - pow(L3, 2));
    float alpha = acos((pow(L1, 2) + pow(d, 2) - pow(L2, 2)) / (2 * L1 * d));
    float beta = asin(-y / d);
    float theta = PI / 2 - alpha - beta;
    float row = acos((pow(L1, 2) + pow(L2, 2) - pow(d, 2)) / (2 * L1 * L2));
    float phi = row - theta;
    float sigma = acos(L3 / (sqrt(pow(x, 2) + pow(z, 2))));
    float omega;
    if (x != 0)
    {
        omega = sigma - atan(-z / fabs(x));
    }
    else
    {
        omega = sigma - PI / 2;
    }

    omega *= 180.0 / PI;
    theta *= 180.0 / PI;
    phi *= 180.0 / PI;

    // check bounds and make sure angles are in range
    if (hip_angle > 90)
    {
        hip_angle = 90;
    }
    else if (hip_angle < 0)
    {
        hip_angle = 0;
    }
    else if (knee_angle > 90)
    {
        knee_angle = 90;
    }
    else if (knee_angle < 0)
    {
        knee_angle = 0;
    }
    else if (abad_angle > 15)
    {
        abad_angle = 15;
    }
    else if (abad_angle < -15)
    {
        abad_angle = -15;
    }

    // transform the absolute coordinates to servo coordinates
    if (type == 1) // FR
    {
        abad_angle = 90 - omega;
        hip_angle = 90 + theta;
        knee_angle = 180 - phi;
    }
    else if (type == 2) // FL
    {
        abad_angle = 90 + omega;
        hip_angle = 90 - theta;
        knee_angle = phi;
    }
    else if (type == 3) // BR
    {
        abad_angle = 90 + omega;
        hip_angle = 90 + theta;
        knee_angle = 180 - phi;
    }
    else if (type == 4) // BL
    {
        abad_angle = 90 - omega;
        hip_angle = 90 - theta;
        knee_angle = phi;
    }
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