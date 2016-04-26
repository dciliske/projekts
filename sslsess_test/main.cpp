#include <predef.h>
#include <constants.h>
#include <ctype.h>
#include <ucos.h>
#include <system.h>
#include <init.h>
#include <crypto/SSLSession.h>
#include <crypto/rsa.h>


const char AppName[] = "sslsess_test";

extern "C" {
    void UserMain( void * pd );
}

SessionDataStruct::SessionDataStruct testSess;
private_key::private_key testKey;

void UserMain( void * pd )
{
    init();

    iprintf("Application: %s\r\nNNDK Revision: %s\r\n",AppName,GetReleaseTag());
    while(1) {
        OSTimeDly(TICKS_PER_SECOND);
    }
}
