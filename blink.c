//***************************************************************************************
//  MSP430 Blink the LED Demo - Software Toggle P1.0
//
//  Description; Toggle P1.0 by xor'ing P1.0 inside of a software loop.
//  ACLK = n/a, MCLK = SMCLK = default DCO
//
//                MSP430x5xx
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|-->LED
//
//  Texas Instruments, Inc
//  July 2013
//***************************************************************************************

#include <msp430.h>

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings
                         // Set P1.0 to output direction

    P1DIR |= 0x10;
    P1OUT &= ~(0x10);

    for (;;){
        P1OUT ^= 0x10;
        __delay_cycles(3);
        P1OUT ^= 0x10;
        __delay_cycles(400000);
    }

    /*for(;;) {
        volatile unsigned int i;            // volatile to prevent optimization

        P1OUT ^= 0x10;

    }*/
}
