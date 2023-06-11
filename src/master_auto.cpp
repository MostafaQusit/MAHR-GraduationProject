#include <Arduino.h>
#include <MAHR.h>

#include <MAHR/COM/Master.h>
#include <MAHR/ROS.h>
//#include <MAHR/GSM.h>
//#include <MAHR/Ultrasonics.h>

void setup() {
  Serial.begin(115200);
  while(!Serial){}

  //GSM_Setup();
  Master_Setup("WE_F6AE4C", "lcw04660"); // ("Koset", "h9f16306");
  ROS_Setup(57600);
}

void loop() {
  //Ultrasonics_DataUpdate();
  //Ultrasonics_ObstacleAvoid();
  // print data:
  //Serial.printf("10\tMotors:  Speed = %3.2f,  linear = %3.2f,  angular = %3.2f\n", Speed, motors_linear, motors_angular);

  //GSM_CheckIncoming();
  ROS_SendData();
  ROS_ReceiveData();
  Master_dataUpdate();
}
