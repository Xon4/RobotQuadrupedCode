#include "Trajectory.h"
#include <Arduino.h>

Trajectory::Trajectory(float step_length_init, float step_height_init, float back_step_depth_init, int phase_init)
{
    phase = phase_init;
    (phase == 1) ? swing = true : swing = false; // legs that are phase 1 will start in swing phase whereas legs that are phase 2 will start in support phase
    step_length = step_length_init;
    step_height = step_height_init;
    back_step_depth = back_step_depth_init;
    x = 0;
    y = 0;
    z = 0;
    interpolations = 20;
}

void Trajectory::interpolateNext(int speed) // speed can be a value from 0 to 100
{
    if (swing)
    {
        y += step_length / (interpolations - ((speed / 100.0) * 10)); // subtract a percent fraction of 20 dependent on the speed from the interpolations, whose default val is 20 (i.e. 40-20 interpolations)

        if (y >= step_length)
        {
            y = step_length;
            swing = false;
        }
        z = step_height * sin(2 * PI / (step_length * 2) * y); // calculate z value after the conditional so if y exceeds step length, it is set to step length and the z val won't go negative
    }
    else
    {
        y -= step_length / (interpolations - ((speed / 100.0) * 10));
        if (y <= 0)
        {
            y = 0;
            swing = true;
        }
        z = -back_step_depth * sin(2 * PI / (step_length * 2) * y);
    }
}

float Trajectory::get_x()
{
    return x;
}

float Trajectory::get_y()
{
    return y;
}

float Trajectory::get_z()
{
    return z;
}