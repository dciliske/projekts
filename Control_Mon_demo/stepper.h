/**************************************************************************//**
 *
 * Copyright 1998-2015 NetBurner, Inc.  ALL RIGHTS RESERVED
 *   Permission is hereby granted to purchasers of NetBurner Hardware
 *   to use or modify this computer program for any use as long as the
 *   resultant program is only executed on NetBurner provided hardware.
 *
 *   No other rights to use this program or it's derivatives in part or
 *   in whole are granted.
 *
 *   It may be possible to license this or other NetBurner software for
 *   use on non-NetBurner Hardware.
 *   Please contact sales@Netburner.com for more information.
 *
 *   NetBurner makes no representation or warranties
 *   with respect to the performance of this computer program, and
 *   specifically disclaims any responsibility for any damages,
 *   special or consequential, connected with the use of this program.
 *
 *---------------------------------------------------------------------
 * NetBurner, Inc.
 * 5405 Morehouse Drive
 * San Diego, California 92121
 *
 * information available at:  http://www.netburner.com
 * E-Mail info@netburner.com
 *
 * Support is available: E-Mail support@netburner.com
 *
 *****************************************************************************/

#include <constants.h>
#include <ctype.h>
#include <pins.h>
#include <ucos.h>

#define STEPPER_MAX_PIN_COUNT 4
#define STEPPER_MAX_SEQ_SIZE 8
#define STEPPER_DEFAULT_STEP_RATE 500

struct StepperConfig {
    PinIO pins[STEPPER_MAX_PIN_COUNT];
    uint8_t pinCount;
    uint8_t seq[STEPPER_MAX_SEQ_SIZE];
    uint8_t seqLen;
    int seqIndex;
    inline void Init() {
        for (int i = 0; i < pinCount; i++) {
            pins[i].function(PIN_GPIO);
            pins[i] = (1 << i) & seq[0];
        }
    }
};

struct StepperControl {
    StepperConfig *motor;
    uint32_t stepsRem;
    int32_t stepsTaken;
    bool runForever;
    int dir; // 1 = normal seq, -1 = reverse seq
    bool done;
    OS_SEM *sem;
};

extern volatile StepperControl Steppers[4];


int ConfigureStepperTimer( int timer, int pit_per_sec );
int ConfigureStepper( StepperConfig *newStepper, int timer );

int StartStepper( int timer, int dir = 1, int stepRate = STEPPER_DEFAULT_STEP_RATE );
int StopStepper( int timer );
int ChangeStepRate( int timer, int stepRate );
int ChangeDir(int timer, int dir);

int MoveStepper( int timer, uint32_t stepCount, int dir = 1,
        int stepRate = STEPPER_DEFAULT_STEP_RATE, OS_SEM *completeSem = NULL );

bool StepperDone( int timer );
