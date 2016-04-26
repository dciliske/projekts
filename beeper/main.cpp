#include <predef.h>
#include <constants.h>
#include <ctype.h>
#include <ucos.h>
#include <system.h>
#include <init.h>
#include <sim.h>
#include <pins.h>
#include "beeper.h"


const char AppName[] = "beeper";

extern "C" {
    void UserMain( void * pd );
}


void UserMain( void * pd )
{
    bool beeping = true;
    init();

    J2[36].function(PINJ2_36_T0OUT);

    iprintf("Application: %s\r\nNNDK Revision: %s\r\n",AppName,GetReleaseTag());
    while(1) {
        getchar();
        Beep(5, 1);
    }
}
