#ifndef MAHR_MOTORS_H_
#define MAHR_MOTORS_H_

#include <MAHR.h>
#include <CytronMotorDriver.h>
#include <PID_v1.h>

#define MOTOR_MAXSPEED_RPM   60.0   // Motor max. speed in rpm

CytronMD motorR(PWM_DIR, MOTOR_RIGHT_PWM, MOTOR_RIGHT_DIR);   // Driver object for Right Motor
CytronMD motorL(PWM_DIR, MOTOR_LEFT_PWM , MOTOR_LEFT_DIR );   // Driver object for Left Motor
// double Setpoint_R, Input_R, Output_R;
// double Setpoint_L, Input_L, Output_L;
// PID PID_R(&Input_R, &Output_R, &Setpoint_R, 0.0, 0.0, 0.0, DIRECT);
// PID PID_L(&Input_L, &Output_L, &Setpoint_L, 0.0, 0.0, 0.0, DIRECT);

// Motors Initialization
void Motors_Setup() {
  //PID_R.SetMode(AUTOMATIC);
  //PID_L.SetMode(AUTOMATIC);
}
// Speed Control Diff. Robot by PID Controller
void Motors_SetSpeed(int16_t LeftMotor_mms, int16_t RightMotor_mms) {
  // Input_R = RightEncoder_Speed;
  // Input_L = LeftEncoder_Speed;
  // Setpoint_R = RightMotor_mms;
  //Setpoint_L = LeftMotor_mms;

  // PID_R.Compute();
  // PID_L.Compute();

  float_t RightMotor_RPM = (RightMotor_mms /*+ Output_R*/) * 60.0/(WHEEL_RADIUS_MM* 2*PI);
  float_t LeftMotor_RPM  = (LeftMotor_mms  /*+ Output_L*/) * 60.0/(WHEEL_RADIUS_MM* 2*PI);

  int16_t RightMotor_Volt = map(RightMotor_RPM, -MOTOR_MAXSPEED_RPM, MOTOR_MAXSPEED_RPM, -255, 255);
  int16_t LeftMotor_Volt  = map(LeftMotor_RPM , -MOTOR_MAXSPEED_RPM, MOTOR_MAXSPEED_RPM, -255, 255);

  motorR.setSpeed(RightMotor_Volt);
  motorL.setSpeed(LeftMotor_Volt);
}

#endif