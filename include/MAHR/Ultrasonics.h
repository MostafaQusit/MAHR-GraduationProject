#ifndef MAHR_ULTRASONICS_H_
#define MAHR_ULTRASONICS_H_

#include <MAHR.h>
#include <NewPing.h>

#define MIN_DISTANCE      3     // Minimum distance (in cm) to ping.
#define MAX_DISTANCE    200     // Maximum distance (in cm) to ping.
#define BREAK_DISTANCE  30.0    // distance that break action activate (in cm)
#define L   0
#define CL  1
#define CR  2
#define R   3

NewPing Ping[4] = { // Sensor object array.
    NewPing(USTX_FL, USRX_FL, MAX_DISTANCE),
    NewPing(USTX_NL, USRX_NL, MAX_DISTANCE),
    NewPing(USTX_NR, USRX_NR, MAX_DISTANCE),
    NewPing(USTX_FR, USRX_FR, MAX_DISTANCE)
};

// Update the Ultrasonics Reading
void Ultrasonics_DataUpdate() {
    for(uint8_t i=0; i<4; i++) {
        ultrasonics[i] = Ping[i].ping_cm();
    }
}
// Print the Ultrasonics Distance every certain time
void Ultrasonics_PrintData() {
  Serial.printf("Ultrasonics: Distance(%4u,%4u,%4u,%4u)cm\n",
                ultrasonics[0], ultrasonics[1], ultrasonics[2], ultrasonics[3]);
}
// Overwrite on Speed values to avoid any odstacle
void Ultrasonics_ObstacleAvoid(){
    if(motors_linear>=0){
        float_t break_action = 1.0;
            
        // Get the minimum distance
        float_t range = ultrasonics[0];
        for(uint8_t posi=1; posi<4; posi++){
            if(range > ultrasonics[posi]){
                range = ultrasonics[posi];
            }
        }

        if(range < BREAK_DISTANCE){
            break_action = range/BREAK_DISTANCE;
            break_action = constrain(break_action, 0.0, 1.0);
        }

        motors_linear *= break_action;
    }
}
#endif