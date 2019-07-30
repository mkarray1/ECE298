/*
 * Authors: Anthony Berbari, Malek Karray
*/


#include <msp430.h>
#include <stdbool.h>
#include "driverlib.h"
#include "Board.h"
#include "microphone.h"
#include "interrupts.h"

/**********************************************************
 * Variables
 *********************************************************/

static bool m_micFlag;

/**********************************************************
 * Prototypes
 *********************************************************/

static void updateMicStatus(void);

/**********************************************************
 * Code
 *********************************************************/

bool emergencyStopRequested(void)
{
    return (m_micFlag = false);
}

void initMic (void)
{
     m_micFlag = false;


     setMicFuncPtr(updateMicStatus);


     GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P8, GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION); // ADC Anolog Input



     ADC_init(0x0700, ADC_SAMPLEHOLDSOURCE_SC, ADC_CLOCKSOURCE_ADCOSC, ADC_CLOCKDIVIDER_1);
     ADC_enable(0x0700);
     ADC_setupSamplingTimer(0x0700, ADC_CYCLEHOLD_16_CYCLES, ADC_MULTIPLESAMPLESENABLE);        // timer trigger needed to start every ADC conversion
     ADC_configureMemory(0x0700, ADC_INPUT_A9, ADC_VREFPOS_INT, ADC_VREFNEG_AVSS);
     ADC_setWindowComp(0x0700, 0x300, 1);
     ADC_clearInterrupt(0x0700, ADC_BELOWTHRESHOLD_INTERRUPT);       //  bit mask of the interrupt flags to be cleared- for new conversion data in the memory buffer
     ADC_enableInterrupt(0x0700, ADC_BELOWTHRESHOLD_INTERRUPT);       //  enable source to reflected to the processor interrupt



     while (PMM_REFGEN_NOTREADY == PMM_getVariableReferenceVoltageStatus()) ;

     PMM_enableInternalReference();      // disabled by default

     ADC_startConversion(0x0700, ADC_REPEATED_SINGLECHANNEL);

}


static void updateMicStatus(void)
{
    m_micFlag = true;
}