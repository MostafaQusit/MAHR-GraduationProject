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
float_t w, r, g, lsx, lsy;
uint16_t amp, other;
int16_t linearSpeed = 120;
int16_t rotatiSpeed = 120;

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
  if     ( PS4.Up()    ) { Target_LeftMotor_mms= linearSpeed; Target_RightMotor_mms= linearSpeed; }
  else if( PS4.Down()  ) { Target_LeftMotor_mms=-linearSpeed; Target_RightMotor_mms=-linearSpeed; }
  else if( PS4.Right() ) { Target_LeftMotor_mms= rotatiSpeed; Target_RightMotor_mms=-rotatiSpeed; }
  else if( PS4.Left()  ) { Target_LeftMotor_mms=-rotatiSpeed; Target_RightMotor_mms= rotatiSpeed; }
  else                   { Target_LeftMotor_mms=           0; Target_RightMotor_mms=           0; }
  /* control robot by analog
  lsx = PS4.LStickX();
  lsy = PS4.LStickY();
  if( abs(lsx) < 17 && abs(lsy) < 17 ) {
    Target_LeftMotor_mms = 0;
    Target_RightMotor_mms = 0;
  }
  else {
    if(lsy>=0) { amp = (uint16_t)  sqrtf(lsx*lsx+lsy*lsy); }
    else       { amp = (uint16_t) -sqrtf(lsx*lsx+lsy*lsy); }
    other = (uint16_t) lsy - abs(lsx);

    amp   = map(amp,   -128, 128, -100, 100);
    other = map(other, -128, 128, -100, 100);

    if(lsx >= 0) { Target_LeftMotor_mms=amp;   Target_RightMotor_mms=other; }
    else         { Target_LeftMotor_mms=other; Target_RightMotor_mms=amp;   }
  }
  */
  
  if     ( PS4.Triangle() ) { zAxis_Speed =  1000; } //z = (zAxis_Speed==25)? 25: z+0.01; zAxis_Speed=(uint16_t)z; }
  else if( PS4.Cross()    ) { zAxis_Speed = -1000; } //z = (zAxis_Speed== 0)?  0: z-0.01; zAxis_Speed=(uint16_t)z; }
  else                      { zAxis_Speed =     0; }

  if( PS4.Circle() ) { w = (wrist==90)? 90: w+0.03; wrist=(uint8_t)w; }
  if( PS4.Square() ) { w = (wrist== 0)?  0: w-0.03; wrist=(uint8_t)w; }

  if( PS4.R1() ) { r = (roll==180)? 180: r+0.06; roll=(uint8_t)r; }
  if( PS4.L1() ) { r = (roll==  0)?   0: r-0.06; roll=(uint8_t)r; }

  if( PS4.R2() ) { g = (Grip==90)? 90: g+0.03; Grip=(uint8_t)g; }
  if( PS4.L2() ) { g = (Grip== 0)?  0: g-0.03; Grip=(uint8_t)g; }

  armX = map(PS4.RStickX(), -128, 127, -100, 100);
  armY = map(PS4.RStickY(), -128, 127, -100, 100);

  // PS4_PrintAll();
}
void onConnect()    {Serial.println(F("PS4 Controller is Connected.   "));}
void onDisconnect() {Serial.println(F("PS4 Controller is Disconnected."));}

// PS4 Initialization
void PS4_Setup() {
  PS4.begin("e8:9e:b4:e2:e4:1c");
  PS4.attach(notify);
  PS4.attachOnConnect(onConnect);
  PS4.attachOnDisconnect(onDisconnect);
}
// Check if any required event (that not in handler function) happen
void PS4_DataUpdate() {
  //------ Digital cross/square/triangle/circle buttons ------
  if(PS4.data.button.cross    && PS4.data.button.down   ) {Serial.println(F("Pressing both the down     & cross     buttons"));}
  if(PS4.data.button.square   && PS4.data.button.left   ) {Serial.println(F("Pressing both the square   & left      buttons"));}
  if(PS4.data.button.triangle && PS4.data.button.up     ) {Serial.println(F("Pressing both the triangle & up        buttons"));}
  if(PS4.data.button.circle   && PS4.data.button.right  ) {Serial.println(F("Pressing both the circle   & right     buttons"));}
  if(PS4.data.button.l1       && PS4.data.button.r1     ) {Serial.println(F("Pressing both the left     & R.bumper  buttons"));}
  if(PS4.data.button.l2       && PS4.data.button.r2     ) {Serial.println(F("Pressing both the left     & R.trigger buttons"));}
  if(PS4.data.button.l3       && PS4.data.button.r3     ) {Serial.println(F("Pressing both the left     & R.stick   buttons"));}
  if(PS4.data.button.share    && PS4.data.button.options) {Serial.println(F("Pressing both the share    & options   buttons"));}
}
// Print all variables that PS4 Controller can update
void PS4_PrintData() {
  Serial.printf("Speed(%4d,%4d)\tzAxis(%5d)\tArm(%4d,%4d)\troll(%4d)\twrist(%4d)\tGrip(%4d)\n",
                Target_LeftMotor_mms, Target_RightMotor_mms,
                zAxis_Speed,
                armX, armY,
                roll, wrist, Grip);
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