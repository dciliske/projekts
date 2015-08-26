#include <predef.h>
#include <constants.h>
#include <ctype.h>
#include <ucos.h>
#include <system.h>
#include <init.h>
#include <utils.h>
#include "servo.h"
#include "nunchuck.h"


const char AppName[] = "nunchuck_servo";

extern "C" {
    void UserMain( void * pd );
}

void UserMain( void * pd )
{
    Servo serv_x(25, false);
    Servo serv_y(27, true);
    NunchuckStruct nun;

    init();
    Nunchuck_Init();

    iprintf("Application: %s\r\nNNDK Revision: %s\r\n",AppName,GetReleaseTag());
    OSTimeDly(2);
    while(1) {
        nun.Update();

        serv_x.SetPos(nun.stick.x);
        serv_y.SetPos(nun.stick.y);
                iprintf("%lu\na_x: %4.4d - a_y: %4.4d - a_z: %4.4d\ns_x: %3.3d - s_y: %3.3d - b_z: %d - b_c: %d\n\n",
                TimeTick, nun.accel.x, nun.accel.y, nun.accel.z,
                nun.stick.x, nun.stick.y,
                nun.button.z, nun.button.c);

        OSTimeDly(1);
    }
}
