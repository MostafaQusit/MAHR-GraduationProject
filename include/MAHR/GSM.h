#ifndef MAHR_GSM_H_
#define MAHR_GSM_H_

#include <MAHR.h>
#include <GSMSimSMS.h>
#include <GSMSimCall.h>

#define attia_phone   "+201140822313"
#define koskot_phone  "+201151638925"

static volatile uint8_t SMS_num;  // SMS  Number
static volatile uint8_t Call_num; // Call Number

HardwareSerial mySerial(1); // Hardware-Serial Object {HardwareSerial(1) = Serial2}
GSMSim      gsm(mySerial, GSM_RST);   // GSM object
GSMSimSMS   sms(mySerial, GSM_RST);   // GSM object for SMS  features
GSMSimCall call(mySerial, GSM_RST);   // GSM object for Call features

/**
 * @brief GSM Setup
 */
void GSM_Setup() {
  // pins setup:
  pinMode(GSM_DTR, OUTPUT);
  pinMode(GSM_RING, INPUT);

  // Serial2 setup:
  mySerial.begin(115200, SERIAL_8N1, GSM_TX, GSM_RX);
  while (!mySerial){}

  // General Init:
  Serial.println(F("GSM initializing..."));
  Serial.println("\tSet Phone Function...  \t" + String(gsm.setPhoneFunc(1)      ));  delay(50);
  Serial.println("\tIs SIM Inserted...     \t" + String(gsm.isSimInserted()      ));  delay(50);
  Serial.println("\tNetwork Registration...\t" + String(gsm.isRegistered()       ));  delay(50);
  Serial.println("\tSignal Quality...      \t" + String(gsm.signalQuality()      ));  delay(50);
  Serial.println("\tOperator Name...       \t" + String(gsm.operatorNameFromSim()));  delay(50);

  // SMS Init:
  Serial.println(F("SMS initializing..."));
  Serial.print(F("\tset Text Mode...  \t"));  Serial.println(sms.setTextMode(true));  delay(50);
  Serial.print(F("\tInit SMS...       \t"));  Serial.println(sms.initSMS()        );  delay(50);
  Serial.print(F("\tList Unread SMS...\t"));  Serial.println(sms.list(true)       );  delay(50);

  // Call Init:
  Serial.println(F("Call initializing..."));
  Serial.print(F("\tInit Call...      \t"));  Serial.println(call.initCall()      );  delay(50);
}

/**
 * @brief   Send a SMS Massege to someone
 *
 * @param   phone_number    phone number that want to text him
 * @param   message         the text content
 * 
 * @return  true(1) if no error, false(0) otherwise
 */
bool GSM_SendSMS(String phone_number, String message) {
  // make buffer to rcevice the data from string variables:
  char msg_buffer[message.length()+1];
  char phone_buffer[phone_number.length()+1];

  // copy the content of string var. to the char array:
  message.toCharArray(msg_buffer, message.length()+1);
  phone_number.toCharArray(phone_buffer, phone_number.length()+1);
  
  return sms.send(phone_buffer, msg_buffer);
}

/**
 * @brief   Call someone
 *
 * @param   phone_number    phone number that want to call him
 *
 * @return  true(1) if no error, false(0) otherwise
 */
bool GSM_MakeCall(String phone_number) {
  // make buffer to rcevice the data from string variable
  char phone_buffer[phone_number.length()+1];

  // copy the content of string var. to the char array
  phone_number.toCharArray(phone_buffer, phone_number.length()+1);

  return call.call(phone_buffer);
}

/**
 * @brief   Check if there are incoming SMS or Call for someone
 */
void GSM_CheckIncoming() {
  if(mySerial.available()) {
      String buffer = "";
      buffer = mySerial.readString();
      if(buffer.indexOf("+CMTI:") != -1){ // if the incoming is SMS message
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
      if(buffer.indexOf("+CLCC:") != -1){ // if the incoming is Call
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

/**
 * @brief   Make GSM go to Sleep (Power Saving mode)
 */
void GSM_Sleep() { digitalWrite(GSM_DTR, HIGH); }

/**
 * @brief   Make GSM Wake up for make a task
 */
void GSM_WakeUp() { digitalWrite(GSM_DTR, LOW); delay(50); }

#endif