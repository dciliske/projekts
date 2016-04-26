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
#include <init.h>
#include <pins.h>
#include <sim.h>
#include <cfinter.h>
#include <intcdefs.h>
#include <ucosmcfc.h>

#define TIMER_A_NUMBER 0
#define TIMER_B_NUMBER 1
#define MIN_COUNT -100
#define MAX_COUNT 100
#define DEBOUNCE_PERIOD (0x00009896)
#define DEBOUNCE_COUNT 4

#define DOUBLE_CLICK_COUNT 500


int32_t count = 0;

volatile timerstruct &timer_a = sim2.timer[TIMER_A_NUMBER];
volatile timerstruct &timer_b = sim2.timer[TIMER_B_NUMBER];

void (*EncoderEdgeFunc)(int dir) = NULL;
void (*ButtonChangeFunc)(bool state) = NULL;
void (*ButtonDoubleClickFunc)() = NULL;

struct EdgeCaptStruct {
    bool prev;
    bool lastStable;
    int  dupCount;
};

static EdgeCaptStruct a;
static EdgeCaptStruct b;
static EdgeCaptStruct zed;
static uint32_t lastButtonUp = 0xFFFFFFFF;

INTERRUPT(EdgeCaptureISR, 0x2600)
{
    bool a_cur = J2[36];
    bool b_cur = J2[34];
    bool zed_cur = J2[28];
    bool a_change = false;
    bool b_change = false;
    bool zed_change = false;
    if (a_cur != a.prev) {
        a.prev = a_cur;
        a.dupCount = 0;
    }
    else if ((a.dupCount >= 0) && (a.dupCount < DEBOUNCE_COUNT)) {
        a.dupCount++;
        if (a.dupCount == DEBOUNCE_COUNT) {
            a.lastStable = a_cur;
            a_change = true;
        }
    }

    if (b_cur != b.prev) {
        b.prev = b_cur;
        b.dupCount = 0;
    }
    else if ((b.dupCount >= 0) && (b.dupCount < DEBOUNCE_COUNT)) {
        b.dupCount++;
        if (b.dupCount == DEBOUNCE_COUNT) {
            b.lastStable = b_cur;
            b_change = true;
        }
    }

    lastButtonUp++;
    if (zed_cur != zed.prev) {
        zed.prev = zed_cur;
        zed.dupCount = 0;
    }
    else if ((zed.dupCount >= 0) && (zed.dupCount < DEBOUNCE_COUNT)) {
        zed.dupCount++;
        if (zed.dupCount == DEBOUNCE_COUNT) {
            zed.lastStable = zed_cur;
            zed_change = true;
            if (ButtonChangeFunc) {
                ButtonChangeFunc(zed.lastStable);
            }
            if (zed.lastStable) {
                if ((lastButtonUp <= DOUBLE_CLICK_COUNT)
                        && ButtonDoubleClickFunc){
                    ButtonDoubleClickFunc();
                }
                else {
                    lastButtonUp = 0;
                }
            }
        }
    }


    if (a_change && (a.lastStable == 1) && (b.lastStable == 1)) {
        if (EncoderEdgeFunc) {
            EncoderEdgeFunc(-1);
        }
    }
    if (b_change && (a.lastStable == 1) && (b.lastStable == 1)) {
        if (EncoderEdgeFunc) {
            EncoderEdgeFunc(1);
        }
    }

    sim2.pit[3].pcsr |= 0x0004;
}

void EnableEncoder( void (*stepFn)(int), void (*changeFn)(bool), void (*dblFn)())
{
    SETUP_PIT3_ISR( EdgeCaptureISR, 6 );

    EncoderEdgeFunc = stepFn;
    ButtonChangeFunc = changeFn;
    ButtonDoubleClickFunc = dblFn;

    J2[36].function(PIN_GPIO);
    J2[34].function(PIN_GPIO);
    J2[28].function(PIN_GPIO);
    a.prev = a.lastStable = J2[36];
    b.prev = b.lastStable = J2[32];
    zed.prev = zed.lastStable = J2[28];

    if (ButtonChangeFunc) { ButtonChangeFunc(zed.lastStable); }

    sim2.pit[3].pcsr = 0x011E;
    sim2.pit[3].pmr = 0xF424; // IRQ every 1msec
    sim2.pit[3].pcsr |= 0x0001;
}


