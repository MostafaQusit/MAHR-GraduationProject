#ifndef MAHR_COM_H_
#define MAHR_COM_H_

#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

// MAC Addresses of the devices:
uint8_t Master_Address[] = {0x58, 0xBF, 0x25, 0x81, 0xC3, 0x10}; // MAC Address of Master ESP32
uint8_t Slave1_Address[] = {0x58, 0xBF, 0x25, 0x81, 0xEA, 0xF0}; // MAC Address of Slave1 ESP32
uint8_t Slave2_Address[] = {0x58, 0xBF, 0x25, 0x81, 0xDA, 0xA8}; // MAC Address of Slave2 ESP32
uint8_t Slave3_Address[] = {0x58, 0xBF, 0x25, 0x81, 0xDA, 0xA8}; // MAC Address of Slave3 ESP-CAM

// the Structure of travelling messages bet. the devices
typedef struct master1_messages {   // Struct message of Master to Salve1
  float_t linear;
  float_t angular;
  float_t zDir;
  int16_t vFile;
  uint8_t mode;
} master1_msgs;

typedef struct slave1_messages {    // Struct message of Slave1 to Master
  int64_t RightPosition;
  int64_t LeftPosition;
} slave1_msgs;

typedef struct master2_messages {   // Struct message of Master to Salve2
  float_t armX_direction;
  float_t armY_direction;
  float_t pitch_direction;
  float_t roll_direction;
  float_t grip_direction;
} master2_msgs;

/**
 * @brief    Check if the 2 MAC Adresses are identical or not
 *
 * @param    mac_1   first  MAC Address
 * @param    mac_2   second MAC Address
 *
 * @return   true(1) if the MAC Addresses are identical, false(0) if not
 */
bool Match_MAC(const uint8_t* mac_1, const uint8_t* mac_2){
  for(uint8_t i=0; i<sizeof(mac_1); i++){
    if(mac_1[i] != mac_2[i]){
      return 0;
    }
  }
  return 1;
}

/**
 * @brief    get the used WiFi Channel for the communication
 *
 * @param    ssid      Network SSID
 *
 * @return   the channel that used channel for communicate
 */
int32_t getWiFiChannel(const char* ssid) {
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

/**
 * @brief    Send ESPNOW data
 *
 * @param    channel   the used channel for communicate
 * @param    mac_addr  the MAC address whose want to send to it
 * @param    data      data to send
 * @param    len       length of data
 *
 * @return
 *           - ESP_OK:                    succeed
 *           - ESP_ERR_ESPNOW_NOT_INIT:   ESPNOW is not initialized
 *           - ESP_ERR_ESPNOW_ARG:        invalid argument
 *           - ESP_ERR_ESPNOW_INTERNAL:   internal error
 *           - ESP_ERR_ESPNOW_NO_MEM:     out of memory
 *           - ESP_ERR_ESPNOW_NOT_FOUND:  peer is not found
 *           - ESP_ERR_ESPNOW_IF:         current WiFi interface doesn't match that of peer
 */
esp_err_t ESPNOW_Send(uint8_t channel, const uint8_t* mac_addr, const uint8_t* data, size_t len){
  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, mac_addr, 6);
  peerInfo.channel = channel;
  peerInfo.encrypt = false;

  if (!esp_now_is_peer_exist(mac_addr)) {
    esp_now_add_peer(&peerInfo);
  }
  esp_err_t result = esp_now_send(mac_addr, data, len); // Send message
  return result;
}

#endif