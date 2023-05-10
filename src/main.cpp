#include <Arduino.h>
#include <MAHR.h>
#define ESP32_MASTER
//#define ESP32_SLAVE1

#ifdef ESP32_MASTER
  #include <MAHR/SPI/Master.h>
  //#include <MAHR/IMU.h>
  #include <MAHR/mobile_app.h>
  //#include <MAHR/GSM.h>
  //#include <MAHR/ROS.h>
#endif

#ifdef ESP32_SLAVE1
  #include <MAHR/SPI/Slave1.h>
  #include <MAHR/Motors.h>
  #include <MAHR/zAxis_Stepper.h>
  //#include <MAHR/MP3.h>
#endif

void setup() {
  Serial.begin(115200);
  while(!Serial){}

  #ifdef ESP32_MASTER
    SPIMaster_Setup();
    //IMU_Setup();
    App_Setup("WE_F6AE4C", "lcw04660"); // ("Koset", "h9f16306");
    //GSM_Setup();
    //ROS_Setup(57600);
  #endif

  #ifdef ESP32_SLAVE1
    SPISlave1_Setup();
    zAxis_Setup(1000, 2000);
    Motors_Setup();
    //Mp3_Setup();
  #endif
}

void loop() {
  #ifdef ESP32_MASTER
    voice_file = 5;
    SPIMaster_DataUpdate();
    //IMU_DataUpdate();
    //IMU_PrintData();
    App_DataUpdate();
    // print data:
    Serial.printf("Motors: Speed(%4d,%4d)\tEncoders: Position(%8lld,%8lld)deg\n",
                  Target_LeftMotor_mms,
                  Target_RightMotor_mms,
                  LeftEncoder_Distance_filtered,
                  RightEncoder_Distance_filtered);
    
    //GSM_CheckIncoming();
    //ROS_DataUpdate();
  #endif

  #ifdef ESP32_SLAVE1
    SPISlave1_DataUpdate();
    Serial.print(zAxis_Speed);  Serial.print("\t");
    Motors_PrintData();
    Motors_RunSpeed();   
    zAxis_Move();
    //Mp3_StateUpdate();
  #endif

}
