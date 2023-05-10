#ifndef MAHR_ZAXIS_STEPPER_H_
#define MAHR_ZAXIS_STEPPER_H_

#include <MAHR.h>
#include <AccelStepper.h>

#define STEPPER_PPR         800.0
#define PULLEY_RADIUS_CM      1.0
#define TRAVEL_DISTANCE_CM   25.0   // Distance between the Top and Bottom.

AccelStepper zAxis(1, STEPPER_STP, STEPPER_DIR);
LowPass<2> lp_Z(1e3, 100e3, true);
// Z-Axis Initialization
void zAxis_Setup(uint16_t MaxSpeed_pps, uint16_t Acceleration_ppss) {
  zAxis.setMaxSpeed(MaxSpeed_pps);
  zAxis.setAcceleration(Acceleration_ppss);
  pinMode(UPPER_LS, INPUT_PULLDOWN);
  pinMode(LOWER_LS, INPUT_PULLDOWN);
  //zAxis_Homing();
  zAxis_Speed=0;
  zAxis.setSpeed(zAxis_Speed);
}
// (1) for TOP or (-1) for BOTTOM
void zAxis_GoTo(uint8_t zPosition=-1) {
  uint8_t ls_pin; // limit switch pin.
  switch (zPosition+1) {
  case 2: ls_pin = UPPER_LS;  break;
  case 0: ls_pin = LOWER_LS;  break;
  }
  while( digitalRead(ls_pin) == 0 ) {
    zAxis.setSpeed(1000*zPosition);
    zAxis.run();
  }
  zAxis.stop();
}
// Go To Home (in the Bottom) 
void zAxis_Homing() {
  Serial.print(F("Z-Axis Homing... "));
  zAxis_GoTo(-1);
  Serial.println(F("DONE"));
}
// free move according to control speed (magnitude & direction)
void zAxis_Move() {
  int16_t zAxis_Speed_filtered = lp_Z.filt(zAxis_Speed);
  if     ( digitalRead(LOWER_LS) && zAxis_Speed_filtered < 0 ) { zAxis.stop(); }
  else if( digitalRead(UPPER_LS) && zAxis_Speed_filtered > 0 ) { zAxis.stop(); }
  else{
    zAxis.setSpeed((float)zAxis_Speed_filtered);
    zAxis.runSpeed();
  }
}

#endif