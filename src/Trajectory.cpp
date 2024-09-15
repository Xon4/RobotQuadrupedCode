#include "Trajectory.h"
#include <Arduino.h>

Trajectory::Trajectory(float step_length_init, float step_height_init, float back_step_depth_init, float side_step_length_init, float side_step_height_init, float side_back_step_depth_init, int leg_init)
{
    leg = leg_init; // 1 -> "FR", 2 -> "FL", 3 -> "BR", 4 -> "BL"
    step_length = step_length_init;
    step_height = step_height_init;
    back_step_depth = back_step_depth_init;
    side_step_length = side_step_length_init;
    side_step_height = side_step_height_init;
    side_back_step_depth = side_back_step_depth_init;
    interpolations = 40;
    stop_param = 0;
    dir = 'F';
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
    // if (leg == 4 || leg == 1)
    // {
    //     x = 0;
    //     y = 0;
    //     z = 0;
    //     swing = true; // legs that are phase 1 will start in swing phase whereas legs that are phase 2 will start in support phase
    // }
    // else if (leg == 2 || leg == 3)
    // {
    //     x = -side_step_length;
    //     y = 0;
    //     z = 0;
    //     swing = false;
    // }
}

void Trajectory::interpolateNext(int speed) // speed can be a value from 0 to 100
{
    if (dir == 'F')
    {
        float rate = step_length / (interpolations - ((speed / 100.0) * 10));
        if (swing)
        {
            y += rate;
            if (y >= step_length)
            {
                y = step_length;
                swing = false;
            }
            z = step_height * sin(2 * PI / (step_length * 2) * y); // calculate z value after the conditional so if y exceeds step length, it is set to step length and the z val won't go negative
        }
        else
        {
            y -= rate;
            if (y <= 0)
            {
                y = 0;
                swing = true;
            }
            z = -back_step_depth * sin(2 * PI / (step_length * 2) * y);
        }
    }

    if (dir == 'B')
    {
        float rate = step_length / (interpolations - ((speed / 100.0) * 10));
        if (swing)
        {
            y -= rate;
            if (y <= 0)
            {
                y = 0;
                swing = false;
            }
            z = step_height * sin(2 * PI / (step_length * 2) * y); // calculate z value after the conditional so if y exceeds step length, it is set to step length and the z val won't go negative
        }
        else
        {
            y += rate;
            if (y >= step_length)
            {
                y = step_length;
                swing = true;
            }
            z = -back_step_depth * sin(2 * PI / (step_length * 2) * y);
        }
    }

    if (dir == 'R')
    {
        float rate = side_step_length / (interpolations - ((speed / 100.0) * 10));
        if (swing)
        {
            x += rate;
            if (x >= side_step_length)
            {
                x = side_step_length;
                swing = false;
            }
            z = side_step_height * sin(2 * PI / (side_step_length * 2) * x);
        }
        else
        {
            x -= rate;
            if (x <= 0)
            {
                x = 0;
                swing = true;
            }
            z = -side_back_step_depth * sin(2 * PI / (side_step_length * 2) * x);
        }
    }

    if (dir == 'L')
    {
        float rate = side_step_length / (interpolations - ((speed / 100.0) * 10));
        if (swing)
        {
            x -= rate;
            if (x <= -side_step_length)
            {
                x = -side_step_length;
                swing = false;
            }
            z = side_step_height * sin(2 * PI / (side_step_length * 2) * (x + side_step_length));
        }
        else
        {
            x += rate;
            if (x >= 0)
            {
                x = 0;
                swing = true;
            }
            z = -side_back_step_depth * sin(2 * PI / (side_step_length * 2) * (x + side_step_length));
        }
    }

    if (dir == 'l')
    {
        float rate = side_step_length / (interpolations - ((speed / 100.0) * 10));
        if (leg == 1)
        {
            if (swing)
            {
                x -= rate;
                if (x <= 0)
                {
                    x = 0;
                    swing = false;
                }
                z = side_step_height * sin(2 * PI / (side_step_length * 2) * x);
            }
            else
            {
                x += rate;
                if (x >= side_step_length)
                {
                    x = side_step_length;
                    swing = true;
                }
                z = -side_back_step_depth * sin(2 * PI / (side_step_length * 2) * x);
            }
        }
        else if (leg == 2)
        {
            if (swing)
            {
                x -= rate;
                if (x <= -side_step_length)
                {
                    x = -side_step_length;
                    swing = false;
                }
                z = side_step_height * sin(2 * PI / (side_step_length * 2) * (x + side_step_length));
            }
            else
            {
                x += rate;
                if (x >= 0)
                {
                    x = 0;
                    swing = true;
                }
                z = -side_back_step_depth * sin(2 * PI / (side_step_length * 2) * (x + side_step_length));
            }
        }
        else if (leg == 3)
        {
            if (swing)
            {
                x += rate;
                if (x >= side_step_length)
                {
                    x = side_step_length;
                    swing = false;
                }
                z = side_step_height * sin(2 * PI / (side_step_length * 2) * x);
            }
            else
            {
                x -= rate;
                if (x <= 0)
                {
                    x = 0;
                    swing = true;
                }
                z = -side_back_step_depth * sin(2 * PI / (side_step_length * 2) * x);
            }
        }
        else if (leg == 4)
        {
            if (swing)
            {
                x += rate;
                if (x >= 0)
                {
                    x = 0;
                    swing = false;
                }
                z = side_step_height * sin(2 * PI / (side_step_length * 2) * (x + side_step_length));
            }
            else
            {
                x -= rate;
                if (x <= -side_step_length)
                {
                    x = -side_step_length;
                    swing = true;
                }
                z = -side_back_step_depth * sin(2 * PI / (side_step_length * 2) * (x + side_step_length));
            }
        }
        stop_param = 0;
    }
}

bool Trajectory::stop()
{
    if (x == 0)
    {
        float rate = step_length / 5;
        if (!swing)
        {
            if (y > 0)
            {
                y -= rate;
                z = -back_step_depth * sin(2 * PI / (step_length * 2) * y);
            }
            else if (y < 0)
            {
                y += rate;
                z = -back_step_depth * sin(2 * PI / (step_length * 2) * (y + step_length));
            }
        }
        else
        {
            if (stop_param < step_length)
            {
                stop_param += rate;
                z = step_height * sin(2 * PI / (step_length * 2) * stop_param);
            }
        }
    }
    else if (y == 0)
    {
        float rate = side_step_length / 5;
        if (!swing)
        {
            if (x > 0)
            {
                x -= rate;
                z = -side_back_step_depth * sin(2 * PI / (side_step_length * 2) * x);
            }
            else if (x < 0)
            {
                x += rate;
                z = -side_back_step_depth * sin(2 * PI / (side_step_length * 2) * (x + side_step_length));
            }
        }
        else
        {
            if (stop_param < side_step_length)
            {
                stop_param += rate;
                z = step_height * sin(2 * PI / (side_step_length * 2) * stop_param);
            }
        }
    }

    if (x == 0 && y == 0 && z == 0)
    {
        return true;
    }
    return false;
}

bool Trajectory::checkGrounded()
{
    return (x == 0 && y == 0 && z == 0);
}

void Trajectory::setDir(char dir_val)
{
    dir = dir_val;
    if (dir == 'F')
    {
        if (leg == 1 || leg == 4)
        {
            swing = true;
            x = 0;
            y = 0;
            z = 0;
        }
        else if (leg == 2 || leg == 3)
        {
            swing = false;
            x = 0;
            y = step_length;
            z = 0;
        }
    }
    else if (dir == 'B')
    {
        if (leg == 1 || leg == 4)
        {
            swing = true;
            x = 0;
            y = 0;
            z = 0;
        }
        else if (leg == 2 || leg == 3)
        {
            swing = false;
            x = 0;
            y = -step_length;
            z = 0;
        }
    }
    else if (dir == 'R')
    {
        if (leg == 1 || leg == 4)
        {
            swing = true;
            x = 0;
            y = 0;
            z = 0;
        }
        else if (leg == 2 || leg == 3)
        {
            swing = false;
            x = side_step_length;
            y = 0;
            z = 0;
        }
    }
    else if (dir == 'L')
    {
        if (leg == 1 || leg == 4)
        {
            swing = true;
            x = 0;
            y = 0;
            z = 0;
        }
        else if (leg == 2 || leg == 3)
        {
            swing = false;
            x = -side_step_length;
            y = 0;
            z = 0;
        }
    }
    else if (dir == 'r')
    {
        if (leg == 1 || leg == 4)
        {
            swing = true;
            x = 0;
            y = 0;
            z = 0;
        }
        else if (leg == 2 || leg == 3)
        {
            swing = false;
            x = 0;
            y = 0;
            z = 0;
        }
    }
    else if (dir == 'l')
    {
        if (leg == 2 || leg == 3)
        {
            swing = true;
            x = 0;
            y = 0;
            z = 0;
        }
        else if (leg == 1 || leg == 4)
        {
            swing = false;
            x = 0;
            y = 0;
            z = 0;
        }
    }
    else if (dir == 'S')
    {
        x = 0;
        y = 0;
        z = 0;
    }
}

char Trajectory::getDir() {
    return dir;
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