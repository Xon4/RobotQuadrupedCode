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
}

void Trajectory::interpolateNext(int speed) // speed can be a value from 0 to 100
{
    if (swing)
    {
        x += step_length / (interpolations - ((speed / 100.0) * 20)); // subtract a percent fraction of 20 dependent on the speed from the interpolations, whose default val is 20 (i.e. 40-20 interpolations)

        if (x >= step_length)
        {
            x = step_length;
            swing = false;
        }
        y = step_height * sin(2 * PI / (step_length * 2) * x); // calculate y value after the conditional so if x exceeds step length, it is set to step length and the y val won't go negative
    }
    else
    {
        x -= step_length / (interpolations - ((speed / 100.0) * 20));
        if (x <= 0)
        {
            x = 0;
            swing = true;
        }
        y = -back_step_depth * sin(2 * PI / (step_length * 2) * x);
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