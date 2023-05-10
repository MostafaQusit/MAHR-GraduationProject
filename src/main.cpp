#include <Arduino.h>
#include <MAHR.h>
//#define ESP32_MASTER
#define ESP32_SLAVE1

#ifdef ESP32_MASTER
#include <MAHR/IMU.h>
#include <MAHR/COM/Master.h>
//#include <MAHR/GSM.h>
#include <MAHR/ROS.h>

void setup() {
  Serial.begin(115200);
  while(!Serial){}

  IMU_Setup();
  Master_Setup("WE_F6AE4C", "lcw04660"); // ("Koset", "h9f16306");
  //GSM_Setup();
  ROS_Setup(57600);
}

void loop() {
  IMU_DataUpdate();
  //IMU_PrintData();
  Master_dataUpdate();
  /* print data:
  Serial.printf("Motors: Speed(%4d,%4d)\tEncoders: Position(%8lld,%8lld)deg\n",
                Target_LeftMotor_mms,
                Target_RightMotor_mms,
                LeftEncoder_Distance,
                RightEncoder_Distance);
  */
  //GSM_CheckIncoming();
  ROS_DataUpdate();
}
#endif


#ifdef ESP32_SLAVE1
#include <MAHR/COM/Slave1.h>
#include <MAHR/Motors.h>
#include <MAHR/zAxis_Stepper.h>
//#include <MAHR/MP3.h>

void setup() {
  Serial.begin(115200);
  while(!Serial){}

  Slave1_Setup("WE_F6AE4C");
  zAxis_Setup(1000, 2000);
  Motors_Setup();
  //Mp3_Setup();
}

void loop() {
  Slave1_DataUpdate();
  //Serial.print(zAxis_Speed);  Serial.print("\t");
  //Motors_PrintData();
  Motors_RunSpeed();   
  zAxis_Move();
  //Mp3_StateUpdate();
}
#endif