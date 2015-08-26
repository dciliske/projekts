#include <predef.h>
#include <constants.h>
#include <ctype.h>
#include <ucos.h>
#include <system.h>
#include <init.h>
#include <sim.h>
#include <pins.h>
#include <pinconstant.h>

#define PWM_DAT_LEN 16

uint32_t PWM_Dat[PWM_DAT_LEN] __attribute__(( aligned(32) )); // 256 bit data stream, aka 8-bit resolution


const char AppName[] = "SPI_PWM";

extern "C" {
    void UserMain( void * pd );
}

void SetupDSPI()
{
    sim1.dspi3.mcr = 0x80000C01; // clear fifos, halt transfers
    sim1.dspi3.ctar[0] = 0x78000000;
    sim1.dspi3.sr = 0x9A0A0000; // clear flags
    sim1.dspi3.rser = 0x03000000; // enable TFF_RE and dma requests

    // DSPI3 pins are UART1, UART1 slewrate is bits [3:2]
    sim1.gpio.srcr_uart     &= ~0x0C;
    sim1.gpio.srcr_uart     |= 0x08;
}

void SetupDMA()
{
    volatile edma_tcdstruct &tcd = sim2.edma.tcd[45];

    sim2.edma.cr &= ~0x000300FF;

    iprintf("saddr: %p\n", PWM_Dat);
    tcd.attr = 0x0202;
    tcd.saddr = (uint32_t)PWM_Dat;
    tcd.soff = 0x0004;
    tcd.nbytes = 0x0004;

    tcd.daddr = ((uint32_t)&sim1.dspi3.pushr);
    tcd.doff = 0x0000;
    tcd.slast = -(PWM_DAT_LEN * 4); // reset to start PWM_Dat buffer
    tcd.dlast_sga = 0x00000000;
    tcd.citer = PWM_DAT_LEN;
    tcd.biter = PWM_DAT_LEN;
    tcd.csr = 0x0000;
}

uint16_t LeadBitLookup[16] = {
    0x0000, 0x8000, 0xC000, 0xE000,
    0xF000, 0xF800, 0xFC00, 0xFE00,
    0xFF00, 0xFF80, 0xFFC0, 0xFFE0,
    0xFFF0, 0xFFF8, 0xFFFC, 0xFFFE
};

void ReadyPWMDat(uint8_t val)
{
    uint8_t i = 0;
    while (val > 16) {
        PWM_Dat[i] = 0xFFFF;
        i++;
        val -= 16;
    }
    PWM_Dat[i] = LeadBitLookup[val];
    i++;
    for (; i < PWM_DAT_LEN; i++) {
        PWM_Dat[i] = 0x0000;
    }
    for (i = 0; i < PWM_DAT_LEN; i++) {
        PWM_Dat[i] |= 1 << 31;
    }
}

void StartPWM(uint8_t val)
{
    sim1.dspi3.mcr = 0x80000C01; // clear fifos, halt transfers
    sim1.dspi3.sr = 0x9A0A0000; // clear flags
    ReadyPWMDat(val);
    sim2.edma.serq = 45;
    sim1.dspi3.rser = 0x03000000; // enable TFF_RE and dma requests
    sim1.dspi3.mcr = 0xC0000000; // start transfers
}

void StopPWM()
{
    sim1.dspi3.mcr = 0x80000C01; // clear fifos, halt transfers
}

void UserMain( void * pd )
{
    init();

    SetupDSPI();
    SetupDMA();
    J2[22].function(PINJ2_22_DSPI3_SOUT);
    J2[24].function(PINJ2_24_DSPI3_SCK);

    iprintf("Application: %s\r\nNNDK Revision: %s\r\n",AppName,GetReleaseTag());

    StartPWM( 0x2F );
    while(1) {
        iprintf("PWM_DAT:\n  [\n    ");
        for (int i = 0; i < PWM_DAT_LEN; i++) {
            if (i) { putchar(',');
                if ((i%4) == 0) { iprintf("\n    "); }
            }
            iprintf(" 0x%08lX", PWM_Dat[i]);
        }
        iprintf("\n  ]\n\n");
        OSTimeDly(TICKS_PER_SECOND);

    }
}
