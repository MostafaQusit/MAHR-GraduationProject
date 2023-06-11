#include <Arduino.h>
#include <MAHR.h>

#include <MAHR/COM/Master.h>
#include <MAHR/PS4_Controller.h>
//#include <MAHR/GSM.h>
//#include <MAHR/Ultrasonics.h>

void setup() {
  Serial.begin(115200);
  while(!Serial){}

  //GSM_Setup();
  //PS4_Setup();
  Master_Setup("Koset", "h9f16306"); // ("WE_F6AE4C", "lcw04660");
  //PS4_Reconnect();
}

void loop() {
  //Ultrasonics_DataUpdate();
  //Ultrasonics_ObstacleAvoid();
  // print data:
  //Serial.printf("10\tMotors:  Speed = %3.2f,  linear = %3.2f,  angular = %3.2f\n", Speed, motors_linear, motors_angular);

  //GSM_CheckIncoming();
  //PS4_DataUpdate();
  //PS4_PrintData();
  //App_DataUpdate();
  Master_dataUpdate();
}
