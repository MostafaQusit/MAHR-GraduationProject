#ifndef MAHR_COM_SLAVE1_H_
#define MAHR_COM_SLAVE1_H_

#include <MAHR.h>
#include <MAHR/COM.h>

master1_msgs master1_data;
slave1_msgs slave1_data;

/**
 * @brief Call-back function when any recviced data arrive to Salve1
 *
 * @param   mac_addr        the MAC address whose want to send to it
 * @param   incomingData    recviced data
 * @param   len             length of data
 */
void Slave1_OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {  
  if(match_array<const uint8_t>(mac_addr, Master_Address, 6)){
    memcpy(&master1_data, incomingData, sizeof(master1_data));
    motors_linear  = master1_data.linear;
    motors_angular = master1_data.angular;

    // linear & angular Range:
    robot_mode = master1_data.mode;
    if(robot_mode == AUTONO_MODE){
      if(motors_linear > 0 && motors_linear <  0.65) { motors_linear  =  0.65; }
      if(motors_linear < 0 && motors_linear > -0.65) { motors_linear  = -0.65; }

      if     (motors_angular >  0.1) { motors_angular =  0.65;  motors_linear  = 0.00; }
      else if(motors_angular < -0.1) { motors_angular = -0.65;  motors_linear  = 0.00; }
      else                           { motors_angular =  0.00;                         }
      zAxis_position = master1_data.zPos;
    }
    else{   // mode = MANUAL_MODE
      if(motors_linear  > 0 && motors_linear  <  0.20) { motors_linear  =  0.20; }
      if(motors_linear  < 0 && motors_linear  > -0.20) { motors_linear  = -0.20; }

      if(motors_angular > 0 && motors_angular <  0.20) { motors_angular =  0.20; }
      if(motors_angular < 0 && motors_angular > -0.20) { motors_angular = -0.20; }
      zAxis_direction = constrain(master1_data.zDir, -1.0, 1.0);
    }

    Required_LeftMotor_mms  = constrain(300.0*(motors_linear - motors_angular), -300.0, 300.0);
    Required_RightMotor_mms = constrain(300.0*(motors_linear + motors_angular), -300.0, 300.0);
    voice_file = master1_data.vFile;
  }
  else {
    Serial.print("\tError in mac\t");
  }
}

/**
 * @brief Communication Setup of Slave1
 */
void COM_Slave1Setup() {
  WiFi.mode(WIFI_STA);    // Set the device as a Wi-Fi Station
  
  // Set WiFi Channel:
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  Serial.println("done WiFi");

  //Init ESP-NOW:
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(Slave1_OnDataRecv); // Call-back

  // Register peer devices:
  memcpy(peerInfo.peer_addr, Master_Address, 6);
  peerInfo.channel = CHANNEL;
  peerInfo.encrypt = false;

  // Add peer devices:
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  Serial.println("done ESPNOW");
  delay(100);
}

/**
 * @brief Send to the data others
 */
void ESPNOW_Send() {
  slave1_data.LeftPosition  = LeftEncoder_Distance;
  slave1_data.RightPosition = RightEncoder_Distance;
  esp_now_send(Master_Address, (const uint8_t *)&slave1_data, sizeof(slave1_data));
}

#endif