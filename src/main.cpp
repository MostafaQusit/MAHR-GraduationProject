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
  #include <MAHR/Motors.h>
  //#include <MAHR/zAxis_Stepper.h>
  //#include <MAHR/MP3.h>
#endif

void setup() {
  Serial.begin(115200);
  while(!Serial){}

  #ifdef ESP32_MASTER
    //IMU_Setup();
    //ROS_Setup(57600);
    //PS4_Setup();
    App_Setup("WE_F6AE4C", "lcw04660");
    //App_Setup("Koset", "h9f16306");
  #endif

  #ifdef ESP32_SLAVE1
    //zAxis_Setup(1000, 2000);
    Motors_Setup();
    //Mp3_Setup();
  #endif
}

void loop() {
  #ifdef ESP32_MASTER
    //IMU_DataUpdate();
    //IMU_PrintData();
    App_DataUpdate();
    //ROS_DataUpdate();
  #endif

  #ifdef ESP32_SLAVE1
    Motors_RunSpeed(LeftMotor_Speed,RightMotor_Speed);   // true -> digitalRead(LOWER_LS)
    /*
    Serial.printf("Speed(%4d,%4d)\tzAxis(%5d)\tArm(%4d,%4d)\troll(%4d)\twrist(%4d)\tGrip(%4d)\n",
                  LeftMotor_Speed, RightMotor_Speed,
                  zAxis_Speed,
                  armX, armY,
                  roll, wrist, Grip);
    */
    Encoders_PrintData();
    //else { Motors_SetSpeed(0,0); }
    //zAxis_Move();
    //Mp3_StateUpdate();
  #endif

}
