#include <Arduino.h>
#include <MAHR.h>

#define TEST1
//#define TEST2

#ifdef TEST1
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//#include "credentials.h"

IPAddress local_IP(192, 168, 1, 184); // Set your Static  IP address
IPAddress gateway(192, 168, 1, 1);    // Set your Gateway IP address
IPAddress subnet(255, 255, 0, 0);

void setup(){
  Serial.begin(115200);
  while(!Serial){}

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println(F("STA Failed to configure"));
  }

  Serial.print(F("Connecting to WiFi"));
  WiFi.mode(WIFI_AP_STA);     // Set the device as a Station and Soft Access Point simultaneously
  WiFi.begin("WE_F6AE4C", "lcw04660"); // Set device as a Wi-Fi Station
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.print("\nStation IP Address: ");   Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");        Serial.println(WiFi.channel());

  ArduinoOTA.begin(WiFi.localIP(), "Arduino", "password", InternalStorage);
}

void loop(){
  ArduinoOTA.poll();
  Serial.println(5);
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