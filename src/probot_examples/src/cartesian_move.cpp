#include <ros/ros.h>
#include "moveit/move_group_interface/move_group_interface.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "cartesian_move");
  ros::NodeHandle nh;
  ros::AsyncSpinner spinner(1);
  spinner.start();

  moveit::planning_interface::MoveGroupInterface arm("manipulator");
  std::string end_effector_link = arm.getEndEffector();

  std::string reference_frame = "base_link";
  arm.setPoseReferenceFrame(reference_frame);

  arm.allowReplanning(true);

  arm.setGoalOrientationTolerance(0.01);
  arm.setGoalPositionTolerance(0.001);

  arm.setMaxAccelerationScalingFactor(0.2);
  arm.setMaxVelocityScalingFactor(0.2);

  arm.setNamedTarget("home");
  arm.move();
  sleep(1);

  geometry_msgs::Pose start_pose = arm.getCurrentPose(end_effector_link).pose;

  std::vector<geometry_msgs::Pose> waypoints;
  waypoints.push_back(start_pose);

  start_pose.position.z -= 0.2;
  waypoints.push_back(start_pose);

  start_pose.position.x += 0.1;
  waypoints.push_back(start_pose);

  start_pose.position.y += 0.1;
  waypoints.push_back(start_pose);

  moveit_msgs::RobotTrajectory trajectory;
  const double jump_threshhold = 0.0;
  const double step = 0.01;
  double fraction = 0.0;
  int max_tries = 100;
  int attempts = 0;

  while(fraction < 1.0 && attempts < max_tries){
    fraction = arm.computeCartesianPath(waypoints, step, jump_threshhold, trajectory);
    attempts++;
  }

  if(fraction == 1){
    ROS_INFO_STREAM("Path computed successfully, moving the arm.");

    moveit::planning_interface::MoveGroupInterface::Plan plan;
    plan.trajectory_ = trajectory;

    arm.execute(plan);
    sleep(1);
  }
  else {
    ROS_INFO_STREAM("Path planning failed with " << max_tries << " tries");
  }

  arm.setNamedTarget("home");
  arm.move();
  sleep(1);

  ros::shutdown();
  return 0;

}
