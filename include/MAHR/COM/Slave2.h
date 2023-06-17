#ifndef MAHR_COM_SLAVE2_H_
#define MAHR_COM_SLAVE2_H_

#include <MAHR.h>
#include <MAHR/COM.h>

master2_msgs master2_data;

int32_t slave2_channel;   // WiFi Channel of Slave2

/**
 * @brief Call-back function when any recviced data arrive to Salve2
 *
 * @param   mac_addr        the MAC address whose want to send to it
 * @param   incomingData    recviced data
 * @param   len             length of data
 */
void Slave2_OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {  
  if(Match_MAC(mac_addr, Master_Address)){
    memcpy(&master2_data, incomingData, sizeof(master2_data));
    armX_direction  = master2_data.armX_direction;
    armY_direction  = master2_data.armY_direction;
    pitch_direction = master2_data.pitch_direction;
    roll_direction  = master2_data.roll_direction;
    grip_direction  = master2_data.grip_direction;
  }
}

/**
 * @brief Communication Setup of Slave2
 *
 * @param   ssid        Network SSID
 */
void COM_Slave2Setup(const char* ssid) {
  WiFi.mode(WIFI_STA);    // Set the device as a Wi-Fi Station

  // Set WiFi Channel:
  slave2_channel = getWiFiChannel(ssid);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(slave2_channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  
  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(Slave2_OnDataRecv); // Call-back
}

/**
 * @brief Send to the data others
 */
void COM_Slave2Update() {
  // do nothing
}


#endif