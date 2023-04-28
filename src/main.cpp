#include <Arduino.h>
#include <MAHR.h>
#define ESP32_MASTER
#define ESP32_SLAVE1

#ifdef ESP32_MASTER
  //#include <MAHR/SPI/Master.h>
  //#include <MAHR/IMU.h>
  //#include <MAHR/PS4_Controller.h>
  #include <MAHR/mobile_app.h>
  //#include <MAHR/GSM.h>
  //#include <MAHR/ROS.h>
#endif

#ifdef ESP32_SLAVE1
  //#include <MAHR/SPI/Slave1.h>
  #include <MAHR/Motors.h>
  //#include <MAHR/zAxis_Stepper.h>
  //#include <MAHR/MP3.h>
#endif

void setup() {
  Serial.begin(115200);
  while(!Serial){}

  #ifdef ESP32_MASTER
    //SPIMaster_Setup();
    //IMU_Setup();
    //PS4_Setup();
    App_Setup("WE_F6AE4C", "lcw04660");   //("Koset", "h9f16306");
    //GSM_Setup();
    //ROS_Setup(57600);
  #endif

  #ifdef ESP32_SLAVE1
    //SPISlave1_Setup();
    //zAxis_Setup(1000, 2000);
    Motors_Setup();
    //Mp3_Setup();
  #endif
}

void loop() {
  #ifdef ESP32_MASTER
    //SPIMaster_DataUpdate();
    //IMU_DataUpdate();
    //IMU_PrintData();
    App_DataUpdate();
    //GSM_CheckIncoming();
    //ROS_DataUpdate();
  #endif

  #ifdef ESP32_SLAVE1
    //SPISlave1_DataUpdate();
    Motors_RunSpeed(LeftMotor_Speed,RightMotor_Speed);   // if(digitalRead(LOWER_LS)==HIGH) {}
    //else { Motors_SetSpeed(0,0); }
    Serial.printf("Speed(%4d,%4d),\t", LeftMotor_Speed, RightMotor_Speed);
    Encoders_PrintData();
    //zAxis_Move();
    //Mp3_StateUpdate();
  #endif

}
