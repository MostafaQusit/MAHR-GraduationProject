#ifndef MAHR_COM_SLAVE1_H_
#define MAHR_COM_SLAVE1_H_

#include <MAHR.h>
#include <MAHR/COM.h>

#define LINEAR_LIMIT  0.60
#define ANGULAR_LIMIT (LINEAR_LIMIT*2.0)

master1_msgs master1_data;
slave1_msgs slave1_data;

// MAC Addresses of the receivers
int32_t slave1_channel;

void Slave1_OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {  
  if(Match_MAC(mac_addr, Master_Address)){
    memcpy(&master1_data, incomingData, sizeof(master1_data));
    motors_linear  = master1_data.linear;
    motors_angular = master1_data.angular;

    // linear & angular Range:
    if(motors_linear >0 && motors_linear <  LINEAR_LIMIT) { motors_linear  =   LINEAR_LIMIT; }
    if(motors_linear <0 && motors_linear >- LINEAR_LIMIT) { motors_linear  = - LINEAR_LIMIT; }

    //if(motors_angular>0 && motors_angular< ANGULAR_LIMIT) { motors_angular =  ANGULAR_LIMIT; }
    //if(motors_angular<0 && motors_angular>-ANGULAR_LIMIT) { motors_angular = -ANGULAR_LIMIT; }

    if     (motors_angular> 0.2 || (abs(master1_data.linear)<0.1 && motors_angular>0.0)) { motors_angular =  0.60;  motors_linear  = 0.00; }
    else if(motors_angular<-0.2 || (abs(master1_data.linear)<0.1 && motors_angular<0.0)) { motors_angular = -0.60;  motors_linear  = 0.00; }
    else                                                                                 { motors_angular =  0.00; }
    Required_LeftMotor_mms  = 300.0 * (motors_linear - motors_angular);
    Required_RightMotor_mms = 300.0 * (motors_linear + motors_angular);

    /* from abs(0:300) to abs(0 & 90:300)
    if(Required_RightMotor_mms>0 && Required_RightMotor_mms< 90) { Required_RightMotor_mms =  90; }
    if(Required_RightMotor_mms<0 && Required_RightMotor_mms>-90) { Required_RightMotor_mms = -90; }

    if( Required_LeftMotor_mms>0 &&  Required_LeftMotor_mms< 90) {  Required_LeftMotor_mms =  90; }
    if( Required_LeftMotor_mms<0 &&  Required_LeftMotor_mms>-90) {  Required_LeftMotor_mms = -90; }
    */
    // Offset:
    //if(Required_LeftMotor_mms  > 0) { Required_LeftMotor_mms  += 60.0; }
    //else                            { Required_LeftMotor_mms  -= 60.0; }
    //if(Required_RightMotor_mms > 0) { Required_RightMotor_mms += 60.0; }
    //else                            { Required_RightMotor_mms -= 60.0; }
    
    zAxis_Speed = master1_data.zSpeed;
    voice_file = master1_data.vFile;
  }
}

// Slave1 Initialization
void Slave1_Setup(const char* ssid) {
  // Set device as a Wi-Fi Station and set channel
  WiFi.mode(WIFI_STA);
  slave1_channel = getWiFiChannel(ssid);

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(slave1_channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  
  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(Slave1_OnDataRecv);
}
// Send to/receive from the Master
void Slave1_DataUpdate() {
  slave1_data.LeftPosition  = LeftEncoder_Distance;
  slave1_data.RightPosition = RightEncoder_Distance;
  ESPNOW_Send(slave1_channel, Master_Address, (const uint8_t *)&slave1_data, sizeof(slave1_data));
}

#endif