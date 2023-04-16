#ifndef MAHR_ROS_H_
#define MAHR_ROS_H_

#include <MAHR.h>
#include <ros.h>
#include <ros/time.h>
#include <sensor_msgs/Range.h>
#include <std_msgs/Int64.h>
#include <geometry_msgs/Vector3.h>
#include <geometry_msgs/Quaternion.h>
#include <geometry_msgs/Twist.h>

ros::NodeHandle nh;

  // Publishers:
sensor_msgs::Range range[4];
geometry_msgs::Vector3 target;
geometry_msgs::Quaternion q;
std_msgs::Int64 RightEnc, LeftEnc;

ros::Publisher range_pub("/ultrasonics_cm", range);
ros::Publisher quat_pub("Quaternionfilter", &q);
ros::Publisher target_pub("/TargetPosition_cm", &target);
ros::Publisher RightEncoder_pub("/RightEncoder_deg", &RightEnc);
ros::Publisher LeftEncoder_pub("/LeftEncoder_deg", &LeftEnc);

  // Subscribers:
void Motors(const geometry_msgs::Twist &cmd_msg) {
  RightMotor_Speed = (int16_t) ( cmd_msg.linear.x + (cmd_msg.angular.z)*WHEEL_BASE_MM/2.0 );
  LeftMotor_Speed  = (int16_t) ( cmd_msg.linear.x - (cmd_msg.angular.z)*WHEEL_BASE_MM/2.0 );
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

  // ultrasonics
  nh.advertise(range_pub);
  for(uint8_t i=0; i<4; i++){
    range[i].radiation_type = sensor_msgs::Range::ULTRASOUND;
    range[i].field_of_view = 15; // 10:15 degree
    range[i].min_range = 3;
    range[i].max_range = 200;
  }
  range[0].header.frame_id = "/ultrasonic_fl";
  range[1].header.frame_id = "/ultrasonic_nl";
  range[2].header.frame_id = "/ultrasonic_nr";
  range[3].header.frame_id = "/ultrasonic_fr";
  
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

  // ultrasonics
  for (uint8_t i = 0; i < 4; i++) {
    range[i].range = ultrasonics[i];
    range[i].header.stamp = nh.now();
    delay(50);
  }
  range_pub.publish(range);

  nh.spinOnce();
  delay(1);
}

#endif