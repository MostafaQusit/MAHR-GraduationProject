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
#define MICROSTEP       HALF_STEP 

#define STEPPER_PPR     (200.0/MICROSTEP)   // Stepper Resolution in (Pulse Per Resolution -> PPR)
#define LINK1_LENGTH_MM 100.0               // link1 length in (mm)
#define LINK2_LENGTH_MM 100.0               // link2 length in (mm)
#define MAX_RADIUS      (sqrtf(powf(LINK1_LENGTH_MM,2)+powf(LINK2_LENGTH_MM,2)))    // Max Radius can robotic arm reach in (mm)

float_t grip_angle;     // Grip  joint angle in (radian)
float_t pitch_angle;    // Pitch joint angle in (radian)
float_t roll_angle;     // Roll  joint angle in (radian)
float_t ArmX_position;  // End-Effector planer position in X-axis in (mm)
float_t ArmY_position;  // End-Effector planer position in Y-axis in (mm)

float_t link1_angle;    // link1 joint angle in (radian)
float_t link2_angle;    // link2 joint angle in (radian)

Servo rGrip, lGrip;  // grip servos objects
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
    while((digitalRead(LS_LINK1) && digitalRead(LS_LINK2) && digitalRead(LS_PITCH)) == false){
        if(digitalRead(LS_LINK1) == false) { link1.setSpeed(-1000*MICROSTEP);    link1.run(); }
        if(digitalRead(LS_LINK2) == false) { link2.setSpeed(-1000*MICROSTEP);    link2.run(); }
        if(digitalRead(LS_PITCH) == false) { lwrst.setSpeed(-1000*MICROSTEP);    lwrst.run();
                                             rwrst.setSpeed(-1000*MICROSTEP);    rwrst.run(); }
    }
    while(digitalRead(LS_ROLL) == false){
        lwrst.setSpeed(-1000*MICROSTEP);    lwrst.run();
        rwrst.setSpeed( 1000*MICROSTEP);    rwrst.run();
    }
    lGrip.write(0);
    rGrip.write(0);

    // set the initial angles:
    link1_angle = 0;
    link2_angle = PI/2.0;
    pitch_angle = 0;
    roll_angle  = 0;
    grip_angle  = 0;
    
    link1.setCurrentPosition(link1_angle/(2.0*PI) * STEPPER_PPR);
    link2.setCurrentPosition(link2_angle/(2.0*PI) * STEPPER_PPR);
    lwrst.setCurrentPosition(pitch_angle/(2.0*PI) * STEPPER_PPR);
    rwrst.setCurrentPosition(roll_angle /(2.0*PI) * STEPPER_PPR);

    // Forward Kinametics for initial angles
    ArmX_position = LINK1_LENGTH_MM*cosf(link1_angle) + LINK2_LENGTH_MM*cosf(link1_angle+link2_angle);
    ArmX_position = LINK1_LENGTH_MM*sinf(link1_angle) + LINK2_LENGTH_MM*sinf(link1_angle+link2_angle);
}

/**
 * @brief   Arm Setup
 *
 * @param   MaxSpeed_pps        Max Speed    in (Pulse Per Sec   -> PPS )
 * @param   Acceleration_ppss   Acceleration in (Pulse Per Sec^2 -> PPSS)
 */
void Arm_Setup(float_t MaxSpeed_pps, float_t Acceleration_ppss){
    // Servo Setup:
	//      Allow allocation of all timers (fro servo library):
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);

    //      Set servo frequency to 50 Hz
    lGrip.setPeriodHertz(50);
    rGrip.setPeriodHertz(50);

    //      Set servo pins and min and max pulse width in (uS) where [0~180]deg -> [min~max]uS
    //      min=500, max=2500 is the parameters for MG996R Metallic Servo
    lGrip.attach(SERVO_RGRIP, 500, 2500);
    rGrip.attach(SERVO_LGRIP, 500, 2500);

    // Steppers Setup:
    microstepping(MICROSTEP);   // set the required micro-stepping

    // set the max speed and acceleration:
    link1.setMaxSpeed(MaxSpeed_pps/MICROSTEP);    link1.setAcceleration(Acceleration_ppss);
    link2.setMaxSpeed(MaxSpeed_pps/MICROSTEP);    link2.setAcceleration(Acceleration_ppss);
    lwrst.setMaxSpeed(MaxSpeed_pps/MICROSTEP);    lwrst.setAcceleration(Acceleration_ppss);
    rwrst.setMaxSpeed(MaxSpeed_pps/MICROSTEP);    rwrst.setAcceleration(Acceleration_ppss);

    // limit switches setup:
    pinMode(LS_LINK1, INPUT_PULLDOWN);
    pinMode(LS_LINK2, INPUT_PULLDOWN);
    pinMode(LS_PITCH, INPUT_PULLDOWN);
    pinMode(LS_ROLL , INPUT_PULLDOWN);

    // micro-stepping mode pins setup:
    pinMode(MS1, INPUT_PULLDOWN);
    pinMode(MS2, INPUT_PULLDOWN);
    pinMode(MS3, INPUT_PULLDOWN);

    Arm_Homing();   // homing every time the code start (for resetting the stepper motors)

    // Stop all stepper motors:
    link1.stop();
    link2.stop();
    lwrst.stop();
    rwrst.stop();
}

/**
 * @brief   Run all joint simultaneously to reach the required position of robotic arm
 */
void Arm_RunToPosition(){
    // Update the Target Positions:
    ArmX_position += 0.10 * armX_direction;
    ArmY_position += 0.10 * armY_direction;
    pitch_angle   += 0.01 * pitch_direction;
    roll_angle    += 0.01 * roll_direction;
    grip_angle    += 0.01 * grip_direction;

    // Apply the constrains on Arm End-Effector Position:
    float_t radius = sqrtf(powf(ArmX_position,2) + powf(ArmY_position,2));
    float_t theta  = atan2f(ArmY_position,ArmX_position);
    radius = constrain(radius, 20, MAX_RADIUS);

    // Get corrected positions:
    ArmX_position = radius * cosf(theta);
    ArmY_position = radius * sinf(theta);

    // Calculate the Target Angles (inverse kinematics):
    link2_angle = acosf( (powf(ArmX_position,2) + powf(ArmY_position,2) - powf(LINK1_LENGTH_MM,2) - powf(LINK2_LENGTH_MM,2)) /
                         (2.0*LINK1_LENGTH_MM*LINK2_LENGTH_MM) );

    link1_angle = ( atan2f(ArmY_position,ArmX_position) -
                    atan2f(LINK2_LENGTH_MM*sinf(link2_angle),LINK1_LENGTH_MM+LINK2_LENGTH_MM*cosf(link2_angle)) );

    link2_angle += PI/2.0;

    // Apply the constrains on angles:
    link1_angle = constrain(link1_angle, 0, PI);
    link2_angle = constrain(link2_angle, 0, PI);
    pitch_angle = constrain(pitch_angle, 0, PI);
    roll_angle  = constrain(roll_angle , 0, PI);
    grip_angle  = constrain(grip_angle , 0, PI);
    
    // Apply any ratios between motors and joints: (map function)
    pitch_angle *= 3;
    roll_angle  *= 6;

    // Update the motor angles: (radian -> Pulses)
    link1.moveTo((long)(link1_angle             /(2.0*PI) * STEPPER_PPR));
    link2.moveTo((long)(link2_angle             /(2.0*PI) * STEPPER_PPR));
    lwrst.moveTo((long)((pitch_angle+roll_angle)/(2.0*PI) * STEPPER_PPR));
    rwrst.moveTo((long)((pitch_angle-roll_angle)/(2.0*PI) * STEPPER_PPR));

    // Control action:
    while( link1.run() && link2.run() && lwrst.run() && rwrst.run() );  // simultaneous motion
    lGrip.write(grip_angle);
    rGrip.write(grip_angle);
}

/**
 * @brief   print all the Arm parameters
 */
void Arm_PrintData(){
    Serial.printf("Arm(%5.1f,%5.1f)mm->(%4.1f,%4.1f)deg\tpitch(%4.1f)deg\troll(%4.1f)deg\tgrip(%4.1f)deg\n",
                   ArmX_position,
                   ArmY_position,
                   link1_angle*(180.0/PI),
                   link2_angle*(180.0/PI),
                   pitch_angle*(180.0/PI),
                   roll_angle *(180.0/PI),
                   grip_angle *(180.0/PI));
}

#endif