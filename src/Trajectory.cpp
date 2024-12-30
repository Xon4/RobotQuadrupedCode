#include "Trajectory.h"
#include <Arduino.h>
#include <BasicLinearAlgebra.h>

using namespace BLA;

Trajectory::Trajectory(float step_length_init, float step_height_init, float back_step_depth_init, float side_step_length_init, float side_step_height_init, float side_back_step_depth_init, int leg_init, float ground_depth_init)
{
    leg = leg_init; // 1 -> "FR", 2 -> "FL", 3 -> "BR", 4 -> "BL"
    step_length = step_length_init;
    step_height = step_height_init;
    back_step_depth = back_step_depth_init;
    side_step_length = side_step_length_init;
    side_step_height = side_step_height_init;
    side_back_step_depth = side_back_step_depth_init;
    interpolations = 20;
    speed_factor = 8;
    body_length = 244;
    body_width = 105;
    abad_length = 38.7;
    ground_depth = ground_depth_init;
    
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
        float rate = step_length / (interpolations - ((speed / 100.0) * speed_factor));
        if (swing)
        {
            y += rate;
            if (y >= 1.0 / 2 * step_length)
            {
                y = 1.0 / 2 * step_length;
                swing = false;
            }
            z = step_height * sin(2 * PI / (step_length * 2) * (y + 1.0 / 2 * step_length)); // calculate z value after the conditional so if y exceeds step length, it is set to step length and the z val won't go negative
        }
        else
        {
            y -= rate;
            if (y <= -1.0 / 2 * step_length)
            {
                y = -1.0 / 2 * step_length;
                swing = true;
            }
            z = -back_step_depth * sin(2 * PI / (step_length * 2) * (y + 1.0 / 2 * step_length));
        }
    }

    if (dir == 'B')
    {
        float rate = step_length / (interpolations - ((speed / 100.0) * speed_factor));
        if (swing)
        {
            y -= rate;
            if (y <= -1.0 / 2 * step_length)
            {
                y = -1.0 / 2 * step_length;
                swing = false;
            }
            z = step_height * sin(2 * PI / (step_length * 2) * (y + 1.0 / 2 * step_length)); // calculate z value after the conditional so if y exceeds step length, it is set to step length and the z val won't go negative
        }
        else
        {
            y += rate;
            if (y >= 1.0 / 2 * step_length)
            {
                y = 1.0 / 2 * step_length;
                swing = true;
            }
            z = -back_step_depth * sin(2 * PI / (step_length * 2) * (y + 1.0 / 2 * step_length));
        }
    }

    if (dir == 'R')
    {
        float rate = side_step_length / (interpolations - ((speed / 100.0) * speed_factor));
        if (swing)
        {
            x += rate;
            if (x >= 1.0 / 2 * side_step_length)
            {
                x = 1.0 / 2 * side_step_length;
                swing = false;
            }
            z = side_step_height * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
        }
        else
        {
            x -= rate;
            if (x <= -1.0 / 2 * side_step_length)
            {
                x = -1.0 / 2 * side_step_length;
                swing = true;
            }
            z = -side_back_step_depth * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
        }
    }

    if (dir == 'L')
    {
        float rate = side_step_length / (interpolations - ((speed / 100.0) * speed_factor));
        if (swing)
        {
            x -= rate;
            if (x <= -1.0 / 2 * side_step_length)
            {
                x = -1.0 / 2 * side_step_length;
                swing = false;
            }
            z = side_step_height * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
        }
        else
        {
            x += rate;
            if (x >= 1.0 / 2 * side_step_length)
            {
                x = 1.0 / 2 * side_step_length;
                swing = true;
            }
            z = -side_back_step_depth * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
        }
    }

    if (dir == 'l')
    {
        float rate = side_step_length / (interpolations - ((speed / 100.0) * speed_factor));
        if (leg == 1)
        {
            if (swing)
            {
                x -= rate;
                if (x <= -1.0 / 2 * side_step_length)
                {
                    x = -1.0 / 2 * side_step_length;
                    swing = false;
                }
                z = side_step_height * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
            }
            else
            {
                x += rate;
                if (x >= 1.0 / 2 * side_step_length)
                {
                    x = 1.0 / 2 * side_step_length;
                    swing = true;
                }
                z = -side_back_step_depth * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
            }
        }
        else if (leg == 2)
        {
            if (swing)
            {
                x -= rate;
                if (x <= -1.0 / 2 * side_step_length)
                {
                    x = -1.0 / 2 * side_step_length;
                    swing = false;
                }
                z = side_step_height * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
            }
            else
            {
                x += rate;
                if (x >= 1.0 / 2 * side_step_length)
                {
                    x = 1.0 / 2 * side_step_length;
                    swing = true;
                }
                z = -side_back_step_depth * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
            }
        }
        else if (leg == 3)
        {
            if (swing)
            {
                x += rate;
                if (x >= 1.0 / 2 * side_step_length)
                {
                    x = 1.0 / 2 * side_step_length;
                    swing = false;
                }
                z = side_step_height * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
            }
            else
            {
                x -= rate;
                if (x <= -1.0 / 2 * side_step_length)
                {
                    x = -1.0 / 2 * side_step_length;
                    swing = true;
                }
                z = -side_back_step_depth * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
            }
        }
        else if (leg == 4)
        {
            if (swing)
            {
                x += rate;
                if (x >= 1.0 / 2 * side_step_length)
                {
                    x = 1.0 / 2 * side_step_length;
                    swing = false;
                }
                z = side_step_height * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
            }
            else
            {
                x -= rate;
                if (x <= -1.0 / 2 * side_step_length)
                {
                    x = -1.0 / 2 * side_step_length;
                    swing = true;
                }
                z = -side_back_step_depth * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
            }
        }
    }
    if (dir == 'r')
    {
        float rate = side_step_length / (interpolations - ((speed / 100.0) * speed_factor));
        if (leg == 1)
        {
            if (swing)
            {
                x += rate;
                if (x >= 1.0 / 2 * side_step_length)
                {
                    x = 1.0 / 2 * side_step_length;
                    swing = false;
                }
                z = side_step_height * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
            }
            else
            {
                x -= rate;
                if (x <= -1.0 / 2 * side_step_length)
                {
                    x = -1.0 / 2 * side_step_length;
                    swing = true;
                }
                z = -side_back_step_depth * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
            }
        }
        else if (leg == 2)
        {
            if (swing)
            {
                x += rate;
                if (x >= 1.0 / 2 * side_step_length)
                {
                    x = 1.0 / 2 * side_step_length;
                    swing = false;
                }
                z = side_step_height * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
            }
            else
            {
                x -= rate;
                if (x <= -1.0 / 2 * side_step_length)
                {
                    x = -1.0 / 2 * side_step_length;
                    swing = true;
                }
                z = -side_back_step_depth * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
            }
        }
        else if (leg == 3)
        {
            if (swing)
            {
                x -= rate;
                if (x <= -1.0 / 2 * side_step_length)
                {
                    x = -1.0 / 2 * side_step_length;
                    swing = false;
                }
                z = side_step_height * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
            }
            else
            {
                x += rate;
                if (x >= 1.0 / 2 * side_step_length)
                {
                    x = 1.0 / 2 * side_step_length;
                    swing = true;
                }
                z = -side_back_step_depth * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
            }
        }
        else if (leg == 4)
        {
            if (swing)
            {
                x -= rate;
                if (x <= -1.0 / 2 * side_step_length)
                {
                    x = -1.0 / 2 * side_step_length;
                    swing = false;
                }
                z = side_step_height * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
            }
            else
            {
                x += rate;
                if (x >= 1.0 / 2 * side_step_length)
                {
                    x = 1.0 / 2 * side_step_length;
                    swing = true;
                }
                z = -side_back_step_depth * sin(2 * PI / (side_step_length * 2) * (x + 1.0 / 2 * side_step_length));
            }
        }
    }
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
            y = -1.0 / 2 * step_length;
            z = 0;
        }
        else if (leg == 2 || leg == 3)
        {
            swing = false;
            x = 0;
            y = 1.0 / 2 * step_length;
            z = 0;
        }
    }
    else if (dir == 'B')
    {
        if (leg == 1 || leg == 4)
        {
            swing = true;
            x = 0;
            y = 1.0 / 2 * step_length;
            z = 0;
        }
        else if (leg == 2 || leg == 3)
        {
            swing = false;
            x = 0;
            y = -1.0 / 2 * step_length;
            z = 0;
        }
    }
    else if (dir == 'R')
    {
        if (leg == 1 || leg == 4)
        {
            swing = true;
            x = -1.0 / 2 * side_step_length;
            y = 0;
            z = 0;
        }
        else if (leg == 2 || leg == 3)
        {
            swing = false;
            x = 1.0 / 2 * side_step_length;
            y = 0;
            z = 0;
        }
    }
    else if (dir == 'L')
    {
        if (leg == 1 || leg == 4)
        {
            swing = true;
            x = 1.0 / 2 * side_step_length;
            y = 0;
            z = 0;
        }
        else if (leg == 2 || leg == 3)
        {
            swing = false;
            x = -1.0 / 2 * side_step_length;
            y = 0;
            z = 0;
        }
    }
    else if (dir == 'r')
    {
        if (leg == 1)
        {
            swing = true;
            x = -1.0 / 2 * side_step_length;
            y = 0;
            z = 0;
        }
        else if (leg == 4)
        {
            swing = true;
            x = 1.0 / 2 * side_step_length;
            y = 0;
            z = 0;
        }
        else if (leg == 2)
        {
            swing = false;
            x = 1.0 / 2 * side_step_length;
            y = 0;
            z = 0;
        }
        else if (leg == 3)
        {
            swing = false;
            x = -1.0 / 2 * side_step_length;
            y = 0;
            z = 0;
        }
    }
    else if (dir == 'l')
    {
        if (leg == 2)
        {
            swing = true;
            x = 1.0 / 2 * side_step_length;
            y = 0;
            z = 0;
        }
        if (leg == 3)
        {
            swing = true;
            x = -1.0 / 2 * side_step_length;
            y = 0;
            z = 0;
        }
        else if (leg == 1)
        {
            swing = false;
            x = -1.0 / 2 * side_step_length;
            y = 0;
            z = 0;
        }
        else if (leg == 4)
        {
            swing = false;
            x = 1.0 / 2 * side_step_length;
            y = 0;
            z = 0;
        }
    }
    else if (dir == 'S')
    {
        x = 0;
        y = 0;
        z = 20;
    }
}
void Trajectory::posControl(float pos[3])
{
    x = -pos[0] / 100.0 * 25;
    y = -pos[1] / 100.0 * 25;
    z = 20 - pos[2] / 100.0 * 30;
}

void Trajectory::orientControl(float orient[3])
{
    float psi = -PI/10 * orient[2]/100.0;
    float theta = PI/15 * orient[1]/100.0;
    float phi = PI/15 * orient[0]/100.0;

    int xOffsetDir = 1;
    int yOffsetDir = 1;

    if (leg == 1) {
        xOffsetDir = -1;
        yOffsetDir = -1;
    } else if (leg == 2) {
        xOffsetDir = 1;
        yOffsetDir = -1;
    } else if (leg == 3) {
        xOffsetDir = -1;
        yOffsetDir = 1;
    } else if (leg == 4) {
        xOffsetDir = 1;
        yOffsetDir = 1;
    }

    Matrix<4> d_L0 = {-xOffsetDir*abad_length, 0, -ground_depth + 20, 1};

    Matrix<4, 4> H_L0_B0 = {1, 0, 0, xOffsetDir*body_width/2.0, 0, 1, 0, yOffsetDir*body_length/2.0, 0, 0, 1, 0, 0, 0, 0, 1};
    Matrix<4, 4> H_B3_L1 = {1, 0, 0, -xOffsetDir*body_width/2.0, 0, 1, 0, -yOffsetDir*body_length/2.0, 0, 0, 1, 0, 0, 0, 0, 1};

    Matrix<4, 4> H_B0_B1 = {cos(psi), -sin(psi), 0, 0, sin(psi), cos(psi), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    Matrix<4, 4> H_B1_B2 = {1, 0, 0, 0, 0, cos(theta), -sin(theta), 0, 0, sin(theta), cos(theta), 0, 0, 0, 0, 1};
    Matrix<4, 4> H_B2_B3 = {cos(phi), 0, -sin(phi), 0, 0, 1, 0, 0, sin(phi), 0, cos(phi), 0, 0, 0, 0, 1};

    Matrix<4, 4> H_L0_L1 = H_L0_B0 * H_B0_B1 * H_B1_B2 * H_B2_B3 * H_B3_L1;
    Matrix<4> d_L1 = Inverse(H_L0_L1) * d_L0;
    
    x = d_L1(0);
    y = d_L1(1);
    z = d_L1(2);
}   

char Trajectory::getDir()
{
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