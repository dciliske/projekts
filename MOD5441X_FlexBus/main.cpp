#include <predef.h>
#include <constants.h>
#include <ctype.h>
#include <ucos.h>
#include <system.h>
#include <init.h>
#include <smarttrap.h>
#include <sim.h>
#include <utils.h>
#include <pins.h>
#include <pinconstant.h>

#define TRACE_BASE  0x10000000
#define CS_NUM      4

volatile uint16_t *TraceAddr = (volatile uint16_t *)(TRACE_BASE+8);

const char AppName[] = "MOD5441X_FlexBus";

extern "C" {
    void UserMain( void * pd );
    void trace_exception();
}

void Config()
{

    // Configure PA[0] (J2[9]) for GPIO output mode
    // Configure PA[1] (J2[10]) for GPIO input mode
    sim1.gpio.ppdsdr_h = 0x10;
    sim1.gpio.srcr_ssi0 = 0x03;
    sim1.gpio.srcr_fb1 = 0x03;
    sim1.gpio.pddr_b &= ~0x01;
    sim1.gpio.par_cs &= ~0x0C;
    sim1.gpio.pddr_h |= 0x10;
    sim1.gpio.par_be &= ~0x0F;
    sim1.gpio.par_ssi0h &= ~0x03;
}

extern BYTE TraceRunning;

void EnableTrace(int cs)
{
    // Chip selects 0 and 2 are used by the main system
    if ((cs == 0) || (cs == 2)) { return; }

    // Setup NULL catching FlexBus chip select
    sim2.cs[cs].csar = TRACE_BASE; // Set base address to 0x00xx_xxxx
    sim2.cs[cs].cscr = 0x003F2580; // Enable internal transfer acknowledge
    sim1.ccm.misccr2 |= 0x00000002; // Set the FlexBus to run at f_sys/4
    sim2.cs[cs].csmr = 0x00000001; // Set the valid bit for this chipselect

    vector_base.table[9] = (uint32_t)trace_exception;

    asm("move.l %d0, %sp@-");
    asm("move.l %d1, %sp@-");
    asm("move.w %sr, %d0");
    asm("bset #15, %d0");
    asm("move.b #1, %d1");
    asm("move.b %d1, TraceRunning");
    asm("move.w %d0, %sr");
    asm("move.l %sp@+, %d1");
    asm("move.l %sp@+, %d0");

}

extern "C" WORD GetSR_IntLevel();
void UserMain( void * pd )
{
    Config();
    initWithWeb();
    EnableSmartTraps();

    iprintf("Application: %s\r\nNNDK Revision: %s\r\n",AppName,GetReleaseTag());
    getchar();
    EnableTrace(CS_NUM);
    while(1) {
//        for (uint16_t i = 0; i < 512; i++) {
//            trace = i;
//        }
        iprintf("SR: 0x%04X\n", GetSR_IntLevel());
        OSTimeDly(TICKS_PER_SECOND);
    }
}
