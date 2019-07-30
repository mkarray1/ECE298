/*
 * Authors: Anthony Berbari, Malek Karray
*/

#include "leds.h"
#include <msp430.h>
#include "driverlib.h"
#include "Board.h"
#include <stdbool.h>


/**********************************************************
 * Code
 *********************************************************/

void InitLeds(void)
{
    P1DIR |= ( 1 << 0 );
    P1DIR |= ( 1 << 1 );
    P2DIR |= ( 1 << 7 );
}

void TurnOnGreen(void)
{
    P2OUT |= ( 1 << 7 );
    P1OUT &= ~( 1 << 1 );
    P1OUT &= ~( 1 << 0 );
}
void TurnOnYellow(void)
{

    P2OUT |= ( 1 << 7 );
    P1OUT |= ( 1 << 1 );
    P1OUT &= ~( 1 << 0 );
}
void TurnOnRed(void)
{
    P2OUT |= ( 1 << 7 );
    P1OUT &= ~( 1 << 1 );
    P1OUT |= ( 1 << 0 );

}

void ToggleRed(void)
{
    static bool onLastTime;

    if (onLastTime)
    {
        P2OUT &= ~( 1 << 7 );
        P1OUT &= ~( 1 << 1 );
        P1OUT &= ~( 1 << 0 );
        onLastTime = false;
    }

    else
    {
        P2OUT |= ( 1 << 7 );
        P1OUT &= ~( 1 << 1 );
        P1OUT |= ( 1 << 0 );
        onLastTime = true;
    }

}

void ToggleGreen(void)
{
    static bool onLastTime;

    if (onLastTime)
    {
        P2OUT |= ( 1 << 7 );
        P1OUT &= ~( 1 << 1 );
        P1OUT &= ~( 1 << 0 );
        onLastTime = false;

    }

    else
    {
        P2OUT &= ~( 1 << 7 );
        P1OUT &= ~( 1 << 1 );
        P1OUT &= ~( 1 << 0 );
        onLastTime = true;
    }
}
