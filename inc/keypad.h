/*
 * Authors: Anthony Berbari, Malek Karray
*/

#include <stdbool.h>

/**********************************************************
 * Prototypes
 *********************************************************/

void initKeypad(void);

bool getCharacter(char *character);

bool userResetRequested(void);

bool keypadInputComplete(char* userInput);

void debounceHoldAvoidance(void);
