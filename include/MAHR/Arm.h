#ifndef MAHR_ARM_H_
#define MAHR_ARM_H_

#include <MAHR.h>
#include <ESP32Servo.h>
#include <AccelStepper.h>

#define STEPPER_PPR     800.0
#define LINK1_LENGTH_MM 100.0
#define LINK2_LENGTH_MM 100.0
#define RADIUS          (sqrtf(powf(LINK1_LENGTH_MM,2)+powf(LINK2_LENGTH_MM,2)))

float_t grip_angle, roll_angle, wrist_angle;
float_t ArmX_position, ArmY_position;
float_t link1_angle, link2_angle;

Servo rGrip, lGrip;  // create servo object to control a servo
AccelStepper link1(1, SM_LINK1_STP, SM_LINK1_DIR);
AccelStepper link2(1, SM_LINK2_STP, SM_LINK2_DIR);
AccelStepper wrist(1, SM_WRIST_STP, SM_WRIST_DIR);
AccelStepper roll (1, SM_ROLL_STP , SM_ROLL_DIR );

// Go Home
void Arm_Homing(){
    while((digitalRead(LS_LINK1) && digitalRead(LS_LINK2) &&
           digitalRead(LS_WRIST) && digitalRead(LS_ROLL )) == false){

        if(digitalRead(LS_LINK1) == false) { link1.run(); }
        if(digitalRead(LS_LINK2) == false) { link2.run(); }
        if(digitalRead(LS_WRIST) == false) { wrist.run(); }
        if(digitalRead(LS_ROLL ) == false) { roll .run(); }
    }
    lGrip.write(0);
    rGrip.write(0);

    link1.setCurrentPosition(0);
    link2.setCurrentPosition(0);
    wrist.setCurrentPosition(0);
    roll .setCurrentPosition(0);
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

    link1.setMaxSpeed(1000);    link1.setAcceleration(2000);
    link2.setMaxSpeed(1000);    link2.setAcceleration(2000);
    wrist.setMaxSpeed(1000);    wrist.setAcceleration(2000);
    roll .setMaxSpeed(1000);    roll .setAcceleration(2000);

    pinMode(LS_LINK1, INPUT_PULLDOWN);
    pinMode(LS_LINK2, INPUT_PULLDOWN);
    pinMode(LS_WRIST, INPUT_PULLDOWN);
    pinMode(LS_ROLL , INPUT_PULLDOWN);

    Arm_Homing();

    link1.stop();
    link2.stop();
    wrist.stop();
    roll .stop();
}

void Arm_run(){
    // Update the Target Positions:
    ArmX_position += 0.001 * armX;
    ArmY_position += 0.001 * armY;
    wrist_angle   += 0.001 * wrist_speed;
    roll_angle    += 0.001 * roll_speed;
    grip_angle    += 0.001 * grip_speed;

    // Apply the constrains on Arm End-Effector Position:
    float_t radius = sqrtf(powf(ArmX_position,2) + powf(ArmY_position,2));
    float_t theta  = tanhf(ArmY_position/ArmX_position);
    radius = constrain(radius, 10, RADIUS);

    ArmX_position = radius * cosf(theta);
    ArmY_position = radius * sinf(theta);

    // Calculate the Target Angles (inverse kinematics):


    // Apply the constrains on angles:
    link1_angle = constrain(link1_angle, 0, 180);
    link2_angle = constrain(link2_angle, 0, 180);
    wrist_angle = constrain(wrist_angle, 0, 180);
    roll_angle  = constrain(roll_angle , 0, 180);
    grip_angle  = constrain(grip_angle , 0, 180);

    // Update the Target Angles (convert from degree to Steps):
    link1.moveTo(link1_angle/360 * STEPPER_PPR);
    link2.moveTo(link2_angle/360 * STEPPER_PPR);
    wrist.moveTo(wrist_angle/360 * STEPPER_PPR);
    roll .moveTo(roll_angle /360 * STEPPER_PPR);

    // Action:
    while( link1.run() && link2.run() && wrist.run() && roll.run() );
    lGrip.write(grip_angle);
    rGrip.write(grip_angle);
}

#endif