#ifndef MAHR_COM_SLAVE2_H_
#define MAHR_COM_SLAVE2_H_

#include <MAHR.h>
#include <MAHR/COM.h>

master2_msgs master2_data;

// MAC Addresses of the receivers
int32_t slave2_channel;

void Slave2_OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {  
  if(Match_MAC(mac_addr, Master_Address)){
    memcpy(&master2_data, incomingData, sizeof(master2_data));
    roll_speed  = master2_data.roll_speed;
    grip_speed  = master2_data.grip_speed;
    pitch_speed = master2_data.pitch_speed;
    armX        = master2_data.armX;
    armY        = master2_data.armY;
  }
}

// Slave2 Initialization
void Slave2_Setup(const char* ssid) {
  // Set device as a Wi-Fi Station and set channel
  WiFi.mode(WIFI_STA);
  slave2_channel = getWiFiChannel(ssid);

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(slave2_channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  
  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(Slave2_OnDataRecv);
}
// Send to/receive from the Master
void Slave2_DataUpdate() {
    // do nothing...
}

#endif