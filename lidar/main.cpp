#include <predef.h>
#include <constants.h>
#include <ctype.h>
#include <ucos.h>
#include <system.h>
#include <init.h>
#include "lidar_lite.h"


const char AppName[] = "lidar";

extern "C" {
    void UserMain( void * pd );
}

void UserMain( void * pd )
{
    init();

    iprintf("Application: %s\r\nNNDK Revision: %s\r\n",AppName,GetReleaseTag());

    getchar();
    LLInit(1);

    while(1) {
        uint16_t dist = LLRead(1);
        iprintf("dist: 0x%04X\n", dist);
    }
}
