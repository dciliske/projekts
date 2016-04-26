#include <predef.h>
#include <constants.h>
#include <ctype.h>
#include <ucos.h>
#include <system.h>
#include <init.h>
#include <smarttrap.h>


const char AppName[] = "FlashCmpTest";

extern "C" {
    void UserMain( void * pd );
}

#define SPI_Config_Base (0)
#define GCSTORE_SIZE (8192)
int FlashCompare( const uint8_t *pWhere, const uint8_t *pWhat_in, uint32_t olen );

void UserMain( void * pd )
{
    initWithWeb();
    EnableSmartTraps();

    iprintf("Application: %s\r\nNNDK Revision: %s\r\n",AppName,GetReleaseTag());
    iprintf("FlashCompare: %s\n", FlashCompare( SPI_Config_Base, (const uint8_t *)&gConfigRec, GCSTORE_SIZE) == 0
                                ? "Success" : "Failure");
    iprintf("FlashCompare: %s\n", FlashCompare( SPI_Config_Base, ((const uint8_t *)&gConfigRec)+1, GCSTORE_SIZE) == 0
                                ? "Success" : "Failure");
    while(1) {
        OSTimeDly(TICKS_PER_SECOND);
    }
}
