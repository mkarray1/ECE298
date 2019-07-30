/*
 * Authors: Anthony Berbari, Malek Karray
*/

#include <msp430.h>
#include "driverlib.h"
#include "Board.h"
#include <stdbool.h>

#define LEFT 2
#define RIGHT 1

/**********************************************************
 * variables
 *********************************************************/

static void (*UltrasonicFuncPtr)(uint16_t timestamp);
static void (*RpmFuncPtr)(uint16_t timestamp, int caller);
static void (*MicFuncPtr)(void);
static bool echoCompareFlag = false;

/**********************************************************
 * Prototypes
 *********************************************************/
static bool echoVal(){

    volatile int pinVal = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN5);

    if (pinVal == GPIO_INPUT_PIN_LOW){
        return false;
    }

    return true;
}



void setUltrasonicFuncPtr( void (*getUltrasonicTimestamp)(uint16_t timestamp) )
{
    UltrasonicFuncPtr = getUltrasonicTimestamp;
}

void setRpmFuncPtr( void (*convertCounter)(uint16_t timestamp, int caller) )
{
    RpmFuncPtr = convertCounter;
}

void setMicFuncPtr( void (*getMicStatus)(void) )
{
    MicFuncPtr = getMicStatus;
}

#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{

    volatile bool echoReading = echoVal();

    /*  Hall effect */
    if (echoCompareFlag == false && echoReading == false){

        RpmFuncPtr(Timer_A_getCounterValue(TIMER_A1_BASE), RIGHT);
    }
    /* Ultrasonic rising edge */
    else if (echoCompareFlag == false && echoReading == true){
        UltrasonicFuncPtr(Timer_A_getCounterValue(TIMER_A0_BASE));
        echoCompareFlag = true;
        GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN5, GPIO_HIGH_TO_LOW_TRANSITION);
    }
    /* Ultrasonic falling edge */
    else if (echoCompareFlag == true && echoReading == false){
        UltrasonicFuncPtr(Timer_A_getCounterValue(TIMER_A0_BASE));
        echoCompareFlag = false;
        GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN5, GPIO_LOW_TO_HIGH_TRANSITION);
    }
    /* hall effect */
    else{
        RpmFuncPtr(Timer_A_getCounterValue(TIMER_A1_BASE), RIGHT);
    }

    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN5);

}

#pragma vector = PORT2_VECTOR
__interrupt void PORT2_ISR(void)
{
    /*Left wheel*/
    RpmFuncPtr( Timer_A_getCounterValue(TIMER_A1_BASE), LEFT ); //(change to rpm for left wheel)
    GPIO_clearInterrupt(GPIO_PORT_P2, GPIO_PIN5);
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(ADC_VECTOR)))
#endif

void ADC_ISR (void)
{
    MicFuncPtr();
    ADC_clearInterrupt(0x0700, ADC_BELOWTHRESHOLD_INTERRUPT);
}

