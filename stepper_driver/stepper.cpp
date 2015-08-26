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

#include <predef.h>
#include <constants.h>
#include <ctype.h>
#include <ucos.h>
#include <system.h>
#include <pins.h>
#include <utils.h>
#include <sim.h>
#include <cfinter.h>
#include <intcdefs.h>
#include <ucos.h>
#include "stepper.h"

volatile StepperControl Steppers[4];
static WORD     pit_pcsr_clr[4];
static WORD     new_pmr[4];
extern DWORD CPU_CLOCK;

inline void StepperISR( int timerNum )
{
    StepperConfig *motor = Steppers[timerNum].motor;
    for (int i = 0; i < Steppers[timerNum].motor->pinCount; i++) {
        motor->pins[i] = (1 << i) & motor->seq[motor->seqIndex];
    }

    motor->seqIndex = (motor->seqIndex + Steppers[timerNum].dir) % motor->seqLen;
    if (motor->seqIndex < 0) { motor->seqIndex = motor->seqLen - 1; }

    if (Steppers[timerNum].runForever == false) {
        Steppers[timerNum].stepsRem--;
        if (Steppers[timerNum].stepsRem == 0) {
            sim2.pit[timerNum].pcsr &= ~0xFE; // Disable PIT
            Steppers[timerNum].done = true;
            if (Steppers[timerNum].sem) {
                OSSemPost(Steppers[timerNum].sem);
            }
        }
    }
}


INTERRUPT( StepPit1_ISR, 0x2600 )
{
    if (new_pmr[1]) {
        sim2.pit[1].pmr = new_pmr[1];
        new_pmr[1] = 0;
    }
    sim2.pit[1].pcsr = pit_pcsr_clr[1];
    StepperISR(1);
}


INTERRUPT( StepPit2_ISR, 0x2600 )
{
    if (new_pmr[2]) {
        sim2.pit[2].pmr = new_pmr[2];
        new_pmr[2] = 0;
    }
    sim2.pit[2].pcsr = pit_pcsr_clr[1];
    StepperISR(2);
}



int ConfigureStepperTimer( int timer, int pit_per_sec )
{
    if ( (timer <= 0) || (timer >= 3) )
        return -1;
    // Determine 4-bit prescaler value range from 1 to 32768
    DWORD div = 2;
    DWORD pcsr = 0x00F;
    while (( ( CPU_CLOCK / div ) / pit_per_sec ) > 65536)
    {
        div *= 2;
        pcsr += 0x100;
    }

    pit_pcsr_clr[timer] = pcsr;

    if ( timer == 1 )
    {
        sim2.pit[1].pmr = ( ( CPU_CLOCK / div ) / pit_per_sec); // Set the PIT modulus value
        sim2.pit[1].pcsr = pcsr;
        SETUP_PIT1_ISR( &StepPit1_ISR, 4 );
    }
    else if ( timer == 2 )

    {
        sim2.pit[2].pmr = ( ( CPU_CLOCK / div ) / pit_per_sec); // Set the PIT modulus value
        sim2.pit[2].pcsr = pcsr;
        SETUP_PIT2_ISR( &StepPit2_ISR, 4 );
    }

    return 0;
}

int ConfigureStepper( StepperConfig *newStepper, int timer )
{
    // 0 reserved for system clock, 3 reserved for debugger
    if ( (timer <= 0) || (timer >= 3) )
        return -1;

    Steppers[timer].motor = newStepper;
    return 0;
}

int StartStepper( int timer, int dir, int stepRate )
{
    // 0 reserved for system clock, 3 reserved for debugger
    if ( (timer <= 0) || (timer >= 3) )
        return -1;

    if (!Steppers[timer].motor) {
        return -2;
    }

    sim2.pit[timer].pcsr &= ~0xFE;
    Steppers[timer].dir = dir;
    Steppers[timer].runForever = true;
    Steppers[timer].done = false;

    ConfigureStepperTimer( timer, stepRate );

    return 0;
}

int ChangeStepRate( int timer, int stepRate )
{
    if ( (timer <= 0) || (timer >= 3) )
        return -1;
    // Determine 4-bit prescaler value range from 1 to 32768
    DWORD div = 2;
    DWORD pcsr = 0x00F;
    while (( ( CPU_CLOCK / div ) / stepRate ) > 65536)
    {
        div *= 2;
        pcsr += 0x100;
    }

    pit_pcsr_clr[timer] = pcsr;
    new_pmr[timer] = ( ( CPU_CLOCK / div ) / stepRate); // Set the PIT modulus value
    return 0;

}

int StopStepper( int timer )
{
    // 0 reserved for system clock, 3 reserved for debugger
    if ( (timer <= 0) || (timer >= 3) )
        return -1;

    if (!Steppers[timer].motor) {
        return -2;
    }

    sim2.pit[timer].pcsr &= ~0xFE;
    Steppers[timer].done = true;
    return 0;
}

int MoveStepper( int timer, uint32_t stepCount, int dir, int stepRate, OS_SEM * completeSem )
{
    // 0 reserved for system clock, 3 reserved for debugger
    if ( (timer <= 0) || (timer >= 3) )
        return -1;

    if (!Steppers[timer].motor) {
        return -2;
    }

    if (stepCount == 0) {
        return -3;
    }

    sim2.pit[timer].pcsr &= ~0xFE;
    Steppers[timer].dir = dir;
    Steppers[timer].runForever = false;
    Steppers[timer].done = false;
    Steppers[timer].stepsRem = stepCount;
    Steppers[timer].sem = completeSem;

    ConfigureStepperTimer( timer, stepRate );
    StepperISR(timer);

    return 0;
}

bool StepperDone( int timer )
{
    // 0 reserved for system clock, 3 reserved for debugger
    if ( (timer <= 0) || (timer >= 3) )
        return true;

    if (!Steppers[timer].motor) {
        return true;
    }

    return Steppers[timer].done;
}
