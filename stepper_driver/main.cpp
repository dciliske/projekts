#include <predef.h>
#include <constants.h>
#include <ctype.h>
#include <ucos.h>
#include <system.h>
#include <init.h>
#include <pins.h>
#include <pitr_sem.h>
#include <utils.h>
#include <ucos.h>
#include "stepper.h"

#define STEPPER_PIN_COUNT 4
#define STEP_RATE 100
#define STEPPER_SEQ_SIZE 4

const char AppName[] = "stepper_driver";

extern "C" {
    void UserMain( void * pd );
}

uint8_t seqIndex = 0;

// IO pins are the first component of the config
StepperConfig stepper = {
                            { J2[27], J2[28], J2[29], J2[30] },
                            STEPPER_PIN_COUNT,
//                            { 0x09, 0x05, 0x06, 0x0A },
                            { 0x0A, 0x06, 0x05, 0x09 },
                            STEPPER_SEQ_SIZE,
                            0
                        };
OS_SEM stepSem;

void UserMain( void * pd )
{
    init();

    stepper.Init();

    iprintf("Application: %s\r\nNNDK Revision: %s\r\n",AppName,GetReleaseTag());

    ConfigureStepper( &stepper, 1 );

    OSSemInit(&stepSem, 0);
    while(1) {
        OSTimeDly(TICKS_PER_SECOND);
        iprintf("Tick: %lu\n", TimeTick);
        MoveStepper( 1, 500, 1, 100, &stepSem );
        OSSemPend( &stepSem, 0 );

        MoveStepper( 1, 500, -1, 100, &stepSem );
        OSSemPend( &stepSem, 0 );
    }
}
