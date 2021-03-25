#include <ros/ros.h>
#include "moveit/move_group_interface/move_group_interface.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "random");
  ros::NodeHandle nh;
  ros::AsyncSpinner spinner(1);
  spinner.start();

  moveit::planning_interface::MoveGroupInterface arm("manipulator");

  arm.setGoalTolerance(0.001);
  arm.setMaxVelocityScalingFactor(0.2);
  arm.setMaxVelocityScalingFactor(0.2);

  arm.setNamedTarget("home");
  arm.move();
  sleep(1);

  arm.setRandomTarget();

  arm.move();
  sleep(1);

  arm.setNamedTarget("home");
  arm.move();
  sleep(1);

  ros::shutdown();

  return 0;

}
