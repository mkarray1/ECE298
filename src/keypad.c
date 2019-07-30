/*
 * Authors: Anthony Berbari, Malek Karray
*/

#include "keypad.h"
#include <msp430.h>
#include "driverlib.h"
#include "Board.h"
#include <stdbool.h>


/**********************************************************
 * Code
 *********************************************************/


void initKeypad(void)
{
    // Row Outputs (to mux, these are actually shared with the led's)
    P1DIR |= ( 1 << 0 );
    P1DIR |= ( 1 << 1 );
    P2DIR |= ( 1 << 7 );

    // Column inputs
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P8, GPIO_PIN0);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P8, GPIO_PIN2);
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P8, GPIO_PIN3);

}

void debounceHoldAvoidance(void)
{
    volatile char dummyChar;
    while (getCharacter(&dummyChar) == true);
    __delay_cycles(100000);
}


bool keypadInputComplete(char* userInput)
{
    static int index;
    char character;
    if (getCharacter(&character) == true){
        userInput[index] = character;
        index++;
    }

    if (index == 2){
            index = 0;
            return true;
    }
    return false;
}
bool getCharacter(char *character)
{

    // itterate through all combos, save results

    // check row 3

    P2OUT &= ~( 1 << 7 );
    P1OUT |= ( 1 << 1 );
    P1OUT &= ~( 1 << 0 );

    *character = 'z';
    // column 3
    if (GPIO_getInputPinValue(GPIO_PORT_P8, GPIO_PIN3) == GPIO_INPUT_PIN_HIGH)
    {
        *character = '3';
    }

    else if (GPIO_getInputPinValue(GPIO_PORT_P8, GPIO_PIN2) == GPIO_INPUT_PIN_HIGH)
    {
        *character = '2';
    }

    else if (GPIO_getInputPinValue(GPIO_PORT_P8, GPIO_PIN0) == GPIO_INPUT_PIN_HIGH)
    {
        *character = '1';
    }

    // check row 3
    P2OUT |= ( 1 << 7 );
    P1OUT |= ( 1 << 1 );
    P1OUT |= ( 1 << 0 );

    // column 3
    if (GPIO_getInputPinValue(GPIO_PORT_P8, GPIO_PIN3) == GPIO_INPUT_PIN_HIGH)
    {
        *character = '#';
    }

    else if (GPIO_getInputPinValue(GPIO_PORT_P8, GPIO_PIN2) == GPIO_INPUT_PIN_HIGH)
    {
        *character = '0';
    }

    else if (GPIO_getInputPinValue(GPIO_PORT_P8, GPIO_PIN0) == GPIO_INPUT_PIN_HIGH)
    {
        *character = '*';

    }


    // check row 1
    P2OUT &= ~( 1 << 7 );
    P1OUT &= ~( 1 << 1 );
    P1OUT |= ( 1 << 0 );

    // column 3
    if (GPIO_getInputPinValue(GPIO_PORT_P8, GPIO_PIN3) == GPIO_INPUT_PIN_HIGH)
    {
        *character = '6';
    }

    else if (GPIO_getInputPinValue(GPIO_PORT_P8, GPIO_PIN2) == GPIO_INPUT_PIN_HIGH)
    {
        *character = '5';
    }

    else if (GPIO_getInputPinValue(GPIO_PORT_P8, GPIO_PIN0) == GPIO_INPUT_PIN_HIGH)
    {
        *character = '4';
    }

    // check row 3 (not associated to keypad
    P2OUT &= ~( 1 << 7 );
    P1OUT |= ( 1 << 1 );
    P1OUT |= ( 1 << 0 );

    // column 3
    if (GPIO_getInputPinValue(GPIO_PORT_P8, GPIO_PIN3) == GPIO_INPUT_PIN_HIGH)
    {
        *character = '9';
    }

    else if (GPIO_getInputPinValue(GPIO_PORT_P8, GPIO_PIN2) == GPIO_INPUT_PIN_HIGH)
    {
        *character = '8';
    }

    else if (GPIO_getInputPinValue(GPIO_PORT_P8, GPIO_PIN0) == GPIO_INPUT_PIN_HIGH)
    {
        *character = '7';
    }

    if (*character == 'z'){
        return false;
    }

    return true;
}

bool userResetRequested(void)
{
    // check row 3
    P2OUT |= ( 1 << 7 );
    P1OUT |= ( 1 << 1 );
    P1OUT |= ( 1 << 0 );

    if (GPIO_getInputPinValue(GPIO_PORT_P8, GPIO_PIN0) == GPIO_INPUT_PIN_HIGH)
    {
        return true;
    }

    return false;
}
