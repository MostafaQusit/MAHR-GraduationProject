#ifndef MAHR_ULTRASONICS_H_
#define MAHR_ULTRASONICS_H_

#include <MAHR.h>
#include <NewPing.h>

#define MIN_DISTANCE      3     // Minimum distance (in cm) to ping.
#define MAX_DISTANCE    200     // Maximum distance (in cm) to ping.
#define BREAK_DISTANCE  20.0    // distance that break action activate (in cm)

NewPing Ping[4] = { // Sensor object array
    NewPing(USTX_FL, USRX_FL, MAX_DISTANCE),
    NewPing(USTX_NL, USRX_NL, MAX_DISTANCE),
    NewPing(USTX_NR, USRX_NR, MAX_DISTANCE),
    NewPing(USTX_FR, USRX_FR, MAX_DISTANCE)
};

/**
 * @brief   Update the Ultrasonics Reading
 */
void Ultrasonics_DataUpdate() {
    for(uint8_t i=0; i<4; i++) {
        ultrasonics[i] = Ping[i].ping_cm();
    }
}

/**
 * @brief   Print the Ultrasonics Distance
 */
void Ultrasonics_PrintData() {
    Serial.printf("Ultrasonics: Distance(%4u,%4u,%4u,%4u)cm\n",
                    ultrasonics[0], ultrasonics[1], ultrasonics[2], ultrasonics[3]);
}

/**
 * @brief   Overwrite on Speed values (minimize) to avoid any odstacle
 */
void Ultrasonics_ObstacleAvoid(){
    if(motors_linear>=0){   // if there any linear motion
        
        float_t break_action = 1.0; // break effect on speed [0~1]  (Precentage)
            
        // Get the minimum distance in all ultrasonis
        float_t range = ultrasonics[0];
        for(uint8_t posi=1; posi<4; posi++){
            if(range > ultrasonics[posi]){
                range = ultrasonics[posi];
            }
        }

        if(range < BREAK_DISTANCE){ // if the robot become close to obstacle more than the limit
            break_action = range/BREAK_DISTANCE;    // calculate the break effect
            break_action = constrain(break_action, 0.0, 1.0);
            PS4.setRumble(1.0-break_action, 0);
        }
        else{
            PS4.setRumble(0, 0);
        }

        motors_linear *= break_action;  // update the linear speed accordingly
    }
    else{
        PS4.setRumble(0, 0);
    }
}
#endif