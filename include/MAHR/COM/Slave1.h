#ifndef MAHR_COM_SLAVE1_H_
#define MAHR_COM_SLAVE1_H_

#include <MAHR.h>
#include <MAHR/COM.h>

master1_msgs master1_data;
slave1_msgs slave1_data;

// MAC Addresses of the receivers
uint8_t Master_Address[] = {0x58, 0xBF, 0x25, 0x81, 0xA0, 0x10};
int32_t slave1_channel;

void Slave1_OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {  
  if(Match_MAC(mac_addr, Master_Address)){
    memcpy(&master1_data, incomingData, sizeof(master1_data));
    Target_LeftMotor_mms  = master1_data.LeftSpeed;
    Target_RightMotor_mms = master1_data.RightSpeed;
    zAxis_Speed = master1_data.zSpeed;
    voice_file = master1_data.vFile;
  }
}
void ESPNOW_Send(const uint8_t *mac_addr, const uint8_t *data, size_t len){
  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, mac_addr, 6);
  peerInfo.channel = slave1_channel;
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

// Slave1 Initialization
void Slave1_Setup(const char* ssid) {
  // Set device as a Wi-Fi Station and set channel
  WiFi.mode(WIFI_STA);
  slave1_channel = getWiFiChannel(ssid);

  //WiFi.printDiag(Serial); // Uncomment to verify channel number before
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(slave1_channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  //WiFi.printDiag(Serial); // Uncomment to verify channel change after

  //WiFi.disconnect();
  
  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(Slave1_OnDataRecv);

  /* Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, Master_Address, 6);
  peerInfo.channel = slave1_channel;
  peerInfo.encrypt = false;
          
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  */
}
// Send to/receive from the Master
void Slave1_DataUpdate() {
  slave1_data.LeftPosition  = LeftEncoder_Distance;
  slave1_data.RightPosition = RightEncoder_Distance;
  ESPNOW_Send(Master_Address, (const uint8_t *) &slave1_data, sizeof(slave1_data));
}

#endif