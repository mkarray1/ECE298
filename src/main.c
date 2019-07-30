/*
 * Authors: Malek Karray, Anthony Berbari
*/

#include <msp430.h>
#include <Board.h>
#include "driverlib.h"
#include "interrupts.h"
#include <stdint.h>
#include "rpm.h"
#include "microphone.h"
#include "ultrasonic.h"
#include "keypad.h"
#include "motorControl.h"
#include "leds.h"
#include "hal_LCD.h"
#include <stdio.h>
#include <math.h>

/**********************************************************
 * Definitions
 *********************************************************/

 typedef struct {

   bool isEmergency;
   bool isOriented;
   float distance;
   rpmValues_t* rpmValues;

}sensorData;

/**********************************************************
 * Variables
 *********************************************************/
char string[2];

enum modes {KEYPAD, MOTOR};
static sensorData data;

static volatile int  DistanceLeftToTravelm;

/**********************************************************
 * Prototypes
 *********************************************************/

static void initPeriph();
static void readSensorData(sensorData* res);
static void displayData(sensorData* data);
static bool isOriented();
static void displayOrientScreen(void);
static void displayWaitingScreen(void);
static bool UserhasNotYelledBegin(void);
static bool UserHasArrived(void);
static void displayArrived(void);
static void updateLeds(void);

/**********************************************************
 * Code
 *********************************************************/

int main(void){

    initMic();
    initRPM();
    initUltrasonic();
    initPeriph();
    initKeypad();
    initMotorControl();
    Init_LCD();
    InitLeds();

    enum modes state = KEYPAD;
    bool inputReady = false;

    while (1){
        switch(state){

            case KEYPAD:
                clearNumRotations();
                data.isEmergency = false;
                data.isOriented = false;

                displayScrollText("INPUT COORDINATES");

                inputReady = false;

                while (inputReady == false){

                    debounceHoldAvoidance();
                    inputReady  = keypadInputComplete(string);
                    getDistance();
                }
                showChar('D',pos1);
                showChar('I',pos2);
                showChar('R',pos3);


                if(string[0] == '2')
                {
                    showChar('N',pos5);

                }
                else if(string[0] == '4')
                {
                    showChar('W',pos5);

                }
                else if(string[0] == '6')
                {
                    showChar('E',pos5);

                }
                else if(string[0] == '8')
                {
                    showChar('S',pos5);

                }
                else
                {
                    state = KEYPAD;
                    break;
                }

                __delay_cycles(1000000);

                displayWaitingScreen();
                while(UserhasNotYelledBegin());

                displayOrientScreen();
                __delay_cycles(600000);

                state = MOTOR;
                break;

            case MOTOR:
                readSensorData(&data);
                 if(data.isEmergency){
                     killAll();
                     state = KEYPAD;

                     volatile int i = 0;
                     for(i = 0; i < 6; i++)
                     {
                         ToggleRed();
                         __delay_cycles(1000000);
                     }

                     break;
                 }
                 if(data.isOriented){
                     displayData(&data);
                     driveForward();
                     updateLeds();
                     if(UserHasArrived())
                     {
                         killAll();
                         displayArrived();
                         volatile int i = 0;
                         for(i = 0; i < 6; i++)
                         {
                             ToggleGreen();
                             __delay_cycles(1000000);
                         }
                         state = KEYPAD;
                         break;
                     }
                 }
                 else
                 {
                     if(string[0] == 'E' || string[0] == 'S')
                     {
                         orientDirection('R');
                     }
                     else
                     {
                         orientDirection('L');
                     }

                 }

                 break;
        }
    }
}

static void readSensorData(sensorData* res){


    volatile float distance =  getDistance();

    if ( (distance < 0.3f) && (distance != 0.0f)){
        res->isEmergency = true;
        return;
    }

    if (emergencyStopRequested()){
        res->isEmergency = true;
        return;
    }

    res -> isEmergency = false;
    res -> distance = getDistance();
    res -> rpmValues = getRPM();
    res -> isOriented = isOriented();
    return;
}
static void displayData(sensorData* data){
    static int displayCounter;
    volatile int number = (int)fmod(data->rpmValues->rightWheel, 100.0);

    char charVals[10];

    if (displayCounter >= 500){
        snprintf(charVals, sizeof(charVals), "%d", number);
        clearLCD();
        showChar(charVals[0],pos1);
        showChar(charVals[1],pos2);

        snprintf(charVals, sizeof(charVals), "%d", DistanceLeftToTravelm);
        showChar(charVals[0],pos5);

        displayCounter = 0;
    }
    displayCounter++;


    return;
}
static bool isOriented(){

    if(data.isOriented)
    {
        return true;
    }

    if(string[0] == '2')
    {
        clearNumRotations();
        return true;
    }

    else if ( (string[0] == '6') || (string[0] == '4'))
    {

        if(getNumRotations() == 5)
        {
            clearNumRotations();
            return true;
        }

    }

    else if (string[0] == '8')
    {
        if(getNumRotations() == 10)
        {
            clearNumRotations();
            return true;
        }
    }

    return false;
}

static bool UserHasArrived(void)
{
    volatile int userNumCm = ( ( (int) (string[1] - 48)) * 100);
    volatile int numRotations = getNumRotations();
    DistanceLeftToTravelm = ( userNumCm -  (numRotations *25)) / 100;

    if(DistanceLeftToTravelm == 0)
    {
        clearNumRotations();
        DistanceLeftToTravelm = 0;
        return true;
    }

    return false;
}

static void updateLeds(void)
{
    if ((100 * DistanceLeftToTravelm) < (33 * ((int) (string[1] - 48))))    // already travelled more than 2/3
    {
        TurnOnGreen();
    }

    else if ((100 * DistanceLeftToTravelm) < (66 * ((int) (string[1] - 48))))    // already travelled more than 1/3
    {
        TurnOnYellow();
    }

    else
    {
        TurnOnRed();
    }

}


static void displayOrientScreen(void)
{
    clearLCD();
    showChar('O',pos1);
    showChar('R',pos2);
    showChar('N',pos3);
    showChar('T',pos4);
    showChar('N',pos5);
    showChar('G',pos6);

}

static void displayWaitingScreen(void)
{
    clearLCD();
    showChar('W',pos1);
    showChar('A',pos2);
    showChar('I',pos3);
    showChar('T',pos4);
    showChar('N',pos5);
    showChar('G',pos6);
}

static void displayArrived(void)
{
    clearLCD();
    showChar('A',pos1);
    showChar('R',pos2);
    showChar('R',pos3);
    showChar('I',pos4);
    showChar('V',pos5);
    showChar('D',pos6);
}

static bool UserhasNotYelledBegin(void)
{
    if(emergencyStopRequested())
    {
        return false;
    }

    return true;
}


static void initPeriph(){

    PMM_unlockLPM5();           // allow writing to registers
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    _EINT();                    // Start interrupt

}
