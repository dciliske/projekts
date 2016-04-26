#include <predef.h>
#include <constants.h>
#include <ctype.h>
#include <ucos.h>
#include <system.h>
#include <init.h>
#include <utils.h>
#include <ucos.h>



const char AppName[] = "SchedulerTest";

extern "C" {
    void UserMain( void * pd );
}

DWORD SemTestTask_Stk[USER_TASK_STK_SIZE];

void SemTestTask( void *pd )
{
    OS_SEM *pSem = (OS_SEM *)pd;
    while (1) {
        if(OSSemPend(pSem, 10) == OS_TIMEOUT) {
            iprintf("Pend Failed\n");
        }
        else {
            iprintf("Sem Tickled\n");
        }
    }
}



DWORD FifoTestTask_Stk[USER_TASK_STK_SIZE];

void FifoTestTask( void *pd )
{
    OS_FIFO *pFifo = (OS_FIFO *)pd;
    OS_FIFO_EL *pEl;
    while (1) {
        pEl = OSFifoPend(pFifo, 10);
        if (!pEl) {
            iprintf("Fifo Failed\n");
        }
        else {
            iprintf("Fifo posted\n");
        }
    }
}

    rtosTaskBitList testList;
void UserMain( void * pd )
{
    OS_SEM TestSem;
    OS_FIFO TestFifo;
    OS_FIFO_EL Element;

    OSSemInit(&TestSem, 0);
    OSFifoInit(&TestFifo);

    testList.set(5);
    testList.set(10);
    testList.set(33);
    testList.set(34);
    iprintf("LowBit: %ld -- Should Be: %ld\n", testList.getlow(), 34);
    testList.clr(34);
    iprintf("LowBit: %ld -- Should Be: %ld\n", testList.getlow(), 33);
    testList.clr(33);
    iprintf("LowBit: %ld -- Should Be: %ld\n", testList.getlow(), 10);
    testList.clr(10);
    iprintf("LowBit: %ld -- Should Be: %ld\n", testList.getlow(), 5);
    testList.clr(5);
    iprintf("LowBit: %ld -- Should Be: %ld\n", testList.getlow(), -1);
    iprintf("Application: %s\r\nNNDK Revision: %s\r\n",AppName,GetReleaseTag());

    OSChangePrio(MAIN_PRIO);
    OSTaskCreate( SemTestTask, &TestSem, &SemTestTask_Stk[USER_TASK_STK_SIZE], SemTestTask_Stk, IP_PRIO, true );
    OSTaskCreate( FifoTestTask, &TestFifo, &FifoTestTask_Stk[USER_TASK_STK_SIZE], FifoTestTask_Stk, ETHER_SEND_PRIO, true );

    while(1) {
        iprintf("TimeTick: %lu\n", TimeTick);
        OSTimeDly(TICKS_PER_SECOND);
        OSSemPost(&TestSem);
        OSFifoPost(&TestFifo, &Element);
    }
}
