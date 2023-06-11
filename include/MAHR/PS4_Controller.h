#ifndef MAHR_PS4_CONTROLLER_H_
#define MAHR_PS4_CONTROLLER_H_

#include <MAHR.h>
#include <PS4Controller.h>
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_err.h"

#define PRESS_DELAY_MS 20

uint8_t battery = 0;

// Print all variables that PS4 Controller can update
void PS4_PrintAll() {
  Serial.printf("%4d,%4d,%4d,%4d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d\n",
                PS4.LStickX(), PS4.LStickY(),
                PS4.RStickX(), PS4.RStickY(),
                PS4.Left(), PS4.Down(),
                PS4.Right(), PS4.Up(),
                PS4.Square(),
                PS4.Cross(),
                PS4.Circle(),
                PS4.Triangle(),
                PS4.L1(), PS4.R1(),
                PS4.L2(), PS4.R2(),
                PS4.L3(), PS4.R3(),
                PS4.Share(),
                PS4.Options(),
                PS4.PSButton(),
                PS4.Touchpad(),
                PS4.Charging(),
                PS4.Audio(),
                PS4.Mic(),
                PS4.Battery());
}
// make action for any required event in the PS4 Controller
void notify(){
  if     (PS4.UpRight()  ) { armX =  1.0;  armY =  1.0; }
  else if(PS4.UpLeft()   ) { armX = -1.0;  armY =  1.0; }
  else if(PS4.DownRight()) { armX =  1.0;  armY = -1.0; }
  else if(PS4.DownLeft() ) { armX = -1.0;  armY = -1.0; }
  else if(PS4.Up()       ) { armX =  0.0;  armY =  1.0; }
  else if(PS4.Down()     ) { armX =  0.0;  armY = -1.0; }
  else if(PS4.Right()    ) { armX =  1.0;  armY =  0.0; }
  else if(PS4.Left()     ) { armX = -1.0;  armY =  0.0; }
  else                     { armX =  0.0;  armY =  0.0; }

  motors_linear  =  ((float_t)PS4.LStickY())/(128.0);
  motors_angular = -((float_t)PS4.RStickX())/(128.0);
  if(abs(motors_linear )<0.2) { motors_linear  = 0.0; }
  if(abs(motors_angular)<0.2) { motors_angular = 0.0; }

  if     ( PS4.Triangle() ) { zAxis_Speed =  1000; }
  else if( PS4.Cross()    ) { zAxis_Speed = -1000; }
  else                      { zAxis_Speed =     0; }

  if     ( PS4.Circle() ) { pitch_speed =  1.0; }
  else if( PS4.Square() ) { pitch_speed = -1.0; }
  else                    { pitch_speed =  0.0; }

  if     ( PS4.R1() ) { roll_speed =  1.0; }
  else if( PS4.L1() ) { roll_speed = -1.0; }
  else                { roll_speed =  0.0; }

  if     ( PS4.R2() ) { grip_speed =  1.0; }
  else if( PS4.L2() ) { grip_speed = -1.0; }
  else                { grip_speed =  0.0; }

  // PS4_PrintAll();
}
void onConnect()    {Serial.println(F("PS4 Controller is Connected.   "));}
void onDisconnect() {
  motors_angular = 0;
  motors_linear  = 0;
  armX = 0;
  armY = 0;
  pitch_speed = 0;
  roll_speed  = 0;
  grip_speed  = 0;
  Serial.println(F("PS4 Controller is Disconnected."));
}

// PS4 Initialization
void PS4_Setup() {
  PS4.begin("e8:9e:b4:e2:e4:1c");
  //PS4.attach(notify);
  PS4.attachOnConnect(onConnect);
  PS4.attachOnDisconnect(onDisconnect);
}
// Check if any required event (that not in handler function) happen
void PS4_DataUpdate() {
  if(PS4.isConnected()){
    notify();
  }
  /*------ Digital cross/square/triangle/circle buttons ------
  if(PS4.data.button.cross    && PS4.data.button.down   ) {Serial.println(F("Pressing both the down     & cross     buttons"));}
  if(PS4.data.button.square   && PS4.data.button.left   ) {Serial.println(F("Pressing both the square   & left      buttons"));}
  if(PS4.data.button.triangle && PS4.data.button.up     ) {Serial.println(F("Pressing both the triangle & up        buttons"));}
  if(PS4.data.button.circle   && PS4.data.button.right  ) {Serial.println(F("Pressing both the circle   & right     buttons"));}
  if(PS4.data.button.l1       && PS4.data.button.r1     ) {Serial.println(F("Pressing both the left     & R.bumper  buttons"));}
  if(PS4.data.button.l2       && PS4.data.button.r2     ) {Serial.println(F("Pressing both the left     & R.trigger buttons"));}
  if(PS4.data.button.l3       && PS4.data.button.r3     ) {Serial.println(F("Pressing both the left     & R.stick   buttons"));}
  if(PS4.data.button.share    && PS4.data.button.options) {Serial.println(F("Pressing both the share    & options   buttons"));}
  */
}
// Print all variables that PS4 Controller can update
void PS4_PrintData() {
  Serial.printf("Speed(l:%4.2f,a:%4.2f)\tzAxis(%5d)\tArm(%2.0f,%2.0f)\troll(%2.0f)\tpitch(%2.0f)\tGrip(%2.0f)\n",
                motors_linear, motors_angular,
                zAxis_Speed,
                armX, armY,
                roll_speed, pitch_speed, grip_speed);
}
// use it if PS4 don't connect (remove all paired devices due to reach max number of attempts)
void PS4_Reconnect() {
  uint8_t pairedDeviceBtAddr[20][6];
  int32_t count = esp_bt_gap_get_bond_device_num();
  esp_bt_gap_get_bond_device_list(&count, pairedDeviceBtAddr);
  for(uint8_t i=0; i<count; i++) {
    esp_bt_gap_remove_bond_device(pairedDeviceBtAddr[i]);
  }
}
#endif