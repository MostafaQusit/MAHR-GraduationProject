#include <Arduino.h>
#include <MAHR.h>

#include <MAHR/Arm.h>
#include <MAHR/COM/Slave2.h>

void setup() {
  Serial.begin(115200);
  while(!Serial){}

  Arm_Setup(3000, 1000);
  COM_Slave2Setup();  
}

void loop() {
  Arm_AnglesUpdate();
  Arm_RunToPosition();
  Arm_PrintData();
}