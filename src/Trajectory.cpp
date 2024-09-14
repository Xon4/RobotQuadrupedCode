#include "Trajectory.h"
#include <Arduino.h>

Trajectory::Trajectory(float step_length_init, float step_height_init, float back_step_depth_init, int leg_init)
{
    leg = leg_init; // 1 -> "FR", 2 -> "FL", 3 -> "BR", 4 -> "BL"
    step_length = step_length_init;
    step_height = step_height_init;
    back_step_depth = back_step_depth_init;
    interpolations = 40;
    dir = "forward";
    if (leg == 4 || leg == 1)
    {
        x = 0;
        y = 0;
        z = 0;
        swing = true; // legs that are phase 1 will start in swing phase whereas legs that are phase 2 will start in support phase
    }
    else if (leg == 2 || leg == 3)
    {
        x = 0;
        y = step_length;
        z = 0;
        swing = false;
    }
}

void Trajectory::interpolateNext(int speed) // speed can be a value from 0 to 100
{
    if (dir == "forward")
    {
        if (swing)
        {
            y += step_length / (interpolations - ((speed / 100.0) * 10)); // subtract a percent fraction of 40 dependent on the speed from the interpolations, whose default val is 30 (i.e. 40-10 interpolations)

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