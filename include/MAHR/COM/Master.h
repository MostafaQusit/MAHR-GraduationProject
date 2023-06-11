#ifndef MAHR_COM_MASTER_H_
#define MAHR_COM_MASTER_H_

#include <MAHR.h>
#include <MAHR/COM.h>
#include <ESPAsyncWebServer.h>
//#include <AsyncTCP.h>
//#include <AsyncElegantOTA.h>
//#include <ArduinoOTA.h>
//#include <ElegantOTA.h>

master1_msgs master1_data;
master2_msgs master2_data;
slave1_msgs slave1_data;

//WiFiServer server(80);
//String header; // Variable to store the HTTP request
AsyncWebServer server(80);
AsyncEventSource events("/events");
#define SEND_REQUEST  (request->send(200, "text/plain", "OK"))
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP-NOW DASHBOARD</title>
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
IPAddress subnet(255, 255, 0, 0);

unsigned long currentTime = millis(); // Current time
unsigned long previousTime = 0;       // Previous time
const long timeoutTime = 100;         // Define timeout time in milliseconds (example: 2000ms = 2s)

// MAC Addresses of the receivers
int32_t master_channel;

// Speed Slider
int Speed_Start = 0;
int Speed_End = 0;
String SpeedString;
float_t Speed;

void Master_OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  if(Match_MAC(mac_addr, Slave1_Address)){
    memcpy(&slave1_data, incomingData, sizeof(slave1_data));
    LeftEncoder_Distance  = slave1_data.LeftPosition;
    RightEncoder_Distance = slave1_data.RightPosition;
  }
  else {
    Serial.print("\tError in mac\t");
  }
}
/* app
void App_DataUpdate() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    //Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client

    client.println("<p>(" + String(Current_PositionX) + "," + String(Current_PositionY) + ")</p>");
    if(client.connected()) {client.println("<p>Robot is Connected    </p>");}
    else                   {client.println("<p>Robot is Not Connected</p>");}

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
            // page Setup:
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
            client.println(F("\t <button onmousedown=\"right()\"    onmouseup=\"stop()\">  Right   </button></p>"));
            client.println(F("<p><button onmousedown=\"backward()\" onmouseup=\"stop()\"> Backward </button></p>"));
            client.println(F("<script>function forward()  { $.get(\"/forward\" ); {Connection: close}; }"));
            client.println(F(        "function left()     { $.get(\"/left\"    ); {Connection: close}; }"));
            client.println(F(        "function right()    { $.get(\"/right\"   ); {Connection: close}; }"));
            client.println(F(        "function backward() { $.get(\"/backward\"); {Connection: close}; }"));
            client.println(F(        "function stop()     { $.get(\"/stop\"    ); {Connection: close}; }</script>"));

            client.println(F("<p>Speed: <span id=\"speedPos\"></span></p>"));
            client.println(F("<input type=\"range\" min=\"0\" max=\"300\" class=\"slider\" id=\"speedSlider\" "));
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
            // Robot:
            if      (header.indexOf("GET /forward" )>=0) { motors_linear =  Speed;   motors_angular =      0; }
            else if (header.indexOf("GET /backward")>=0) { motors_linear = -Speed;   motors_angular =      0; }
            else if (header.indexOf("GET /left"    )>=0) { motors_linear =      0;   motors_angular =  Speed; }
            else if (header.indexOf("GET /right"   )>=0) { motors_linear =      0;   motors_angular = -Speed; }
            
            else if (header.indexOf("GET /FR"      )>=0) { motors_linear =  0.5*Speed;   motors_angular = -0.5*Speed; }
            else if (header.indexOf("GET /FL"      )>=0) { motors_linear =  0.5*Speed;   motors_angular =  0.5*Speed; }
            else if (header.indexOf("GET /DR"      )>=0) { motors_linear = -0.5*Speed;   motors_angular = -0.5*Speed; }
            else if (header.indexOf("GET /DL"      )>=0) { motors_linear = -0.5*Speed;   motors_angular =  0.5*Speed; }

            else if (header.indexOf("GET /stop"    )>=0) { motors_linear =      0;   motors_angular =      0; }
            
            // Arm:
            else if (header.indexOf("GET /AUp"   )>=0) { armX= 1; armY= 1; }
            else if (header.indexOf("GET /Adown" )>=0) { armX=-1; armY=-1; }
            else if (header.indexOf("GET /Aleft" )>=0) { armX=-1; armY= 1; }
            else if (header.indexOf("GET /Aright")>=0) { armX= 1; armY=-1; }
            else if (header.indexOf("GET /Astop" )>=0) { armX= 0; armY= 0; }

            // Z-axis:
            else if (header.indexOf("GET /zUp"  )>=0) { zAxis_Speed =  1000; }
            else if (header.indexOf("GET /zDown")>=0) { zAxis_Speed = -1000; }
            else if (header.indexOf("GET /zStop")>=0) { zAxis_Speed =     0; }

            // Wrist:
            else if (header.indexOf("GET /wristUp"  )>=0) { pitch_speed =  1; }
            else if (header.indexOf("GET /wristDown")>=0) { pitch_speed = -1; }
            else if (header.indexOf("GET /wStop"    )>=0) { pitch_speed =  0; }

            // Roll:
            else if (header.indexOf("GET /rollCW")>=0) { roll_speed =  1; }
            else if (header.indexOf("GET /rolCCW")>=0) { roll_speed = -1; }
            else if (header.indexOf("GET /rStop" )>=0) { roll_speed =  0; }

            // Grip:
            else if (header.indexOf("GET /grip"  )>=0) { grip_speed =  1; }
            else if (header.indexOf("GET /ungrip")>=0) { grip_speed = -1; }
            else if (header.indexOf("GET /gStop" )>=0) { grip_speed =  0; }

            // GET /?value=180& HTTP/1.1
            else if(header.indexOf("GET /Speed=")>=0) {
              Speed_Start = header.indexOf('=');
              Speed_End   = header.indexOf('&');
              SpeedString = header.substring(Speed_Start+1, Speed_End);
              Speed = ((float_t)SpeedString.toInt())/100.0; // 0:100 -> 0:1
            }
            // else if() for receive Target_Position X & Y
            else { Serial.print(header); }

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
*/
// Master Initialization
void Master_Setup(const char* ssid, const char* password) {
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println(F("STA Failed to configure"));
  }

  Serial.print(F("Connecting to WiFi"));
  WiFi.mode(WIFI_STA);     // Set the device as a Station and Soft Access Point simultaneously
  
  WiFi.begin(ssid, password); // Set device as a Wi-Fi Station
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.print("\nStation IP Address: ");   Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");        Serial.println(WiFi.channel());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  //esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(Master_OnDataRecv);
  master_channel = getWiFiChannel(ssid);
  Serial.println(master_channel);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/update?Speed=<value>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage1;
    // GET input1 value on <ESP_IP>/update?Speed=<value>
    if (request->hasParam("Speed")) {
      Speed = request->getParam("Speed")->value().toFloat();
    }
    request->send(200, "text/plain", "OK");
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

  server.on("/AUp"   , HTTP_GET, [](AsyncWebServerRequest *request){armX =  0;   armY =  1;  SEND_REQUEST;});
  server.on("/Adown" , HTTP_GET, [](AsyncWebServerRequest *request){armX =  0;   armY = -1;  SEND_REQUEST;});
  server.on("/Aleft" , HTTP_GET, [](AsyncWebServerRequest *request){armX = -1;   armY =  0;  SEND_REQUEST;});
  server.on("/Aright", HTTP_GET, [](AsyncWebServerRequest *request){armX =  1;   armY =  0;  SEND_REQUEST;});
  server.on("/Astop" , HTTP_GET, [](AsyncWebServerRequest *request){armX =  0;   armY =  0;  SEND_REQUEST;});

  server.on("/zUp"  , HTTP_GET, [](AsyncWebServerRequest *request){zAxis_Speed =  1000;  SEND_REQUEST;});
  server.on("/zDown", HTTP_GET, [](AsyncWebServerRequest *request){zAxis_Speed = -1000;  SEND_REQUEST;});
  server.on("/zStop", HTTP_GET, [](AsyncWebServerRequest *request){zAxis_Speed =     0;  SEND_REQUEST;});

  server.on("/wristUp"  , HTTP_GET, [](AsyncWebServerRequest *request){pitch_speed =  1;  SEND_REQUEST;});
  server.on("/wristDown", HTTP_GET, [](AsyncWebServerRequest *request){pitch_speed = -1;  SEND_REQUEST;});
  server.on("/wristStop", HTTP_GET, [](AsyncWebServerRequest *request){pitch_speed =  0;  SEND_REQUEST;});

  server.on("/rollCW", HTTP_GET, [](AsyncWebServerRequest *request){roll_speed =  1;  SEND_REQUEST;});
  server.on("/rolCCW", HTTP_GET, [](AsyncWebServerRequest *request){roll_speed = -1;  SEND_REQUEST;});
  server.on("/rStop" , HTTP_GET, [](AsyncWebServerRequest *request){roll_speed =  0;  SEND_REQUEST;});

  server.on("/grip"  , HTTP_GET, [](AsyncWebServerRequest *request){grip_speed =  1;  SEND_REQUEST;});
  server.on("/ungrip", HTTP_GET, [](AsyncWebServerRequest *request){grip_speed = -1;  SEND_REQUEST;});
  server.on("/gStop" , HTTP_GET, [](AsyncWebServerRequest *request){grip_speed =  0;  SEND_REQUEST;});

  events.onConnect([](AsyncEventSourceClient *client){
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin();
  Serial.println(F("HTTP server started"));
  delay(500);
}
// Send to/receive from slaves
void Master_dataUpdate() {
  static unsigned long lastEventTime = millis();
  static const unsigned long EVENT_INTERVAL_MS = 5000;
  if ((millis() - lastEventTime) > EVENT_INTERVAL_MS) {
    events.send("ping",NULL,millis());
    lastEventTime = millis();
  }
  if(master1_data.linear!=motors_linear || master1_data.angular!=motors_angular ||
     master1_data.zSpeed!=zAxis_Speed || master1_data.vFile!=voice_file){
      
      master1_data.linear  = motors_linear;
      master1_data.angular = motors_angular;
      master1_data.zSpeed  = zAxis_Speed;
      master1_data.vFile   = voice_file;
      ESPNOW_Send(master_channel, Slave1_Address, (const uint8_t *)&master1_data, sizeof(master1_data));
     }
  /* for slave 2:
  master2_data.roll_speed  = roll_speed;
  master2_data.grip_speed  = grip_speed;
  master2_data.pitch_speed = pitch_speed;
  master2_data.armX        = armX;
  master2_data.armY        = armY;
  ESPNOW_Send(master_channel, Slave2_Address, (const uint8_t *)&master1_data, sizeof(master1_data));
  */
}

#endif