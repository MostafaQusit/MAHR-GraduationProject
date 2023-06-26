#ifndef MAHR_H_
#define MAHR_H_

#include <Arduino.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <private_data.h>
#include <MAHR/MP3_List.h>
#include <MAHR/PinsList.h>
using namespace std;

#define R2D(RADIAN)   (RADIAN*180.0/PI) // convert from Radian to Degree
#define D2R(DEGREE)   (DEGREE*PI/180.0) // convert from Degree to Radian

#define WHEEL_BASE_MM   374.2   // Robot's Wheel Base in (mm)
#define WHEEL_RADIUS_MM  54.0   // Robot's Radius in (mm)

#define AUTONO_MODE   0             // Autonomous Mode
#define MANUAL_MODE   1             // Manual(RC) Mode
uint8_t robot_mode = AUTONO_MODE;   // Robot Mode

#define BASE_MODE   0             // Base (DC Motors and Z-Axis) Mode
#define  ARM_MODE   1             // Arm Mode
uint8_t control_mode = BASE_MODE; // Control Mode

// Encoders:
int64_t RightEncoder_Distance;    // Right Encoder: Travelling Distance in (degrees)
int64_t LeftEncoder_Distance;     //  Left Encoder: Travelling Distance in (degrees)

// IMU:
float_t accelerometer[3];   // IMU: Accelerometer data array [0,1,2 = x,y,z]
float_t gyroscope[3];       // IMU: Gyroscope     data array [0,1,2 = x,y,z]
float_t magnometer[3];      // IMU: Magnometer    data array [0,1,2 = x,y,z]
float_t quaternion[4];      // IMU: Quaternion    data array [0,1,2,3 = x,y,z,w]
float_t Yaw_angle;          // IMU: Yaw   Angle
float_t Pitch_angle;        // IMU: Pitch Angle
float_t Roll_angle;         // IMU: Roll  Angle

// Ultrasonics:
uint32_t ultrasonics[4];    // Ultrasonics: Distance in (cm)

// DC Motors
float_t Required_RightMotor_mms;  // Right Motor: linear speed in (mm/s)
float_t Required_LeftMotor_mms;   //  Left Motor: linear speed in (mm/s)
float_t motors_linear;    // Motors:  linear vector [0~1] (presentage)
float_t motors_angular;   // Motors: angular vector [0~1] (presentage)

// MP3:
uint16_t voice_file=1;    // MP3: file number [1~255]

// ROS:
int32_t Target_PositionX;   // ROS: Target Position in X-axis
int32_t Target_PositionY;   // ROS: Target Position in Y-axis

int32_t Current_PositionX;   // ROS: Current Position in X-axis
int32_t Current_PositionY;   // ROS: Current Position in Y-axis

// Z-Axis Stepper:
float_t zAxis_direction;  // Z-Axis Stepper: direction, (1) is for a direction, (-1) is for the other, (0) is Stop
float_t zAxis_position;   // Z-Axis Stepper: position (Height) from home point [0~250] in (mm)

// Arm:
#define X 0 // X-axis index
#define Y 1 // Y-axis index
#define P 2 // Pitch  index
#define R 3 // Roll   index
#define G 4 // Grip   index

#define A 0 // link1 index
#define B 1 // link2 index

/**
 * Arm parameters as directions: (1) is for a direction, (-1) is for the other, (0) is Stop
 *  - index [0]=[X] is End-Effector planer direction in X-axis
 *  - index [1]=[Y] is End-Effector planer direction in Y-axis
 *  - index [2]=[P] is Pitch joint direction
 *  - index [3]=[R] is Roll  joint direction
 *  - index [4]=[G] is Grip  joint direction
 */
float_t arm_directions[5];

/**
 * Arm parameters as angles in (degree):
 *  - index [0]=[A] is link1 joint angle
 *  - index [1]=[B] is link2 joint angle
 *  - index [2]=[P] is Pitch joint angle
 *  - index [3]=[R] is Roll  joint angle
 *  - index [4]=[G] is Grip  joint angle
 */
float_t arm_angles[5];

/**
 * @brief   get the sign of any type of data
 *
 * @param   number  the number
 *
 * @return  +1 if positive(+ve), -1 if negative(-ve) and 0 otherwise
 */
template <typename S> S sign(S number){
  S sign = 0;
  if     (number > 0) { sign =  1; }
  else if(number < 0) { sign = -1; }
  return sign;
}

/**
 * @brief   match between 2 arrays of any type
 * 
 * @param   arr1  array number 1
 * @param   arr2  array number 2
 * 
 * @return  true(1) if are identical, false(0) if not
 */
template <typename T> bool match_array(T* arr1, T* arr2, size_t len){
  for(size_t i=0; i<len; i++){
    if(arr1[i] != arr2[i]){
      return 0;
    }
  }
  return 1;
}

/**
 * @brief Low Pass Filter Class
 * 
 * @param   order   order of the filter: 1st-order(1) or 2nd-order(2) only
 */
template <int order> class LowPass {
  private:
    float a[order];
    float b[order+1];
    float omega0;
    float dt;
    bool adapt;
    float tn1 = 0;
    float x[order+1]; // Raw values
    float y[order+1]; // Filtered values

  public:
    /**
      * @brief  Low-Pass filter Constructor
      *
      * @param  f0   cutoff frequency (Hz)
      * @param  fs   sample frequency (Hz)
      * @param  f0   boolean flag, if set to true(1), the code will automatically set
                     the sample frequency based on the time history
    */
    LowPass(float f0, float fs, bool adaptive=0){      
      omega0 = 6.28318530718*f0;
      dt = 1.0/fs;
      adapt = adaptive;
      tn1 = -dt;
      for(int k = 0; k < order+1; k++){
        x[k] = 0;
        y[k] = 0;        
      }
      setCoef();
    }
    
    /**
      * @brief  set the coefficients
    */
    void setCoef(){
      if(adapt){
        float t = micros()/1.0e6;
        dt = t - tn1;
        tn1 = t;
      }
      
      float alpha = omega0*dt;
      if(order==1){
        a[0] = -(alpha - 2.0)/(alpha+2.0);
        b[0] = alpha/(alpha+2.0);
        b[1] = alpha/(alpha+2.0);        
      }
      if(order==2){
        float alphaSq = alpha*alpha;
        float beta[] = {1, sqrt(2), 1};
        float D = alphaSq*beta[0] + 2*alpha*beta[1] + 4*beta[2];
        b[0] = alphaSq/D;
        b[1] = 2*b[0];
        b[2] = b[0];
        a[0] = -(2*alphaSq*beta[0] - 8*beta[2])/D;
        a[1] = -(beta[0]*alphaSq - 2*beta[1]*alpha + 4*beta[2])/D;      
      }
    }
    
    /**
      * @brief  filtering the data
      * 
      * @param  xn  the current raw data
      * 
      * @return the current filtered data 
    */
    float filt(float xn){
      if(adapt){
        setCoef(); // Update coefficients if necessary      
      }
      y[0] = 0;
      x[0] = xn;
      // Compute the filtered values
      for(int k = 0; k < order; k++){
        y[0] += a[k]*y[k+1] + b[k]*x[k];
      }
      y[0] += b[order]*x[order];

      // Save the historical values
      for(int k = order; k > 0; k--){
        y[k] = y[k-1];
        x[k] = x[k-1];
      }
  
      // Return the filtered value    
      return y[0];
    }
};

#endif