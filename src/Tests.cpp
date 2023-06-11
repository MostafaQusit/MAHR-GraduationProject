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

const char* ssid = "Koset"; // "WE_F6AE4C";
const char* password = "h9f16306"; // "lcw04660";

IPAddress local_IP(192, 168, 1, 184); // Set your Static  IP address
IPAddress gateway(192, 168, 1, 1);    // Set your Gateway IP address
IPAddress subnet(255, 255, 0, 0);

AsyncWebServer server(80);

void setup(void) {
  Serial.begin(115200);
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println(F("STA Failed to configure"));
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! This is a sample response.");
  });

  AsyncElegantOTA.begin(&server);    // Start AsyncElegantOTA
  server.begin();
  Serial.println("HTTP server started");
  Serial.println(333);
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