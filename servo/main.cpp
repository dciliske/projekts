#include <predef.h>
#include <constants.h>
#include <ctype.h>
#include <ucos.h>
#include <system.h>
#include <init.h>
#include "servo.h"

#define SEQ_LEN 16

const char AppName[] = "servo";

extern "C" {
    void UserMain( void * pd );
}

double seq[SEQ_LEN] = {
    0, 0.25, 0.5, 0.75, 1, 0.75, 0.5, 0.25,
    0, -0.25, -0.5, -0.75, -1, -0.75, -0.5, -0.25
};

void UserMain( void * pd )
{
    init();

    Servo serv_x(25, false);
    Servo serv_y(27);
    iprintf("Application: %s\r\nNNDK Revision: %s\r\n",AppName,GetReleaseTag());
    while(1) {
        for (int i = 0; i < (sizeof(seq)/sizeof(double)); i++) {
            printf("Moving to: %d - %f\n", i, seq[i]);
            serv_x.SetPos(seq[i]);
            serv_y.SetPos(seq[i]);
            OSTimeDly(TICKS_PER_SECOND);
        }
        serv_x.SetPos(1);
        serv_y.SetPos(1);
        OSTimeDly(TICKS_PER_SECOND);
        serv_x.SetPos(-1);
        serv_y.SetPos(-1);
        OSTimeDly(TICKS_PER_SECOND);
    }
}
