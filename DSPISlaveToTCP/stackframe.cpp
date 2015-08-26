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
#include <nettypes.h>
#include <sim5441x.h>
#include <ucos.h>
#include <ucosmcfc.h>
#include <cfinter.h>
#include <intcdefs.h>
#include "stackframe.h"


static FrameBuf FrameBuffers[FRAME_BUF_COUNT] __attribute__(( aligned(2))) FAST_USER_VAR;
static FrameBufPtr FrameHead;
static bool FramesInited = NULL;

void InitFrameBufs()
{
    if ( FramesInited ) {
        return;
    }
    for (int i = 0; i < FRAME_BUF_COUNT; i++) {
        FrameBufPtr pf = FrameBuffers + i;
        FreeFrameBuf(pf);
    }
}

FrameBufPtr GetFrameBuf()
{
    FrameBufPtr tmp;
    USER_ENTER_CRITICAL();
    tmp = FrameHead;
    if (tmp) {
        FrameHead = tmp->pNext;
    }
    USER_EXIT_CRITICAL();

    return tmp;
}

void FreeFrameBuf(FrameBufPtr pf)
{
    USER_ENTER_CRITICAL();
    pf->pNext = FrameHead;
    FrameHead = pf;
    USER_EXIT_CRITICAL();

}
