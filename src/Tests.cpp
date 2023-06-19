#include <Arduino.h>
#include <MAHR.h>

//#define TEST1
#define TEST2
//#define TEST3

#ifdef TEST1
#include <WiFi.h>
#include <esp_wifi.h>
//#include <WiFiNINA.h>
#include <ArduinoOTA.h>

IPAddress local_IP(192, 168, 1, 186); // Set your Static  IP address
IPAddress gateway(192, 168, 1, 1);    // Set your Gateway IP address
IPAddress subnet(255, 255, 0, 0);

void setup(){
  Serial.begin(115200);
  while(!Serial){}

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println(F("STA Failed to configure"));
  }

  Serial.print(F("Connecting to WiFi"));
  WiFi.mode(WIFI_AP_STA);     // Set the device as a Station and Soft Access Point simultaneously
  WiFi.begin("WE_F6AE4C", "lcw04660"); // Set device as a Wi-Fi Station
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.print("\nStation IP Address: ");   Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");        Serial.println(WiFi.channel());

  ArduinoOTA.begin(WiFi.localIP(), "Arduino", "password", InternalStorage);
}

void loop(){
  ArduinoOTA.poll();
  Serial.println(5);
}
#endif

#ifdef TEST2
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#define SEND_REQUEST (request->send(200, "text/plain", "OK"))   // response on every request from server

const char* ssid = "WE_F6AE4C";   // "Koset";
const char* password = "lcw04660";  // "h9f16306";

IPAddress local_IP(192, 168, 1, 186); // Set your Static  IP address
IPAddress gateway(192, 168, 1, 1);    // Set your Gateway IP address
IPAddress subnet(255, 255, 0, 0);

// To get the local time we must include DNS servers. Google's used here.
IPAddress primaryDNS(8, 8, 8, 8);   // Primary   DNS server
IPAddress secondaryDNS(8, 8, 4, 4); // Secondary DNS server

AsyncWebServer server(80);
AsyncEventSource events("/events"); // Event Object

float_t Speed; // Speed Level

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

void setup(void) {
  Serial.begin(115200);
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println(F("STA Failed to configure"));
  }

  WiFi.mode(WIFI_STA);  // Set the device as a Wi-Fi Station

  // Connect to the Network:
  WiFi.begin(ssid, password);
  Serial.print(F("Connecting to WiFi"));
  while (WiFi.status() != WL_CONNECTED) { // Check if connected or still not:
    Serial.print(".");
    delay(100);
  }
  Serial.print("\nStation IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Get WiFi Channel:
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", html_page);
  });

  // Send a GET request to <ESP_IP>/update?Speed=<value>
  server.on("/update?", HTTP_GET, [] (AsyncWebServerRequest *request) {
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

  events.onConnect([](AsyncEventSourceClient *client){
    client->send("hello!", NULL, millis(), 10000);
  });
  delay(200);

  AsyncElegantOTA.begin(&server);    // Start AsyncElegantOTA
  server.addHandler(&events);
  server.begin();
  Serial.println("HTTP server started");
  Serial.println(1);
}

void loop(void) {
}
#endif

#ifdef TEST3

#include <MAHR/PS4_Controller.h>
void setup() {
  Serial.begin(115200);
  PS4_Setup();
}

void loop() {
  PS4_PrintData();
}

#endif