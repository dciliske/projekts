#include <predef.h>
#include <constants.h>
#include <ctype.h>
#include <ucos.h>
#include <system.h>
#include <init.h>
#include <utils.h>
#include <buffers.h>
#include <http.h>
#include <websockets.h>
#include <iosys.h>
#include <system.h>
#include <iointernal.h>
#include "servo.h"
#include "nunchuck.h"


const char AppName[] = "servo_streamer";
extern http_wshandler *TheWSHandler;

extern "C" {
    void UserMain( void * pd );
}

int ws_fd = -1;

int httpstricmp( PCSTR s1, PCSTR sisupper2 )
{
   while ( (*s1) && (*sisupper2) && (toupper( *s1 ) == toupper( *sisupper2 )) )
   {
      s1++;
      sisupper2++;
   }
   if ( (*s1) && (*sisupper2) )
   {
      return 0;
   }
   return 1;
}

int MyDoWSUpgrade( HTTP_Request *req, int sock, PSTR url, PSTR rxb )
{
    if (httpstricmp(url, "SERVO")) {
        if (ws_fd < 0) {
            int rv = WSUpgrade( req, sock );
            if (rv >= 0) {
                ws_fd = rv;
                NB::WebSocket::ws_setoption(ws_fd, WS_SO_TEXT);
                return 2;
            }
            else {
                return 0;
            }
        }
    }
    NotFoundResponse( sock, url );
    return 0;
}


void UserMain( void * pd )
{
    Servo serv_x(25, false);
    Servo serv_y(27, true);
    NunchuckStruct nun;

    initWithWeb();
    Nunchuck_Init();
    TheWSHandler = MyDoWSUpgrade;


    iprintf("Application: %s\r\nNNDK Revision: %s\r\n",AppName,GetReleaseTag());
    OSTimeDly(2);
    SMPoolPtr pp;
    while(1) {
        fd_set error_fds;
        FD_ZERO( &error_fds );
        nun.Update();

        serv_x.SetPos(nun.stick.x);
        serv_y.SetPos(nun.stick.y);

//        iprintf("%lu\na_x: %4.4d - a_y: %4.4d - a_z: %4.4d\ns_x: %3.3d - s_y: %3.3d - b_z: %d - b_c: %d\n\n",
//            TimeTick, nun.accel.x, nun.accel.y, nun.accel.z,
//            nun.stick.x, nun.stick.y,
//            nun.button.z, nun.button.c);

        if (ws_fd > 0) {
            FD_SET( ws_fd, &error_fds );

            int dataLen = nun.Serialize((char *)pp->pData, ETHER_BUFFER_SIZE);
            writeall(ws_fd, (char *)pp->pData, dataLen);
        }
        if(select(1, NULL, NULL, &error_fds, 1)) {
            close(ws_fd);
            ws_fd = -1;
        }
    }
}
