#include <Arduino.h>
#include <MAHR.h>

#include <MAHR/COM/Master.h>
#include <MAHR/ROS.h>
#include <MAHR/GSM.h>
#include <MAHR/Ultrasonics.h>
#include <MAHR/PS4_Controller.h>

void setup() {
  SerialBT.begin("ESP32-Master"); // Bluetooth device name
  Serial.begin(57600);
  while(!Serial){}

  GSM_Setup();
  PS4_Setup();
  
  COM_MasterSetup("WE_F6AE4C", "lcw04660"); // ("Koset", "h9f16306");
  ROS_Setup(57600);
}

void loop() {
  Ultrasonics_DataUpdate();
  Ultrasonics_ObstacleAvoid();
  GSM_CheckIncoming();
  ROS_SendData();
  if(robot_mode == AUTONO_MODE){
    ROS_CheckIncoming();
  }
  SerialBT.printf("%d,  l:%3.2f,  a:%3.2f\n", robot_mode, motors_linear, motors_angular);
  COM_MasterUpdate();

  // print data:
  //Serial.printf("10\tMotors:  Speed = %3.2f,  linear = %3.2f,  angular = %3.2f\n", Speed, motors_linear, motors_angular);
}
