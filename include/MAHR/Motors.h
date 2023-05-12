#ifndef MAHR_MOTORS_H_
#define MAHR_MOTORS_H_

#include <MAHR.h>
#include <ESP32Encoder.h>
#include "esp_task_wdt.h"
#include <CytronMotorDriver.h>

#define ENCODER_PPR   360.0   // Encoder Resolution in pulse/rev.
#define LINEAR    0
#define ROTATINAL 1
#define DIAGONAL  2

static IRAM_ATTR void RightEncoder_cb(void* arg) {
  ESP32Encoder* enc = (ESP32Encoder*) arg;
}
static IRAM_ATTR void LeftEncoder_cb(void* arg) {
  ESP32Encoder* enc = (ESP32Encoder*) arg;
}

extern bool loopTaskWDTEnabled;
extern TaskHandle_t loopTaskHandle;
ESP32Encoder RightEncoder(true, RightEncoder_cb);             // ESP32Encoder object for Right Encoder.
ESP32Encoder LeftEncoder (true, LeftEncoder_cb );             // ESP32Encoder object for Left Encoder.

float_t dt;
double previous_time;
float_t RightEncoder_degs, LeftEncoder_degs;
int64_t RightEncoder_PrevDistance, LeftEncoder_PrevDistance;

int8_t sign;
float_t diff, Rout_offset, Rin_offset;
int8_t PrevState, CurrState = LINEAR;
int64_t RightEncoder_CurrDistance, LeftEncoder_CurrDistance;       // Average Previous travelled distance of Encoders in deg.
double_t RightEncoder_OffsetDistance, LeftEncoder_OffsetDistance;  // Offest distance of Encoders in deg.

static const char *LOG_TAG = "main";

// Filter:
LowPass<2> lp_sr(300, 100e3, false);
LowPass<2> lp_sl(300, 100e3, false);

CytronMD motorR(PWM_DIR, MOTOR_RIGHT_PWM, MOTOR_RIGHT_DIR);   // Driver object for Right Motor
CytronMD motorL(PWM_DIR, MOTOR_LEFT_PWM , MOTOR_LEFT_DIR );   // Driver object for Left Motor
float_t LeftMotor_mmss, RightMotor_mmss;
float_t LeftMotor_mms, RightMotor_mms;
int8_t LeftMotor_dir, RightMotor_dir;

// Motors Initialization
void Motors_Setup() {
  // Motors: ...

  // Encoders:
  loopTaskWDTEnabled = true;

  RightEncoder.attachSingleEdge(ENCODER_RIGHT_A, ENCODER_RIGHT_B);
  LeftEncoder.attachSingleEdge(ENCODER_LEFT_A, ENCODER_LEFT_B);

  RightEncoder.clearCount();
  LeftEncoder.clearCount();

  RightEncoder.setFilter(1023);
  LeftEncoder.setFilter(1023);

  esp_log_level_set("*", ESP_LOG_DEBUG);
  esp_log_level_set("main", ESP_LOG_DEBUG);
  esp_log_level_set("ESP32Encoder", ESP_LOG_DEBUG);
  esp_task_wdt_add(loopTaskHandle);
}

void Encoders_DataUpdate(){
  // Position Calcu. :
  RightEncoder_Distance = -RightEncoder.getCount();
  LeftEncoder_Distance  = -LeftEncoder.getCount();

  // Speed Calcu. :
  RightEncoder_degs = ((float_t) (RightEncoder_Distance - RightEncoder_PrevDistance))/(((float_t) millis())/1000.0 - previous_time);
  LeftEncoder_degs  = ((float_t) ( LeftEncoder_Distance -  LeftEncoder_PrevDistance))/(((float_t) millis())/1000.0 - previous_time);
  
  previous_time = ((float_t) millis())/1000.0;
  RightEncoder_PrevDistance = -RightEncoder.getCount();
  LeftEncoder_PrevDistance  = -LeftEncoder.getCount();

  RightEncoder_mms = lp_sr.filt(RightEncoder_degs) * (PI/180.0)*WHEEL_RADIUS_MM;
  LeftEncoder_mms  = lp_sl.filt(LeftEncoder_degs)  * (PI/180.0)*WHEEL_RADIUS_MM;
}

// Speed Control Diff. Robot by Differential Controller
void Motors_RunSpeed() {
  // Define Motion State:
  if      (Required_RightMotor_mms ==    Required_LeftMotor_mms) { CurrState=LINEAR;    sign= 1; }
  else if (Required_RightMotor_mms == -1*Required_LeftMotor_mms) { CurrState=ROTATINAL; sign=-1; }
  else                                                           { CurrState=DIAGONAL;  sign= 1; }

  // Encoder Resetting between States:
  if( CurrState != PrevState) {
    RightEncoder_OffsetDistance = RightEncoder_Distance;
    LeftEncoder_OffsetDistance  = LeftEncoder_Distance;
  }
  RightEncoder_CurrDistance = RightEncoder_Distance - RightEncoder_OffsetDistance;
  LeftEncoder_CurrDistance  = LeftEncoder_Distance  - LeftEncoder_OffsetDistance;

  // calcu. error difference:
  static double prevDiff = 0;
  static double prevPrevDiff = 0;
  double currDifference = RightEncoder_CurrDistance - LeftEncoder_CurrDistance; // RightEncoder_mms - LeftEncoder_mms;
  double avgDifference = (prevDiff+prevPrevDiff+currDifference)/3;
  prevPrevDiff = prevDiff;
  prevDiff = currDifference;

  static double prevSum = 0;
  static double prevPrevSum = 0;
  double currSummation = RightEncoder_CurrDistance + LeftEncoder_CurrDistance; // RightEncoder_mms + LeftEncoder_mms;
  double avgSummation = (prevSum+prevPrevSum+currSummation)/3;
  prevPrevSum = prevSum;
  prevSum = currSummation;

  Rout_offset = Required_LeftMotor_mms - Required_RightMotor_mms; // In (rotation point out side robot) case
  Rin_offset =  Required_LeftMotor_mms + Required_RightMotor_mms; // In (rotation point in  side robot) case [in progressing]

  if(CurrState == ROTATINAL) { diff = 5.0*((float_t)(avgSummation  + Rin_offset )); }
  else                       { diff = 5.0*((float_t)(avgDifference + Rout_offset)); }


  // Correct the action values:
  Required_RightMotor_mms -= (int)(0.5*diff);
  Required_LeftMotor_mms  += (int)(0.5*diff*sign);

  /*********************************************************************************************************************************/

  // Deadzone:
  if(Required_RightMotor_mms <= 80) {Required_RightMotor_mms = 0;}
  if(Required_LeftMotor_mms  <= 80) {Required_LeftMotor_mms  = 0;}


  // If the required PWM is of opposite sign as the output PWM, we want to
  // stop the car before switching direction
  if ((Required_LeftMotor_mms * LeftEncoder_mms < 0 && LeftMotor_mms != 0) ||
      (Required_RightMotor_mms * RightEncoder_mms < 0 && RightMotor_mms != 0)) {
    Required_LeftMotor_mms  = 0;
    Required_RightMotor_mms = 0;
  }

  // Set the direction of the motors
  if      (Required_LeftMotor_mms >  0) {LeftMotor_dir =  1;}
  else if (Required_LeftMotor_mms >  0) {LeftMotor_dir = -1;}
  else if (Required_LeftMotor_mms == 0
                 && LeftMotor_mms == 0) {LeftMotor_dir =  0;}
  else                                  {LeftMotor_dir =  0;}

  if      (Required_RightMotor_mms >  0) {RightMotor_dir =  1;}
  else if (Required_RightMotor_mms >  0) {RightMotor_dir = -1;}
  else if (Required_RightMotor_mms == 0
                 && RightMotor_mms == 0) {RightMotor_dir =  0;}
  else                                   {RightMotor_dir =  0;}

  // Increase the required PWM if the robot is not moving
  if (Required_LeftMotor_mms  != 0 && LeftEncoder_mms  == 0) {Required_LeftMotor_mms  *= 1.5;}
  if (Required_RightMotor_mms != 0 && RightEncoder_mms == 0) {Required_RightMotor_mms *= 1.5;}


  // Calculate the output PWM value by making slow changes to the current value (acceleration)
  if      (abs(Required_LeftMotor_mms) > LeftMotor_mms) {LeftMotor_mms += 2;}
  else if (abs(Required_LeftMotor_mms) < LeftMotor_mms) {LeftMotor_mms -= 2;}
  else{}
   
  if      (abs(Required_RightMotor_mms) > RightMotor_mms) {RightMotor_mms += 2;}
  else if (abs(Required_RightMotor_mms) < RightMotor_mms) {RightMotor_mms -= 2;}
  else{}
  
  // convert motor-speed from mm/s to RPM:
  float_t RightMotor_RPM = RightMotor_mms * 60.0/(WHEEL_RADIUS_MM* 2*PI);
  float_t LeftMotor_RPM  = LeftMotor_mms  * 60.0/(WHEEL_RADIUS_MM* 2*PI);

  // convert motor-speed from RPM to Volt:
  float_t RightMotor_Volt = RightMotor_RPM * 255.0/69.0;
  float_t LeftMotor_Volt  = LeftMotor_RPM  * 255.0/69.0;

  motorR.setSpeed((int16_t) round(RightMotor_Volt)*RightMotor_dir);
  motorL.setSpeed((int16_t) round(LeftMotor_Volt )*LeftMotor_dir );

  PrevState = CurrState;
}
// Print the Encoder Position and Speed
void Motors_PrintData() {
  Serial.printf("Motors: Speed(%4.0f,%4.0f)\t\tEncoders: Position(%8lld,%8lld)deg\tSpeed(%10.0f,%10.0f)deg\n",
                LeftMotor_mms,
                RightMotor_mms,
                LeftEncoder_Distance,
                RightEncoder_Distance,
                LeftEncoder_mms,
                RightEncoder_mms);
}

#endif