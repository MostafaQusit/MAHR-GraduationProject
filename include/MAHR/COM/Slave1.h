#ifndef MAHR_COM_SLAVE1_H_
#define MAHR_COM_SLAVE1_H_

#include <MAHR.h>
#include <MAHR/COM.h>

master1_msgs master1_data;
slave1_msgs slave1_data;

int32_t slave1_channel;   // WiFi Channel of Slave1

/**
 * @brief Call-back function when any recviced data arrive to Salve1
 *
 * @param   mac_addr        the MAC address whose want to send to it
 * @param   incomingData    recviced data
 * @param   len             length of data
 */
void Slave1_OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {  
  if(Match_MAC(mac_addr, Master_Address)){
    memcpy(&master1_data, incomingData, sizeof(master1_data));
    motors_linear  = master1_data.linear;
    motors_angular = master1_data.angular;

    // linear & angular Range:
    if(master1_data.mode == AUTONO_MODE){
      if(motors_linear > 0 && motors_linear <  0.80) { motors_linear  =  0.80; }
      if(motors_linear < 0 && motors_linear > -0.80) { motors_linear  = -0.80; }

      if     (motors_angular >  0.2) { motors_angular =  0.80;  motors_linear  = 0.00; }
      else if(motors_angular < -0.2) { motors_angular = -0.80;  motors_linear  = 0.00; }
      else                           { motors_angular =  0.00;                         }
    }
    else{
      if(motors_linear  > 0 && motors_linear  <  0.20) { motors_linear  =  0.20; }
      if(motors_linear  < 0 && motors_linear  > -0.20) { motors_linear  = -0.20; }

      if(motors_angular > 0 && motors_angular <  0.20) { motors_angular =  0.20; }
      if(motors_angular < 0 && motors_angular > -0.20) { motors_angular = -0.20; }
    }

    Required_LeftMotor_mms  = 300.0 * (motors_linear - motors_angular);
    Required_RightMotor_mms = 300.0 * (motors_linear + motors_angular);
    zAxis_direction = master1_data.zDir;
    voice_file = master1_data.vFile;
  }
  else {
    Serial.print("\tError in mac\t");
  }
}

/**
 * @brief Communication Setup of Slave1
 *
 * @param   ssid        Network SSID
 */
void COM_Slave1Setup(const char* ssid) {
  WiFi.mode(WIFI_STA);    // Set the device as a Wi-Fi Station

  // Set WiFi Channel:
  slave1_channel = getWiFiChannel(ssid);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(slave1_channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  
  //Init ESP-NOW:
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(Slave1_OnDataRecv); // Call-back
}

/**
 * @brief Send to the data others
 */
void COM_Slave1Update() {
  slave1_data.LeftPosition  = LeftEncoder_Distance;
  slave1_data.RightPosition = RightEncoder_Distance;
  ESPNOW_Send(slave1_channel, Master_Address, (const uint8_t *)&slave1_data, sizeof(slave1_data));
}

#endif