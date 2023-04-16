#ifndef MAHR_GSM_H_
#define MAHR_GSM_H_

#include <MAHR.h>
#include <GSMSimSMS.h>
#include <GSMSimCall.h>

#define attia_phone   "+201140822313"
#define koskot_phone  "+201151638925"

static volatile uint8_t SMS_num, Call_num;
HardwareSerial mySerial(1); // Serial2
GSMSim      gsm(mySerial, GSM_RST);
GSMSimSMS   sms(mySerial, GSM_RST);
GSMSimCall call(mySerial, GSM_RST);

// GSM Initialization
void GSM_Setup() {
  //pinMode(GSM_DTR, OUTPUT);
  //pinMode(GSM_RING, INPUT);
  mySerial.begin(115200, SERIAL_8N1, GSM_TX, GSM_RX);
  while (!mySerial){}
  // General Init:
  Serial.println(F("GSM initializing..."));
  Serial.println("\tSet Phone Function...  \t" + String(gsm.setPhoneFunc(1)      ));  delay(100);
  Serial.println("\tIs SIM Inserted...     \t" + String(gsm.isSimInserted()      ));  delay(100);
  Serial.println("\tNetwork Registration...\t" + String(gsm.isRegistered()       ));  delay(100);
  Serial.println("\tSignal Quality...      \t" + String(gsm.signalQuality()      ));  delay(100);
  Serial.println("\tOperator Name...       \t" + String(gsm.operatorNameFromSim()));  delay(100);
  // SMS Init:
  Serial.println(F("SMS initializing..."));
  Serial.print(F("\tset Text Mode...  \t"));  Serial.println(sms.setTextMode(true));   delay(100);
  Serial.print(F("\tInit SMS...       \t"));  Serial.println(sms.initSMS()        );   delay(100);
  Serial.print(F("\tList Unread SMS...\t"));  Serial.println(sms.list(true)       );   delay(100);
  // Call Init:
  Serial.println(F("Call initializing..."));
  Serial.print(F("\tInit Call...      \t"));  Serial.println(call.initCall());  delay(100);
}
// Send a SMS Massege to someone
bool GSM_SendSMS(String phone_number, String message) {
  char msg_buffer[message.length()+1];
  char phone_buffer[phone_number.length()+1];

  message.toCharArray(msg_buffer, message.length()+1);
  phone_number.toCharArray(phone_buffer, phone_number.length()+1);

  return sms.send(phone_buffer, msg_buffer);
}
// Call a someone
bool GSM_MakeCall(String phone_number) {
  char phone_buffer[phone_number.length()+1];
  phone_number.toCharArray(phone_buffer, phone_number.length()+1);

  return call.call(phone_buffer);
}
// Check if there are incoming SMS or Call for someone
void GSM_CheckIncoming() {
  if(mySerial.available()) {
      String buffer = "";
      buffer = mySerial.readString();
      if(buffer.indexOf("+CMTI:") != -1){ //SMS
        SMS_num++;
        Serial.print(SMS_num);
        Serial.print(F(". "));

        Serial.print(F("SMS Index No... "));
        int indexno = sms.indexFromSerial(buffer);
        Serial.println(indexno);

        Serial.print(F("Who send the message?..."));
        Serial.println(sms.getSenderNo(indexno));

        Serial.print(F("Read the message... "));
        Serial.println(sms.readFromSerial(buffer));
      }
      if(buffer.indexOf("+CLCC:") != -1){ //Call
        Call_num++;
        Serial.print(Call_num);
        Serial.print(F(". "));
        Serial.print(F("Call "));
        Serial.println(call.readCurrentCall(buffer));
      }
      else{
        Serial.println(buffer);
      }
  }
}
// Make GSM go to Sleep (Power Saving mode)
void GSM_Sleep() { digitalWrite(GSM_DTR, HIGH); }
// Make GSM Wake up for make a task
void GSM_WakeUp() { digitalWrite(GSM_DTR, LOW); delay(50); }

#endif