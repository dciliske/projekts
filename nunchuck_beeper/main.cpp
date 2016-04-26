#include <predef.h>
#include <constants.h>
#include <ctype.h>
#include <ucos.h>
#include <system.h>
#include <init.h>
#include <pins.h>
#include "nunchuck.h"
#include "beeper.h"


#define CENTER_X_VAL 120
#define CENTER_Y_VAL 130

#define CHANGE_SCALER 10
#define MAX_REF_TICKS 500000
#define MIN_REF_TICKS 2000
#define DEFAULT_REF_TICKS 6250


const char AppName[] = "nunchuck_beeper";

extern "C" {
    void UserMain( void * pd );
}

void UserMain( void * pd )
{
    NunchuckStruct nun;
    bool beeping = false;
    bool last_c = false;
    bool c_changed = false;

    uint8_t center_x = CENTER_X_VAL;
    uint8_t center_y = CENTER_Y_VAL;

    uint32_t refTicks = DEFAULT_REF_TICKS;
    int32_t deltaAccum = 0;
    init();

    J2[36].function(PINJ2_36_T0OUT);

    iprintf("Application: %s\r\nNNDK Revision: %s\r\n",AppName,GetReleaseTag());
    nun.Init();
    while(1) {
        nun.Update();
        if (nun.button.c != last_c) {
            c_changed = true;
            last_c = nun.button.c;
        }
        if (c_changed && nun.button.c) {
            if (beeping) {
                iprintf("Stopping beep\n");
                TimerStop();
            }
            else {
                iprintf("Starting beep\n");
                TimerSetup(refTicks);
            }
            c_changed = false;
            beeping = !beeping;
        }
        if (nun.button.c && nun.button.z) {
            center_x = nun.stick.x;
            center_y = nun.stick.y;
        }
        else if (nun.button.z) {
            deltaAccum += (center_x - nun.stick.x);
            if ((deltaAccum > 0) && (deltaAccum >= CHANGE_SCALER)) {
                int32_t scaleAccum = deltaAccum/CHANGE_SCALER;
                int32_t refChange = scaleAccum * refTicks/100;
                if ((refTicks + refChange) > MAX_REF_TICKS) {
                    refTicks = MAX_REF_TICKS;
                }
                else {
                    refTicks += refChange;
                }
                deltaAccum -= scaleAccum * CHANGE_SCALER;
            }
            else if ((deltaAccum < 0) && (deltaAccum < (-CHANGE_SCALER))) {
                int32_t scaleAccum = -1 *deltaAccum/CHANGE_SCALER;
                int32_t refChange = scaleAccum * refTicks/100;
                if ((refTicks - refChange) < MIN_REF_TICKS) {
                    refTicks = MIN_REF_TICKS;
                }
                else {
                    refTicks -= refChange;
                }
                deltaAccum += scaleAccum * CHANGE_SCALER;
            }
            if (beeping) {
                TimerChange(refTicks);
            }
        }
        iprintf("refTicks: %lu\n", refTicks);
        OSTimeDly(1);
    }
}
