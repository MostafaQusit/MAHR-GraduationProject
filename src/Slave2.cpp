#include <Arduino.h>
#include <MAHR.h>

#include <MAHR/Arm.h>
#include <MAHR/COM/Slave2.h>

void setup() {
  Serial.begin(115200);
  while(!Serial){}

  Arm_Setup();
  Slave2_Setup("WE_F6AE4C");
}

void loop() {
  Slave2_DataUpdate();
  Arm_run();
}
