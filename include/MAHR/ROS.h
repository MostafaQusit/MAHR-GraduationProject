#ifndef MAHR_ROS_H_
#define MAHR_ROS_H_

#include <MAHR.h>
#include <ros.h>
#include <ros/time.h>
#include <std_msgs/Int64.h>
#include <geometry_msgs/Vector3.h>
#include <geometry_msgs/Quaternion.h>
#include <sensor_msgs/Imu.h>
#include <geometry_msgs/Twist.h>

ros::NodeHandle nh;

  // Publishers:
geometry_msgs::Vector3 target;
geometry_msgs::Quaternion q;
sensor_msgs::Imu Imu;
std_msgs::Int64 RightEnc, LeftEnc;

ros::Publisher quat_pub("quaternion", &q);
ros::Publisher imu_pub("imu", &Imu);
ros::Publisher target_pub("/TargetPosition_cm", &target);
ros::Publisher RightEncoder_pub("/right_ticks", &RightEnc);
ros::Publisher LeftEncoder_pub("/left_ticks", &LeftEnc);

  // Subscribers:
void Motors(const geometry_msgs::Twist &cmd_msg) {
  Target_RightMotor_mms = (int16_t) ( cmd_msg.linear.x + (cmd_msg.angular.z)*WHEEL_BASE_MM/2.0 );
  Target_LeftMotor_mms  = (int16_t) ( cmd_msg.linear.x - (cmd_msg.angular.z)*WHEEL_BASE_MM/2.0 );
}
ros::Subscriber<geometry_msgs::Twist> Motors_sub("Motors_mms", Motors);

// ROS Initialization
void ROS_Setup(int32_t BaudRate) {
  nh.getHardware()->setBaud(BaudRate);
  nh.initNode();

  nh.advertise(quat_pub);
  nh.advertise(target_pub);

  nh.advertise(RightEncoder_pub);
  nh.advertise(LeftEncoder_pub);
  
  // DC-Motors:
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
  LeftEnc.data  = LeftEncoder_Distance;    LeftEncoder_pub.publish(&LeftEnc);
  
  // Target Position:
  if( targetX != PrevTargetX || targetY != PrevTargetY ) {
    zAxis_Speed = -1000;

    target.x = targetX;
    target.y = targetY;

    PrevTargetX = targetX;
    PrevTargetY = targetY;
  }
  else {
    zAxis_Speed = 1000;
  }

  nh.spinOnce();
  delay(1);
}

#endif