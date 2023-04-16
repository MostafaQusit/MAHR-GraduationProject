#ifndef MAHR_ENCODERS_H_
#define MAHR_ENCODERS_H_

#include <MAHR.h>
#include <ESP32Encoder.h>
#include "esp_task_wdt.h"

#define ENCODER_PPR 360.0   // Encoder Resolution in pulse/rev.

static IRAM_ATTR void RightEncoder_cb(void* arg) {
  ESP32Encoder* enc = (ESP32Encoder*) arg;
}
static IRAM_ATTR void LeftEncoder_cb(void* arg) {
  ESP32Encoder* enc = (ESP32Encoder*) arg;
}

extern bool loopTaskWDTEnabled;
extern TaskHandle_t loopTaskHandle;
ESP32Encoder RightEncoder(true, RightEncoder_cb);             // ESP32Encoder object for Right Encoder.
ESP32Encoder LeftEncoder (true, LeftEncoder_cb );             // ESP32Encoder object for Left Encoder.
int64_t RightEncoder_PrevDistance, LeftEncoder_PrevDistance;  // Previous travelled distance of Encoders in deg.
float_t RightEncoder_degs, LeftEncoder_degs;                  // Enocoders Speed in deg/s.
uint32_t Prev_millis;
static const char *LOG_TAG = "main";

// Encoders Initialization
void Encoders_Setup() {
  loopTaskWDTEnabled = true;

  RightEncoder.attachSingleEdge(ENCODER_RIGHT_A, ENCODER_RIGHT_B);
  LeftEncoder.attachSingleEdge (ENCODER_LEFT_A , ENCODER_LEFT_B );

  RightEncoder.clearCount();
  LeftEncoder.clearCount();

  RightEncoder.setFilter(1023);
  LeftEncoder.setFilter(1023);

  esp_log_level_set("*", ESP_LOG_DEBUG);
  esp_log_level_set("main", ESP_LOG_DEBUG);
  esp_log_level_set("ESP32Encoder", ESP_LOG_DEBUG);
  esp_task_wdt_add(loopTaskHandle);
}
// Calculate the Updated Encoder Position and Speed
void Encoders_DataUpdate() {
  RightEncoder_Distance = -RightEncoder.getCount();
  LeftEncoder_Distance  = -LeftEncoder.getCount();
  /* Speed Calcu.
  RightEncoder_degs = (RightEncoder_Distance - RightEncoder_PrevDistance) / (millis() - Prev_millis);
  LeftEncoder_degs  = ( LeftEncoder_Distance -  LeftEncoder_PrevDistance) / (millis() - Prev_millis);

  RightEncoder_Speed = (int16_t) RightEncoder_degs * (PI/180.0)*WHEEL_RADIUS_MM;
  LeftEncoder_Speed = (int16_t) LeftEncoder_degs  * (PI/180.0)*WHEEL_RADIUS_MM;

  Prev_millis = millis();
  */
}
// Print the Encoder Position and Speed
void Encoders_PrintData() {
  Serial.print(LeftEncoder_Distance); Serial.print(F("\t"));   Serial.println(RightEncoder_Distance);
  /*
  Serial.printf("Encoders: Position(%6d,%6d)deg\tSpeed(%6d,%6d)\n",
                LeftEncoder_Distance,
                RightEncoder_Distance, 
                LeftEncoder_Speed, 
                RightEncoder_Speed );
  */
}

#endif