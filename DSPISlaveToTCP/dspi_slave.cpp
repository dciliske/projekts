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
#include <nettypes.h>
#include <sim5441x.h>
#include <ucos.h>
#include <ucosmcfc.h>
#include <cfinter.h>
#include <intcdefs.h>
#include <iosys.h>
#include <buffers.h>
#include <string.h>
#include <pins.h>
#include "edma.h"
#include "stackframe.h"

#include <stdio.h>
#define LOGME iprintf("We made it to line %d of file %s.\n", __LINE__, __FILE__);

// DMA CHANNEL NUMBERS
#define DMA_CH_DSPI_0_RX    0x0C    // (12)
#define DMA_CH_DSPI_0_TX    0x0D    // (13)
#define DMA_CH_DSPI_1_RX    0x0E    // (14)
#define DMA_CH_DSPI_1_TX    0x0F    // (15)
#define DMA_CH_DSPI_2_RX    0x1C    // (28)
#define DMA_CH_DSPI_2_TX    0x1D    // (29)
#define DMA_CH_DSPI_3_RX    0x2C    // (44)
#define DMA_CH_DSPI_3_TX    0x2D    // (45)

#define RATE_LIMIT_CH 46
#define RX_FRAME_COUNT 40

PTCD tcdHead, tcdTail;
uint8_t pinSetBuf = 0x08; // used by the rate limit transfer, sets Port C 3


OS_FIFO dspiRXFifo;

buffer_list DestBuffers;
static bool BackHalf;
static uint8_t RxBuffer[RX_FRAME_COUNT*FRAME_SIZE] __attribute__(( aligned(2) )) FAST_USER_VAR;

void DumpTCD(volatile PTCD ptcd) {
    iprintf("tcd: %p\n", ptcd);
    iprintf("saddr:  %08lX\n", ptcd->saddr);
    iprintf("attr:   %04X\n", ptcd->attr);
    iprintf("soff:   %04X\n", ptcd->soff);
    iprintf("nbytes: %08lX\n", ptcd->nbytes);
    iprintf("daddr:  %08lX\n", ptcd->daddr);
    iprintf("citer:  %04X\n", ptcd->citer);
    iprintf("doff:   %04X\n", ptcd->doff);
    iprintf("dlast:  %08lX\n", ptcd->dlast_sga);
    iprintf("biter:  %04X\n", ptcd->biter);
    iprintf("csr:    %04X\n", ptcd->csr);
}

INTERRUPT( dmarx, 0x2700 )
{
    Pins[49] = 1;
    PoolPtr pp = DestBuffers.RemoveHead();
    if (pp) {
        memcpy(
                pp->pData,
                RxBuffer + (BackHalf ? RX_FRAME_COUNT*FRAME_SIZE/2 : 0),
                RX_FRAME_COUNT*FRAME_SIZE/2
              );
        pp->usedsize = RX_FRAME_COUNT*FRAME_SIZE/2;

        OSFifoPost( &dspiRXFifo, (OS_FIFO_EL *)pp);
    }
    BackHalf = !BackHalf;
    sim2.edma.cint = DMA_CH_DSPI_3_RX;
}

//void ReadyRateLimiter()
//{
//    volatile edma_tcdstruct &tcd = sim2.edma.tcd[RATE_LIMIT_CH];
//    tcd.saddr = (DWORD)&pinSetBuf;
//    tcd.attr = 0x0000;
//    tcd.soff = 0;
//    tcd.nbytes = 1;
//    tcd.slast = 0;
//    tcd.daddr = (DWORD)&sim1.gpio.ppdsdr_c;
//    tcd.citer = 1;
//    tcd.doff = 0;
//    tcd.dlast_sga = 0;
//    tcd.biter = 1;
//    tcd.csr = 0x0000;
//    sim2.edma.cdne = RATE_LIMIT_CH;
//    sim2.edma.serq = RATE_LIMIT_CH;
//}

//void PrepTCD( volatile PTCD ptcd, volatile dspistruct &spi, DWORD daddr )
//{
//    ptcd->saddr = ((DWORD)&spi.popr)+2;
//    ptcd->attr = 0x0101;
//    ptcd->soff = 0x0000;
//    ptcd->nbytes = 2;
//    ptcd->slast = 0;
//    ptcd->daddr = daddr;
//    ptcd->citer = FRAME_SIZE/2;
//    ptcd->doff = 2;
//    ptcd->dlast_sga = 0;
//    ptcd->biter = FRAME_SIZE/2;
//    ptcd->csr = RATE_LIMIT_CH << 8 | 0x2A;
//}


//void LoadNewTCD(volatile dspistruct &spi, int spiCh)
//{
//    FrameBufPtr pf;
//    PTCD ptcd;
//    pf = GetFrameBuf();
//    if ( !pf ) {
//        return;
//    }
//    ptcd = GetTCD();
//    if ( !ptcd ) {
//        FreeFrameBuf( pf );
//        return;
//    }
//    PrepTCD( ptcd, spi, (DWORD)pf->pdata );
//    // FIXME: This is major hack that has a hugely insedious timing issue.
//    uint32_t erqh = sim2.edma.erqh;
//    uint32_t erql = sim2.edma.erql;
//    sim2.edma.cerq = spiCh;
//    volatile edma_tcdstruct &rxtcd = sim2.edma.tcd[spiCh];
//    // Wait until minor transfer completes
//    while (rxtcd.csr & 0x0040) ;
//
//    UCOS_ENTER_CRITICAL();
//    if (tcdTail) {
//        tcdTail->dlast_sga = ptcd;
//        tcdTail->csr = (tcdTail->csr & 0xFF77) | 0x0010;
//        if (tcdTail == tcdHead) {
//            rxtcd.dlast_sga = (DWORD)ptcd;
//            rxtcd.csr = (tcdTail->csr & 0xFF77);
//            rxtcd.csr |= 0x0010;
//        }
//        tcdTail = ptcd;
//    }
//    else {
//        bool enabled;
//        if (spiCh > 31) {
//            enabled = erqh & (1 << (spiCh - 32));
//        }
//        else {
//            enabled = erql & (1 << spiCh);
//        }
//        if (enabled) {
//            rxtcd.dlast_sga = (DWORD)ptcd;
//            rxtcd.csr = (rxtcd.csr & 0xFFF7) | 0x0012;
//        }
//        else {
//            PrepTCD((PTCD)&rxtcd, spi, (DWORD)pf->pdata);
//        }
//        tcdHead = tcdTail = ptcd;
//    }
//    sim1.gpio.podr_c = 0x00;
//    UCOS_EXIT_CRITICAL();
//    sim2.edma.serq = spiCh;
//}

void ReadyDMA( volatile dspistruct &spi, int spiCh)
{
    volatile edma_tcdstruct &rxtcd = sim2.edma.tcd[spiCh];
    rxtcd.saddr = ((uint32_t)&spi.popr)+2;
    rxtcd.attr = 0x0101; // 16 bit transfer
    rxtcd.soff = 0x0000;
    rxtcd.nbytes = 2;
    rxtcd.slast = 0;
    rxtcd.daddr = (uint32_t)RxBuffer;
    rxtcd.citer = (RX_FRAME_COUNT*FRAME_SIZE)/2;
    rxtcd.doff = 2;
    rxtcd.dlast_sga = -(RX_FRAME_COUNT*FRAME_SIZE);
    rxtcd.biter = (RX_FRAME_COUNT*FRAME_SIZE)/2;
    rxtcd.csr = 0x0006; // enable IRQ at half and full
}


void ConfigRX(volatile dspistruct &spi, int spiCh)
{
    OSFifoInit(&dspiRXFifo);
//    InitTCDs();
//    InitFrameBufs();
//    for (int i = 0; i < (DESCRIPTOR_COUNT - 1); i++) {
//        LoadNewTCD(spi, spiCh);
//    }

    SETUP_DMA44_ISR( dmarx, 6);
    spi.mcr = 0x00FF0C01;
    spi.ctar[0] = 0x7A000001;
    spi.sr = 0x9A0A0000;
    spi.rser = 0x00030000; // enable only RFDF via DMA
    spi.mcr = 0x00FF0E00;

    for (int i = 0; i < 10; i++) {
        PoolPtr pp;
        while (!(pp = GetBuffer()));
        DestBuffers.InsertTail(pp);
    }
//    ReadyRateLimiter();
    ReadyDMA(spi, spiCh);
    sim2.edma.serq = spiCh;
}

void dspiRXTask( void *arg )
{

}



