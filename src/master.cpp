#include <Arduino.h>
#include <MAHR.h>

#include <MAHR/COM/Master.h>
#include <MAHR/ROS.h>
//#include <MAHR/GSM.h>
#include <MAHR/Ultrasonics.h>
#include <MAHR/PS4_Controller.h>

void setup() {
  Serial.begin(115200);
  while(!Serial){}

  //GSM_Setup();
  //SerialBT.begin("ESP32-Master"); // Bluetooth device name
  PS4_Setup();
  COM_MasterSetup("WE_F6AE4C", "lcw04660"); // ("Koset", "h9f16306");
  ROS_Setup(57600);
}

void loop() {
  PS4_ModeUpdate();
  Ultrasonics_DataUpdate();
  //Ultrasonics_ObstacleAvoid();
  //GSM_CheckIncoming();
  ROS_SendData();
  if(robot_mode == AUTONO_MODE) {ROS_CheckIncoming();}
  else                          {PS4_DataUpdate();   }
  COM_MasterUpdate();
  // print data:
  //Serial.printf("10\tMotors: linear = %3.2f,  angular = %3.2f\n", motors_linear, motors_angular);
  //SerialBT.printf("%d,  x: %3.2f,  z: %3.2f\n", robot_mode, motors_linear, motors_angular);
}
