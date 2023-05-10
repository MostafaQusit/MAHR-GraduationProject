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
uint32_t current_time, previous_time;
float_t RightEncoder_mms, LeftEncoder_mms;
float_t RightEncoder_degs, LeftEncoder_degs;
uint64_t RightEncoder_PrevDistance, LeftEncoder_PrevDistance;

int8_t sign;
float_t diff, side_offset;
int8_t PrevState, CurrState = LINEAR;
int64_t RightEncoder_CurrDistance, LeftEncoder_CurrDistance;       // Average Previous travelled distance of Encoders in deg.
double_t RightEncoder_OffsetDistance, LeftEncoder_OffsetDistance;  // Offest distance of Encoders in deg.

static const char *LOG_TAG = "main";

// Filter:
LowPass<2> lp_sr(1e3, 100e3, false);
LowPass<2> lp_sl(1e3, 100e3, false);
LowPass<2> lp_tr(1e3, 100e3, false);
LowPass<2> lp_tl(1e3, 100e3, false);

CytronMD motorR(PWM_DIR, MOTOR_RIGHT_PWM, MOTOR_RIGHT_DIR);   // Driver object for Right Motor
CytronMD motorL(PWM_DIR, MOTOR_LEFT_PWM , MOTOR_LEFT_DIR );   // Driver object for Left Motor
float_t LeftMotor_mmss, RightMotor_mmss;
float_t LeftMotor_mms, RightMotor_mms;

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
// Speed Control Diff. Robot by Differential PID Controller
void Motors_RunSpeed() {
  //Target_LeftMotor_mms  = lp_tl.filt(Target_LeftMotor_mms);
  //Target_RightMotor_mms = lp_tl.filt(Target_RightMotor_mms);

  // Check z-Axis State:
  if(true) {  // digitalRead(LOWER_LS)==HIGH
    // Acceleration & Speed Profile (like exp.):
    LeftMotor_mmss  = ((float_t) (Target_LeftMotor_mms -LeftMotor_mms ))/25.0;
    RightMotor_mmss = ((float_t) (Target_RightMotor_mms-RightMotor_mms))/25.0;
    LeftMotor_mms  += LeftMotor_mmss;
    RightMotor_mms += RightMotor_mmss;
  }
  else {
    LeftMotor_mms  = 0;
    RightMotor_mms = 0;
  }

  // Position Calcu. :
  noInterrupts();
  RightEncoder_Distance = -RightEncoder.getCount();
  LeftEncoder_Distance  = -LeftEncoder.getCount();
  interrupts();

  // Speed Calcu. :
  current_time = micros();
  dt = ((float_t) (current_time - previous_time))/1.0e6;
  RightEncoder_degs = ((float_t) (RightEncoder_Distance - RightEncoder_PrevDistance))/dt;
  LeftEncoder_degs  = ((float_t) ( LeftEncoder_Distance -  LeftEncoder_PrevDistance))/dt;
  
  previous_time = current_time;
  RightEncoder_PrevDistance = RightEncoder_Distance;
  LeftEncoder_PrevDistance  = LeftEncoder_Distance;

  RightEncoder_degs = lp_sr.filt(RightEncoder_degs);
  LeftEncoder_degs  = lp_sl.filt(LeftEncoder_degs);

  RightEncoder_mms = RightEncoder_degs *(PI/180)*WHEEL_RADIUS_MM;
  LeftEncoder_mms  = LeftEncoder_degs  *(PI/180)*WHEEL_RADIUS_MM;


  // Define Motion State:
  if      (RightMotor_mms ==    LeftMotor_mms) { CurrState=LINEAR;    sign= 1; }
  else if (RightMotor_mms == -1*LeftMotor_mms) { CurrState=ROTATINAL; sign=-1; }
  else                                         { CurrState=DIAGONAL;  sign= 1; }

  // Encoder Resetting between States:
  if( CurrState != PrevState) {
    RightEncoder_OffsetDistance = RightEncoder_Distance;
    LeftEncoder_OffsetDistance  = LeftEncoder_Distance;
  }
  RightEncoder_CurrDistance = RightEncoder_Distance - RightEncoder_OffsetDistance;
  LeftEncoder_CurrDistance  = LeftEncoder_Distance  - LeftEncoder_OffsetDistance;

  side_offset = LeftMotor_mms - RightMotor_mms; // In 4 crossing-direction case: [in progressing]
  
  // calcu. error difference:
  if(CurrState == ROTATINAL) { diff = 5.0*((float_t)(RightEncoder_CurrDistance + LeftEncoder_CurrDistance)); }
  else                       { diff = 5.0*((float_t)(RightEncoder_CurrDistance - LeftEncoder_CurrDistance + side_offset)); }
  
  // convert motor-speed from mm/s to RPM:
  float_t RightMotor_RPM = (RightMotor_mms - 0.5*diff     ) * 60.0/(WHEEL_RADIUS_MM* 2*PI);
  float_t LeftMotor_RPM  = (LeftMotor_mms  + 0.5*diff*sign) * 60.0/(WHEEL_RADIUS_MM* 2*PI);

  // convert motor-speed from RPM to Volt:
  float_t RightMotor_Volt = RightMotor_RPM * 255.0/69.0;
  float_t LeftMotor_Volt  = LeftMotor_RPM  * 255.0/69.0;

  motorR.setSpeed((int16_t) round(RightMotor_Volt));
  motorL.setSpeed((int16_t) round(LeftMotor_Volt));

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