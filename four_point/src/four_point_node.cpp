#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/tf.h>
#include <vector>
 
int main(int argc, char** argv)
{
  ros::init(argc, argv, "move_test");
 

  actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> cycle_client("move_base", true);
 

  while(!cycle_client.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for move_base action server...");
  }
 
  ROS_INFO("Connected to move base server");


  double pi = 3.1415;
  std::vector<double> euler_angles;
  euler_angles.push_back(pi / 2.0);
  euler_angles.push_back(pi);
  euler_angles.push_back(3.0 * pi / 2.0);
  euler_angles.push_back(0);


  std::vector<geometry_msgs::Quaternion> quaternions;


  for(int i = 0; i < euler_angles.size(); i++){
    quaternions.push_back( tf::createQuaternionMsgFromRollPitchYaw(0, 0, euler_angles[i]) );
  }

  //position
  std::vector<geometry_msgs::Point> pose;
  geometry_msgs::Point pose0, pose1, pose2, pose3;

  pose0.x = -5.1; pose0.y = -4.0; pose0.z = 0.0; 
  pose1.x = 4.0; pose1.y = -3.9; pose1.z = 0.0; 
  pose2.x = 3.0; pose2.y = -0.1; pose2.z = 0.0; 
  pose3.x = 0.0; pose3.y = 0.0; pose3.z = 0.0; 

  pose.push_back(pose0);
  pose.push_back(pose1);
  pose.push_back(pose2);
  pose.push_back(pose3);

  int count = 0;
  while(count < 4){
    
    
    move_base_msgs::MoveBaseGoal goal;
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.header.stamp = ros::Time::now();
 
    goal.target_pose.pose.position = pose[count];
    goal.target_pose.pose.orientation = quaternions[count];
    
    ROS_INFO("Sending goal");

    cycle_client.sendGoal(goal);
    
    //60S内如果不能达到目标点则放弃该目标点
		bool finished_within_time = cycle_client.waitForResult(ros::Duration(60));

		if(!finished_within_time){
			cycle_client.cancelGoal();
			ROS_INFO("Timed out achieving goal");
		}
		else{
			//导航成功
			if(cycle_client.getState() == actionlib::SimpleClientGoalState::SUCCEEDED){
				ROS_INFO("Goal succeeded!");
			}
			//导航失败
			else{
				ROS_INFO("move plan is error");
      }
    }
    count = count + 1;

  }

  
				
  return 0;

}

