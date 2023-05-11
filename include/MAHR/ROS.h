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

  // Subscribers:
void Motors(const geometry_msgs::Twist &cmd_msg) {
  Target_RightMotor_mms = (int16_t) 300*( cmd_msg.linear.x + (cmd_msg.angular.z)/2.0 );
  Target_LeftMotor_mms  = (int16_t) 300*( cmd_msg.linear.x - (cmd_msg.angular.z)/2.0 );
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
// Send the sensory data to/receive control action from ROS
void ROS_DataUpdate() {
  // IMU:
  q.x = quaternion[0];
  q.y = quaternion[1];
  q.z = quaternion[2];
  q.w = quaternion[3];
  quat_pub.publish(&q);
  
  // Encoders:
  RightEnc.data = RightEncoder_Distance;    RightEncoder_pub.publish(&RightEnc);
  LeftEnc.data  = LeftEncoder_Distance;     LeftEncoder_pub.publish(&LeftEnc);    

  nh.spinOnce();
  delay(1);
}

#endif