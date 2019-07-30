/*
 * Authors: Anthony Berbari, Malek Karray
*/


#include <msp430.h>
#include "driverlib.h"
#include "Board.h"
#include <math.h>
#include "interrupts.h"

#define TIMEFACTOR 9.9E-7
#define SPEED_OF_SOUND 340

#define MIN_CYCLES_BETWEEN_TRIGS 30

/**********************************************************
 * Variables
 *********************************************************/

static float m_newestDistance;

/**********************************************************
 * Prototypes
 *********************************************************/

static void sendNewTrigger(void);
static void getUltrasonicTimestamp(uint16_t timestamp);

/**********************************************************
 * Code
 *********************************************************/
static void getUltrasonicTimestamp(uint16_t timestamp){
    static uint16_t previousTimestamp;
    static bool isFallingEdge;
    int32_t counter = timestamp - previousTimestamp;
    previousTimestamp = timestamp;
    if (isFallingEdge == true){
        volatile float secondsBetweenRevolutions = fabs( counter * TIMEFACTOR);
        float distance = (secondsBetweenRevolutions * SPEED_OF_SOUND) / 2;
        m_newestDistance = distance;
    }
    isFallingEdge ^= 1;
}





static void initUltrasonicTimer(void){

    Timer_A_initContinuousModeParam ultrasonicTimer= {0};
    ultrasonicTimer.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    ultrasonicTimer.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    ultrasonicTimer.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;  // TODO this is sketch. Reason for this interrupt ois for use by the motor control module
    ultrasonicTimer.timerClear = TIMER_A_DO_CLEAR;
    ultrasonicTimer.startTimer = true;
    Timer_A_initContinuousMode(TIMER_A0_BASE, &ultrasonicTimer);

    m_newestDistance = 1000;
}
void initUltrasonic(void){
    initUltrasonicTimer();
    setUltrasonicFuncPtr(getUltrasonicTimestamp);

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);     // Column 2: Input direction
    GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN5, GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_setAsInputPin(GPIO_PORT_P1, GPIO_PIN5);
    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN5);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN5);


                                            // to activate previously configured port settings
                         // Set P1.0 to output direction

    P5DIR |= (1<<1);
    P5OUT &= ~(1<<1);

    sendNewTrigger();

}

float getDistance(void){

    sendNewTrigger();

    return m_newestDistance;
}


static void sendNewTrigger(void){

    static uint16_t lastTime;
    uint16_t thisTime = Timer_A_getCounterValue(TIMER_A1_BASE);

    if( abs( thisTime - lastTime) < MIN_CYCLES_BETWEEN_TRIGS)
    {
        return;
    }

    lastTime = thisTime;

    P5OUT ^= ( 1<<1 );
    __delay_cycles(7);
    P5OUT ^= ( 1<<1 );
}














