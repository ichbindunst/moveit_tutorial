#include <ros/ros.h>
#include "moveit/move_group_interface/move_group_interface.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "inverse_kinematics");
  ros::NodeHandle nh;
  ros::AsyncSpinner spinner(1);
  spinner.start();

  moveit::planning_interface::MoveGroupInterface arm("manipulator");

  std::string end_effector_lin = arm.getEndEffector();
  std::string reference_framme = "base_link";
  arm.setPoseReferenceFrame(reference_framme);

  arm.allowReplanning(true);

  arm.setMaxAccelerationScalingFactor(0.2);
  arm.setMaxAccelerationScalingFactor(0.2);

  arm.setNamedTarget("home");
  arm.move();
  sleep(1);

  geometry_msgs::Pose target_pose;
  target_pose.orientation.x = 0.70692;
  target_pose.orientation.y = 0.0;
  target_pose.orientation.z = 0.0;
  target_pose.orientation.w = 0.70729;

  target_pose.position.x = 0.2593;
  target_pose.position.y = 0.0636;
  target_pose.position.z = 0.1787;

  arm.setStartStateToCurrentState();
  arm.setPoseTarget(target_pose);

  moveit::planning_interface::MoveGroupInterface::Plan plan;
  moveit::planning_interface::MoveItErrorCode success = arm.plan(plan);

  ROS_INFO("Plan (pose goal) %s ", success?"":"FAILED");

  if(success){
    arm.execute(plan);
  }

  sleep(1);

  arm.setNamedTarget("home");
  arm.move();
  sleep(1);

  ros::shutdown();

  return 0;
}
