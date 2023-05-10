#ifndef MAHR_COM_SLAVE1_H_
#define MAHR_COM_SLAVE1_H_

#include <MAHR.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

typedef struct master1_messages {
  int16_t RightSpeed;
  int16_t LeftSpeed;
  int16_t zSpeed;
  int16_t vFile;
} master1_msgs;

typedef struct slave1_messages {
  int64_t RightPosition;
  int64_t LeftPosition;
} slave1_msgs;

master1_msgs master1_data;
slave1_msgs slave1_data;

// MAC Addresses of the receivers
uint8_t Master_Address[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
int32_t channel = 6;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.printf("\rSent to %02x:%02x:%02x:%02x:%02x:%02x\t", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "[Delivery Success]" : "[Delivery Fail]");
}
void Slave1_OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  Serial.printf("\rSent to %02x:%02x:%02x:%02x:%02x:%02x\t", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  
  if(!strcmp((const char *)mac_addr, (const char *)Master_Address)){
    memcpy(&master1_data, incomingData, sizeof(master1_data));
    Serial.printf("Motors Speed(%4d,%4d)mm/s\tzAxis(%5d)\tvoice file(%d)\n",
                  master1_data.LeftSpeed,
                  master1_data.RightSpeed,
                  master1_data.zSpeed,
                  master1_data.vFile);
  }
}
void ESPNOW_Send(const uint8_t *mac_addr, const uint8_t *data, size_t len){
  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, mac_addr, 6);
  peerInfo.channel = channel;
  peerInfo.encrypt = false;

  if (!esp_now_is_peer_exist(mac_addr)) {
    esp_now_add_peer(&peerInfo);
  }
  esp_err_t result = esp_now_send(mac_addr, data, len); // Send message

  // Print results to serial monitor
  if      (result == ESP_OK                  ) {Serial.println("Broadcast message success");}
  else if (result == ESP_ERR_ESPNOW_NOT_INIT ) {Serial.println("ESP-NOW not Init.");}
  else if (result == ESP_ERR_ESPNOW_ARG      ) {Serial.println("Invalid Argument");}
  else if (result == ESP_ERR_ESPNOW_INTERNAL ) {Serial.println("Internal Error");}
  else if (result == ESP_ERR_ESPNOW_NO_MEM   ) {Serial.println("ESP_ERR_ESPNOW_NO_MEM");}
  else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {Serial.println("Peer not found.");}
  else                                         {Serial.println("Unknown error");}
}

// Slave1 Initialization
void Slave1_Setup() {
  // Set device as a Wi-Fi Station and set channel
  WiFi.mode(WIFI_STA);

  //WiFi.printDiag(Serial); // Uncomment to verify channel number before
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  //WiFi.printDiag(Serial); // Uncomment to verify channel change after

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(Slave1_OnDataRecv);
}
// Send to/receive from the Master
void Slave1_DataUpdate() {
  ESPNOW_Send(Master_Address, (const uint8_t *) &slave1_data, sizeof(slave1_data));
}

#endif