#ifndef MAHR_COM_SLAVE2_H_
#define MAHR_COM_SLAVE2_H_

#include <MAHR.h>
#include <MAHR/COM.h>

master2_msgs master2_data;

esp_now_peer_info_t peerInfo_master;

/**
 * @brief Call-back function when any recviced data arrive to Salve2
 *
 * @param   mac_addr        the MAC address whose want to send to it
 * @param   incomingData    recviced data
 * @param   len             length of data
 */
void Slave2_OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {  
  if(match_array<const uint8_t>(mac_addr, Master_Address, 6)){
    memcpy(&master2_data, incomingData, sizeof(master2_data));
    robot_mode = master2_data.mode;
    for(uint8_t i=0; i<5; i++) {arm_directions[i] = constrain(master2_data.arm_dir[i], -1.0, 1.0);}
    for(uint8_t i=0; i<5; i++) {arm_angles    [i] = master2_data.arm_ang[i];}
  }
  else {
    Serial.print("\tError in mac\t");
  }
}

/**
 * @brief Communication Setup of Slave2
 */
void COM_Slave2Setup() {
  WiFi.mode(WIFI_STA);    // Set the device as a Wi-Fi Station
  
  // Set WiFi Channel:
  Channel = CHANNEL;
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(Channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  
  //Init ESP-NOW:
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(Slave2_OnDataRecv); // Call-back
  
  // Register peer:
  memcpy(peerInfo.peer_addr, Master_Address, 6);
  peerInfo.channel = Channel;
  peerInfo.encrypt = false;

  // Add peer:
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  delay(100);
}

/**
 * @brief Send to the data others
 */
void COM_Slave2Update() {
  // do nothing
}

#endif