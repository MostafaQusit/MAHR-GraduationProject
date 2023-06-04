#ifndef MAHR_COM_MASTER_H_
#define MAHR_COM_MASTER_H_

#include <MAHR.h>
#include <MAHR/COM.h>

master1_msgs master1_data;
master2_msgs master2_data;
slave1_msgs slave1_data;

WiFiServer server(80);
String header; // Variable to store the HTTP request

IPAddress local_IP(192, 168, 1, 184); // Set your Static  IP address
IPAddress gateway(192, 168, 1, 1);    // Set your Gateway IP address
IPAddress subnet(255, 255, 0, 0);

unsigned long currentTime = millis(); // Current time
unsigned long previousTime = 0;       // Previous time
const long timeoutTime = 100;         // Define timeout time in milliseconds (example: 2000ms = 2s)

// MAC Addresses of the receivers
uint8_t Slave1_Address[] = {0x58, 0xBF, 0x25, 0x81, 0xEA, 0xF0};
//uint8_t Slave2_Address[] = {0x58, 0xBF, 0x25, 0x81, 0xDA, 0xA8};
int32_t master_channel;

// Speed Slider
int Speed_Start = 0;
int Speed_End = 0;
String SpeedString;
uint16_t Speed;

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
            /* page Setup:
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
            */



            // Incoming Requests:
            // Robot:
            if      (header.indexOf("GET /forward" )>=0) { Required_LeftMotor_mms= Speed;   Required_RightMotor_mms= Speed; }
            else if (header.indexOf("GET /backward")>=0) { Required_LeftMotor_mms=-Speed;   Required_RightMotor_mms=-Speed; }
            else if (header.indexOf("GET /left"    )>=0) { Required_LeftMotor_mms=-Speed;   Required_RightMotor_mms= Speed; }
            else if (header.indexOf("GET /right"   )>=0) { Required_LeftMotor_mms= Speed;   Required_RightMotor_mms=-Speed; }
            
            else if (header.indexOf("GET /FR"      )>=0) { Required_LeftMotor_mms= Speed;   Required_RightMotor_mms= Speed/2; }
            else if (header.indexOf("GET /FL"      )>=0) { Required_LeftMotor_mms= Speed/2; Required_RightMotor_mms= Speed;   }
            else if (header.indexOf("GET /DR"      )>=0) { Required_LeftMotor_mms=-Speed;   Required_RightMotor_mms=-Speed/2; }
            else if (header.indexOf("GET /DL"      )>=0) { Required_LeftMotor_mms=-Speed/2; Required_RightMotor_mms=-Speed;   }

            else if (header.indexOf("GET /stop"    )>=0) { Required_LeftMotor_mms=0; Required_RightMotor_mms=0; }
            
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
            else if (header.indexOf("GET /wristUp"  )>=0) { wrist_speed =  1; }
            else if (header.indexOf("GET /wristDown")>=0) { wrist_speed = -1; }
            else if (header.indexOf("GET /wStop"    )>=0) { wrist_speed =  0; }

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
              Speed = map(SpeedString.toInt(), 0, 100, 0, 350); // 387
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

// Master Initialization
void Master_Setup(const char* ssid, const char* password) {
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println(F("STA Failed to configure"));
  }

  Serial.print(F("Connecting to WiFi"));
  WiFi.mode(WIFI_AP_STA);     // Set the device as a Station and Soft Access Point simultaneously
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

  server.begin();
  Serial.println(F("HTTP server started"));
  delay(1000);
}
// Send to/receive from slaves
void Master_dataUpdate() {
  App_DataUpdate();

  master1_data.linear  = motors_linear;
  master1_data.angular = motors_angular;
  master1_data.zSpeed  = zAxis_Speed;
  master1_data.vFile   = voice_file;
  ESPNOW_Send(master_channel, Slave1_Address, (const uint8_t *)&master1_data, sizeof(master1_data));
  /* for slave 2:
  master2_data.roll_speed  = roll_speed;
  master2_data.grip_speed  = grip_speed;
  master2_data.wrist_speed = wrist_speed;
  master2_data.armX        = armX;
  master2_data.armY        = armY;
  ESPNOW_Send(master_channel, Slave2_Address, (const uint8_t *)&master1_data, sizeof(master1_data));
  */
}

#endif