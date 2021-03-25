#include <ros/ros.h>
#include "moveit/move_group_interface/move_group_interface.h"

void speed_control(moveit::planning_interface::MoveGroupInterface::Plan &plan, double scale){
  int n_joints = plan.trajectory_.joint_trajectory.joint_names.size();
  for(int i = 0; i < plan.trajectory_.joint_trajectory.points.size(); i++){
    plan.trajectory_.joint_trajectory.points[i].time_from_start *= 1/scale;
    for (int j=0; j<n_joints; j++) {
      plan.trajectory_.joint_trajectory.points[i].velocities[j] *= scale;
      plan.trajectory_.joint_trajectory.points[i].accelerations[j] *= scale*scale;
    }
  }
}


int main(int argc, char **argv)
{
  ros::init(argc, argv, "speed_control");
  ros::NodeHandle nh;
  ros::AsyncSpinner spinner(1);
  spinner.start();

  moveit::planning_interface::MoveGroupInterface arm("manipulator");

  arm.setGoalJointTolerance(0.001);
  arm.setMaxVelocityScalingFactor(1);
  arm.setMaxAccelerationScalingFactor(1);

  arm.setNamedTarget("home");
  arm.move();
  sleep(1);

  double targetPose[6] = {0.391410, -0.676384, -0.376217, 0.0, 1.052834, 0.454125};
  std::vector<double> joint_group_positions(6);
  for (int i=0; i<6; i++) {
    joint_group_positions[i] = targetPose[i];
  }

  arm.setJointValueTarget(joint_group_positions);
  arm.move();
  sleep(1);

  arm.setNamedTarget("home");
  arm.move();
  sleep(1);

  arm.setJointValueTarget(joint_group_positions);
  // if you want to control the speed of robot
  // you have to change it between plan and excecute
  moveit::planning_interface::MoveGroupInterface::Plan plan;
  moveit::planning_interface::MoveItErrorCode success = arm.plan(plan);

  ROS_INFO("Plan (pose gola) %s", success?"":"FAILED");

  speed_control(plan, 0.25);

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
