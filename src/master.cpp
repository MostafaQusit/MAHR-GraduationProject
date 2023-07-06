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
  ROS_Setup(57600);
  COM_MasterSetup(HOME2_SSID, HOME2_PASS);
  voice_file = INTRODUCTION;
}

void loop() {
  delay(5);
  PS4_OptionsUpdate();
  Ultrasonics_DataUpdate();
  Ultrasonics_PrintData();
  //GSM_CheckIncoming();
  ROS_SendData();
  if(robot_mode == AUTONO_MODE) {ROS_CheckIncoming();}
  else                          {PS4_DataUpdate();  /*Ultrasonics_ObstacleAvoid();*/}
  if(control_mode == ARM_MODE) {ESPNOW2_Send();}
  else                         {ESPNOW1_Send();}
}