#ifndef MAHR_MOBILE_APP_H_
#define MAHR_MOBILE_APP_H_

#include <MAHR.h>
#include <WiFi.h>

WiFiServer server(80);
String header; // Variable to store the HTTP request

unsigned long currentTime = millis(); // Current time
unsigned long previousTime = 0;       // Previous time
const long timeoutTime = 100;        // Define timeout time in milliseconds (example: 2000ms = 2s)

IPAddress local_IP(192, 168, 1, 184); // Set your Static  IP address
IPAddress gateway(192, 168, 1, 1);    // Set your Gateway IP address
IPAddress subnet(255, 255, 0, 0);

int Speed_Start = 0;
int Speed_End = 0;
String SpeedString;
uint16_t Speed;

/* Transaction (Send & Receive) Setup for App
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

MDNSResponder mdns;
String webpage = "";

void App_TransactionSetup() {
  if (mdns.begin("esp")) {
    Serial.println(F("MDNS responder started"));
  }

  webpage = "<h1>ESP32 Web Server</h1>\n\
             <p>Move Robot:</p>\
             <p>                     <a href=\"forward\" ><button>Forward </button></a></p>\
             <p> <a href=\"left\"><button>Left</button></a>          <a href=\"right\"><button>Right</button></a></p>\
             <p>                     <a href=\"backward\"><button>Backward</button></a></p>";

  webpage += "<p>Speed:<span id=\"Speed\"></span></p>\
              <input type=\"range\" min=\"0\" max=\"314\" class=\"slider\" id=\"speedSlider\"\
              onchange=\"speedFun(this.value)\" value=\"" + SpeedString + "\"/>\
              <script>var slider = document.getElementById(\"speedSlider\");\
              var speedP = document.getElementById(\"speedPos\"); speedP.innerHTML = slider.value;\
              slider.oninput = function() { slider.value = this.value; speedP.innerHTML = this.value; }\
              $.ajaxSetup({timeout:1000});\
              function speedFun(pos) {$.get(\"/Speed\" + pos + \"&\"); {Connection: close};}</script>";

  server.on("/"        , []() {server.send(200, "text/html", webpage);  LeftMotor_Speed=     0; RightMotor_Speed=       0;});
  server.on("/forward" , []() {server.send(200, "text/html", webpage);  LeftMotor_Speed= Speed; RightMotor_Speed= Speed;});
  server.on("/backward", []() {server.send(200, "text/html", webpage);  LeftMotor_Speed=-Speed; RightMotor_Speed=-Speed;});
  server.on("/right"   , []() {server.send(200, "text/html", webpage);  LeftMotor_Speed= Speed; RightMotor_Speed=-Speed;});
  server.on("/left"    , []() {server.send(200, "text/html", webpage);  LeftMotor_Speed=-Speed; RightMotor_Speed= Speed;});
  server.on("/Speed"   , []() {server.send(200, "text/html", webpage);  SpeedString=server.arg(0); Speed=SpeedString.toInt();});
  //server.handleClient();
}
*/
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
  server.begin();
  Serial.println(F("HTTP server started"));
}
// For handling the client operations
void App_DataUpdate() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    //Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println(F("HTTP/1.1 200 OK"));
            client.println(F("Content-type:text/html"));
            client.println(F("Connection: close"));
            client.println();

            // Display the HTML web page
            client.println(F("<!DOCTYPE html><html>"));
            client.println(F("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"));
            client.println(F("<link rel=\"icon\" href=\"data:,\">"));

            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println(F("<style>body { text-align: center;"));
            client.println(F("font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto;}"));
            client.println(F(".slider { width: 300px; }</style>"));
            client.println(F("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>"));
                     
            // Web Page
            client.println(F("</head><body><h1>MAHR Control WebSite</h1>"));

            client.println(F("<p> Move Robot </p>"));
            client.println(F("<p><button onmousedown=\"forward()\"  onmouseup=\"stop()\"> Forward  </button></p>"));
            client.print  (F("<p><button onmousedown=\"left()\"     onmouseup=\"stop()\">   Left   </button>\t\t"));
            client.println(F("/t <button onmousedown=\"right()\"    onmouseup=\"stop()\">  Right   </button></p>"));
            client.println(F("<p><button onmousedown=\"backward()\" onmouseup=\"stop()\"> Backward </button></p>"));
            client.println(F("<script>function forward()  { $.get(\"/forward\" ); {Connection: close}; }"));
            client.println(F(        "function left()     { $.get(\"/left\"    ); {Connection: close}; }"));
            client.println(F(        "function right()    { $.get(\"/right\"   ); {Connection: close}; }"));
            client.println(F(        "function backward() { $.get(\"/backward\"); {Connection: close}; }"));
            client.println(F(        "function stop()     { $.get(\"/stop\"    ); {Connection: close}; }</script>"));

            client.println(F("<p>Speed: <span id=\"speedPos\"></span></p>"));
            client.println(F("<input type=\"range\" min=\"0\" max=\"314\" class=\"slider\" id=\"speedSlider\" "));
            client.println("onchange=\"speedf(this.value)\" Speed=\""+SpeedString+"\"/>");
            client.println(F("<script>var slider = document.getElementById(\"speedSlider\");"));
            client.println(F("var speedP = document.getElementById(\"speedPos\"); speedP.innerHTML = slider.value;"));
            client.println(F("slider.oninput = function() { slider.value = this.value; speedP.innerHTML = this.value; }"));
            client.println(F("$.ajaxSetup({timeout:1000});"));
            client.println(F("function speedf(pos) { $.get(\"/Speed=\" + pos + \"&\"); {Connection: close}; }</script>"));

            client.println(F("<p> Move Z-Axis Table </p>"));
            client.println(F("<p><button onmousedown=\"zUp()\"   onmouseup=\"zStop()\">  UP  </button></p>"));
            client.println(F("<p><button onmousedown=\"zDown()\" onmouseup=\"zStop()\"> Down </button></p>"));
            client.println(F("<script>function zUp()   { $.get(\"/zUp\"  ); {Connection: close}; }"));
            client.println(F(        "function zDown() { $.get(\"/zDown\"); {Connection: close}; }"));
            client.println(F(        "function zStop() { $.get(\"/zStop\"); {Connection: close}; }</script>"));

            client.println(F("</body></html>"));     
            
            // Incoming Requests:
            if      (header.indexOf("GET /forward" )>=0) { LeftMotor_Speed= Speed; RightMotor_Speed= Speed; }
            else if (header.indexOf("GET /backward")>=0) { LeftMotor_Speed=-Speed; RightMotor_Speed=-Speed; }
            else if (header.indexOf("GET /left"    )>=0) { LeftMotor_Speed=-Speed; RightMotor_Speed= Speed; }
            else if (header.indexOf("GET /right"   )>=0) { LeftMotor_Speed= Speed; RightMotor_Speed=-Speed; }
            else if (header.indexOf("GET /stop"    )>=0) { LeftMotor_Speed=     0; RightMotor_Speed=     0; }

            if      (header.indexOf("GET /zUp"  )>=0) { zAxis_Speed =  1000; }
            else if (header.indexOf("GET /zDown")>=0) { zAxis_Speed = -1000; }
            else if (header.indexOf("GET /zStop")>=0) { zAxis_Speed =     0; }

            if(header.indexOf("GET /Speed=")>=0) { //GET /?value=180& HTTP/1.1
              Speed_Start = header.indexOf('=');
              Speed_End   = header.indexOf('&');
              SpeedString = header.substring(Speed_Start+1, Speed_End);
              Speed = SpeedString.toInt();
            }

            client.println(); // The HTTP response ends with another blank line
            break;            // Break out of the while loop
          }
          else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        }
        else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    header = ""; // Clear the header variable
    // Close the connection
    client.stop();
    //Serial.println(F("Client disconnected."));
    //Serial.println(F(""));
  }
}

#endif