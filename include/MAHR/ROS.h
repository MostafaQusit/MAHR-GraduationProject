#ifndef MAHR_ROS_H_
#define MAHR_ROS_H_

#include <MAHR.h>
#include <ros.h>
#include <ros/time.h>
#include <std_msgs/Int16.h>
#include <geometry_msgs/Vector3.h>
#include <geometry_msgs/Quaternion.h>
#include <geometry_msgs/Twist.h>

ros::NodeHandle nh;

  // Publishers:
geometry_msgs::Quaternion q;
std_msgs::Int16 RightEnc, LeftEnc;

ros::Publisher quat_pub("quaternion", &q);
ros::Publisher RightEncoder_pub("/right_ticks", &RightEnc);
ros::Publisher LeftEncoder_pub("/left_ticks", &LeftEnc);
double lastCmdVelReceived = 0;

  // Subscribers:
void Motors(const geometry_msgs::Twist &cmd_msg) {
  // Record timestamp of last velocity command received
  lastCmdVelReceived = (millis() / 1000);

  Required_RightMotor_mms = 300 * cmd_msg.linear.x + 50;
  Required_LeftMotor_mms  = 300 * cmd_msg.linear.x + 50;

  // Check if we need to turn 
  if (cmd_msg.angular.z != 0.0) {
    Required_RightMotor_mms =  200 * cmd_msg.angular.z;
    Required_LeftMotor_mms  = -200 * cmd_msg.angular.z;
  }
}
ros::Subscriber<geometry_msgs::Twist> Motors_sub("cmd_vel", Motors);

// ROS Initialization
void ROS_Setup(int32_t BaudRate) {
  nh.getHardware()->setBaud(BaudRate);
  nh.initNode();

  nh.advertise(quat_pub);
  nh.advertise(RightEncoder_pub);
  nh.advertise(LeftEncoder_pub);

  nh.subscribe(Motors_sub);
}
// Send the sensory data to ROS
void ROS_SendData() {
  delay(30);
  nh.spinOnce();

  // IMU:
  q.x = quaternion[0];
  q.y = quaternion[1];
  q.z = quaternion[2];
  q.w = quaternion[3];
  quat_pub.publish(&q);
  
  // Encoders:
  RightEnc.data = RightEncoder_Distance;    RightEncoder_pub.publish(&RightEnc);
  LeftEnc.data  = LeftEncoder_Distance;     LeftEncoder_pub.publish(&LeftEnc);    
}
// receive control action from ROS
void ROS_ReceiveData(){
  // Stop the car if there are no cmd_vel messages
  if((millis()/1000) - lastCmdVelReceived > 1) {
    Required_LeftMotor_mms  = 0;
    Required_RightMotor_mms = 0;
  }
}

#endif