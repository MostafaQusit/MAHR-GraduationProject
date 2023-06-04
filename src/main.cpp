#include <Arduino.h>
#include <MAHR.h>
//#define ESP32_MASTER
#define ESP32_SLAVE1
//#define TEST
//#define TEST2

#ifdef ESP32_MASTER
//#include <MAHR/Ultrasonics.h>
#include <MAHR/COM/Master.h>
//#include <MAHR/GSM.h>
//#include <MAHR/ROS.h>

void setup() {
  Serial.begin(115200);
  while(!Serial){}

  Master_Setup("WE_F6AE4C", "lcw04660"); // ("Koset", "h9f16306");
  //GSM_Setup();
  //ROS_Setup(57600);
}

void loop() {
  //Ultrasonics_DataUpdate();
  //Ultrasonics_ObstacleAvoid();
  // print data:
  Serial.printf("10\t\tMotors: Speed(%4.0f,%4.0f)\n", Required_LeftMotor_mms, Required_RightMotor_mms);
  
  //GSM_CheckIncoming();
  //ROS_SendData();
  //ROS_ReceiveData();
  Master_dataUpdate();
}
#endif

#ifdef ESP32_SLAVE1
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
#endif

#ifdef TEST
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <AsyncElegantOTA.h>
#include <ESPAsyncWebServer.h>
AsyncWebServer AsyncServer(80);

void setup(){
  Serial.begin(115200);
  while(!Serial){}
  AsyncServer.begin();
  AsyncElegantOTA.begin(&AsyncServer);}

void loop(){
  // nothing.
}
#endif

#ifdef TEST2
#include <ESP32Servo.h>

Servo myservo;  // create servo object to control a servo
// 16 servo objects can be created on the ESP32

int pos = 0;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33
int servoPin = 27;

void setup() {
	// Allow allocation of all timers
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	myservo.setPeriodHertz(50);    // standard 50 hz servo
	myservo.attach(servoPin, 500, 2500); // attaches the servo on pin 18 to the servo object
	// using default min/max of 1000us and 2000us
	// different servos may require different min/max settings for an accurate 0 to 180 sweep
}

void loop() {
	for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
		// in steps of 1 degree
		myservo.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}
	for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
		myservo.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}
}
#endif