#include <Arduino.h>
#include <MAHR.h>
#define ESP32_MASTER
//#define ESP32_SLAVE1
//#define TEST


#ifdef ESP32_MASTER
//#include <MAHR/IMU.h>
#include <MAHR/Ultrasonics.h>
#include <MAHR/COM/Master.h>
//#include <MAHR/GSM.h>
#include <MAHR/ROS.h>

void setup() {
  Serial.begin(115200);
  while(!Serial){}

  //IMU_Setup();
  Master_Setup("WE_F6AE4C", "lcw04660"); // ("Koset", "h9f16306");
  //GSM_Setup();
  ROS_Setup(57600);
}

void loop() {
  //IMU_DataUpdate();
  //IMU_PrintData();
  //Ultrasonics_DataUpdate();
  //Ultrasonics_ObstacleAvoid();
  /* print data:
  Serial.printf("Motors: Speed(%4d,%4d)\tEncoders: Position(%8lld,%8lld)deg\n",
                Target_LeftMotor_mms,
                Target_RightMotor_mms,
                LeftEncoder_Distance,
                RightEncoder_Distance);
  */
  //GSM_CheckIncoming();
  ROS_SendData();
  ROS_ReceiveData();
  Master_dataUpdate();
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
  Encoders_DataUpdate();
  Slave1_DataUpdate();
  //Serial.print(zAxis_Speed);  Serial.print("\t");
  Motors_PrintData();
  Motors_RunSpeed();   
  zAxis_Move();
  //Mp3_StateUpdate();
}
#endif

#ifdef TEST
//#include <MAHR/MP3.h>
#include <MAHR/GSM.h>

void setup(){
  Serial.begin(115200);
  while(!Serial){}

  GSM_Setup();
  //Mp3_Setup();
  //mp3.playFolder(1,1);
  GSM_SendSMS(koskot_phone,"gsm test");
  GSM_MakeCall(attia_phone);
  //mp3.start();
}

void loop(){
  GSM_CheckIncoming();
  //Mp3_StateUpdate();
  //Serial.print(mp3.currentMode());
  //Serial.print(mp3.isPlaying()? "\tplaying...\t" : "\tnot playing\t");
  //mp3.printError();
}
#endif