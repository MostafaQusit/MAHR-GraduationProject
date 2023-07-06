#ifndef MAHR_COM_MASTER_H_
#define MAHR_COM_MASTER_H_

#include <MAHR.h>
#include <MAHR/COM.h>
#include <ESPAsyncWebServer.h>

#define SEND_REQUEST (request->send(200, "text/plain", "OK"))   // response on every request from server

master1_msgs master1_data;
master2_msgs master2_data;
slave1_msgs slave1_data;

AsyncWebServer server(80);          // Asynchronous WebServer Object
AsyncEventSource events("/events"); // Event Object

// HTML Page
const char html_page[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP-MASTER DASHBOARD</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p {  font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #2f4468; color: white; font-size: 1.7rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 700px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); }
    .reading { font-size: 2.8rem; }
    .packet { color: #bebebe; }
    .card.temperature { color: #fd7e14; }
    .card.humidity { color: #1b78e2; }
  </style>
</head>
<body>
  <div class="topnav">
    <h3>ESP-NOW DASHBOARD</h3>
  </div>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);
}
</script>
</body>
</html>)rawliteral";

IPAddress local_IP(192, 168, 1, 184); // Set your Static  IP address
IPAddress gateway(192, 168, 1, 1);    // Set your Gateway IP address
IPAddress subnet(255, 255, 0, 0);     // Set Subnet address

float_t Speed;  // Speed Level

/**
 * @brief Call-back function when any data have been sent
 *
 * @param   mac_addr        the MAC address whose have received from the message
 * @param   incomingData    sending data
 * @param   len             length of data
 */
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet from: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

/**
 * @brief Call-back function when any recviced data arrive to Master
 *
 * @param   mac_addr        the MAC address whose want to send to it
 * @param   incomingData    recviced data
 * @param   len             length of data
 */
void Master_OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  if(match_array<const uint8_t>(mac_addr, Slave1_Address, 6)){
    memcpy(&slave1_data, incomingData, sizeof(slave1_data));
    LeftEncoder_Distance  = slave1_data.LeftPosition;
    RightEncoder_Distance = slave1_data.RightPosition;
  }
  else {
    Serial.print("\tError in mac\t");
  }
}

/**
 * @brief Server Setup for app
 */
void Server_Setup(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", html_page);
  });

  // Send a GET request to <ESP_IP>/update?Speed=<value>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage1;
    // GET input1 value on <ESP_IP>/update?Speed=<value>
    if (request->hasParam("Speed")) {
      Speed = request->getParam("Speed")->value().toFloat()/100.0;
    }
    SEND_REQUEST;
  });

  server.on("/forward" , HTTP_GET, [](AsyncWebServerRequest *request){motors_linear =  Speed;  motors_angular =      0;  SEND_REQUEST;});
  server.on("/backward", HTTP_GET, [](AsyncWebServerRequest *request){motors_linear = -Speed;  motors_angular =      0;  SEND_REQUEST;});
  server.on("/left"    , HTTP_GET, [](AsyncWebServerRequest *request){motors_linear =      0;  motors_angular =  Speed;  SEND_REQUEST;});
  server.on("/right"   , HTTP_GET, [](AsyncWebServerRequest *request){motors_linear =      0;  motors_angular = -Speed;  SEND_REQUEST;});
  
  server.on("/FR", HTTP_GET, [](AsyncWebServerRequest *request){motors_linear =  0.75*Speed;  motors_angular = -0.25*Speed;  SEND_REQUEST;});
  server.on("/FL", HTTP_GET, [](AsyncWebServerRequest *request){motors_linear =  0.75*Speed;  motors_angular =  0.25*Speed;  SEND_REQUEST;});
  server.on("/DR", HTTP_GET, [](AsyncWebServerRequest *request){motors_linear = -0.75*Speed;  motors_angular = -0.25*Speed;  SEND_REQUEST;});
  server.on("/DL", HTTP_GET, [](AsyncWebServerRequest *request){motors_linear = -0.75*Speed;  motors_angular =  0.25*Speed;  SEND_REQUEST;});

  server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request){motors_linear = 0;  motors_angular = 0;  SEND_REQUEST;});

  server.on("/AUp"   , HTTP_GET, [](AsyncWebServerRequest *request){arm_directions[X] =  0;   arm_directions[Y] =  1;  SEND_REQUEST;});
  server.on("/Adown" , HTTP_GET, [](AsyncWebServerRequest *request){arm_directions[X] =  0;   arm_directions[Y] = -1;  SEND_REQUEST;});
  server.on("/Aleft" , HTTP_GET, [](AsyncWebServerRequest *request){arm_directions[X] = -1;   arm_directions[Y] =  0;  SEND_REQUEST;});
  server.on("/Aright", HTTP_GET, [](AsyncWebServerRequest *request){arm_directions[X] =  1;   arm_directions[Y] =  0;  SEND_REQUEST;});
  server.on("/Astop" , HTTP_GET, [](AsyncWebServerRequest *request){arm_directions[X] =  0;   arm_directions[Y] =  0;  SEND_REQUEST;});

  server.on("/zUp"  , HTTP_GET, [](AsyncWebServerRequest *request){zAxis_direction =  1;  SEND_REQUEST;});
  server.on("/zDown", HTTP_GET, [](AsyncWebServerRequest *request){zAxis_direction = -1;  SEND_REQUEST;});
  server.on("/zStop", HTTP_GET, [](AsyncWebServerRequest *request){zAxis_direction =  0;  SEND_REQUEST;});

  server.on("/wristUp"  , HTTP_GET, [](AsyncWebServerRequest *request){arm_directions[P] =  1;  SEND_REQUEST;});
  server.on("/wristDown", HTTP_GET, [](AsyncWebServerRequest *request){arm_directions[P] = -1;  SEND_REQUEST;});
  server.on("/wStop"    , HTTP_GET, [](AsyncWebServerRequest *request){arm_directions[P] =  0;  SEND_REQUEST;});

  server.on("/rollCW"   , HTTP_GET, [](AsyncWebServerRequest *request){arm_directions[R] =  1;  SEND_REQUEST;});
  server.on("/rolCCW"   , HTTP_GET, [](AsyncWebServerRequest *request){arm_directions[R] = -1;  SEND_REQUEST;});
  server.on("/rStop"    , HTTP_GET, [](AsyncWebServerRequest *request){arm_directions[R] =  0;  SEND_REQUEST;});

  server.on("/grip"     , HTTP_GET, [](AsyncWebServerRequest *request){arm_directions[G] =  1;  SEND_REQUEST;});
  server.on("/ungrip"   , HTTP_GET, [](AsyncWebServerRequest *request){arm_directions[G] = -1;  SEND_REQUEST;});
  server.on("/gStop"    , HTTP_GET, [](AsyncWebServerRequest *request){arm_directions[G] =  0;  SEND_REQUEST;});

  events.onConnect([](AsyncEventSourceClient *client){
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin();
  Serial.println(F("HTTP server started"));
}

/**
 * @brief send a ping every 5 seconds to check on the client side, if the server is still running
 */
void Server_Update(){
  static unsigned long lastEventTime = millis();
  static const unsigned long EVENT_INTERVAL_MS = 5000;
  if ((millis() - lastEventTime) > EVENT_INTERVAL_MS) {
    events.send("ping", NULL, millis());
    lastEventTime = millis();
  }
}

/**
 * @brief Communication Setup of Master
 *
 * @param   ssid        Network SSID
 * @param   password    Network Password
 */
void COM_MasterSetup(const char* ssid, const char* password) {
  WiFi.mode(WIFI_STA);  // Set the device as a Wi-Fi Station (set to WIFI_AP_STA in case of WiFi & ESPNOW)

  /** WiFi Init in app vis WiFi case
  WiFi.begin(ssid, password);   // Connect to the Network:
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println(F("STA Failed to configure"));
  }
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Setting as a Wi-Fi Station..");
  }
  Serial.print("Station IP Address: "); Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");      Serial.println(WiFi.channel());
  */

  // Set WiFi Channel: (in case for no WiFi connection)
  esp_wifi_set_promiscuous(true); // Required to allow setting of channel
  esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false); // EXPERIMENT

  // Init ESP-NOW:
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(Master_OnDataRecv);  // Call-back
  //esp_now_register_send_cb(OnDataSent);
  
  // Register peer:
  peerInfo.channel = CHANNEL;
  peerInfo.encrypt = false;

  // Add peer devices:
  memcpy(peerInfo.peer_addr, Slave1_Address, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  
  memcpy(peerInfo.peer_addr, Slave2_Address, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

/**
 * @brief Send the data to Slave1 by ESP-NOW
 */
void ESPNOW1_Send(){
  master1_data.linear  = motors_linear;
  master1_data.angular = motors_angular;
  master1_data.zDir    = zAxis_direction;
  master1_data.zPos    = zAxis_position;
  master1_data.vFile   = voice_file;
  master1_data.mode    = robot_mode;
  esp_now_send(Slave1_Address, (const uint8_t *)&master1_data, sizeof(master1_data)); // Send message
}

/**
 * @brief Send the data to Slave2 by ESP-NOW
 */
void ESPNOW2_Send(){
  for(uint8_t i=0; i<5; i++) {master2_data.arm_dir[i] = arm_directions[i];}
  for(uint8_t i=0; i<5; i++) {master2_data.arm_ang[i] = arm_angles    [i];}
  master2_data.mode = robot_mode;
  esp_now_send(Slave2_Address, (const uint8_t *)&master2_data, sizeof(master2_data)); // Send message
}

#endif