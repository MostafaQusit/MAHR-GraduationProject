#ifndef MAHR_MP3_H_
#define MAHR_MP3_H_

#include <MAHR.h>
#include <DFRobotDFPlayerMini.h>

DFRobotDFPlayerMini mp3;
HardwareSerial mySerial(1); // Serial2
uint16_t current_file;

// MP3 Initialization
void Mp3_Setup(uint16_t volume=30) {
  mySerial.begin(9600, SERIAL_8N1, MP3_TX, MP3_RX);
  Serial.println(F("Mp3 initializing... (May take 3~5 seconds)"));
  if (!mp3.begin(mySerial)) { // Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("\t1.Please recheck the connection!"));
    Serial.println(F("\t2.Please insert the SD card!"));
    while(1);
  }
  Serial.println(F("DFPlayer Mini online."));

  volume = constrain(volume, 0, 30);
  mp3.volume(volume); // Set volume value (0~30).
  mp3.EQ(DFPLAYER_EQ_NORMAL);
  mp3.outputDevice(DFPLAYER_DEVICE_SD);

  mp3.playFolder(1,1);
  current_file = 1;
}
// Print the State of MP3
void Mp3_PrintState(uint8_t type, uint16_t value) {
  switch (type) {
    case TimeOut:               Serial.println(F("Time Out!      "));  break;
    case WrongStack:            Serial.println(F("Stack Wrong!   "));  break;
    case DFPlayerCardInserted:  Serial.println(F("Card Inserted! "));  break;
    case DFPlayerCardRemoved:   Serial.println(F("Card Removed!  "));  break;
    case DFPlayerCardOnline:    Serial.println(F("Card Online!   "));  break;
    case DFPlayerUSBInserted:   Serial.println(F("USB Inserted!  "));  break;
    case DFPlayerUSBRemoved:    Serial.println(F("USB Removed!   "));  break;
    case DFPlayerPlayFinished:  Serial.print(F("Number:"));
                                Serial.print(value);
                                Serial.println(F(" Play Finished!"));  break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:              Serial.println(F("Card not found         ")); break;
        case Sleeping:          Serial.println(F("Sleeping               ")); break;
        case SerialWrongStack:  Serial.println(F("Get Wrong Stack        ")); break;
        case CheckSumNotMatch:  Serial.println(F("Check Sum Not Match    ")); break;
        case FileIndexOut:      Serial.println(F("File Index Out of Bound")); break;
        case FileMismatch:      Serial.println(F("Cannot Find File       ")); break;
        case Advertise:         Serial.println(F("In Advertise           ")); break;
        default:                                                              break;
      }
      break;
    default:                                                          break;
  }
}
// Check if anything happen and read it
void Mp3_StateUpdate() {
  if (mp3.available()) {
    Mp3_PrintState(mp3.readType(), mp3.read()); // Print the detail message from DFPlayer to handle different errors and states.
  }
}
void Mp3_play(uint16_t required_file){
  int16_t diff_file = required_file - current_file;
  mp3.stop();
  if(diff_file != 0){
    for (uint16_t f = 0; f < abs(diff_file); f++){
      if(diff_file > 0) {mp3.next();}
      if(diff_file < 0) {mp3.previous();}
    }
    current_file = required_file;
  }
  mp3.start();
}

#endif