#include <Arduino.h>
#include <MAHR.h>

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