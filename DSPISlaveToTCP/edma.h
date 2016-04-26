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

#ifndef  __EDMA_H
#define  __EDMA_H

#include <predef.h>
#include <nettypes.h>
#include <sim5441x.h>

#define DESCRIPTOR_COUNT 64

typedef struct TCD {
   vudword saddr;           /*      0x0000 ->      0x0003 - Source Address                                          */
   vuword  attr;            /*      0x0004 ->      0x0005 - Transfer Attributes                                     */
   vuword  soff;            /*      0x0006 ->      0x0007 - Signed Source Address Offset                            */
   vudword nbytes;          /*      0x0008 ->      0x000B - Signed Minor Loop Offset/Minor Byte Count               */
   vudword slast;           /*      0x000C ->      0x000F - Last Source Address Adjustment                          */
   vudword daddr;           /*      0x0010 ->      0x0013 - Destination Address                                     */
   vuword  citer;           /*      0x0014 ->      0x0015 - Current Minor Loop Link/Major Loop Count                */
   vuword  doff;            /*      0x0016 ->      0x0017 - Signed Destination Address Offset                       */
   struct TCD    *dlast_sga;       /*      0x0018 ->      0x001B - Last Destination Addr. Adjustment/Scatter Gather Addr.  */
   vuword  biter;           /*      0x001C ->      0x001D - Beginning Minor Loop Link/Major Loop Count              */
   vuword  csr;             /*      0x001E ->      0x001F - Control and Status                                      */
} __attribute__(( packed )) TCD;

typedef TCD *PTCD;

void InitTCDs();

PTCD GetTCD();

void FreeTCD(PTCD tcd);

#endif   /* ----- #ifndef __EDMA_H  ----- */
