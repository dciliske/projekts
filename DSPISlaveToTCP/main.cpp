#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <dhcpclient.h>
#include <dspi.h>
#include <pins.h>
#include <sim5441x.h>
#include <tcp.h>
#include "edma.h"
#include "stackframe.h"
#include "dspi_slave.h"

#define TARGET_ADDR 0x0A01014B


extern "C" {
void UserMain(void * pd);
}


const char * AppName="DSPI_slave";

const uint32_t BUFFER_SIZE = 4096;

extern dspiDriverStruct driverCxt[];
BYTE TXBuffer[BUFFER_SIZE];
BYTE RXBuffer[BUFFER_SIZE];
OS_SEM DSPI_SEM;

void dumpDSPIStruct( int module, volatile dspistruct &spi )
{
    iprintf("DSPI %d:\r\n", module);
    iprintf("   MCR: %08lX\r\n", spi.mcr);
    iprintf("   TCR: %08lX\r\n", spi.tcr);
    for (int i = 0; i < 8; i++)
        iprintf("  CTAR[%d]: %08lX\r\n", i, spi.ctar[i]);
    iprintf("     SR: %08lX\r\n", spi.sr);
    iprintf("   RSER: %08lX\r\n", spi.rser);
}

extern OS_FIFO dspiRXFifo;

void DumpTCD(volatile PTCD ptcd);

void DumpDSPIStruct( int module, volatile dspistruct &spi )
{
iprintf("DSPI %d:\n", module);
iprintf(" MCR: %08lX\n", spi.mcr);
iprintf(" TCR: %08lX\n", spi.tcr);
for (int i = 0; i < 8; i++)
iprintf(" CTAR[%d]: %08lX\n", i, spi.ctar[i]);
iprintf(" SR: %08lX\n", spi.sr);
iprintf(" RSER: %08lX\n", spi.rser);
}

extern buffer_list DestBuffers;

void MonTask(void *arg)
{
    while (1) {
        OSTimeDly(TICKS_PER_SECOND);
        iprintf("\n");
        DumpTCD((PTCD)&sim2.edma.tcd[DMA_CH_DSPI_3_RX]);
        DumpDSPIStruct(3, sim1.dspi3);
//        iprintf("daddr: 0x%08lX\n", sim2.edma.tcd[DMA_CH_DSPI_3_RX].daddr);
    }
}

void UserMain(void * pd)
{
    InitializeStack();/* Setup the TCP/IP stack buffers etc.. */

    GetDHCPAddressIfNecessary();/*Get a DHCP address if needed*/
    /*You may want to add a check for the return value from this function*/
    /*See the function definition in  \nburn\include\dhcpclient.h*/

    OSChangePrio(MAIN_PRIO);/* Change our priority from highest to something in the middle */

    EnableAutoUpdate();/* Enable the ability to update code over the network */

    iprintf("Application started\n");

    // Setup DSPI3
    Pins[36].function(PIN_36_DSPI3_PCS0);
    Pins[38].function(PIN_38_DSPI3_SCK);
    Pins[32].function(PIN_32_DSPI3_SIN);
    Pins[34].function(PIN_34_DSPI3_SOUT);
    Pins[49].function(PIN_49_GPIO);

    PoolPtr pp;
    volatile dspistruct &spi = sim1.dspi3;
    int spiCh = DMA_CH_DSPI_3_RX;
    int RemoteFD;
    Pins[49] = 0;

    getchar();
    while ((RemoteFD = connect( TARGET_ADDR, 0, 20035, TICKS_PER_SECOND*5)) < 0) {
    iprintf("Error: %d\n", RemoteFD);
    }
    iprintf("Connected: %d\n", RemoteFD);
    ConfigRX(spi, spiCh);
    iprintf("Running\n");
//    OSSimpleTaskCreate(MonTask, MAIN_PRIO+1);
    while (1) {
        pp = (PoolPtr)OSFifoPend( &dspiRXFifo, 0 );
//        iprintf("pp: %p\n", pp);
//        iprintf("sr: %08lX\n", spi.sr);
//        ShowData( pp->pData, pp->usedsize);
        writeall( RemoteFD, (char *)pp->pData, pp->usedsize );
        Pins[49] = 0;
        DestBuffers.InsertTail(pp);
//        putchar('.');
//        FreeFrameBuf( pp );

    }
}
