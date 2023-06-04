#ifndef MAHR_COM_H_
#define MAHR_COM_H_

#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

typedef struct master1_messages {
  float_t linear;
  float_t angular;
  int16_t zSpeed;
  int16_t vFile;
} master1_msgs;

typedef struct slave1_messages {
  int64_t RightPosition;
  int64_t LeftPosition;
} slave1_msgs;

typedef struct master2_messages {
  float_t grip_speed;
  float_t wrist_speed;
  float_t roll_speed;
  float_t armX, armY;
} master2_msgs;

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

void ESPNOW_Send(uint8_t channel, const uint8_t *mac_addr, const uint8_t *data, size_t len){
  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, mac_addr, 6);
  peerInfo.channel = channel;
  peerInfo.encrypt = false;

  if (!esp_now_is_peer_exist(mac_addr)) {
    esp_now_add_peer(&peerInfo);
  }
  esp_err_t result = esp_now_send(mac_addr, data, len); // Send message
  /* Print results to serial monitor
  if      (result == ESP_OK                  ) {Serial.println("Broadcast message success");}
  else if (result == ESP_ERR_ESPNOW_NOT_INIT ) {Serial.println("ESP-NOW not Init.");}
  else if (result == ESP_ERR_ESPNOW_ARG      ) {Serial.println("Invalid Argument");}
  else if (result == ESP_ERR_ESPNOW_INTERNAL ) {Serial.println("Internal Error");}
  else if (result == ESP_ERR_ESPNOW_NO_MEM   ) {Serial.println("ESP_ERR_ESPNOW_NO_MEM");}
  else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {Serial.println("Peer not found.");}
  else                                         {Serial.println("Unknown error");}
  */
}

#endif