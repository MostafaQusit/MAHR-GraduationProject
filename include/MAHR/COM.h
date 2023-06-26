#ifndef MAHR_COM_H_
#define MAHR_COM_H_

#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

#define CHANNEL         11      // Wi-Fi Channel in ESPNOW case only

int32_t Channel;    // WiFi Channel
esp_now_peer_info_t peerInfo;

// MAC Addresses of the devices:
uint8_t Master_Address[] = {0x58, 0xBF, 0x25, 0x81, 0xC3, 0x10}; // MAC Address of Master ESP32
uint8_t Slave1_Address[] = {0x58, 0xBF, 0x25, 0x81, 0xDA, 0xA8}; // MAC Address of Slave1 ESP32
uint8_t Slave2_Address[] = {0x58, 0xBF, 0x25, 0x81, 0xEA, 0xF0}; // MAC Address of Slave2 ESP32

typedef struct master1_messages { // Struct message of Master to Salve1
  float_t linear;
  float_t angular;
  float_t zDir;
  float_t zPos;
  int16_t vFile;
  uint8_t mode;
} master1_msgs;

typedef struct slave1_messages {    // Struct message of Slave1 to Master
  int64_t RightPosition;
  int64_t LeftPosition;
} slave1_msgs;

typedef struct master2_messages {   // Struct message of Master to Salve2
  float_t arm_dir[5];
  float_t arm_ang[5];
  float_t mode;
} master2_msgs;

/**
 * @brief    get the used WiFi Channel for the communication
 * 
 * @attention   this function used in slave setup in case of use (WiFi + ESPNOW) together
 *
 * @param    ssid      Network SSID
 *
 * @return   the channel that used channel for communicate
 */
int32_t getWiFiChannel(const char* ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
      for (int32_t i=0; i<n; i++) {
          if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
              return WiFi.channel(i);
          }
      }
      Serial.println("Done scanning");
  }
  return 0;
}

#endif