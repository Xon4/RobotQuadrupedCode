#ifndef TRAJECTORY_H
#define TRAJECTORY_H

class Trajectory
{
private:
  float step_length;
  float step_height;
  float back_step_depth;
  float side_step_length;
  float side_step_height;
  float side_back_step_depth;
  float x;
  float y;
  float z;
  int interpolations;
  bool swing;
  int leg;
  char dir;
  float stop_param;

public:
  Trajectory(float step_length_init, float step_height_init, float back_step_depth_init, float side_step_length_init, float side_step_height_init, float side_back_step_height_init, int leg_init);
  void interpolateNext(int speed);
  bool stop();
  bool checkGrounded();
  void setDir(char dir_val);
  char getDir();
  float get_x();
  float get_y();
  float get_z();
};

#endif