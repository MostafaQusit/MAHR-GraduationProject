#include <Arduino.h>
#include <MAHR.h>

#include <MAHR/COM/Slave1.h>
#include <MAHR/Motors_Encoders.h>
#include <MAHR/zAxis_Stepper.h>
//#include <MAHR/MP3.h>

void setup() {
  Serial.begin(115200);
  while(!Serial){}

  COM_Slave1Setup("WE_F6AE4C");
  //zAxis_Setup(1000, 2000);
  Encoders_Setup();
  //Mp3_Setup();
}

void loop() {
  Encoders_DataUpdate();
  COM_Slave1Update();
  //Serial.print(zAxis_Speed);  Serial.print("\t");
  MotorsEncoders_PrintData();
  Motors_RunSpeed();   
  //zAxis_RunSpeed();
  //Mp3_StateUpdate();
}