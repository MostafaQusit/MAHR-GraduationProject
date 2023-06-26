#ifndef MAHR_ARM_H_
#define MAHR_ARM_H_

#include <MAHR.h>
#include <ESP32Servo.h>
#include <AccelStepper.h>

// Micro-stepping Modes
#define FULL_STEP       1.0000
#define HALF_STEP       0.5000
#define QUARTER_STEP    0.2500
#define EIGHTH_STEP     0.1250
#define SIXTEENTH_STEP  0.0625
#define MICROSTEP       EIGHTH_STEP 

#define STEPPER_PPR     (200.0/MICROSTEP)   // Stepper Resolution in (Pulse Per Resolution -> PPR)
#define LINK1_LENGTH_MM 185.0               // link1 length in (mm)
#define LINK2_LENGTH_MM 203.6               // link2 length in (mm)
#define MAX_RADIUS      (LINK1_LENGTH_MM + LINK2_LENGTH_MM)                      - 20    // Max Radius can robotic arm reach in (mm)
#define MIN_RADIUS      (sqrtf(powf(LINK1_LENGTH_MM,2)+powf(LINK2_LENGTH_MM,2))) + 20    // Min Radius can robotic arm reach in (mm)

float_t armX_position;  // End-Effector planer position in X-axis in (mm)
float_t armY_position;  // End-Effector planer position in Y-axis in (mm)

float_t link1_angle;    // link1 joint angle in (radian)
float_t link2_angle;    // link2 joint angle in (radian)
float_t grip_angle;     // Grip  joint angle in (radian)
float_t pitch_angle;    // Pitch joint angle in (radian)
float_t roll_angle;     // Roll  joint angle in (radian)

Servo grip;  // grip servo object
AccelStepper link1(1, SM_LINK1_STP, SM_LINK1_DIR);  // link1       stepper-motor object
AccelStepper link2(1, SM_LINK2_STP, SM_LINK2_DIR);  // link2       stepper-motor object
AccelStepper lwrst(1, SM_LWRST_STP, SM_LWRST_DIR);  // left  wrist stepper-motor object
AccelStepper rwrst(1, SM_RWRST_STP, SM_RWRST_DIR);  // right wrist stepper-motor object

/**
 * @brief   set the 3 mode pins accordingly to micro-stepping
 *
 * @param   microstepping_resolution    micro-stepping resolution (FULL_STEP - HALF_STEP - QUARTER_STEP - 
 *                                                                              EIGHTH_STEP - SIXTEENTH_STEP)
 */
void microstepping(float_t microstepping_resolution){
    if     (microstepping_resolution == FULL_STEP     ) {digitalWrite(MS3, LOW); digitalWrite(MS2, LOW); digitalWrite(MS1, LOW);}
    else if(microstepping_resolution == HALF_STEP     ) {digitalWrite(MS3, LOW); digitalWrite(MS2, LOW); digitalWrite(MS1,HIGH);}
    else if(microstepping_resolution == QUARTER_STEP  ) {digitalWrite(MS3, LOW); digitalWrite(MS2,HIGH); digitalWrite(MS1, LOW);}
    else if(microstepping_resolution == EIGHTH_STEP   ) {digitalWrite(MS3, LOW); digitalWrite(MS2,HIGH); digitalWrite(MS1,HIGH);}
    else if(microstepping_resolution == SIXTEENTH_STEP) {digitalWrite(MS3,HIGH); digitalWrite(MS2,HIGH); digitalWrite(MS1,HIGH);}
    else                                                {Serial.println("that micro-step is not found");                        }
}

/**
 * @brief   make robotic arm go to home position
 */
void Arm_Homing(){
    // move all joint until reach all limit switches:
    Serial.print("Homing");
    link1.setSpeed( 100/MICROSTEP);
    link2.setSpeed(-100/MICROSTEP);
    lwrst.setSpeed(-100/MICROSTEP);
    rwrst.setSpeed(-100/MICROSTEP);
    while((digitalRead(LS_LINK1) || digitalRead(LS_LINK2) || digitalRead(LS_PITCH)) == true){
        if(digitalRead(LS_LINK1) == true) { link1.runSpeed();}
        if(digitalRead(LS_LINK2) == true) { link2.runSpeed();}
        if(digitalRead(LS_PITCH) == true) { lwrst.runSpeed();
                                            rwrst.runSpeed();}
    }
    lwrst.setSpeed(-100/MICROSTEP);
    rwrst.setSpeed( 100/MICROSTEP);
    while(digitalRead(LS_ROLL) == true){
        lwrst.runSpeed();
        rwrst.runSpeed();
    }
    grip.write(0);
    Serial.println(" -> DONE");

    // set the initial angles:
    link1_angle =   0.0;
    link2_angle =  83.0;
    pitch_angle =   0.0;
    roll_angle  =   0.0;
    grip_angle  =   0.0;
    
    link1.setCurrentPosition(  -(45.0/4.0)*link1_angle         /360.0 * STEPPER_PPR);
    link2.setCurrentPosition(   (45.0/4.0)*(90-link2_angle)    /360.0 * STEPPER_PPR);
    lwrst.setCurrentPosition((3.0*pitch_angle + 6.0*roll_angle)/360.0 * STEPPER_PPR);
    rwrst.setCurrentPosition((3.0*pitch_angle - 6.0*roll_angle)/360.0 * STEPPER_PPR);

    // Forward Kinametics for initial angles
    armX_position = -(LINK1_LENGTH_MM*cosf(D2R(link1_angle)) + LINK2_LENGTH_MM*cosf(D2R(link1_angle+link2_angle)));
    armY_position =   LINK1_LENGTH_MM*sinf(D2R(link1_angle)) + LINK2_LENGTH_MM*sinf(D2R(link1_angle+link2_angle));
}

/**
 * @brief   Arm Setup
 *
 * @param   MaxSpeed_pps        Max Speed    in (Pulse Per Sec   -> PPS )
 * @param   Acceleration_ppss   Acceleration in (Pulse Per Sec^2 -> PPSS)
 */
void Arm_Setup(float_t MaxSpeed_pps, float_t Acceleration_ppss){
    // Servo Setup:
	    // Allow allocation of all timers (fro servo library):
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);

        // Set servo frequency to 50 Hz
    grip.setPeriodHertz(50);

        // Set servo pin and min and max pulse width in (uS) where [0~180]deg -> [min~max]uS
        // min=500, max=2500 is the parameters for MG996R Metallic Servo
    grip.attach(SERVO_GRIP, 500, 2500);

    // Steppers Setup:
    microstepping(MICROSTEP);   // set the required micro-stepping

    // set the max speed and acceleration:
    link1.setMaxSpeed(MaxSpeed_pps/MICROSTEP);    link1.setAcceleration(Acceleration_ppss);
    link2.setMaxSpeed(MaxSpeed_pps/MICROSTEP);    link2.setAcceleration(Acceleration_ppss);
    lwrst.setMaxSpeed(MaxSpeed_pps/MICROSTEP);    lwrst.setAcceleration(Acceleration_ppss);
    rwrst.setMaxSpeed(MaxSpeed_pps/MICROSTEP);    rwrst.setAcceleration(Acceleration_ppss);

    // limit switches setup:
    pinMode(LS_LINK1, INPUT);
    pinMode(LS_LINK2, INPUT);
    pinMode(LS_PITCH, INPUT);
    pinMode(LS_ROLL , INPUT);

    // micro-stepping mode pins setup:
    pinMode(MS1, OUTPUT);
    pinMode(MS2, OUTPUT);
    pinMode(MS3, OUTPUT);
    
    Arm_Homing();   // homing every time the code start (for resetting the stepper motors)

    // Stop all stepper motors:
    link1.stop();
    link2.stop();
    lwrst.stop();
    rwrst.stop();
}

/**
 * @brief   Update Arm parameters (angles)
 */
void Arm_AnglesUpdate(){
    if(robot_mode == MANUAL_MODE){
        // Update the Target Positions:
        armX_position += /*0.03*/ 0.04 * arm_directions[X];
        armY_position += /*0.03*/ 0.04 * arm_directions[Y];
        pitch_angle   += /*0.04*/ 0.05 * arm_directions[P];
        roll_angle    += /*0.03*/ 0.04 * arm_directions[R];
        grip_angle    += /*0.04*/ 0.04 * arm_directions[G];
        
        // Apply the constrains on Arm End-Effector Position:
        float_t radius = sqrtf(powf(armX_position,2) + powf(armY_position,2));
        float_t theta  = atan2f(armY_position,armX_position);
        radius = constrain(radius, MIN_RADIUS, MAX_RADIUS);

        // Get corrected positions:
        armX_position = radius * cosf(theta);
        armY_position = radius * sinf(theta);

        // Calculate the Target Angles (inverse kinematics):
        link2_angle = acosf( (armX_position*armX_position + armY_position*armY_position
                              - LINK1_LENGTH_MM*LINK1_LENGTH_MM - LINK2_LENGTH_MM*LINK2_LENGTH_MM) /
                             (2.0*LINK1_LENGTH_MM*LINK2_LENGTH_MM) );

        link1_angle = ( atan2f(armY_position,-armX_position) -
                        atan2f(LINK2_LENGTH_MM*sinf(link2_angle),LINK1_LENGTH_MM+LINK2_LENGTH_MM*cosf(link2_angle)) );

        // convert from radian to degree:
        link1_angle = R2D(link1_angle);
        link2_angle = R2D(link2_angle);
    }
    else{   // robot_mode = AUTONO_MODE
        link1_angle = arm_angles[A];
        link2_angle = arm_angles[B];
        pitch_angle = arm_angles[P];
        roll_angle  = arm_angles[R];
        grip_angle  = arm_angles[G];
    }

    // Apply the constrains on angles:
    link1_angle = constrain(link1_angle,  0.0, 180.0);
    link2_angle = constrain(link2_angle,-90.0,  90.0);
    pitch_angle = constrain(pitch_angle,  0.0,  90.0);
    roll_angle  = constrain(roll_angle ,  0.0, 180.0);
    grip_angle  = constrain(grip_angle ,  0.0,  50.0);

    // Update the motor angles (degree -> Pulses) + apply any ratios between motors and joints + offsets + direction change
    link1.moveTo((long)( -(45.0/4.0)*link1_angle          /360.0 * STEPPER_PPR));
    link2.moveTo((long)(  (45.0/4.0)*(90-link2_angle)     /360.0 * STEPPER_PPR));
    lwrst.moveTo((long)((3.0*pitch_angle + 6.0*roll_angle)/360.0 * STEPPER_PPR));
    rwrst.moveTo((long)((3.0*pitch_angle - 6.0*roll_angle)/360.0 * STEPPER_PPR));
}

/**
 * @brief   Run all joint simultaneously to reach the required position of robotic arm
 */
void Arm_RunToPosition(){ 
    // idea 1.0: (setSpeed & runSpeed) together [Sequential]
    link1.setSpeed( 2000/MICROSTEP * sign<long>(link1.distanceToGo()) );    link1.runSpeed();
    link2.setSpeed( 2000/MICROSTEP * sign<long>(link2.distanceToGo()) );    link2.runSpeed();
    lwrst.setSpeed( 2000/MICROSTEP * sign<long>(lwrst.distanceToGo()) );    lwrst.runSpeed();
    rwrst.setSpeed( 2000/MICROSTEP * sign<long>(rwrst.distanceToGo()) );    rwrst.runSpeed();


    /* idea 2.0: (setSpeed & runSpeed) together [loop]
    while((link1.distanceToGo() != 0 || link1.speed()) || 
          (link2.distanceToGo() != 0 || link2.speed()) || 
          (lwrst.distanceToGo() != 0 || lwrst.speed()) || 
          (rwrst.distanceToGo() != 0 || rwrst.speed())   ){

            link1.setSpeed( 500/MICROSTEP * sign<long>(link1.distanceToGo()) );    link1.runSpeed();
            link2.setSpeed( 500/MICROSTEP * sign<long>(link2.distanceToGo()) );    link2.runSpeed();
            lwrst.setSpeed( 500/MICROSTEP * sign<long>(lwrst.distanceToGo()) );    lwrst.runSpeed();
            rwrst.setSpeed( 500/MICROSTEP * sign<long>(rwrst.distanceToGo()) );    rwrst.runSpeed();
    }
    */

    /* idea 3.0: (run) [Sequential]
    link1.run();
    link2.run();
    lwrst.run();
    rwrst.run();
    */

    /* idea 4.0: (run) [loop]
    while(link1.run() && link2.run() && lwrst.run() && rwrst.run());
    */


    grip.write(1.0 * grip_angle);
}

/**
 * @brief   print all the Arm parameters
 */
void Arm_PrintData(){
    Serial.printf("Arm(%5.1f,%5.1f)mm->(%4.1f,%4.1f)deg\tpitch(%4.1f)deg\troll(%4.1f)deg\tgrip(%4.1f)deg\n",
                   armX_position,
                   armY_position,
                   link1_angle,
                   link2_angle,
                   pitch_angle,
                   roll_angle,
                   grip_angle);
}

#endif