#ifndef MAHR_ARM_H_
#define MAHR_ARM_H_

#include <MAHR.h>
#include <ESP32Servo.h>
#include <AccelStepper.h>

#define FULL_STEP       1.0000
#define HALF_STEP       0.5000
#define QUARTER_STEP    0.2500
#define EIGHTH_STEP     0.1250
#define SIXTEENTH_STEP  0.0625
#define MICROSTEP   HALF_STEP 

#define STEPPER_PPR     (200.0/MICROSTEP)
#define LINK1_LENGTH_MM 100.0
#define LINK2_LENGTH_MM 100.0
#define RADIUS          (sqrtf(powf(LINK1_LENGTH_MM,2)+powf(LINK2_LENGTH_MM,2)))

float_t grip_angle, pitch_angle, roll_angle;
float_t pitch_CurrAngle, roll_CurrAngle;
float_t ArmX_position, ArmY_position;
float_t link1_angle, link2_angle;

Servo rGrip, lGrip;  // create servo object to control a servo
AccelStepper link1(1, SM_LINK1_STP, SM_LINK1_DIR);
AccelStepper link2(1, SM_LINK2_STP, SM_LINK2_DIR);
AccelStepper lwrst(1, SM_LWRST_STP, SM_LWRST_DIR);
AccelStepper rwrst(1, SM_RWRST_STP, SM_RWRST_DIR);

void microstepping(float_t microstepping_resolution){
    if     (microstepping_resolution == FULL_STEP     ) {digitalWrite(MS3, LOW); digitalWrite(MS2, LOW); digitalWrite(MS1, LOW);}
    else if(microstepping_resolution == HALF_STEP     ) {digitalWrite(MS3, LOW); digitalWrite(MS2, LOW); digitalWrite(MS1,HIGH);}
    else if(microstepping_resolution == QUARTER_STEP  ) {digitalWrite(MS3, LOW); digitalWrite(MS2,HIGH); digitalWrite(MS1, LOW);}
    else if(microstepping_resolution == EIGHTH_STEP   ) {digitalWrite(MS3, LOW); digitalWrite(MS2,HIGH); digitalWrite(MS1,HIGH);}
    else if(microstepping_resolution == SIXTEENTH_STEP) {digitalWrite(MS3,HIGH); digitalWrite(MS2,HIGH); digitalWrite(MS1,HIGH);}
}

// Go Home
void Arm_Homing(){
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

    link1_angle = 0;
    link2_angle = PI/2.0;
    pitch_angle = 0;
    roll_angle  = 0;
    grip_angle  = 0;

    pitch_CurrAngle = 0;
    roll_CurrAngle  = 0;

    // Forward Kinametics for initial angles
    ArmX_position = LINK1_LENGTH_MM*cosf(link1_angle) + LINK2_LENGTH_MM*cosf(link1_angle+link2_angle);
    ArmX_position = LINK1_LENGTH_MM*sinf(link1_angle) + LINK2_LENGTH_MM*sinf(link1_angle+link2_angle);

    link1.setCurrentPosition(link1_angle/(2.0*PI) * STEPPER_PPR);
    link2.setCurrentPosition(link2_angle/(2.0*PI) * STEPPER_PPR);
    lwrst.setCurrentPosition(pitch_angle/(2.0*PI) * STEPPER_PPR);
    rwrst.setCurrentPosition(roll_angle /(2.0*PI) * STEPPER_PPR);
}

// Arm Initialization
void Arm_Setup(){
	// Allow allocation of all timers
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);

	lGrip.setPeriodHertz(50);    lGrip.attach(SERVO_RGRIP, 500, 2500);
    rGrip.setPeriodHertz(50);    rGrip.attach(SERVO_LGRIP, 500, 2500);

    microstepping(MICROSTEP);

    link1.setMaxSpeed(2000*MICROSTEP);    link1.setAcceleration(1000);
    link2.setMaxSpeed(2000*MICROSTEP);    link2.setAcceleration(1000);
    wrist.setMaxSpeed(2000*MICROSTEP);    wrist.setAcceleration(1000);
    roll .setMaxSpeed(2000*MICROSTEP);    roll .setAcceleration(1000);

    pinMode(LS_LINK1, INPUT_PULLDOWN);
    pinMode(LS_LINK2, INPUT_PULLDOWN);
    pinMode(LS_WRIST, INPUT_PULLDOWN);
    pinMode(LS_ROLL , INPUT_PULLDOWN);

    pinMode(MS1, INPUT_PULLDOWN);
    pinMode(MS2, INPUT_PULLDOWN);
    pinMode(MS3, INPUT_PULLDOWN);

    Arm_Homing();

    link1.stop();
    link2.stop();
    wrist.stop();
    roll .stop();
}

void Arm_run(){
    // Update the Target Positions:
    ArmX_position += 0.10 * armX;
    ArmY_position += 0.10 * armY;
    pitch_angle   += 0.01 * pitch_speed;
    roll_angle    += 0.01 * roll_speed;
    grip_angle    += 0.01 * grip_speed;

    // Apply the constrains on Arm End-Effector Position:
    float_t radius = sqrtf(powf(ArmX_position,2) + powf(ArmY_position,2));
    float_t theta  = atan2f(ArmY_position,ArmX_position);
    radius = constrain(radius, 20, RADIUS);

    ArmX_position = radius * cosf(theta);
    ArmY_position = radius * sinf(theta);

    // Calculate the Target Angles (inverse kinematics):
    link2_angle = acosf( (powf(ArmX_position,2) + powf(ArmY_position,2) - powf(LINK1_LENGTH_MM,2) - powf(LINK2_LENGTH_MM,2)) /
                         (2.0*LINK1_LENGTH_MM*LINK2_LENGTH_MM) )

    link1_angle = ( atan2f(ArmY_position,ArmX_position) -
                    atan2f(LINK2_LENGTH_MM*sinf(link2_angle),LINK1_LENGTH_MM+LINK2_LENGTH_MM*cosf(link2_angle)) );

    link2_angle += PI/2.0;

    // Apply the constrains on angles:
    link1_angle = constrain(link1_angle, 0, PI);
    link2_angle = constrain(link2_angle, 0, PI);
    pitch_angle = constrain(pitch_angle, 0, PI);
    roll_angle  = constrain(roll_angle , 0, PI);
    grip_angle  = constrain(grip_angle , 0, PI);
    
    // if there are any ratios in motors and joints: (map function)
    pitch_angle *= 3;
    roll_angle  *= 6;

    // Update the motor angles:
    link1.moveTo(link1_angle             /(2.0*PI) * STEPPER_PPR);
    link2.moveTo(link2_angle             /(2.0*PI) * STEPPER_PPR);
    lwrst.moveTo((pitch_angle+roll_angle)/(2.0*PI) * STEPPER_PPR);
    rwrst.moveTo((pitch_angle-roll_angle)/(2.0*PI) * STEPPER_PPR);

    // Action:
    while( link1.run() && link2.run() && lwrst.run() && rwrst.run() );
    lGrip.write(grip_angle);
    rGrip.write(grip_angle);
}

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