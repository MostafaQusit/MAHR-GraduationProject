#ifndef MAHR_ZAXIS_STEPPER_H_
#define MAHR_ZAXIS_STEPPER_H_

#include <MAHR.h>
#include <AccelStepper.h>

// Micro-stepping Modes
#define FULL_STEP       1.0000
#define HALF_STEP       0.5000
#define QUARTER_STEP    0.2500
#define EIGHTH_STEP     0.1250
#define SIXTEENTH_STEP  0.0625
#define MICROSTEP       QUARTER_STEP

#define STEPPER_PPR         (200.0/MICROSTEP)   // Stepper resolution in (pulse per resolution -> PPR)
#define PULLEY_RADIUS_CM     10.0               // Pulley Radius in (mm)
#define TRAVEL_DISTANCE_CM  250.0               // Distance between the Top and Bottom in (mm)

AccelStepper zAxis(1, STEPPER_STP, STEPPER_DIR); // Z-Axis stepper-motor object

float_t zAxis_Distance;   // Z-Axis Distance (Height) from home point [0~250] in (mm)
float_t zAxis_Angle;      // Z-Axis Angle             from home point [0~ 25] in (radian)


/**
 * @brief   Go to any of exterme positions
 * 
 * @param   zPosition   exterme position: (1) for TOP, (-1) for BOTTOM
 */
void zAxis_GoTo(uint8_t zPosition=-1) {
  uint8_t ls_pin; // limit switch pin

  // check the responsible limit switch according to the extreme position
  switch (zPosition+1) {
  case 2: ls_pin = UPPER_LS;  break;
  case 0: ls_pin = LOWER_LS;  break;
  }

  // move until reach the limit switch:
  while( digitalRead(ls_pin) == 0 ) {
    zAxis.setSpeed(1000/MICROSTEP * zPosition);
    zAxis.run();
  }

  zAxis.stop(); // stop the stepper motor
}

/**
 * @brief   make robotic arm go to home position (BOTTOM position)
 */
void zAxis_Homing() {
  Serial.print(F("Z-Axis Homing... "));
  zAxis_GoTo(-1);   // Go to BOTTOM exterme position
  Serial.println(F("DONE"));
}

/**
 * @brief   Z-Axis Setup
 *
 * @param   MaxSpeed_pps        Max Speed    in (Pulse Per Sec   -> PPS )
 * @param   Acceleration_ppss   Acceleration in (Pulse Per Sec^2 -> PPSS)
 */
void zAxis_Setup(float_t MaxSpeed_pps, float_t Acceleration_ppss) {
  // set the max speed and acceleration:
  zAxis.setMaxSpeed(MaxSpeed_pps/MICROSTEP);
  zAxis.setAcceleration(Acceleration_ppss);

  // limit switches setup:
  pinMode(UPPER_LS, INPUT_PULLDOWN);
  pinMode(LOWER_LS, INPUT_PULLDOWN);

  zAxis_Homing(); // homing every time the code start (for resetting the stepper motors)
  
  zAxis.stop(); // stop the stepper motor
}

/**
 * @brief   free move according to control speed direction within the limits
 */
void zAxis_RunSpeed() {
  if     ( digitalRead(LOWER_LS) && zAxis_direction < 0 ) { zAxis.stop(); }
  else if( digitalRead(UPPER_LS) && zAxis_direction > 0 ) { zAxis.stop(); }
  else{
    zAxis.setSpeed(1000/MICROSTEP * zAxis_direction);
    zAxis.runSpeed();
  }
}

/**
 * @brief   Run the stepper-motor to reach the required height
 */
void zAxis_RunToPosition() {
  zAxis_Distance += 0.1 * zAxis_direction;                    // Update the Distance
  zAxis_Angle = zAxis_Distance/PULLEY_RADIUS_CM;              // Convert from linear motion to angular motion
  zAxis.moveTo((long)(zAxis_Angle/(2.0*PI) * STEPPER_PPR));   // Update the motor angle: (radian -> Pulses)
  zAxis.run();                                                // move step over step until reach the Target
}

#endif