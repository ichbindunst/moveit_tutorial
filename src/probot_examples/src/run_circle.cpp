#include <ros/ros.h>
#include "moveit/move_group_interface/move_group_interface.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "run_circle");
  ros::NodeHandle nh;
  ros::AsyncSpinner spinner(1);
  spinner.start();

  moveit::planning_interface::MoveGroupInterface arm("manipulator");
  std::string end_effector_link = arm.getEndEffector();

  std::string reference_frame = "base_link";
  arm.setPoseReferenceFrame(reference_frame);

  arm.allowReplanning(true);
  arm.setGoalPositionTolerance(0.001);
  arm.setGoalOrientationTolerance(0.01);

  arm.setMaxAccelerationScalingFactor(0.8);
  arm.setMaxVelocityScalingFactor(0.8);

  arm.setNamedTarget("home");
  arm.move();
  sleep(1);

  geometry_msgs::Pose target_pose;
  target_pose.orientation.x = -0.482974;
  target_pose.orientation.y = 0.517043;
  target_pose.orientation.z = -0.504953;
  target_pose.orientation.w = -0.494393;

  target_pose.position.x = 0.331958;
  target_pose.position.y = 0.0;
  target_pose.position.z = 0.307887;

  arm.setPoseTarget(target_pose);
  arm.move();

  std::vector<geometry_msgs::Pose> waypoints;
  waypoints.push_back(target_pose);

  double centerA = target_pose.position.y;
  double centerB = target_pose.position.z;
  double radius = 0.1;

  for(double th=0.0; th<6.28; th=th+0.1){
    target_pose.position.y = centerA + radius * cos(th);
    target_pose.position.z = centerB + radius * sin(th);
    waypoints.push_back(target_pose);
  }

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
    ROS_INFO_STREAM("Path computed successfully. Moving the arm. ");

    moveit::planning_interface::MoveGroupInterface::Plan plan;
    plan.trajectory_ = trajectory;
    arm.execute(plan);
    sleep(1);
  }

  else {
    ROS_INFO_STREAM("Path planning failed with " << max_tries << " times tries");
  }

  arm.setNamedTarget("home");
  arm.move();
  sleep(1);

  ros::shutdown();
  return 0;

}
