#ifndef TRAJECTORY_H
#define TRAJECTORY_H

class Trajectory
{
private:
  float step_length;
  float step_height;
  float back_step_depth;
  float x;
  float y;
  float z;
  int interpolations;
  bool swing;
  int leg;

public:
  Trajectory(float step_length_init, float step_height_init, float back_step_depth_init, int leg_init);
  void interpolateNext(int speed);
  float get_x();
  float get_y();
  float get_z();
};

#endif