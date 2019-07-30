/*
 * Authors: Anthony Berbari, Malek Karray
*/

/**********************************************************
 * Definitions
 *********************************************************/

#include <stdbool.h>

 typedef struct {
    float leftWheel;
    float rightWheel;
}rpmValues_t;

/**********************************************************
 * Prototypes
 *********************************************************/

void initRPM(void);

rpmValues_t *getRPM(void);

int getNumRotations(void);

void clearNumRotations(void);
