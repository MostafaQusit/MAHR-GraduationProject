#include <Arduino.h>
#include <MAHR.h>

//#include <MAHR/Ultrasonics.h>
#include <MAHR/COM/Master.h>
//#include <MAHR/GSM.h>
#include <MAHR/ROS.h>

void setup() {
  Serial.begin(115200);
  while(!Serial){}

  Master_Setup("WE_F6AE4C", "lcw04660"); // ("Koset", "h9f16306");
  //GSM_Setup();
  ROS_Setup(57600);
}

void loop() {
  //Ultrasonics_DataUpdate();
  //Ultrasonics_ObstacleAvoid();
  /* print data:
  Serial.printf("10\t\tMotors: Speed(%4.0f,%4.0f)\n", Required_LeftMotor_mms, Required_RightMotor_mms);
  */
  //GSM_CheckIncoming();
  ROS_SendData();
  ROS_ReceiveData();
  Master_dataUpdate();
}
