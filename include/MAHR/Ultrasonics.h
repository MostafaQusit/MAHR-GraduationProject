#ifndef MAHR_ULTRASONICS_H_
#define MAHR_ULTRASONICS_H_

#include <MAHR.h>
#include <NewPing.h>

#define MIN_DISTANCE   3 // Minimum distance (in cm) to ping.
#define MAX_DISTANCE 200 // Maximum distance (in cm) to ping.

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
  Serial.printf("Ultrasonics: Distance(%4d,%4d,%4d,%4d)cm\n",
                ultrasonics[0], ultrasonics[1], ultrasonics[2], ultrasonics[3]);
}

#endif