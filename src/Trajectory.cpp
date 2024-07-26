#include "Trajectory.h"
#include <Arduino.h>

Trajectory::Trajectory(float step_length_init, float step_height_init, float back_step_depth_init, int phase_init)
{
    phase = phase_init;
    (phase == 1) ? forward = true : forward = false; // legs that are phase 1 will start in forward motion whereas legs that are phase 2 will start in backwards motion
    step_length = step_length_init;
    step_height = step_height_init;
    back_step_depth = back_step_depth_init;
    x = 0;
    y = 0;
}

void Trajectory::interpolateNext()
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
}

float Trajectory::get_x()
{
    return x;
}

float Trajectory::get_y()
{
    return y;
}