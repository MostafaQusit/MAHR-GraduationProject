#include <Arduino.h>
#include <MAHR.h>
#include <MAHR/IMU.h>
#include <MAHR/Encoders.h>
#include <MAHR/ROS.h>
#include <MAHR/PS4_Controller.h>
#include <MAHR/Motors.h>
#include <MAHR/zAxis_Stepper.h>

void setup() {
  Serial.begin(115200);
  while(!Serial){}

  //IMU_Setup();
  //Encoders_Setup();
  //ROS_Setup(57600);

  PS4_Setup();
  //zAxis_Setup(1000, 2000);
}

void loop() {
  //IMU_DataUpdate();
  //Encoders_DataUpdate();
  Motors_SetSpeed(LeftMotor_Speed,RightMotor_Speed);   // true -> digitalRead(LOWER_LS)
  //else { Motors_SetSpeed(0,0); }
  //ROS_DataUpdate();

  //zAxis_Move();
}

