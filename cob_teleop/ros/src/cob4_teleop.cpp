#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <sensor_msgs/JointState.h>
#include <trajectory_msgs/JointTrajectory.h>
#include <geometry_msgs/Twist.h>
#include <cob_srvs/Trigger.h>
#define noop
int mode=0;
bool deadman_button=0;
#include <sstream>   // std::cout
#include <string>     // std::string, std::to_string




class TeleopCOB4
{
public:
  TeleopCOB4();

private:
  void joyCallback(const sensor_msgs::Joy::ConstPtr& joy);
  
  ros::NodeHandle nh_;

  int linear_, angular_;
  double l_scale_, a_scale_;
  ros::Publisher vel_pub_;
  ros::Publisher joint_pub_;
  ros::Subscriber joy_sub_;
  
};

TeleopCOB4::TeleopCOB4():
  linear_(1),
  angular_(2)
{

  nh_.param("axis_linear", linear_, linear_);
  nh_.param("axis_angular", angular_, angular_);
  nh_.param("scale_angular", a_scale_, a_scale_);
  nh_.param("scale_linear", l_scale_, l_scale_);


  vel_pub_ = nh_.advertise<geometry_msgs::Twist>("cob4/command_velocity", 1);
  joint_pub_ = nh_.advertise<sensor_msgs::JointState>("cob4/joint_state", 1);


  joy_sub_ = nh_.subscribe<sensor_msgs::Joy>("joy", 10, &TeleopCOB4::joyCallback, this);

}

void TeleopCOB4::joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
  //Mode Switch

  if (joy->buttons[0])//gets executed multiple times
    {
      ++mode;
      ROS_WARN("Mode switched to: %d",mode);
      ros::Duration(0.5).sleep();//wait (bad place)
    }
    
  if (mode >= 3)
    {
      mode = 0;
    }


  //deadman_button=joy->buttons[11]; //Segmentation Fault? so:
  if (joy->buttons[11])//gets executed multiple times
    {
      deadman_button=1;
      ROS_WARN("Deadman Activated");
      //ros::Duration(0.5).sleep();//wait (very bad place)
    }

  geometry_msgs::Twist vel;
  sensor_msgs::JointState jnt;

  if (deadman_button)
  {
    switch (mode)
      {
      case 1: //Base
      vel.angular.z = joy->axes[2];
      vel.linear.x = joy->axes[1];
      vel.linear.y = joy->axes[0];
      vel_pub_.publish(vel);
      ROS_WARN("In BaseMode");
      break;
      
      case 2: //Arm Joints Left
      jnt.header.frame_id = ("arm_left");
      jnt.header.stamp = ros::Time::now();
      //jnt.name[0]= ("hallo");
      //jnt.velocity[10] = 0.9;//joy->axes[7];
      joint_pub_.publish(jnt);
      ROS_WARN("In arm left joint Mode");
      break;
      
      case 3: //Arm Joints Right
      jnt.header.frame_id = ("arm_right");
      jnt.header.stamp = ros::Time::now();
      //jnt.name[0]= ("hallo");
      //jnt.velocity[10] = 0.9;//joy->axes[7];
      joint_pub_.publish(jnt);
      ROS_WARN("In arm right joint Mode");
      //break;
      
      
    }
  }
  deadman_button=0;
}



int main(int argc, char** argv)
{
  ros::init(argc, argv, "teleop_cob4");
  TeleopCOB4 teleop_cob4;

  ros::spin();
}
