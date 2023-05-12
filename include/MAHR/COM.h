#ifndef MAHR_COM_H_
#define MAHR_COM_H_

#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

typedef struct master1_messages {
  float_t RightSpeed;
  float_t LeftSpeed;
  int16_t zSpeed;
  int16_t vFile;
} master1_msgs;

typedef struct slave1_messages {
  int64_t RightPosition;
  int64_t LeftPosition;
} slave1_msgs;


bool Match_MAC(const uint8_t *mac_1, const uint8_t *mac_2){
  for(uint8_t i=0; i<sizeof(mac_1); i++){
    if(mac_1[i] != mac_2[i]){
      return 0;
    }
  }
  return 1;
}

int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
      for (uint8_t i=0; i<n; i++) {
          if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
              return WiFi.channel(i);
          }
      }
      Serial.println("Done scanning");
  }
  return 0;
}

#endif