#include <ros/ros.h>
#include "moveit/move_group_interface/move_group_interface.h"


int main(int argc, char **argv)
{
  ros::init(argc, argv, "direct_kinematic");
  ros::NodeHandle nh;

  ros::AsyncSpinner spinner(1);
  spinner.start();

  moveit::planning_interface::MoveGroupInterface arm("manipulator");

  arm.setGoalTolerance(0.001);
  arm.setMaxVelocityScalingFactor(0.2);
  arm.setMaxAccelerationScalingFactor(0.2);

  arm.setNamedTarget("home");
  arm.move();
  sleep(1);

  double targetPose[6] = {0.391410, -0.676384, -0.376217, 0.0, 1.052834, 0.454125};

  std::vector<double> joint_group_position(6);
  for (int i=0; i<6; i++) {
    joint_group_position[i] = targetPose[i];
  }

  arm.setJointValueTarget(joint_group_position);
  arm.move();
  sleep(1);

  arm.setNamedTarget("home");
  arm.move();
  sleep(1);

  ros::shutdown();

  return 0;
}
