/*
 * Authors: Anthony Berbari, Malek Karray
*/


#include <msp430.h>
#include "driverlib.h"
#include "Board.h"
#include "motorControl.h"
#include "rpm.h"
#include <math.h>

/**********************************************************
 * Defintions
 *********************************************************/

#define MAX_POWER_NUMBER 0xFFFF
#define TARGET_RPM_FOR_FORWARD 40
#define MARGIN_OF_ERROR 5
#define MIN_CYCLES_BETWEEN_UPDATE_PWM 1000

#define MAX_MOTOR_PERCENTAGE 50


/**********************************************************
 * Variables
 *********************************************************/

static Timer_A_initCompareModeParam leftMotor = {0};
static Timer_A_initCompareModeParam rightMotor = {0};

static int previousPercentageLeft = 0;
static int previousPercentageRight = 0;

/**********************************************************
 * Prototypes
 *********************************************************/

static void setLeftMotorPower(int percentage);
static void setRightMotorPower(int percentage);


/**********************************************************
 * Code
 *********************************************************/

void initMotorControl(void)
{

    P5DIR |= ( (1 << 0) | (1 << 2) | (1 << 3) );
    P1DIR |= (1 << 3);


    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION); // left

    leftMotor.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    leftMotor.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    leftMotor.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;


    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION); // right

    rightMotor.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;
    rightMotor.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    rightMotor.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
}

void orientDirection(char dir)
{

    static uint16_t lastTime;
    uint16_t thisTime = Timer_A_getCounterValue(TIMER_A1_BASE);

    if( abs( thisTime - lastTime) < MIN_CYCLES_BETWEEN_UPDATE_PWM)
    {
        return;
    }

    lastTime = thisTime;


    if (dir == 'L')
    {
       setLeftMotorPower(-30);
       setRightMotorPower(30);
    }

    else if (dir == 'R')
    {
       setLeftMotorPower(30);
       setRightMotorPower(-30);
    }

}

void driveForward(void)
{
    static uint16_t lastTime;
    uint16_t thisTime = Timer_A_getCounterValue(TIMER_A1_BASE);

    if( abs( thisTime - lastTime) < MIN_CYCLES_BETWEEN_UPDATE_PWM)
    {
        return;
    }

    lastTime = thisTime;

    setLeftMotorPower(50);
    setRightMotorPower(50);

#if 0
    rpmValues_t *rpm = getRPM();


    if (rpm->leftWheel < ( TARGET_RPM_FOR_FORWARD - MARGIN_OF_ERROR ))
    {
        int percentageLeft = previousPercentageLeft + 1;

        if (percentageLeft > MAX_MOTOR_PERCENTAGE)
        {
            percentageLeft = MAX_MOTOR_PERCENTAGE;
        }

        setLeftMotorPower(percentageLeft);
        previousPercentageLeft = percentageLeft;
    }

    else if (rpm->leftWheel > ( TARGET_RPM_FOR_FORWARD + MARGIN_OF_ERROR ))
    {
        int percentageLeft = previousPercentageLeft - 1;

        if (percentageLeft < -MAX_MOTOR_PERCENTAGE)
        {
            percentageLeft = -MAX_MOTOR_PERCENTAGE;
        }

        setLeftMotorPower(percentageLeft);
        previousPercentageLeft = percentageLeft;
    }



    if (rpm->rightWheel < ( TARGET_RPM_FOR_FORWARD - MARGIN_OF_ERROR ))
    {
        int percentageRight = previousPercentageRight + 1;

        if (percentageRight > MAX_MOTOR_PERCENTAGE)
        {
            percentageRight = MAX_MOTOR_PERCENTAGE;
        }

        setRightMotorPower(percentageRight);
        previousPercentageRight = percentageRight;
    }

    else if (rpm->rightWheel > ( TARGET_RPM_FOR_FORWARD + MARGIN_OF_ERROR ))
    {
        int percentageRight = previousPercentageRight - 1;

        if (percentageRight < -MAX_MOTOR_PERCENTAGE)
        {
            percentageRight = -MAX_MOTOR_PERCENTAGE;
        }

        setRightMotorPower(percentageRight);
        previousPercentageRight = percentageRight;
    }
#endif
}

void killAll(void)
{
    setRightMotorPower(0);
    setLeftMotorPower(0);

    previousPercentageLeft = 0;
    previousPercentageRight = 0;
}


static void setLeftMotorPower(int percentage)
{
    uint16_t compareValue = (uint16_t) fabs(((float) percentage / 100.0f) * MAX_POWER_NUMBER);

    if (percentage >= 0)
    {
        P5OUT |= (1 << 0);
        P5OUT &= ~( 1 << 2);
    }

    else
    {
        P5OUT &= ~(1 << 0);
        P5OUT |= ( 1 << 2);
    }

    leftMotor.compareValue = compareValue;
    Timer_A_initCompareMode(TIMER_A0_BASE, &leftMotor);
}





static void setRightMotorPower(int percentage)
{
    uint16_t compareValue = (uint16_t) fabs(((float) percentage / 100.0f) * MAX_POWER_NUMBER);

    if (percentage >= 0)
    {
        P5OUT |= (1 << 3);
        P1OUT &= ~( 1 << 3);
    }

    else
    {
        P5OUT &= ~(1 << 3);
        P1OUT |= ( 1 << 3);
    }

    rightMotor.compareValue = compareValue;
    Timer_A_initCompareMode(TIMER_A0_BASE, &rightMotor);
}






