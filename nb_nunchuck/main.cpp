#include <predef.h>
#include <constants.h>
#include <ctype.h>
#include <ucos.h>
#include <system.h>
#include <init.h>
#include <utils.h>
#include <multichanneli2c.h>
#include "nunchuck.h"
//#include <sim.h>

//i2cstruct &i2c = sim2.i2c0;

const char AppName[] = "nb_nunchuck";

extern "C" {
    void UserMain( void * pd );
}

void UserMain( void * pd )
{
    NunchuckStruct nun;

    init();

    iprintf("Application: %s\r\nNNDK Revision: %s\r\n",AppName,GetReleaseTag());

    // Init nunchuck
    Nunchuck_Init();

    OSTimeDly(2);
    while(1) {
        nun.Update();

        iprintf("%lu\na_x: %4.4d - a_y: %4.4d - a_z: %4.4d\ns_x: %3.3d - s_y: %3.3d - b_z: %d - b_c: %d\n\n",
                TimeTick, nun.accel.x, nun.accel.y, nun.accel.z,
                nun.stick.x, nun.stick.y,
                nun.button.z, nun.button.c);
//        getchar();
        OSTimeDly(1);
    }
}
