/* 
* Authors: Malek Karray, Anthony Berbari
*/


#include <msp430.h>
#include "driverlib.h"
#include "Board.h"
#include "rpm.h"
#include "lowPassFilter.h"
#include <math.h>
#include "interrupts.h"

/**********************************************************
 * Defintions
 *********************************************************/

#define MAX_16_BIT_VALUE 0xFFFF
#define TIMEFACTOR 3.05E-4

#define LEFT 2
#define RIGHT 1

/**********************************************************
 * Variables
 *********************************************************/

static rpmValues_t currentVal;
static LowPassFilter_t Filter;
static int numRotations;

/**********************************************************
 * Prototypes
 *********************************************************/

static void initRpmTimer(void);

static void convertCounter(uint16_t timeStamp, int caller);

/**********************************************************
 * Code
 *********************************************************/

rpmValues_t *getRPM(void){
    return &currentVal;
}


void initRPM(void){

    currentVal.leftWheel = 0;
    currentVal.rightWheel = 0;

    setRpmFuncPtr(convertCounter);

    WDT_A_hold(WDT_A_BASE);     // Stop watchdog timer

    initRpmTimer();


    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);     // Column 1: Input direction
    GPIO_selectInterruptEdge(GPIO_PORT_P2, GPIO_PIN5, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_clearInterrupt(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN5);


    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);     // Column 2: Input direction
    GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN4, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);

    Filter = LowPassFilter_CreateFilter(5);
}

int getNumRotations(void)
{
    return numRotations;
}

void clearNumRotations(void)
{
    numRotations = 0;
}


static void initRpmTimer(void){

    Timer_A_initContinuousModeParam rpmTimer = {0};
    rpmTimer.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
    rpmTimer.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_10;
    rpmTimer.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    rpmTimer.timerClear = TIMER_A_DO_CLEAR;
    rpmTimer.startTimer = true;
    Timer_A_initContinuousMode(TIMER_A1_BASE, &rpmTimer);
}



static void convertCounter(uint16_t timeStamp, int caller){
    static int previousTimeStampLeft;
    static int previousTimeStampRight;
    static int index;
    if (caller == LEFT){
        int32_t counter = timeStamp - previousTimeStampLeft;
        float secondsBetweenRevolutions = fabs(counter * TIMEFACTOR);
        float rawLeft = 60 / secondsBetweenRevolutions;
        if (rawLeft > 100)
        {
            return;
        }
        currentVal.leftWheel = LowPassFilter_Execute(Filter, rawLeft);
        previousTimeStampLeft = timeStamp;
    }

    if (caller == RIGHT){
        int32_t counter = timeStamp - previousTimeStampRight;
        float secondsBetweenRevolutions = fabs( counter * TIMEFACTOR);
        float rawRight = 60 / secondsBetweenRevolutions;
        currentVal.rightWheel = LowPassFilter_Execute(Filter, rawRight);
        if (rawRight > 100) {
            return;
        }

        previousTimeStampRight = timeStamp;

        numRotations ++;
   }


}


// SMCLK runs at about in about 1.1MHz




