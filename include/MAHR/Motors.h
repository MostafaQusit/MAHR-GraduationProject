#ifndef MAHR_MOTORS_H_
#define MAHR_MOTORS_H_

#include <MAHR.h>
#include <CytronMotorDriver.h>
//#include <PID_v1.h>

#define MOTOR_MAXSPEED_RPM   60.0   // Motor max. speed in rpm

CytronMD motorR(PWM_DIR, MOTOR_RIGHT_PWM, MOTOR_RIGHT_DIR);   // Driver object for Right Motor
CytronMD motorL(PWM_DIR, MOTOR_LEFT_PWM , MOTOR_LEFT_DIR );   // Driver object for Left Motor
int16_t error_L, error_R;

// Motors Initialization
void Motors_Setup() {
  //PID_R.SetMode(AUTOMATIC);
  //PID_L.SetMode(AUTOMATIC);
}
// Speed Control Diff. Robot by PID Controller
void Motors_SetSpeed(int16_t LeftMotor_mms, int16_t RightMotor_mms) {
  if ( LeftMotor_mms  > 0) { LeftMotor_mms  /= 1.25; }
  error_R = 1.0*(RightMotor_mms-RightEncoder_Speed);
  error_L = 1.0*(LeftMotor_mms-LeftEncoder_Speed);

  float_t RightMotor_RPM = (RightMotor_mms + error_R) * 60.0/(WHEEL_RADIUS_MM* 2*PI);
  float_t LeftMotor_RPM  = (LeftMotor_mms  + error_L) * 60.0/(WHEEL_RADIUS_MM* 2*PI);

  int16_t RightMotor_Volt = map(RightMotor_RPM, -MOTOR_MAXSPEED_RPM, MOTOR_MAXSPEED_RPM, -255, 255);
  int16_t LeftMotor_Volt  = map(LeftMotor_RPM , -MOTOR_MAXSPEED_RPM, MOTOR_MAXSPEED_RPM, -255, 255);

  //if(RightMotor_Volt<50 && RightMotor_Volt>-50 ) { RightMotor_Volt=0; }
  //if(LeftMotor_Volt <50 && LeftMotor_Volt >-50 ) { LeftMotor_Volt=0; }

  motorR.setSpeed(RightMotor_Volt);
  motorL.setSpeed(LeftMotor_Volt);
}

#endif