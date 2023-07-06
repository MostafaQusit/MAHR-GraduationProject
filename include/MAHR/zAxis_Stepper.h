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
#define MICROSTEP       FULL_STEP

#define STEPPER_PPR         (200.0/MICROSTEP)                     // Stepper resolution in (pulse per resolution -> PPR)
#define PULLEY_RADIUS_MM     10.0                                 // Pulley Radius in (mm)
#define MAX_TRAVEL_MM       250.0                                 // Max Travel between the Top and Bottom in (mm)
#define MAX_TRAVEL_DEG      R2D(MAX_TRAVEL_MM/PULLEY_RADIUS_MM)   // Max Travel between the Top and Bottom in (degree)

AccelStepper zAxis(1, STEPPER_STP, STEPPER_DIR); // Z-Axis stepper-motor object

float_t zAxis_angle;    // Z-Axis Angle from home point [0~ 25] in (radian)

/**
 * @brief   Go to any of exterme positions
 * 
 * @param   zPosition   exterme position: (1) for TOP, (-1) for BOTTOM
 */
void zAxis_GoTo(int8_t zPosition) {
  uint8_t ls_pin; // limit switch pin

  // check the responsible limit switch according to the extreme position
  switch (zPosition+1) {
  case 2: ls_pin = UPPER_LS;  break;
  case 0: ls_pin = LOWER_LS;  break;
  }

  // move until reach the limit switch:
  zAxis.setSpeed(250/MICROSTEP * zPosition);
  while( digitalRead(ls_pin) == 0 ) {
    zAxis.runSpeed();
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
  zAxis_angle = 0;
  zAxis.setCurrentPosition(zAxis_angle);
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
}

/**
 * @brief   free move according to control speed direction within the limits
 */
void zAxis_RunSpeed() {
  // stop if you in BOTTOM position and want to go down more:
  if     ( digitalRead(LOWER_LS) && zAxis_direction < 0 ) { zAxis.stop(); }
  // stop if you in TOP position and want to go up more:
  else if( digitalRead(UPPER_LS) && zAxis_direction > 0 ) { zAxis.stop(); }
  // else move freely:
  else{
    zAxis.setSpeed(200/MICROSTEP * zAxis_direction);
    zAxis.runSpeed();
  }
}

/**
 * @brief   Run the stepper-motor to reach the required height
 */
void zAxis_RunToPosition() {
  zAxis_position = constrain(zAxis_position, 0, MAX_TRAVEL_MM);  // check the limit
  zAxis_angle = zAxis_position / PULLEY_RADIUS_MM;               // Convert from linear motion to angular motion
  // Update the motor angle: (radian -> Pulses) and move to the position:
  zAxis.runToNewPosition((long)(zAxis_angle/(2.0*PI) * STEPPER_PPR));
}

#endif