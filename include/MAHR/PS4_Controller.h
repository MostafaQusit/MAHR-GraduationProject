#ifndef MAHR_PS4_CONTROLLER_H_
#define MAHR_PS4_CONTROLLER_H_

#include <MAHR.h>
#include <PS4Controller.h>
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_err.h"

/** if PS4 don't connect, this one of two reasons:
 *    1. the ESP32 reach max. number of attempts with devices
 *        #solution -> run Remove_AllPairedDevice() function in empty sketch and and run you code again
 *
 *    2. flash memory problem
 *        #solution -> Erase Flash Memory (Factory Reset):
 *                        A. install Python 3.7 or newer installed on your system
 *                        B. install esptool (write "pip install esptool" in terminal window)
 *                        C. write "python -m esptool --chip esp32 erase_flash" in terminal window while
 *                           holding on BOOT/FLASH Button and release it when the Erasing process begins
 */


/**
 * @brief   remove all paired devices due to reach max number of attempts
 */
void Remove_AllPairedDevice() {
  uint8_t pairedDeviceBtAddr[20][6];
  int32_t count = esp_bt_gap_get_bond_device_num();
  esp_bt_gap_get_bond_device_list(&count, pairedDeviceBtAddr);
  Serial.print("remove bond device");
  for(int32_t i=0; i<count; i++) {
    esp_bt_gap_remove_bond_device(pairedDeviceBtAddr[i]);
    Serial.print(".");
  }
  Serial.println("\nFinish");
}

/**
 * @brief   call-back function when robot mode is Manual mode
 */
void PS4_ManualMode() {
  robot_mode = MANUAL_MODE;   // change robot mode to Manual mode
  PS4.setLed(0,255,0);        // Set PS4 led to Green
  PS4.sendToController();     // Send this setting to PS4 Controller
  voice_file = MANUAL_ON;
  ESPNOW1_Send();
}

/**
 * @brief   call-back function when when robot mode is Autonomous mode
 */
void PS4_AutonoMode() {
  // Reset all parameters (zeroing):
  motors_angular    = 0;
  motors_linear     = 0;
  arm_directions[X] = 0;
  arm_directions[Y] = 0;
  arm_directions[P] = 0;
  arm_directions[R] = 0;
  arm_directions[G] = 0;

  robot_mode = AUTONO_MODE;   // change robot mode to Autonomous mode
  PS4.setLed(255, 0, 0);      // Set PS4 led to Red
  PS4.sendToController();     // Send this setting to PS4 Controller
  voice_file = MANUAL_OFF;
  ESPNOW1_Send();
}

/**
 * @brief   PS4 Setup
 */
void PS4_Setup() {
  PS4.begin("e8:9e:b4:e2:e4:1c");
  PS4.attachOnConnect(PS4_ManualMode);
  PS4.attachOnDisconnect(PS4_AutonoMode);
}

/**
 * @brief Check if any required event happen
 *
 * @attention   that not in handler function
 */
void PS4_DataUpdate() {
  if(PS4.isConnected()){
    // End-Effector planer linear speed in XY axises
    if     (PS4.UpRight()  ) { arm_directions[X] =  1.0;  arm_directions[Y] =  1.0; }
    else if(PS4.UpLeft()   ) { arm_directions[X] = -1.0;  arm_directions[Y] =  1.0; }
    else if(PS4.DownRight()) { arm_directions[X] =  1.0;  arm_directions[Y] = -1.0; }
    else if(PS4.DownLeft() ) { arm_directions[X] = -1.0;  arm_directions[Y] = -1.0; }
    else if(PS4.Up()       ) { arm_directions[X] =  0.0;  arm_directions[Y] =  1.0; }
    else if(PS4.Down()     ) { arm_directions[X] =  0.0;  arm_directions[Y] = -1.0; }
    else if(PS4.Right()    ) { arm_directions[X] =  1.0;  arm_directions[Y] =  0.0; }
    else if(PS4.Left()     ) { arm_directions[X] = -1.0;  arm_directions[Y] =  0.0; }
    else                     { arm_directions[X] =  0.0;  arm_directions[Y] =  0.0; }

    // Robot motion speed:
    motors_linear  =  ((float_t)PS4.LStickY())/(128.0) * (PS4.L3()? 1.0:0.6);
    motors_angular = -((float_t)PS4.RStickX())/(128.0) * (PS4.R3()? 1.0:0.7);
    if(abs(motors_linear )<0.2) { motors_linear  = 0.0; }
    if(abs(motors_angular)<0.2) { motors_angular = 0.0; }

    // Z-Axis Stepper Speed
    if     ( PS4.Triangle() ) { zAxis_direction =  1; }
    else if( PS4.Cross()    ) { zAxis_direction = -1; }
    else                      { zAxis_direction =  0; }

    // Arm: pitch speed
    if     ( PS4.Circle() ) { arm_directions[P] =  1.0; }
    else if( PS4.Square() ) { arm_directions[P] = -1.0; }
    else                    { arm_directions[P] =  0.0; }

    // Arm: roll speed
    if     ( PS4.R1() ) { arm_directions[R] =  1.0; }
    else if( PS4.L1() ) { arm_directions[R] = -1.0; }
    else                { arm_directions[R] =  0.0; }

    // Arm: grip speed
    if     ( PS4.R2() ) { arm_directions[G] =  1.0; }
    else if( PS4.L2() ) { arm_directions[G] = -1.0; }
    else                { arm_directions[G] =  0.0; }
  }
}

/**
 * @brief   update modes & Options by PS4 command
 *           (options  button = Manual     Mode) 
 *           (share    button = Autonomous Mode) 
 *           (PS       button = Base       Mode) 
 *           (TouchPad button = Arm        Mode) 
 */
void PS4_OptionsUpdate(){
  if(PS4.isConnected()){
     // change robot mode and PS4-Led accordingly
    if( PS4.Options() ) { PS4_ManualMode(); }
    if( PS4.Share()   ) { PS4_AutonoMode(); }
    
    if( PS4.PSButton() ) {
      PS4.setFlashRate(100,  0);
      PS4.sendToController();
      control_mode = BASE_MODE;
      voice_file = BASE_ON;
      ESPNOW1_Send();
    }
    if( PS4.Touchpad() ) {
      PS4.setFlashRate( 50, 50);
      PS4.sendToController();
      control_mode =  ARM_MODE;
      voice_file =  ARM_ON;
      ESPNOW1_Send();
    }
  }
}

/**
 * @brief   Print all variables of PS4 Controller
 */
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

/**
 * @brief   Print all variables that PS4 Controller can update
 */
void PS4_PrintData() {
  Serial.printf("Speed(l:%4.2f,a:%4.2f)\tzAxis(%2.0f)\tArm(%2.0f,%2.0f)\tpitch(%2.0f)\troll(%2.0f)\tGrip(%2.0f)\n",
                motors_linear,
                motors_angular,
                zAxis_direction,
                arm_directions[X],
                arm_directions[Y],
                arm_directions[P],
                arm_directions[R],
                arm_directions[G]);
}

#endif