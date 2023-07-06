#ifndef MAHR_ROS_H_
#define MAHR_ROS_H_

#include <MAHR.h>
#include <ros.h>
#include <ros/time.h>
#include <std_msgs/Int16.h>
#include <geometry_msgs/Twist.h>

ros::NodeHandle nh; // Node Handler Object

std_msgs::Int16 RightEnc; // Right Encoder message
std_msgs::Int16 LeftEnc;  // Left  Encoder message

// Publishers:
ros::Publisher RightEncoder_pub("/right_ticks", &RightEnc); // Right Encoder Publisher 
ros::Publisher  LeftEncoder_pub("/left_ticks" , & LeftEnc); // Left  Encoder Publisher

double lastCmdVelReceived;  // last time velocity command (cmdVel) has send any changes in (sec)

/**
 * @brief   call-back function when receive velocity command
 * 
 * @param   cmd_msg   command message
 */
void Motors(const geometry_msgs::Twist &cmd_msg) {
  lastCmdVelReceived = (millis() / 1000); // record the time
  motors_linear  = cmd_msg.linear.x;
  motors_angular = cmd_msg.angular.z;
}

// Subscribers:
ros::Subscriber<geometry_msgs::Twist> Motors_sub("cmd_vel", Motors);  // Motors Subscriber

/**
 * @brief   ROS Setup
 * 
 * @param   BaudRate    Baud Rate of ROS Serial Communication
 */
void ROS_Setup(int32_t BaudRate) {
  nh.getHardware()->setBaud(BaudRate);  // set the Baud Rate
  nh.initNode();                        // Node Init.

  nh.advertise(RightEncoder_pub); // Right Encoder Publisher Setup (Advertise)
  nh.advertise(LeftEncoder_pub);  // left  Encoder Publisher Setup (Advertise)

  nh.subscribe(Motors_sub); // Motors Subscriber Setup
}

/**
 * @brief   Send the sensory data (Encoders Reading) to ROS
 */
void ROS_SendData() {
  
  // Update Encoders message:
  RightEnc.data = RightEncoder_Distance;
  LeftEnc.data  = LeftEncoder_Distance;

  // Publish the Encoders message to ROS:
  RightEncoder_pub.publish(&RightEnc);
  LeftEncoder_pub.publish(&LeftEnc);
  nh.spinOnce();
}

/**
 * @brief   check if there incoming messages or not
 */
void ROS_CheckIncoming(){
  // Stop the robot if there are no cmdVel messages
  if((millis()/1000) - lastCmdVelReceived > 1) {
    motors_linear  = 0.0;
    motors_angular = 0.0;
  }
}

#endif