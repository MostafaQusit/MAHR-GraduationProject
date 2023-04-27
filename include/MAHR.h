#ifndef MAHR_H_
#define MAHR_H_

#include <Arduino.h>
#include <MAHR/PinsList.h>

#define WHEEL_BASE_MM   374.2
#define WHEEL_RADIUS_MM  54.0

int64_t RightEncoder_Distance, LeftEncoder_Distance;    // Encoders::Distance in degrees

float_t accelerometer[3], gyroscope[3], magnometer[3];  // IMU::9-axis reading data (0,1,2 = x,y,z)
float_t quaternion[4];                                  // IMU::QuaternionFilter Values (0,1,2,3 = x,y,z,w)
float_t Yaw_angle, Pitch_angle, Roll_angle;             // IMU::3-Main Angles

uint32_t ultrasonics[4];                                // Ultrasonics::Distance in cm
int16_t RightMotor_Speed, LeftMotor_Speed;              // Motors::Speed in mm/s
uint16_t voice_file;                                    // MP3::file number
int32_t targetX, targetY, PrevTargetX, PrevTargetY;     // ROS::Target Position
int16_t zAxis_Speed;                                    // Z-Axis Stepper::Speed in pulse/s
int16_t Grip, wrist, roll, armX, armY;         // Arm:: Position parameters

#endif