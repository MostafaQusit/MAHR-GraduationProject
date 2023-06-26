#include <Arduino.h>
#include <MAHR.h>

#include <MAHR/COM/Master.h>
#include <MAHR/ROS.h>
#include <MAHR/GSM.h>
#include <MAHR/PS4_Controller.h>
#include <MAHR/Ultrasonics.h>

void setup() {
  Serial.begin(57600);
  while(!Serial){}

  //GSM_Setup();
  PS4_Setup();
  COM_MasterSetup(HOME2_SSID, HOME2_PASS);
  ROS_Setup(57600);
  voice_file = INTRODUCTION;
  delay(100);
}

void loop() {
  delay(5);
  PS4_OptionsUpdate();
  ROS_SendData();
  Ultrasonics_DataUpdate();
  Ultrasonics_PrintData();
  //GSM_CheckIncoming();
  if(robot_mode == AUTONO_MODE) {
    ROS_CheckIncoming();
  }
  else {
    PS4_DataUpdate();
    //Ultrasonics_ObstacleAvoid();
  }
  
  if(control_mode == ARM_MODE) {ESPNOW2_Send();}
  else                         {ESPNOW1_Send();}
}