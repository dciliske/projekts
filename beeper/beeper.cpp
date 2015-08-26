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

#include <stdint.h>
#include <sim.h>
#include <ucos.h>
#include "beeper.h"

volatile timerstruct &timer = sim2.timer[TIMER_NUMBER];

void TimerSetup(uint32_t refticks)
{
    // Prescale by 5, toggle output, bus div 1, timer clock is 25MHz
    timer.tmr = (5 << 8) | 0x2A;
    timer.txmr = 0x00;
    timer.ter = 0x03;
    timer.trr = refticks; // 25 MHz/6250 = 4 kHz

    timer.tmr |= 0x01;
}

void TimerStop()
{
    timer.tmr = 0x00;
}

void Beep(int count, uint32_t beepPeriod, uint32_t refticks)
{
    for (int i = 0; i < count; i++) {
        TimerSetup(refticks);
        OSTimeDly(beepPeriod);
        TimerStop();
        OSTimeDly(beepPeriod);
    }
}

