#ifndef MAHR_MOBILE_APP_H_
#define MAHR_MOBILE_APP_H_

#include <MAHR.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

MDNSResponder mdns;
WebServer server(80);
String webpage = "";
uint8_t Speed;

IPAddress local_IP(192, 168, 1, 184); // Set your Static  IP address
IPAddress gateway(192, 168, 1, 1);    // Set your Gateway IP address

IPAddress subnet(255, 255, 0, 0);
//IPAddress primaryDNS(8, 8, 8, 8);   // optional
//IPAddress secondaryDNS(8, 8, 4, 4); // optional

// Transaction (Send & Receive) Setup for App
void App_TransactionSetup() {
  webpage = "<h1>ESP32 Web Server</h1>\n\
             <p>Move Robot  <a href=\"forward\" ><button>Forward </button></a></p>\n\
             <p>Move Robot  <a href=\"backward\"><button>Backward</button></a></p>\n\
             <p>Move Robot  <a href=\"right\"   ><button>Right   </button></a></p>\n\
             <p>Move Robot  <a href=\"left\"    ><button>Left    </button></a></p>\n\
             <p>Robot Speed <a href=\"Speed\"   >"  +  String(Speed)  +  "</a></p>\n";
  
  server.on("/"        , []() {server.send(200, "text/html", webpage);  LeftMotor_Speed=          0; RightMotor_Speed=          0;});
  server.on("/forward" , []() {server.send(200, "text/html", webpage);  LeftMotor_Speed= 3.14*Speed; RightMotor_Speed= 3.14*Speed;});
  server.on("/backward", []() {server.send(200, "text/html", webpage);  LeftMotor_Speed=-3.14*Speed; RightMotor_Speed=-3.14*Speed;});
  server.on("/right"   , []() {server.send(200, "text/html", webpage);  LeftMotor_Speed= 3.14*Speed; RightMotor_Speed=-3.14*Speed;});
  server.on("/left"    , []() {server.send(200, "text/html", webpage);  LeftMotor_Speed=-3.14*Speed; RightMotor_Speed= 3.14*Speed;});
  server.on("/Speed"   , []() {server.send(200, "text/html", webpage);  Speed=server.arg(0).toInt();});

}
// App Initialization
void App_Setup(const char* ssid, const char* password) {
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println(F("STA Failed to configure"));
  }

  // Connect to Wi-Fi network with SSID and password
  Serial.print(F("Connecting to WiFi"));
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    Serial.print(F("."));
    delay(100);
  }
  Serial.println();
  Serial.print(F("Connected to "));   Serial.println(ssid);
  Serial.print(F("IP address: "));   Serial.println(WiFi.localIP());
  
  if (mdns.begin("esp")) {
    Serial.println(F("MDNS responder started"));
  }
  
  App_TransactionSetup();
  server.begin();
  Serial.println(F("HTTP server started"));
}
// For handling the client operations
void App_DataUpdate() {
  server.handleClient();
}

#endif