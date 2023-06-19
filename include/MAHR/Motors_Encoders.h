#ifndef MAHR_MOTORS_ENCODERS_H_
#define MAHR_MOTORS_ENCODERS_H_

#include <MAHR.h>
#include <ESP32Encoder.h>
#include "esp_task_wdt.h"
#include <CytronMotorDriver.h>

#define ENCODER_PPR   360.0   // Encoder Resolution in (pulse/rev -> PPR)
#define LINEAR          0     // Linear Motion State
#define ANGULAR         1     // Angular Motion State
#define STOP            2     // Stop State

/**
 * @brief call-back function for counting Right Encoder
 */
static IRAM_ATTR void RightEncoder_cb(void* arg) {
  ESP32Encoder* enc = (ESP32Encoder*) arg;
}

/**
 * @brief call-back function for counting left Encoder
 */
static IRAM_ATTR void LeftEncoder_cb(void* arg) {
  ESP32Encoder* enc = (ESP32Encoder*) arg;
}

// Encoders:
extern bool loopTaskWDTEnabled;
extern TaskHandle_t loopTaskHandle;
ESP32Encoder RightEncoder(true, RightEncoder_cb);             // ESP32Encoder object for Right Encoder.
ESP32Encoder LeftEncoder (true, LeftEncoder_cb );             // ESP32Encoder object for Left Encoder.

// Encoders Speed parameters:
unsigned long PreviousMillis;       // time in previous iteration in (mS)  for counter resetting
unsigned long previousMillis;       // time in previous iteration in (mS)  for speed update rate
unsigned long previous_time;        // time in previous iteration in (sec) for speed calculation
float_t RightEncoder_degs;          // right Encoder speed in (deg/s)
float_t LeftEncoder_degs;           // left  Encoder speed in (deg/s)
float_t RightEncoder_mms;           // right Encoder speed in (mm/s)
float_t LeftEncoder_mms;            // left  Encoder speed in (mm/s)
int64_t RightEncoder_PrevDistance;  // right Encoder previous distance for speed calculation in (deg)
int64_t LeftEncoder_PrevDistance;   // left  Encoder previous distance for speed calculation in (deg)

// Differential Controller parameters:
int8_t sign;              // the difference between error in each motor: 1(+ve) in linear motion, -1(-ve) in angular motion
float_t DifferenceError;  // Difference Error
float_t Rout_offset;      // the offset in (rotation point out side robot) case
float_t Rin_offset;       // the offset in (rotation point in  side robot) case

int8_t PrevState, CurrState = STOP;
double_t RightEncoder_OffsetDistance;   // Offest distance                                 of right Encoders in (deg)
double_t LeftEncoder_OffsetDistance;    // Offest distance                                 of left  Encoders in (deg)
int64_t RightEncoder_CurrDistance;      // Current distance [after reset for motion state] of right Encoders in (deg)
int64_t LeftEncoder_CurrDistance;       // Current distance [after reset for motion state] of left  Encoders in (deg)

static const char *LOG_TAG = "main";

// Motors parameters:
CytronMD motorR(PWM_DIR, MOTOR_RIGHT_PWM, MOTOR_RIGHT_DIR);   // Driver object for Right Motor
CytronMD motorL(PWM_DIR, MOTOR_LEFT_PWM , MOTOR_LEFT_DIR );   // Driver object for Left Motor
float_t LeftMotor_mmss;   // left  Motor acceleration  in (mm/s^2)
float_t RightMotor_mmss;  // right Motor acceleration  in (mm/s^2)
float_t LeftMotor_mms;    // left  Motor current speed in (mm/s)
float_t RightMotor_mms;   // right Motor current speed in (mm/s)

/**
 * @brief   Motors Setup
 */
void Encoders_Setup() {
  loopTaskWDTEnabled = true;

  // set the Encoders to single edge (trigger RISING or FALLING only) [no prescaling in pulses]
  RightEncoder.attachSingleEdge(ENCODER_RIGHT_A, ENCODER_RIGHT_B);
  LeftEncoder.attachSingleEdge(ENCODER_LEFT_A, ENCODER_LEFT_B);

  // Resetting the counter:
  RightEncoder.clearCount();
  LeftEncoder.clearCount();

  // set the filter to man (1023)
  RightEncoder.setFilter(1023);
  LeftEncoder.setFilter(1023);

  // Task Handler & Debug setting:
  esp_log_level_set("*", ESP_LOG_DEBUG);
  esp_log_level_set("main", ESP_LOG_DEBUG);
  esp_log_level_set("ESP32Encoder", ESP_LOG_DEBUG);
  esp_task_wdt_add(loopTaskHandle);
}

/**
 * @brief   Update Encoder Position and Speed
 */
void Encoders_DataUpdate(){
  // Resetting the counters if the robot does not move for a while
  if(LeftMotor_mms==0 && RightMotor_mms==0 && (millis()-PreviousMillis)>=3000){
    RightEncoder.clearCount();
    LeftEncoder.clearCount();
    PreviousMillis = millis();
  }

  // Position Calculation:
  RightEncoder_Distance = -RightEncoder.getCount();
  LeftEncoder_Distance  = -LeftEncoder.getCount();

  // Speed Calculation: in periodic time more than or equal to 50 mS
  if (millis() - previousMillis >= 50) {
    // calculate speed
    RightEncoder_degs = (RightEncoder_Distance - RightEncoder_PrevDistance)/(millis()/1000.0 - previous_time);
    LeftEncoder_degs  = ( LeftEncoder_Distance -  LeftEncoder_PrevDistance)/(millis()/1000.0 - previous_time);
    
    // update for next iteration
    previous_time = millis()/1000.0;
    RightEncoder_PrevDistance = -RightEncoder.getCount();
    LeftEncoder_PrevDistance  = -LeftEncoder.getCount();

    // convert from (deg/s) to (mm/s)
    RightEncoder_mms = RightEncoder_degs * (PI/180.0)*WHEEL_RADIUS_MM;
    LeftEncoder_mms  = LeftEncoder_degs  * (PI/180.0)*WHEEL_RADIUS_MM;
      
    previousMillis = millis();  // record the time
  }
}

/**
 * @brief   Speed Control Diff. Robot by Differential Controller
 */
void Motors_RunSpeed() {
  // Check z-Axis State: move only if it in BOTTOM position
  if(true) {  //digitalRead(LOWER_LS)==HIGH
    // Acceleration & Speed Profile (like exp.):
    LeftMotor_mmss  = (Required_LeftMotor_mms -LeftMotor_mms )/30.0;
    RightMotor_mmss = (Required_RightMotor_mms-RightMotor_mms)/30.0;
    LeftMotor_mms  += LeftMotor_mmss;
    RightMotor_mms += RightMotor_mmss;
  }
  else {
    LeftMotor_mms  = 0;
    RightMotor_mms = 0;
  }

  // Define Motion State:
  if      (abs(motors_linear) > abs(motors_angular)) { CurrState = LINEAR;  sign =  1; }
  else if (abs(motors_linear) < abs(motors_angular)) { CurrState = ANGULAR; sign = -1; }
  else                                               { CurrState = STOP;               }

  // Encoder Resetting between States:
  if( CurrState != PrevState) {
    RightEncoder_OffsetDistance = RightEncoder_Distance;
    LeftEncoder_OffsetDistance  = LeftEncoder_Distance;
  }
  RightEncoder_CurrDistance = RightEncoder_Distance - RightEncoder_OffsetDistance;
  LeftEncoder_CurrDistance  = LeftEncoder_Distance  - LeftEncoder_OffsetDistance;

  Rout_offset = Required_LeftMotor_mms - Required_RightMotor_mms; // In (rotation point out side robot) case
  Rin_offset =  Required_LeftMotor_mms + Required_RightMotor_mms; // In (rotation point in  side robot) case

  // calculate error difference:
  if     (CurrState == ANGULAR) { DifferenceError = 5.5*((float_t)(RightEncoder_CurrDistance + LeftEncoder_CurrDistance + Rin_offset )); }
  else if(CurrState == LINEAR ) { DifferenceError = 5.5*((float_t)(RightEncoder_CurrDistance - LeftEncoder_CurrDistance + Rout_offset)); }
  else                          { DifferenceError = 0.0; }
  
  // convert motor-speed from mm/s to RPM:
  float_t RightMotor_RPM = (RightMotor_mms - 0.5*DifferenceError     ) * 60.0/(WHEEL_RADIUS_MM* 2*PI);
  float_t LeftMotor_RPM  = (LeftMotor_mms  + 0.5*DifferenceError*sign) * 60.0/(WHEEL_RADIUS_MM* 2*PI);

  // convert motor-speed from RPM to Volt:
  float_t RightMotor_Volt = RightMotor_RPM * 255.0/69.0;
  float_t LeftMotor_Volt  = LeftMotor_RPM  * 255.0/69.0;


  // make the action on motors:
  motorR.setSpeed((int16_t) round(RightMotor_Volt));
  motorL.setSpeed((int16_t) round(LeftMotor_Volt ));

  PrevState = CurrState;  // update state for next iteration
}

/**
 * @brief   Print Motors Speed, Encoders Position and Speed
 */
void MotorsEncoders_PrintData() {
  Serial.printf("Motors: Speed(l:%3.2f, a:%3.2f) -> (%6.2f,%6.2f)\t\tEncoders: Position(%8lld,%8lld)deg\n",
                motors_linear,
                motors_angular,
                LeftMotor_mms,
                RightMotor_mms,
                LeftEncoder_Distance,
                RightEncoder_Distance);
}

#endif