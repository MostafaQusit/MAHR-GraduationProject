#include <Arduino.h>
#include <MAHR.h>
#define ESP32_MASTER
#define ESP32_SLAVE1

#ifdef ESP32_MASTER
  //#include <MAHR/SPI/Master.h>
  //#include <MAHR/IMU.h>
  //#include <MAHR/ROS.h>
  //#include <MAHR/PS4_Controller.h>
  #include <MAHR/mobile_app.h>
  //#include <MAHR/GSM.h>
#endif

#ifdef ESP32_SLAVE1
  //#include <MAHR/SPI/Slave1.h>
  //#include <MAHR/Encoders.h>
  //#include <MAHR/Motors.h>
  //#include <MAHR/zAxis_Stepper.h>
  //#include <MAHR/MP3.h>
#endif

void setup() {
  Serial.begin(115200);
  while(!Serial){}

  #ifdef ESP32_MASTER
    //IMU_Setup();
    //Encoders_Setup();
    //ROS_Setup(57600);
    //PS4_Setup();
    //App_Setup("WE_F6AE4C", "lcw04660");
    App_Setup("Koset", "h9f16306");
  #endif

  #ifdef ESP32_SLAVE1
    //zAxis_Setup(1000, 2000);
    //Motors_Setup();
    //Mp3_Setup();
  #endif
}

void loop() {
  #ifdef ESP32_MASTER
    //IMU_DataUpdate();
    //Encoders_PrintData();
    //Encoders_DataUpdate();
    App_DataUpdate();
    //ROS_DataUpdate();
  #endif

  #ifdef ESP32_SLAVE1
    //Motors_SetSpeed(LeftMotor_Speed,RightMotor_Speed);   // true -> digitalRead(LOWER_LS)
    Serial.printf("Motors: (%4d,%4d)\t\n", LeftMotor_Speed, RightMotor_Speed);
    //else { Motors_SetSpeed(0,0); }
    //zAxis_Move();
    //Mp3_StateUpdate();
  #endif
}
