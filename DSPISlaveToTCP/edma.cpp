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
#include "edma.h"


static TCD DescriptorBlocks[DESCRIPTOR_COUNT] __attribute__(( aligned(32) )) FAST_USER_VAR;
static PTCD DescriptorHead = NULL;
static bool DescriptorsInited;

void InitTCDs()
{
    if ( DescriptorsInited ) {
        return;
    }
    for (int i = 0; i < DESCRIPTOR_COUNT-1; i++) {
        PTCD ptcd = DescriptorBlocks + i;
        FreeTCD( ptcd );
    }
}

PTCD GetTCD()
{
    PTCD tmp;
    USER_ENTER_CRITICAL();
    tmp = DescriptorHead;
    if (DescriptorHead) {
        DescriptorHead = DescriptorHead->dlast_sga;
    }
    USER_EXIT_CRITICAL();

    return tmp;
}

void FreeTCD(PTCD tcd)
{
    USER_ENTER_CRITICAL();
    tcd->dlast_sga = DescriptorHead;
    DescriptorHead = tcd;
    USER_EXIT_CRITICAL();
}
