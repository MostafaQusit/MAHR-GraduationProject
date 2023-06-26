#include <Arduino.h>
#include <MAHR.h>

#include <MAHR/COM/Slave1.h>
#include <MAHR/Motors_Encoders.h>
#include <MAHR/zAxis_Stepper.h>
#include <MAHR/MP3.h>

void setup() {
  Serial.begin(115200);
  while(!Serial){}

  zAxis_Setup(1000, 500);
  Encoders_Setup();
  COM_Slave1Setup();
  Mp3_Setup(30);
}

void loop() {
  MotorsEncoders_PrintData();
  Encoders_DataUpdate();
  Motors_RunSpeed();   
  if(robot_mode == MANUAL_MODE) {zAxis_RunSpeed();}
  else                          {zAxis_RunToPosition();}
  Mp3_play(voice_file);
  ESPNOW_Send();
}