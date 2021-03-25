// if multiple target poses or points are in one trajectorie, wenn the robot moves,
// the move between two poses will not be smooth, in other word, there will be a short pause in it
// this programm is the answer for solving this.

#include <ros/ros.h>
#include "moveit/move_group_interface/move_group_interface.h"
#include "moveit/robot_trajectory/robot_trajectory.h"
#include "moveit/trajectory_processing/iterative_time_parameterization.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "smooth_move");
  ros::NodeHandle nh;
  ros::AsyncSpinner spinner(1);
  spinner.start();

  moveit::planning_interface::MoveGroupInterface arm("manipulator");
  arm.setGoalJointTolerance(0.001);

  double accScale = 0.8;
  double velScale = 0.8;
  arm.setMaxVelocityScalingFactor(velScale);
  arm.setMaxAccelerationScalingFactor(accScale);

  arm.setNamedTarget("home");
  arm.move();
  sleep(1);

  // get the start position of robot
  moveit::core::RobotStatePtr start_state(arm.getCurrentState());
  const robot_state::JointModelGroup *joint_model_group = start_state->getJointModelGroup(arm.getName());

  // copy the start position to joint_group_positions
  std::vector<double> joint_group_positions;
  start_state->copyJointGroupPositions(joint_model_group, joint_group_positions);

  // set the first target position
  joint_group_positions[0] = -0.6;
  arm.setJointValueTarget(joint_group_positions);

  // compute the first trajectorie
  moveit::planning_interface::MoveGroupInterface::Plan plan1;
  moveit::planning_interface::MoveItErrorCode success = arm.plan(plan1);

  joint_model_group = start_state->getJointModelGroup(arm.getName());
  start_state->setJointGroupPositions(joint_model_group, joint_group_positions);
  arm.setStartState(*start_state);

  // set the second target position
  joint_group_positions[0] = -1.2;
  joint_group_positions[1] = -0.5;
  arm.setJointValueTarget(joint_group_positions);

  // compute the second trajectorie
  moveit::planning_interface::MoveGroupInterface::Plan plan2;
  success = arm.plan(plan2);

  // combine two trajectories
  // main ideal: get all the target positions in two trajectories into one
  moveit_msgs::RobotTrajectory trajectory;

  trajectory.joint_trajectory.joint_names = plan1.trajectory_.joint_trajectory.joint_names;
  trajectory.joint_trajectory.points = plan1.trajectory_.joint_trajectory.points;
  // add the target positions into this->trajectory
  for(size_t j=1; j<plan2.trajectory_.joint_trajectory.points.size();j++){
    trajectory.joint_trajectory.points.push_back(plan2.trajectory_.joint_trajectory.points[j]);
  }

  moveit::planning_interface::MoveGroupInterface::Plan combined_plan;
  robot_trajectory::RobotTrajectory rt(arm.getCurrentState()->getRobotModel(), "manipulator");
  rt.setRobotTrajectoryMsg(*arm.getCurrentState(), trajectory);
  trajectory_processing::IterativeParabolicTimeParameterization iptp;
  iptp.computeTimeStamps(rt, velScale, accScale);

  rt.getRobotTrajectoryMsg(trajectory);
  combined_plan.trajectory_ = trajectory;

  if(!arm.execute(combined_plan)){
    ROS_ERROR("Failed to execute plan");
    return false;
  }
  sleep(1);

  arm.setNamedTarget("home");
  arm.move();

  ros::shutdown();
  return 0;

}
