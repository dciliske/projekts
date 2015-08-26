#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <dhcpclient.h>
#include <ucos.h>
#include <pins.h>

#define BITSET_SIZ ((OS_MAX_TASKS/32) + ((OS_MAX_TASKS%32)!=0))
#define IDLE_BITSET_BIT (BITSET_SIZ*32-1)

extern "C" {
void UserMain(void * pd);
}


const char * AppName="roundrobin";

volatile int nInit;
const int rrTasks=5;
static DWORD   rr_Stk[rrTasks][USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
static char rr_names[rrTasks][30];
static char *names[rrTasks];
volatile DWORD rr_Counter[rrTasks];

extern "C" uint32_t GetHighFrom( volatile uint32_t *bitField );

//class rtosTaskBitList {
//    uint32_t bitset[BITSET_SIZ];
//public:
//       void clear_all() volatile {
//           for(int i=0; i<BITSET_SIZ; i++) {
//               bitset[i]=0;
//           }
//           // IdleTask bit must always be set
//           bitset[BITSET_SIZ - 1] = 1;
//       };
//       void set(int n)  volatile {bitset[n>>5]|=(1<<(31-(n % 32))); }
//       void clr(int n)  volatile {bitset[n>>5]&=~((1<<(31-(n % 32))));};
//       int gethigh()    volatile {return GetHighFrom(bitset);};
//};


rtosTaskBitList testList, secondList;


struct RoundRobinElement
{
    OS_TCB * pTcb;
    int n;
    bool Suspended;
};

RoundRobinElement rr_group[rrTasks];



void RegisterInRRGroup(RoundRobinElement * rrg,int n)
{
    rrg[n].pTcb=(OS_TCB *)OSTCBCur;
    rrg[n].Suspended=false;
    rrg[n].n=n;
}



void RoundRobinTask(void * pd)
{
    int n=(int)pd;
    RegisterInRRGroup(rr_group,n);
    OSTimeDly(4);  //Very important that all tasks get registered in Group before ANY really start running...
    while (1)
    {
        //      OSTimeDly(1);
        rr_Counter[n]++;
    }
}


volatile int numSuspended;


void Wake(int n)
{
    rr_group[n].Suspended=false;
    BYTE p = rr_group[n].pTcb->OSTCBPrio;
//    OSRdyGrp |= OSMapTbl[p >> 3];
//    OSRdyTbl[p >> 3] |= OSMapTbl[p & 0x07];
    OSRdyGrp.set(p);
    numSuspended--;

}

void Suspend(int n)
{
    rr_group[n].Suspended=true;
    BYTE p = rr_group[n].pTcb->OSTCBPrio;

//    if ( ( OSRdyTbl[p >> 3] &= ~OSMapTbl[p & 0x07] ) == 0 )
//    {
//        OSRdyGrp &= ~OSMapTbl[p >> 3];
//    }
    OSRdyGrp.clr(p);
    OSTCBCur->OSTCBDly = 0;
    numSuspended++;

}


OS_SEM RRSuspendedSem;



void TopRR(void * pd)
{
    int nSusIn=0;
    while(1)
    {
        if((nSusIn==0) && (numSuspended!=0))
        {
            OSSemPost(&RRSuspendedSem);
        }

        OSTimeDly(1);
        static int n_last_test;
        int nr=-1;
        UCOS_ENTER_CRITICAL();
        nSusIn=numSuspended;
        for (int i=0; i<rrTasks;  i++)
        {
            int nel=(i+n_last_test)%rrTasks;

            if(rr_group[nel].pTcb->OSTCBStat==OS_STAT_RDY)
            {
                if(rr_group[nel].Suspended)
                {
                    Wake(nel);
                    nr=nel;
                }
                else
                {
                    if(nr!=-1)
                    {
                        Suspend(nr);
                    }
                    nr=nel;
                }
            }
        }
        n_last_test=(n_last_test+1)% rrTasks;

        UCOS_EXIT_CRITICAL();
    }
}





void BotRR(void * pd)
{
    while(1)
    {
        OSSemPend(&RRSuspendedSem,0);
        //To get here we have a suspended task and every other test has gone to sleep
        UCOS_ENTER_CRITICAL();
        for (int i=0; i<rrTasks;  i++)
        {
            if(rr_group[i].Suspended)
            {Wake(i);
            }


        }


        UCOS_EXIT_CRITICAL();

    }
}


//#define FIND_FIRST_BIT(bitfield, bit, index) \
//    asm volatile( "movel %2, %%a0\n clrl %%d1\n1: movel (%%a0)+, %%d0\n ff1 %%d0\n cmpil #32, %%d0\n blt 1f\n addil #1, %%d1\n jmp 1b\n 1: movel %%d0, %0\n movel %%d1, %1\n" : "=&g"(bit), "=g"(index) : "g"(bitfield) : "d0","d1","a0");


uint32_t bitField[4] = {0x0000, 0x0000, 0x0030, 0xFFFF};
uint32_t bit, index;

volatile uint32_t testArray[(64/32) + ((64%32)!=0)];

extern "C" int AddTest(uint32_t bitField)
{
    int bit = 5;
    int index = 2;
    return bit + index*32;
}

void DelayTask(void * pd)
{
    while (1) {
        OSTimeDly(TICKS_PER_SECOND);
    }
}

extern volatile bool OSRRPoolActive[];
extern volatile OS_RR_GRP OSRRPool[];

void UserMain(void * pd)
{

    InitializeStack();/* Setup the TCP/IP stack buffers etc.. */

    OSChangePrio(MAIN_PRIO);/* Change our priority from highest to something in the middle */

    GetDHCPAddressIfNecessary();/*Get a DHCP address if needed*/
    /*You may want to add a check for the return value from this function*/
    /*See the function definition in  \nburn\include\dhcpclient.h*/

    EnableAutoUpdate();/* Enable the ability to update code over the network */

    StartHTTP();/* Start the web serrver */

    testList.clear_all();
    secondList.clear_all();
    iprintf("Should be 63: %d\n", testList.getlow());
    for (int i = 0; i < OS_LO_PRIO; i++) {
        if (!(i%2)) {
            secondList.set(i);
        }
        if (!(i%3)) {
            secondList.set(i);
        }
    }
    secondList &= testList;
    iprintf("secondList &= testList:\n");
    for (int i = 0; i < OS_LO_PRIO; i++) {
        if (secondList.isset(i)) {
            iprintf("Bit %d is set\n", i);
        }
    }
    iprintf("Should be 5: %d\n", testList.gethigh());
    testList.clr(5);
    iprintf("Should be 63: %d\n", testList.gethigh());

    iprintf("About to start the dance hit a key..");
    OSSimpleTaskCreate(DelayTask, MAIN_PRIO - 1);
//    while(1) {
//        OSTimeDly(TICKS_PER_SECOND);
//        OSDumpTasks();
//    }
    getchar();

    void *dataArray[rrTasks];
    for(int i=0; i<rrTasks; i++)
    {
        siprintf(rr_names[i],"RR%d",i);
        names[i] = rr_names[i];
        dataArray[i] = (void *)i;
//        if (OSTaskCreatewName(RoundRobinTask,(void *)i,(void *)&(rr_Stk[i][USER_TASK_STK_SIZE]),(void*)&(rr_Stk[i][0]),MAIN_PRIO+(i+2),rr_names[i])!= OS_NO_ERR)
//            printf("Failed to create task prio %d\r\n",MAIN_PRIO+(i+2));
    }
    OSCreateRRPoolwName(RoundRobinTask, dataArray, rr_Stk, USER_TASK_STK_SIZE, MAIN_PRIO+2, rrTasks, names);

//    OSSemInit(&RRSuspendedSem,0);
//    OSSimpleTaskCreatewName(TopRR,MAIN_PRIO+1,"TopRR");
//    OSSimpleTaskCreatewName(BotRR,MAIN_PRIO+rrTasks+2,"BotRR");





    iprintf("Application started\n%d\n", AddTest(20));

    bit = GetHighFrom(bitField);
//    FIND_FIRST_BIT(bitField, bit, index);
    while (1)
    {
        OSTimeDly(TICKS_PER_SECOND * 1);
        iprintf("FirstBit: bitField[%ld]:[%ld]\n", bit >> 5, bit);
        for(int i=0; i<rrTasks; i++) iprintf("RR[%d]=%d ",i,rr_Counter[i]);
        iprintf("\r\n");
//        OSDumpTasks();
//        iprintf("RR_ACTIVE: \n");
//        for (int i = 0; i < 4; i++) { iprintf("  [%d]: %s\n", i, OSRRPoolActive[i] ? "True" : "False");
//        for (int j = 0; j < BITSET_SIZ; j++) {
//            iprintf("    0x%08lX\n", ((uint32_t *)&OSRRPool[i].baseRRGrp)[j]);
//        }
//        }
    }
}
