#include <Arduino.h>
#include <MAHR.h>
#define ESP32_MASTER
#define ESP32_SLAVE1

#ifdef ESP32_MASTER
//#include <MAHR/SPI/Master.h>
//#include <MAHR/IMU.h>
//#include <MAHR/mobile_app.h>
#include <MAHR/COM/Master.h>
//#include <MAHR/GSM.h>
//#include <MAHR/ROS.h>

void setup() {
  Serial.begin(115200);
  while(!Serial){}

  //SPIMaster_Setup();
  //IMU_Setup();
  Master_Setup("WE_F6AE4C", "lcw04660"); // ("Koset", "h9f16306");
  //App_Setup("WE_F6AE4C", "lcw04660"); // ("Koset", "h9f16306");
  //GSM_Setup();
  //ROS_Setup(57600);
}

void loop() {
  voice_file = 5;
  //SPIMaster_DataUpdate();
  //IMU_DataUpdate();
  //IMU_PrintData();
  Master_dataUpdate();
  //App_DataUpdate();
  /* print data:
  Serial.printf("Motors: Speed(%4d,%4d)\tEncoders: Position(%8lld,%8lld)deg\n",
                Target_LeftMotor_mms,
                Target_RightMotor_mms,
                LeftEncoder_Distance_filtered,
                RightEncoder_Distance_filtered);
  */
  //GSM_CheckIncoming();
  //ROS_DataUpdate();
}
#endif


#ifdef ESP32_SLAVE1
//#include <MAHR/SPI/Slave1.h>
#include <MAHR/COM/Slave1.h>
#include <MAHR/Motors.h>
#include <MAHR/zAxis_Stepper.h>
//#include <MAHR/MP3.h>

void setup() {
  Serial.begin(115200);
  while(!Serial){}

  //SPISlave1_Setup();
  Slave1_Setup();
  zAxis_Setup(1000, 2000);
  Motors_Setup();
  //Mp3_Setup();
}

void loop() {
  //SPISlave1_DataUpdate();
  Slave1_DataUpdate();
  //Serial.print(zAxis_Speed);  Serial.print("\t");
  //Motors_PrintData();
  //Motors_RunSpeed();   
  //zAxis_Move();
  //Mp3_StateUpdate();
}
#endif