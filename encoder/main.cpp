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


const char AppName[] = "encoder";

extern "C" {
    void UserMain( void * pd );
}

int32_t count = 0;

volatile timerstruct &timer_a = sim2.timer[TIMER_A_NUMBER];
volatile timerstruct &timer_b = sim2.timer[TIMER_B_NUMBER];

uint8_t state;
uint32_t tcr_a, tcr_b;

uint32_t a_count, b_count, zed_count;

void SetupCaptureInput()
{
//    uint16_t tmr = 0x0302 | (rising ? 0x0040 : 0x0080);
    timer_a.tmr = 0x0000;
    timer_a.tmr = 0x03C2; // prescale by 8
    timer_a.txmr = 0x00;
    timer_a.ter = 0x03;
    timer_a.trr = 0xFFFFFFF0;
    timer_a.tcn = 0x00000000;

    timer_b.tmr = 0x0000;
    timer_b.tmr = 0x03C2;
    timer_b.txmr = 0x00;
    timer_b.ter = 0x03;
    timer_b.trr = 0xFFFFFFF0;
    timer_b.tcn = 0x00000000;

    USER_ENTER_CRITICAL();
    J2[36].function(PIN_GPIO);
    J2[34].function(PIN_GPIO);

    state = (J2[33].read() << 1)| J2[32].read();
    J2[36].function(PINJ2_36_T0IN);
    J2[34].function(PINJ2_34_T1IN);

    timer_a.tmr = 0x03C3;
    timer_b.tmr = 0x03C3;
    USER_EXIT_CRITICAL();
    switch (state) {
        case 3: state = 4; break;
        case 4: state = 3; break;
    }
}

INTERRUPT( EncoderAISR, 0x2400 )
{
    uint32_t tmp_capt = timer_a.tcr;

    if (tmp_capt < tcr_a)   { tmp_capt = tcr_a - tmp_capt; }
    else                    { tmp_capt = tmp_capt - tcr_a; }

    if (tmp_capt < DEBOUNCE_PERIOD) {
        timer_a.ter = 0x01;
        return;
    }
    tcr_a = tmp_capt;

    switch (state) {
        case 0: { state = 1; count++; break; }
        case 1: { state = 0; count--; break; }
        case 2: { state = 3; count++; break; }
        case 3: { state = 2; count--; break; }
    }
    a_count++;
    timer_a.ter = 0x03;
}

INTERRUPT( EncoderBISR, 0x2400 )
{
    uint32_t tmp_capt = timer_a.tcr;

    if (tmp_capt < tcr_b)   { tmp_capt = tcr_b - tmp_capt; }
    else                    { tmp_capt = tmp_capt - tcr_b; }

    if (tmp_capt < DEBOUNCE_PERIOD) {
        timer_b.ter = 0x01;
        return;
    }
    tcr_b = tmp_capt;

    switch (state) {
        case 0: { state = 3; count--; break; }
        case 1: { state = 2; count++; break; }
        case 2: { state = 1; count--; break; }
        case 3: { state = 0; count++; break; }
    }
    b_count++;
    timer_b.ter = 0x03;
}

struct EdgeCaptStruct {
    bool prev;
    bool lastStable;
    int  dupCount;
};

static EdgeCaptStruct a;
static EdgeCaptStruct b;
static EdgeCaptStruct zed;

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
            a_count++;
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
            b_count++;
        }
    }

    if (zed_cur != zed.prev) {
        zed.prev = zed_cur;
        zed.dupCount = 0;
    }
    else if ((zed.dupCount >= 0) && (zed.dupCount < DEBOUNCE_COUNT)) {
        zed.dupCount++;
        if (zed.dupCount == DEBOUNCE_COUNT) {
            zed.lastStable = b_cur;
            zed_change = true;
            zed_count++;
        }
    }


    if (a_change && (a.lastStable == 1) && (b.lastStable == 1)) {
        count--;
    }
    if (b_change && (a.lastStable == 1) && (b.lastStable == 1)) {
        count++;
    }

    sim2.pit[3].pcsr |= 0x0004;
}

void TickleTask(void * pd)
{
    while(1) {
        OSTimeDly(1);
        J2[24] = !J2[24];
    }
}

void UserMain( void * pd )
{
    init();

    iprintf("Application: %s\r\nNNDK Revision: %s\r\n",AppName,GetReleaseTag());
//    J2[36].function(PIN_GPIO);
//    OSTimeDly(2);

//    risingEdge = J2[36].read() == 0;
//    J2[36].function(PINJ2_36_T0IN);
//    J2[34].function(PINJ2_34_T1IN);


//    SETUP_DMATIMER2_ISR( EncoderAISR, 4 );
//    SETUP_DMATIMER3_ISR( EncoderBISR, 4 );
//    SetupCaptureInput();
    SETUP_PIT3_ISR( EdgeCaptureISR, 6 );

    J2[36].function(PIN_GPIO);
    J2[34].function(PIN_GPIO);
    J2[28].function(PIN_GPIO);
    a.prev = a.lastStable = J2[36];
    b.prev = b.lastStable = J2[32];
    zed.prev = zed.lastStable = J2[28];

    sim2.pit[3].pcsr = 0x011E;
    sim2.pit[3].pmr = 0xF424; // IRQ every 1msec
    sim2.pit[3].pcsr |= 0x0001;


    OSSimpleTaskCreate(TickleTask, MAIN_PRIO - 1);
    while(1) {
        OSTimeDly(TICKS_PER_SECOND);
        iprintf("Count: %ld\n", count);
        iprintf("ter_a: 0x%04X\n", timer_a.ter);
        iprintf("ter_b: 0x%04X\n", timer_b.ter);
        iprintf("a_count: %ld\n", a_count);
        iprintf("b_count: %ld\n", b_count);
        iprintf("zed_count: %ld\n", zed_count);
    }
}
