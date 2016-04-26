
#include "predef.h"
/*
************************************************************
*                         UCOS.C
*                         KERNEL
************************************************************
*/
#define UCOS_C //Required to stop GNU compiler complaining - DJF

#include <includes.h>
#include <basictypes.h>
#include <constants.h>
#include <utils.h>
#include <errno.h> /* To pick up errno */
#include <stdio.h>

#ifdef UCOS_TIME
    #include <bsp.h>
#endif

VDWORD TimeTick FAST_SYS_VAR;
VDWORD Secs FAST_SYS_VAR;
volatile BYTE TraceRunning FAST_SYS_VAR = 0;

asm(".global UCOS_ENTER_CRITICAL_C");
asm("UCOS_ENTER_CRITICAL_C:");
asm(" nop");
asm(" move.w #0x2700, %sr");
asm(" move.l %d0, %sp@-");
asm(" move.b TraceRunning, %d0");
asm(" tst.b %d0");
asm(" beq notrace_UCOS_ENTER_CRITICAL");
asm(" move.w #0xA700, %sr");
asm("notrace_UCOS_ENTER_CRITICAL:");
asm(" move.l %sp@+, %d0");
asm(" rts");

asm(".global UCOS_EXIT_CRITICAL_C");
asm("UCOS_EXIT_CRITICAL_C:");
asm(" nop");
asm(" move.l %d0, %sp@-");
asm(" move.l %d1, %sp@-");
asm(" move.w (OSISRLevel), %d1");
asm(" move.b TraceRunning, %d0");
asm(" tst.b %d0");
asm(" beq notrace_UCOS_EXIT_CRITICAL");
asm(" ori.l #0x8000, %d1");
asm("notrace_UCOS_EXIT_CRITICAL:");
asm(" move.w %d1, %sr");
asm(" move.l %sp@+, %d1");
asm(" move.l %sp@+, %d0");
asm(" rts");

asm(".global USER_ENTER_CRITICAL_C");
asm("USER_ENTER_CRITICAL_C:");
asm(" nop");
asm(" move.w #0x2700, %sr");
asm(" move.l %d0, %sp@-");
asm(" move.b TraceRunning, %d0");
asm(" tst.b %d0");
asm(" beq notrace_USER_ENTER_CRITICAL");
asm(" move.w #0xA700, %sr");
asm("notrace_USER_ENTER_CRITICAL:");
asm(" addq.l #1,critical_count ");
asm(" move.l %sp@+, %d0");
asm(" rts");

asm(".global USER_EXIT_CRITICAL_C");
asm("USER_EXIT_CRITICAL_C:");
asm(" nop");
asm(" move.l %d0, %sp@-");
asm(" subq.l #1,critical_count ");
asm(" move.w (critical_count), %d0");
asm(" tst %d0");
asm(" beq exit_USER_EXIT_CRITICAL");
asm(" move.w %sr, %d0;");
asm(" move.w (OSISRLevel), %d1");
asm(" move.l TraceRunning, %d0");
asm(" tst.b %d0");
asm(" beq notrace_UCOS_EXIT_CRITICAL");
asm(" ori.l #0x8000, %d1");
asm("notrace_USER_EXIT_CRITICAL:");
asm(" move.w %d1, %sr");
asm(" move.l %sp@+, %d1");
asm("exit_USER_EXIT_CRITICAL:");
asm(" move.l %sp@+, %d0");
asm(" rts");


#ifdef  UCOS_TASKLIST
#define TASK_SLEEP_AT_SEM 200
#define TASK_WAKE_AT_SEM 201
#define TASK_LOCKED 202
#define TASK_LOCKINC 203
#define TASK_UNLOCKED 204
#define TASK_UNLOCKDEC 205
#define TASK_CHGPRIO 206

#define TASLKSIZ (100)

volatile BYTE tasklisttask[TASLKSIZ] FAST_SYS_VAR;
volatile DWORD tasklisttim[TASLKSIZ] FAST_SYS_VAR;
volatile DWORD taskpc[TASLKSIZ] FAST_SYS_VAR;
volatile DWORD tasklistn FAST_SYS_VAR;

asm(".extern critical_count");


void OSLogTask( BYTE pn )
{
   register DWORD n = tasklistn++;
   n = n % TASLKSIZ;

   tasklisttask[n] = pn;
   if ( pn < 65 )
   {
      taskpc[n] = 0;
   }
   else
   {
      taskpc[n] = 0xCAFEBABE;
   }
   tasklisttim[n] = TimeTick;
}


void OSLogTaskPos( BYTE pn, DWORD * sppos )
{

   DWORD n = tasklistn++;

   n = n % TASLKSIZ;

   tasklisttask[n] = pn;

   if ( sppos )
   {
      taskpc[n] = sppos[16];
   }
   else
   {
      taskpc[n] = 0xFFFFFFFF;
   }
   tasklisttim[n] = TimeTick;

}



void ShowTaskList( void )
{
   DWORD start = 0;
   DWORD end = tasklistn;
   if ( end > ( start + TASLKSIZ ) )
   {
      start = end - TASLKSIZ;
   }

   UCOS_ENTER_CRITICAL();
   for ( ; start != end; start++ )
   {
      BYTE bf = ( tasklisttask[( start ) % TASLKSIZ] );
      DWORD tt = ( tasklisttim[( start ) % TASLKSIZ] );
      DWORD pc = ( taskpc[( start ) % TASLKSIZ] );
      iprintf( " at time= %ld ", tt );
      if ( bf >= 65 )
      {
         switch ( bf )
         {
           case TASK_SLEEP_AT_SEM:
             iprintf( "Wait for Semaphore\r\n" );
             break;
           case TASK_WAKE_AT_SEM:
             iprintf( "Wake from Semaphore\r\n" );
             break;
           case TASK_LOCKED:
             iprintf( "Task locked\r\n" );
             break;

           case TASK_LOCKINC:
             iprintf( "Task lock++\r\n" );
             break;
           case TASK_UNLOCKDEC:
             iprintf( "Task lock--\r\n" );
             break;


           case TASK_UNLOCKED:
             iprintf( "Task unlocked\r\n" );
             break;

           case TASK_CHGPRIO:
             iprintf( "Task priority changed\r\n" );
             break;


           default:
             iprintf( "Unknown flag %d\r\n", ( int ) bf );
         }
      }
      else
      {
         if ( pc != 0 )
         {
            iprintf( "Switched to Task %d PC=%08X\r\n", ( int ) bf, pc );
         }
         else
         {
            iprintf( "Switched to Task %d \r\n", ( int ) bf );
         }
      }
   }
   iprintf( "Total messages: %d\r\n", tasklistn );
   UCOS_EXIT_CRITICAL();
}
#endif

#ifdef UCOS_TIME
static uint32_t OSRunTimes[OS_MAX_TASKS];

uint32_t GetCurrentTaskTime( uint32_t * const TotalTicks )
{
    int index;
    uint32_t runTime;
    *TotalTicks = 0;
    OS_TCB *pTcb = NULL;
    UCOS_ENTER_CRITICAL();
    if (TotalTicks) {
        for ( index = 0; index < OS_MAX_TASKS; index++ ) {
            if ( OSTCBTbl[index].OSTCBPrio ) {
                pTcb = ( OSTCBTbl + index );
                *TotalTicks += pTcb->runningTime;
            }
        }
    }
    runTime = OSTCBCur->runningTime;
    UCOS_EXIT_CRITICAL();
    return runTime;
}

void ShowTaskTimes()
{
    int index;
    uint32_t tickCount = 0;
    OS_TCB *pTcb = NULL;
    iprintf( "uc/OS Tasks\r\nName     Prio    TotTicks    %%\r\n" );
    UCOS_ENTER_CRITICAL();

    for ( index = 0; index < OS_MAX_TASKS; index++ ) {
        if ( OSTCBTbl[index].OSTCBPrio ) {
            pTcb = ( OSTCBTbl + index );
            tickCount += pTcb->runningTime;
            OSRunTimes[index] = pTcb->runningTime;
        }
    }
    UCOS_EXIT_CRITICAL();

    for ( index = 0; index < OS_MAX_TASKS; index++ )
    {
        if ( OSTCBTbl[index].OSTCBPrio )
        {
            pTcb = ( OSTCBTbl + index );
            iprintf("%7.7s | %2d | %10lu | %3lu\r\n",
                    pTcb->pOSTCBName,
                    pTcb->OSTCBPrio,
                    OSRunTimes[index],
                    (OSRunTimes[index] * 100) / tickCount
                   );
        }
    }
}

void ClearTaskTimes()
{

    int index;
    OS_TCB *pTcb = NULL;
    uint16_t tickFrac = BspGetTickFraction();
    UCOS_ENTER_CRITICAL();
    for ( index = 0; index < OS_MAX_TASKS; index++ ) {
        if ( OSTCBTbl[index].OSTCBPrio ) {
            pTcb = ( OSTCBTbl + index );
            pTcb->runningTime = 0;
            pTcb->runningTimeFraction = 0;
            pTcb->switchTimeTick = TimeTick;
            pTcb->switchTimeFraction = tickFrac;
        }
    }
    UCOS_EXIT_CRITICAL();

}

static void LogTaskTime()
{
    register volatile uint16_t tickFrac;
    tickFrac = BspGetTickFraction();
    OSTCBCur->runningTime += TimeTick - OSTCBCur->switchTimeTick;
    OSTCBCur->runningTimeFraction += OSTCBCur->switchTimeFraction - tickFrac;

    // If we're more than twice maxcount, clearly we underflowed...
    if (OSTCBCur->runningTimeFraction >= (2*((DWORD)BspTickMaxCount))) {
        OSTCBCur->runningTimeFraction += BspTickMaxCount;
        OSTCBCur->runningTime--;
    }
    else if (OSTCBCur->runningTimeFraction >= BspTickMaxCount) {
        OSTCBCur->runningTimeFraction -= BspTickMaxCount;
        OSTCBCur->runningTime++;
    }
    OSTCBHighRdy->switchTimeTick = TimeTick;
    OSTCBHighRdy->switchTimeFraction = BspGetTickFraction();

}
#endif


/*
************************************************************
*                         TABLES
************************************************************
*/

#ifdef FAST_SYSTEM_VARIABLES
BYTE OSMapTbl[8] FAST_SYS_VAR;

extern BYTE const FAKE_OSMapTbl[];

BYTE const FAKE_OSMapTbl[] = {
  0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
};
BYTE OSUnMapTbl[256] FAST_SYS_VAR;

extern BYTE const FAKE_OSUnMapTbl[];
BYTE const FAKE_OSUnMapTbl[] = {
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};
#else
extern BYTE const OSMapTbl[]; // needed to export const OSMapTbl symbol
BYTE const OSMapTbl[] = {
  0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
};

BYTE const OSUnMapTbl[] = {
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};
#endif

/*
************************************************************
*                GLOBAL AND LOCAL VARIABLES
************************************************************
*/

volatile OS_TCB *OSTCBPrioTbl[64] FAST_SYS_VAR;
volatile BYTE OSRdyTbl[8] FAST_SYS_VAR;
volatile OS_TCB *OSTCBCur FAST_SYS_VAR;
volatile OS_TCB *OSTCBHighRdy FAST_SYS_VAR;
OS_TCB *OSTCBList FAST_SYS_VAR;
OS_TCB *OSTCBFreeList FAST_SYS_VAR;
volatile DWORD OSIntNesting FAST_SYS_VAR;
volatile DWORD OSLockNesting FAST_SYS_VAR;
volatile DWORD OSRdyGrp FAST_SYS_VAR;
volatile WORD OSISRLevel FAST_SYS_VAR;
volatile BOOLEAN OSRunning FAST_SYS_VAR;
/* Removed depricated
volatile BOOLEAN OSShowTasksOnLeds FAST_SYS_VAR;
*/




/*
************************************************************
*                LOCAL FUNCTION PROTOTYPES
************************************************************
*/
static void OSTaskIdle( void *data );
/*
************************************************************
*                   uCOS INITIALIZATION
************************************************************
*/
void OSInit( void *idle_task_stk_top, void *idle_task_stk_bot, BYTE maxtasks )
{
   volatile BYTE i;

   OSTCBCur = ( OS_TCB * ) 0;
   OSTCBList = ( OS_TCB * ) 0;
   OSIntNesting = 0;
   OSISRLevel = 0x2000;
   OSLockNesting = 0;
   OSRunning = 0;
   OSRdyGrp = 0;
   for ( i = 0; i < 8; i++ )
   {
      OSRdyTbl[i] = 0;
   }
   for ( i = 0; i < 64; i++ )
   {
      OSTCBPrioTbl[i] = ( OS_TCB * ) 0;
   }
   for ( i = 0; i < ( maxtasks - 1 ); i++ )
   {
      OSTCBTbl[i].OSTCBNext = &OSTCBTbl[i + 1];
   }
   OSTCBTbl[maxtasks - 1].OSTCBNext = ( OS_TCB * ) 0;
   OSTCBFreeList = &OSTCBTbl[0];
   OSTaskCreatewName( OSTaskIdle,
                 ( void * ) 0,
                 idle_task_stk_top,
                 idle_task_stk_bot,
                 OS_LO_PRIO,"Idle");
}
/*
************************************************************
*                       IDLE TASK
************************************************************
*/
static void OSTaskIdle( void *data )
{
   data = data;
   while ( 1 )
   {
      asm ( " NOP" );
      asm ( " NOP" );
      asm ( " NOP" );
      asm ( " NOP" );
      asm ( " NOP" );
      asm ( " NOP" );
      asm ( " NOP" );
      asm ( " NOP" );
      asm ( " NOP" );
      //      OS_IDLE_LOOP()
   }
}
/*
************************************************************
*                    START MULTITASKING
************************************************************
*/
void OSStart( void )
{
   register volatile BYTE x, y, p;


   y = OSUnMapTbl[OSRdyGrp];
   x = OSRdyTbl[y];
   p = ( y << 3 ) + OSUnMapTbl[x];
   OSTCBHighRdy = OSTCBPrioTbl[p];
   OSTCBCur = OSTCBHighRdy;
   OSRunning = 1;
   OSStartHighRdy();
}
/*
************************************************************
*                RUN HIGHEST PRIORITY TASK
************************************************************
*/

/* Removed / depricated
void putleds( unsigned char b );
*/

void OSSched( void )
{
   register volatile BYTE x, y, p;

   UCOS_ENTER_CRITICAL();
   if ( OSLockNesting == 0 && OSIntNesting == 0 )
   {
      y = OSUnMapTbl[OSRdyGrp];
      x = OSRdyTbl[y];
      p = ( y << 3 ) + OSUnMapTbl[x];
      OSTCBHighRdy = OSTCBPrioTbl[p];
      if ( OSTCBHighRdy != OSTCBCur )
      {
#ifdef    UCOS_TASKLIST
          DWORD *pStack = ( DWORD * ) OSTCBHighRdy->OSTCBStkPtr;
		 OSLogTaskPos( OSTCBHighRdy->OSTCBPrio, pStack);
#endif

#ifdef UCOS_TIME
         LogTaskTime();
#endif

		 OS_TASK_SW();
         asm (".global  UCOSWAITS_HERE");
         asm ("UCOSWAITS_HERE:");
/* Removed / depricated
         if ( OSShowTasksOnLeds )
         {
            putleds( ( unsigned char ) ( OSTCBCur->OSTCBPrio ) );
         }
*/
      }
   }
   UCOS_EXIT_CRITICAL();
}
/*
************************************************************
*                        ENTER ISR
************************************************************
*/
void OSIntEnterWAS()
{
   UCOS_ENTER_CRITICAL();
   OSIntNesting++;
   UCOS_EXIT_CRITICAL();
}
/*
************************************************************
*                        EXIT ISR
************************************************************
*/

#ifdef    UCOS_TASKLIST
static DWORD *pPcTmp;
#endif


void OSIntExit( void )
{
   register volatile BYTE x, y, p;

   UCOS_ENTER_CRITICAL();
   if ( --OSIntNesting == 0 && OSLockNesting == 0 )
   {
      y = OSUnMapTbl[OSRdyGrp];
      x = OSRdyTbl[y];
      p = ( y << 3 ) + OSUnMapTbl[x];
      OSTCBHighRdy = OSTCBPrioTbl[p];
      if ( OSTCBHighRdy != OSTCBCur )
      {
#ifdef    UCOS_TASKLIST
         pPcTmp = ( DWORD * ) OSTCBHighRdy->OSTCBStkPtr;
		 if(pPcTmp)
			 OSLogTaskPos( OSTCBHighRdy->OSTCBPrio, pPcTmp);
#endif

#ifdef UCOS_TIME
         LogTaskTime();
#endif

         OSIntCtxSw();
      }
   }
   //    UCOS_EXIT_CRITICAL();
   //Not needed the RTE will handle it.
}
/*
************************************************************
*          DELAY TASK 'n' TICKS   (n from 1 to 65535)
************************************************************
*/
void OSTimeDly( WORD ticks )
{
   register volatile BYTE p;

   UCOS_ENTER_CRITICAL();
   p = OSTCBCur->OSTCBPrio;
   if ( ( OSRdyTbl[p >> 3] &= ~OSMapTbl[p & 0x07] ) == 0 )
   {
      OSRdyGrp &= ~OSMapTbl[p >> 3];
   }
   OSTCBCur->OSTCBDly = ticks;
   UCOS_EXIT_CRITICAL();
   OSSched();
}



/*
************************************************************
*          Change DELAY for TASK
************************************************************
*/
void OSChangeTaskDly( WORD task_prio, WORD newticks )
{
   OSTCBPrioTbl[task_prio]->OSTCBDly = newticks;
}

/*
************************************************************
*                  PROCESS SYSTEM TICK
************************************************************
*/
void OSTimeTick( void )
{
   register volatile BYTE p;
   register volatile OS_TCB *ptcb;
   TimeTick++;
   if ( ( TimeTick % TICKS_PER_SECOND ) == 0 )
   {
      Secs++;
   }

   ptcb = OSTCBList;
   while ( ptcb->OSTCBPrio != OS_LO_PRIO )
   {
      UCOS_ENTER_CRITICAL();




      if ( ptcb->OSTCBDly != 0 )
      {
         if ( --ptcb->OSTCBDly == 0 )
         {
            p = ptcb->OSTCBPrio;
            OSRdyGrp |= OSMapTbl[p >> 3];
            OSRdyTbl[p >> 3] |= OSMapTbl[p & 0x07];
         }
      }
      UCOS_EXIT_CRITICAL();
      ptcb = ptcb->OSTCBNext;
   }
}
/*
************************************************************
*                GET TCB FROM FREE TCB LIST
************************************************************
*/
OS_TCB * OSTCBGetFree( void )
{
   register OS_TCB *ptcb;

   UCOS_ENTER_CRITICAL();
   ptcb = OSTCBFreeList;
   if(ptcb!=NULL) OSTCBFreeList = ptcb->OSTCBNext;
   UCOS_EXIT_CRITICAL();
   return ( ptcb );
}
/*
************************************************************
*                   PREVENT SCHEDULING
************************************************************
*/
void OSLock( void )
{
   UCOS_ENTER_CRITICAL();
   OSLockNesting++;
#ifdef    UCOS_TASKLIST
   if ( OSLockNesting == 1 )
   {
      OSLogTask( TASK_LOCKED );
   }
   else
   {
      OSLogTask( TASK_LOCKINC );
   }
#endif

   UCOS_EXIT_CRITICAL();
}
/*
************************************************************
*                    ENABLE SCHEDULING
************************************************************
*/
void OSUnlock( void )
{
   UCOS_ENTER_CRITICAL();
   if (OSLockNesting>0)
   {
	   OSLockNesting--;
   }

#ifdef    UCOS_TASKLIST
   if ( OSLockNesting == 0 )
   {
      OSLogTask( TASK_UNLOCKED );
   }
   else
   {
      OSLogTask( TASK_UNLOCKDEC );
   }
#endif

   UCOS_EXIT_CRITICAL();
   if(OSLockNesting==0)
   	OSSched();
}
/*
************************************************************
*              CHANGE PRIORITY OF RUNNING TASK
************************************************************
*/
BYTE OSChangePrio( BYTE newp )
{
   register volatile BYTE oldp;


   UCOS_ENTER_CRITICAL();
   if ( OSTCBPrioTbl[newp] != ( void * ) 0 )
   {
      UCOS_EXIT_CRITICAL();
      return ( OS_PRIO_EXIST );
   }
   else
   {
      oldp = OSTCBCur->OSTCBPrio;
      if ( ( OSRdyTbl[oldp >> 3] &= ~OSMapTbl[oldp & 0x07] ) == 0 )
      {
         OSRdyGrp &= ~OSMapTbl[oldp >> 3];
      }
      OSRdyGrp |= OSMapTbl[newp >> 3];
      OSRdyTbl[newp >> 3] |= OSMapTbl[newp & 0x07];
      OSTCBCur->OSTCBPrio = newp;
      OSTCBPrioTbl[newp] = ( OS_TCB * )OSTCBCur;
      OSTCBPrioTbl[oldp] =  0;

#ifdef    UCOS_TASKLIST
      OSLogTask( TASK_CHGPRIO );
      OSLogTask( newp );
#endif

      UCOS_EXIT_CRITICAL();
      OSSched();
      return ( OS_NO_ERR );
   }
}
/*
************************************************************
*                    DELETE RUNNING TASK
************************************************************
*/
void OSTaskDelete( void )
{
   register volatile BYTE p;


   UCOS_ENTER_CRITICAL();
   p = OSTCBCur->OSTCBPrio;
   OSTCBPrioTbl[p] = ( OS_TCB * ) 0;
   if ( ( OSRdyTbl[p >> 3] &= ~OSMapTbl[p & 0x07] ) == 0 )
   {
      OSRdyGrp &= ~OSMapTbl[p >> 3];
   }
   if ( OSTCBCur->OSTCBPrev == ( OS_TCB * ) 0 )
   {
      OSTCBCur->OSTCBNext->OSTCBPrev = ( OS_TCB * ) 0;
      OSTCBList = OSTCBCur->OSTCBNext;  /* Rev. A, This line was missing   */
   }
   else
   {
      OSTCBCur->OSTCBPrev->OSTCBNext = OSTCBCur->OSTCBNext;
      OSTCBCur->OSTCBNext->OSTCBPrev = OSTCBCur->OSTCBPrev;
   }
   OSTCBCur->OSTCBNext = OSTCBFreeList;
   OSTCBFreeList = (OS_TCB *)OSTCBCur;
   UCOS_EXIT_CRITICAL();
   OSSched();
}
/*
************************************************************
*                   INITIALIZE SEMAPHORE
************************************************************
*/
BYTE OSSemInit( OS_SEM *psem, long cnt )
{
   UCOS_ENTER_CRITICAL();
   if ( cnt >= 0 )
   {
      psem->OSSemCnt = cnt;
      psem->OSSemGrp = 0x00;
      psem->OSSemTbl[0] = 0x00;
      psem->OSSemTbl[1] = 0x00;
      psem->OSSemTbl[2] = 0x00;
      psem->OSSemTbl[3] = 0x00;
      psem->OSSemTbl[4] = 0x00;
      psem->OSSemTbl[5] = 0x00;
      psem->OSSemTbl[6] = 0x00;
      psem->OSSemTbl[7] = 0x00;
      UCOS_EXIT_CRITICAL();
      return ( OS_NO_ERR );
   }
   else
   {
      UCOS_EXIT_CRITICAL();
      return ( OS_SEM_ERR );
   }
}



/*
************************************************************
*                     PEND ON SEMAPHORE
************************************************************
*/
BYTE OSSemPend( OS_SEM *psem, WORD timeout )
{
   register volatile BYTE x, y, bitx, bity;


   UCOS_ENTER_CRITICAL();
   if ( psem->OSSemCnt-- > 0 )
   {
      UCOS_EXIT_CRITICAL();
      return ( OS_NO_ERR );
   }
   else
   {
      OSTCBCur->OSTCBStat |= OS_STAT_SEM;
      OSTCBCur->OSTCBDly = timeout;
      y = OSTCBCur->OSTCBPrio >> 3;
      x = OSTCBCur->OSTCBPrio & 0x07;
      bity = OSMapTbl[y];
      bitx = OSMapTbl[x];
      if ( ( OSRdyTbl[y] &= ~bitx ) == 0 )
      {
         OSRdyGrp &= ~bity;
      }
      psem->OSSemTbl[y] |= bitx;
      psem->OSSemGrp |= bity;

#ifdef    UCOS_TASKLIST
      OSLogTask( TASK_SLEEP_AT_SEM );
#endif

      UCOS_EXIT_CRITICAL();
      OSSched();
      UCOS_ENTER_CRITICAL();


#ifdef    UCOS_TASKLIST
      OSLogTask( TASK_WAKE_AT_SEM );
#endif

      if ( OSTCBCur->OSTCBStat & OS_STAT_SEM )
      {
         if ( ( psem->OSSemTbl[y] &= ~bitx ) == 0 )
         {
            psem->OSSemGrp &= ~bity;
         }
         OSTCBCur->OSTCBStat = OS_STAT_RDY;
         psem->OSSemCnt++;
         UCOS_EXIT_CRITICAL();
         return ( OS_TIMEOUT );
      }
      else
      {
         UCOS_EXIT_CRITICAL();
         return ( OS_NO_ERR );
      }
   }
}


/*
************************************************************
*                     PEND ON SEMAPHORE
************************************************************
*/
BYTE OSSemPendNoWait( OS_SEM *psem )
{
   UCOS_ENTER_CRITICAL();
   if ( psem->OSSemCnt-- > 0 )
   {
      UCOS_EXIT_CRITICAL();
      return ( OS_NO_ERR );
   }
   else
   {
      psem->OSSemCnt++;
      UCOS_EXIT_CRITICAL();
      return ( OS_TIMEOUT );
   }
}


/*
************************************************************
*                    POST TO A SEMAPHORE
************************************************************
*/
BYTE OSSemPost( OS_SEM *psem )
{
   register volatile BYTE x, y, bitx, bity, p;


   UCOS_ENTER_CRITICAL();
   if ( psem->OSSemCnt < 0x7FFFFFF )
   {
      if ( psem->OSSemCnt++ >= 0 )
      {
         UCOS_EXIT_CRITICAL();
      }
      else
      {
         if ( psem->OSSemGrp != 0 )
         {
            /* Rev. A, This line was missing   */
            y = OSUnMapTbl[psem->OSSemGrp];
            x = OSUnMapTbl[psem->OSSemTbl[y]];
            bity = OSMapTbl[y];
            bitx = OSMapTbl[x];
            if ( ( psem->OSSemTbl[y] &= ~bitx ) == 0 )
            {
               psem->OSSemGrp &= ~bity;
            }
            p = ( y << 3 ) + x;
            OSTCBPrioTbl[p]->OSTCBDly = 0;
            OSTCBPrioTbl[p]->OSTCBStat &= ~OS_STAT_SEM;
            OSRdyGrp |= bity;
            OSRdyTbl[y] |= bitx;
            UCOS_EXIT_CRITICAL();
            OSSched();
         }
         else
         {
            UCOS_EXIT_CRITICAL();
         }
      }
      return ( OS_NO_ERR );
   }
   else
   {
      UCOS_EXIT_CRITICAL();
      return ( OS_SEM_OVF );
   }
}


/*
************************************************************
*                   INITIALIZE MAILBOX
************************************************************
*/
BYTE OSMboxInit( OS_MBOX *pmbox, void *msg )
{
   UCOS_ENTER_CRITICAL();
   pmbox->OSMboxMsg = msg;
   pmbox->OSMboxDataAvail = (msg) ? TRUE : FALSE;
   pmbox->OSMboxGrp = 0x00;
   pmbox->OSMboxTbl[0] = 0x00;
   pmbox->OSMboxTbl[1] = 0x00;
   pmbox->OSMboxTbl[2] = 0x00;
   pmbox->OSMboxTbl[3] = 0x00;
   pmbox->OSMboxTbl[4] = 0x00;
   pmbox->OSMboxTbl[5] = 0x00;
   pmbox->OSMboxTbl[6] = 0x00;
   pmbox->OSMboxTbl[7] = 0x00;
   UCOS_EXIT_CRITICAL();
   return ( OS_NO_ERR );
}


/*
************************************************************
*                     PEND ON MAILBOX
************************************************************
*/
void * OSMboxPend( OS_MBOX *pmbox, WORD timeout, BYTE *err )
{
   register volatile BYTE x, y, bitx, bity;
   register void *msg;


   UCOS_ENTER_CRITICAL();
   if ( pmbox->OSMboxDataAvail )
   {
      msg = pmbox->OSMboxMsg;
      pmbox->OSMboxMsg = ( void * ) 0;
      pmbox->OSMboxDataAvail = FALSE;
      UCOS_EXIT_CRITICAL();
      *err = OS_NO_ERR;
   }
   else
   {
      OSTCBCur->OSTCBStat |= OS_STAT_MBOX;
      OSTCBCur->OSTCBDly = timeout;
      y = OSTCBCur->OSTCBPrio >> 3;
      x = OSTCBCur->OSTCBPrio & 0x07;
      bity = OSMapTbl[y];
      bitx = OSMapTbl[x];
      if ( ( OSRdyTbl[y] &= ~bitx ) == 0 )
      {
         OSRdyGrp &= ~bity;
      }
      pmbox->OSMboxTbl[y] |= bitx;
      pmbox->OSMboxGrp |= bity;
      UCOS_EXIT_CRITICAL();
      OSSched();
      UCOS_ENTER_CRITICAL();
      if ( OSTCBCur->OSTCBStat & OS_STAT_MBOX )
      {
         if ( ( pmbox->OSMboxTbl[y] &= ~bitx ) == 0 )
         {
            pmbox->OSMboxGrp &= ~bity;
         }
         OSTCBCur->OSTCBStat = OS_STAT_RDY;
         msg = ( void * ) 0;
         pmbox->OSMboxDataAvail = FALSE;
         UCOS_EXIT_CRITICAL();
         *err = OS_TIMEOUT;
      }
      else
      {
         msg = pmbox->OSMboxMsg;
         pmbox->OSMboxMsg = ( void * ) 0;
         pmbox->OSMboxDataAvail = FALSE;
         UCOS_EXIT_CRITICAL();
         *err = OS_NO_ERR;
      }
   }
   return ( msg );
}

/************************************************************
*                     PEND ON MAILBOX
************************************************************
*/
void * OSMboxPendNoWait( OS_MBOX *pmbox, BYTE *err )
{
   register void *msg;


   UCOS_ENTER_CRITICAL();
   if ( pmbox->OSMboxDataAvail )
   {
      msg = pmbox->OSMboxMsg;
      pmbox->OSMboxMsg = ( void * ) 0;
      pmbox->OSMboxDataAvail = FALSE;
      UCOS_EXIT_CRITICAL();
      *err = OS_NO_ERR;
   }
   else
   {
      msg = ( void * ) 0;
      *err = OS_TIMEOUT;
      pmbox->OSMboxDataAvail = FALSE;
      UCOS_EXIT_CRITICAL();
   }
   return ( msg );
}




/*
************************************************************
*                    POST TO A MAILBOX
************************************************************
*/

BYTE OSMboxPost( OS_MBOX *pmbox, void *msg )
{
   register volatile BYTE x, y, bitx, bity, p;


   UCOS_ENTER_CRITICAL();
   if ( pmbox->OSMboxDataAvail )
   {
      UCOS_EXIT_CRITICAL();
      return ( OS_MBOX_FULL );
   }
   else
   {
      pmbox->OSMboxMsg = msg;
      pmbox->OSMboxDataAvail = TRUE;
      if ( pmbox->OSMboxGrp != 0 )
      {
         /* Rev. A, This line was missing   */
         y = OSUnMapTbl[pmbox->OSMboxGrp];
         x = OSUnMapTbl[pmbox->OSMboxTbl[y]];
         bity = OSMapTbl[y];
         bitx = OSMapTbl[x];
         if ( ( pmbox->OSMboxTbl[y] &= ~bitx ) == 0 )
         {
            pmbox->OSMboxGrp &= ~bity;
         }
         p = ( y << 3 ) + x;
         OSTCBPrioTbl[p]->OSTCBStat &= ~OS_STAT_MBOX;
         OSTCBPrioTbl[p]->OSTCBDly = 0;
         OSRdyGrp |= bity;
         OSRdyTbl[y] |= bitx;
         UCOS_EXIT_CRITICAL();
         OSSched();
      }
      else
      {
         UCOS_EXIT_CRITICAL();
      }
      return ( OS_NO_ERR );
   }
}


/*
************************************************************
*                    INITIALIZE QUEUE
************************************************************
*/
BYTE OSQInit( OS_Q *pq, void **start, BYTE size )
{
   UCOS_ENTER_CRITICAL();
   pq->OSQStart = start;
   pq->OSQEnd = &start[size];
   pq->OSQIn = start;
   pq->OSQOut = start;
   pq->OSQSize = size;
   pq->OSQEntries = 0;
   pq->OSQGrp = 0x00;
   pq->OSQTbl[0] = 0x00;
   pq->OSQTbl[1] = 0x00;
   pq->OSQTbl[2] = 0x00;
   pq->OSQTbl[3] = 0x00;
   pq->OSQTbl[4] = 0x00;
   pq->OSQTbl[5] = 0x00;
   pq->OSQTbl[6] = 0x00;
   pq->OSQTbl[7] = 0x00;
   UCOS_EXIT_CRITICAL();
   return ( OS_NO_ERR );
}



/*
************************************************************
*                     PEND ON A QUEUE
************************************************************
*/
void * OSQPend( OS_Q *pq, WORD timeout, BYTE *err )
{
   register volatile BYTE x, y, bitx, bity;
   register void *msg;


   UCOS_ENTER_CRITICAL();
   if ( pq->OSQEntries != 0 )
   {
      msg = *pq->OSQOut++;
      pq->OSQEntries--;
      if ( pq->OSQOut == pq->OSQEnd )
      {
         pq->OSQOut = pq->OSQStart;
      }
      UCOS_EXIT_CRITICAL();
      *err = OS_NO_ERR;
   }
   else
   {
      OSTCBCur->OSTCBStat |= OS_STAT_Q;
      OSTCBCur->OSTCBDly = timeout;
      y = OSTCBCur->OSTCBPrio >> 3;
      x = OSTCBCur->OSTCBPrio & 0x07;
      bity = OSMapTbl[y];
      bitx = OSMapTbl[x];
      if ( ( OSRdyTbl[y] &= ~bitx ) == 0 )
      {
         OSRdyGrp &= ~bity;
      }
      pq->OSQTbl[y] |= bitx;
      pq->OSQGrp |= bity;
      UCOS_EXIT_CRITICAL();
      OSSched();
      UCOS_ENTER_CRITICAL();
      if ( OSTCBCur->OSTCBStat & OS_STAT_Q )
      {
         if ( ( pq->OSQTbl[y] &= ~bitx ) == 0 )
         {
            pq->OSQGrp &= ~bity;
         }
         OSTCBCur->OSTCBStat = OS_STAT_RDY;
         msg = ( void * ) 0;
         UCOS_EXIT_CRITICAL();
         *err = OS_TIMEOUT;
      }
      else
      {
         msg = *pq->OSQOut++;
         pq->OSQEntries--;
         if ( pq->OSQOut == pq->OSQEnd )
         {
            pq->OSQOut = pq->OSQStart;
         }
         UCOS_EXIT_CRITICAL();
         *err = OS_NO_ERR;
      }
   }
   return ( msg );
}



/*
************************************************************
*                     PEND ON A QUEUE
************************************************************
*/
void * OSQPendNoWait( OS_Q *pq, BYTE *err )
{
   register void *msg;


   UCOS_ENTER_CRITICAL();
   if ( pq->OSQEntries != 0 )
   {
      msg = *pq->OSQOut++;
      pq->OSQEntries--;
      if ( pq->OSQOut == pq->OSQEnd )
      {
         pq->OSQOut = pq->OSQStart;
      }
      UCOS_EXIT_CRITICAL();
      *err = OS_NO_ERR;
   }
   else
   {
      msg = ( void * ) 0;
      UCOS_EXIT_CRITICAL();
      *err = OS_TIMEOUT;
   }
   return ( msg );
}


/*
************************************************************
*                     POST TO A QUEUE
************************************************************
*/

BYTE OSQPost( OS_Q *pq, void *msg )
{
   register volatile BYTE x, y, bitx, bity, p;


   UCOS_ENTER_CRITICAL();
   if ( pq->OSQEntries >= pq->OSQSize )
   {
      UCOS_EXIT_CRITICAL();
      return ( OS_Q_FULL );
   }
   else
   {
      *pq->OSQIn++ = msg;
      pq->OSQEntries++;
      if ( pq->OSQIn == pq->OSQEnd )
      {
         pq->OSQIn = pq->OSQStart;
      }
      if ( pq->OSQGrp != 0 )
      {
         /* Rev. A, This line was missing   */
         y = OSUnMapTbl[pq->OSQGrp];
         x = OSUnMapTbl[pq->OSQTbl[y]];
         bity = OSMapTbl[y];
         bitx = OSMapTbl[x];
         if ( ( pq->OSQTbl[y] &= ~bitx ) == 0 )
         {
            pq->OSQGrp &= ~bity;
         }
         p = ( y << 3 ) + x;
         OSTCBPrioTbl[p]->OSTCBStat &= ~OS_STAT_Q;
         OSTCBPrioTbl[p]->OSTCBDly = 0;
         OSRdyGrp |= bity;
         OSRdyTbl[y] |= bitx;
         UCOS_EXIT_CRITICAL();
         OSSched();
      }
      else
      {
         UCOS_EXIT_CRITICAL();
      }
      return ( OS_NO_ERR );
   }
}



/*
************************************************************
*                     POST TO HEAD OF A QUEUE
************************************************************
*/

BYTE OSQPostFirst( OS_Q *pq, void *msg )
{
   register volatile BYTE x, y, bitx, bity, p;


   UCOS_ENTER_CRITICAL();
   if ( pq->OSQEntries >= pq->OSQSize )
   {
      UCOS_EXIT_CRITICAL();
      return ( OS_Q_FULL );
   }
   else
   {
	if(pq->OSQOut==pq->OSQStart)
	{
		pq->OSQOut=pq->OSQEnd-1;
    }
	else
		pq->OSQOut--;


      *pq->OSQOut = msg;
      pq->OSQEntries++;

	  if ( pq->OSQGrp != 0 )
      {
         /* Rev. A, This line was missing   */
         y = OSUnMapTbl[pq->OSQGrp];
         x = OSUnMapTbl[pq->OSQTbl[y]];
         bity = OSMapTbl[y];
         bitx = OSMapTbl[x];
         if ( ( pq->OSQTbl[y] &= ~bitx ) == 0 )
         {
            pq->OSQGrp &= ~bity;
         }
         p = ( y << 3 ) + x;
         OSTCBPrioTbl[p]->OSTCBStat &= ~OS_STAT_Q;
         OSTCBPrioTbl[p]->OSTCBDly = 0;
         OSRdyGrp |= bity;
         OSRdyTbl[y] |= bitx;
         UCOS_EXIT_CRITICAL();
         OSSched();
      }
      else
      {
         UCOS_EXIT_CRITICAL();
      }
      return ( OS_NO_ERR );
   }
}





/*
************************************************************
*                   INITIALIZE FIFO
************************************************************
*/
BYTE OSFifoInit( OS_FIFO *pFifo )
{
   UCOS_ENTER_CRITICAL();
   pFifo->pHead = NULL;
   pFifo->pTail = NULL;
   pFifo->OSFifoGrp = 0x00;
   pFifo->OSFifoTbl[0] = 0x00;
   pFifo->OSFifoTbl[1] = 0x00;
   pFifo->OSFifoTbl[2] = 0x00;
   pFifo->OSFifoTbl[3] = 0x00;
   pFifo->OSFifoTbl[4] = 0x00;
   pFifo->OSFifoTbl[5] = 0x00;
   pFifo->OSFifoTbl[6] = 0x00;
   pFifo->OSFifoTbl[7] = 0x00;
   UCOS_EXIT_CRITICAL();
   return ( OS_NO_ERR );
}

/*
************************************************************
*                     PEND ON FIFO
************************************************************
*/
OS_FIFO_EL * OSFifoPend( OS_FIFO *pFifo, WORD timeout )
{
   register volatile BYTE x, y, bitx, bity;
   register OS_FIFO_EL *pRet;


   UCOS_ENTER_CRITICAL();
   if ( pFifo->pHead == NULL )
   {
      OSTCBCur->OSTCBStat |= OS_STAT_FIFO;
      OSTCBCur->OSTCBDly = timeout;
      y = OSTCBCur->OSTCBPrio >> 3;
      x = OSTCBCur->OSTCBPrio & 0x07;
      bity = OSMapTbl[y];
      bitx = OSMapTbl[x];
      if ( ( OSRdyTbl[y] &= ~bitx ) == 0 )
      {
         OSRdyGrp &= ~bity;
      }
      pFifo->OSFifoTbl[y] |= bitx;
      pFifo->OSFifoGrp |= bity;
      UCOS_EXIT_CRITICAL();
      OSSched();
      UCOS_ENTER_CRITICAL();
      if ( OSTCBCur->OSTCBStat & OS_STAT_FIFO )
      {
         if ( ( pFifo->OSFifoTbl[y] &= ~bitx ) == 0 )
         {
            pFifo->OSFifoGrp &= ~bity;
         }
         OSTCBCur->OSTCBStat = OS_STAT_RDY;
         UCOS_EXIT_CRITICAL();
         return NULL;
      }
   }//Head was NULL

   pRet = pFifo->pHead;
   pFifo->pHead = pRet->pNext;
   if ( pFifo->pHead == NULL )
   {
      pFifo->pTail = NULL;
   }
   pRet->pNext = NULL;
   UCOS_EXIT_CRITICAL();
   return pRet;
}


/*
************************************************************
*                     PEND ON FIFO
************************************************************
*/
OS_FIFO_EL * OSFifoPendNoWait( OS_FIFO *pFifo )
{
   register OS_FIFO_EL *pRet;


   UCOS_ENTER_CRITICAL();
   if ( pFifo->pHead == NULL )
   {
      UCOS_EXIT_CRITICAL();
      return NULL;
   }//Head was NULL

   pRet = pFifo->pHead;
   pFifo->pHead = pRet->pNext;
   if ( pFifo->pHead == NULL )
   {
      pFifo->pTail = NULL;
   }
   pRet->pNext = NULL;
   UCOS_EXIT_CRITICAL();
   return pRet;
}




/*
************************************************************
*                    POST TO A FIFO
************************************************************
*/


BYTE OSFifoPost( OS_FIFO *pFifo, OS_FIFO_EL *pToPost )
{
   register volatile BYTE x, y, bitx, bity, p;

   UCOS_ENTER_CRITICAL();

   pToPost->pNext = NULL;
   if ( pFifo->pTail == NULL )
   {
      pFifo->pHead = pToPost;
   }
   else
   {
      pFifo->pTail->pNext = pToPost;
   }
   pFifo->pTail = pToPost;


   if ( pFifo->OSFifoGrp != 0 ) /*WE have something pending */
   {
      y = OSUnMapTbl[pFifo->OSFifoGrp];
      x = OSUnMapTbl[pFifo->OSFifoTbl[y]];
      bity = OSMapTbl[y];
      bitx = OSMapTbl[x];
      if ( ( pFifo->OSFifoTbl[y] &= ~bitx ) == 0 )
      {
         pFifo->OSFifoGrp &= ~bity;
      }
      p = ( y << 3 ) + x;
      OSTCBPrioTbl[p]->OSTCBDly = 0;
      OSTCBPrioTbl[p]->OSTCBStat &= ~OS_STAT_FIFO;
      OSRdyGrp |= bity;
      OSRdyTbl[y] |= bitx;
      UCOS_EXIT_CRITICAL();
      OSSched();
   }
   else
   {
      UCOS_EXIT_CRITICAL();
   }

   return ( OS_NO_ERR );
}



/*
************************************************************
*                    POST TO A FIFO
************************************************************
*/
BYTE OSFifoPostFirst( OS_FIFO *pFifo, OS_FIFO_EL *pToPost )
{
   register volatile BYTE x, y, bitx, bity, p;

   UCOS_ENTER_CRITICAL();
   if ( pFifo->pTail == NULL )
   {
      pFifo->pTail = pToPost;
   }

   pToPost->pNext = pFifo->pHead;
   pFifo->pHead = pToPost;


   if ( pFifo->OSFifoGrp != 0 ) /*WE have something pending */
   {
      y = OSUnMapTbl[pFifo->OSFifoGrp];
      x = OSUnMapTbl[pFifo->OSFifoTbl[y]];
      bity = OSMapTbl[y];
      bitx = OSMapTbl[x];
      if ( ( pFifo->OSFifoTbl[y] &= ~bitx ) == 0 )
      {
         pFifo->OSFifoGrp &= ~bity;
      }
      p = ( y << 3 ) + x;
      OSTCBPrioTbl[p]->OSTCBDly = 0;
      OSTCBPrioTbl[p]->OSTCBStat &= ~OS_STAT_FIFO;
      OSRdyGrp |= bity;
      OSRdyTbl[y] |= bitx;
      UCOS_EXIT_CRITICAL();
      OSSched();
   }
   else
   {
      UCOS_EXIT_CRITICAL();
   }

   return ( OS_NO_ERR );
}


extern "C"
{int getpid()
 {
    return OSTCBCur->OSTCBPrio;
 }
}


BYTE OSCritInit( OS_CRIT *pCrit )
{
   pCrit->OSCritTaskNum = 0;
   pCrit->OSCritDepthCount = 0;
   pCrit->OSCritGrp = 0x00;
   pCrit->OSCritTbl[0] = 0x00;
   pCrit->OSCritTbl[1] = 0x00;
   pCrit->OSCritTbl[2] = 0x00;
   pCrit->OSCritTbl[3] = 0x00;
   pCrit->OSCritTbl[4] = 0x00;
   pCrit->OSCritTbl[5] = 0x00;
   pCrit->OSCritTbl[6] = 0x00;
   pCrit->OSCritTbl[7] = 0x00;
   return ( OS_NO_ERR );
}


BYTE OSCritEnter( OS_CRIT *pCrit, WORD timeout )
{
   UCOS_ENTER_CRITICAL();
   if ( pCrit->OSCritTaskNum == OSTCBCur->OSTCBPrio )
   {
      pCrit->OSCritDepthCount++;
      UCOS_EXIT_CRITICAL();
      return ( OS_NO_ERR );
   }
   if ( pCrit->OSCritTaskNum == 0 )
   {
      pCrit->OSCritTaskNum = OSTCBCur->OSTCBPrio;
      pCrit->OSCritDepthCount++;
      UCOS_EXIT_CRITICAL();
      return ( OS_NO_ERR );
   }
   /* So we are entering the critical section and we have to wait */
   {
      register volatile BYTE x, y, bitx, bity;

      OSTCBCur->OSTCBStat |= OS_STAT_CRIT;
      OSTCBCur->OSTCBDly = timeout;
      y = OSTCBCur->OSTCBPrio >> 3;
      x = OSTCBCur->OSTCBPrio & 0x07;
      bity = OSMapTbl[y];
      bitx = OSMapTbl[x];
      if ( ( OSRdyTbl[y] &= ~bitx ) == 0 )
      {
         OSRdyGrp &= ~bity;
      }
      pCrit->OSCritTbl[y] |= bitx;
      pCrit->OSCritGrp |= bity;

      UCOS_EXIT_CRITICAL();
      OSSched();
      UCOS_ENTER_CRITICAL();
      if ( pCrit->OSCritTaskNum == OSTCBCur->OSTCBPrio )
      {
         pCrit->OSCritDepthCount++;
         UCOS_EXIT_CRITICAL();
         return ( OS_NO_ERR );
      }
   }

   UCOS_EXIT_CRITICAL();
   return ( OS_TIMEOUT );
}


BYTE OSCritEnterNoWait( OS_CRIT *pCrit )
{
   UCOS_ENTER_CRITICAL();
   if ( pCrit->OSCritTaskNum == OSTCBCur->OSTCBPrio )
   {
      pCrit->OSCritDepthCount++;
      UCOS_EXIT_CRITICAL();
      return ( OS_NO_ERR );
   }
   if ( pCrit->OSCritTaskNum == 0 )
   {
      pCrit->OSCritTaskNum = OSTCBCur->OSTCBPrio;
      pCrit->OSCritDepthCount++;
      UCOS_EXIT_CRITICAL();
      return ( OS_NO_ERR );
   }
   UCOS_EXIT_CRITICAL();
   return ( OS_TIMEOUT );
}






BYTE OSCritLeave( OS_CRIT *pCrit )
{
   UCOS_ENTER_CRITICAL();
   if ( pCrit->OSCritTaskNum == OSTCBCur->OSTCBPrio )
   {
      pCrit->OSCritDepthCount--;
      if ( pCrit->OSCritDepthCount == 0 )
      {
         pCrit->OSCritTaskNum = 0;

         if ( pCrit->OSCritGrp != 0 ) /*WE have something pending */
         {
            register BYTE x, y, bitx, bity, p;
            y = OSUnMapTbl[pCrit->OSCritGrp];
            x = OSUnMapTbl[pCrit->OSCritTbl[y]];
            bity = OSMapTbl[y];
            bitx = OSMapTbl[x];
            if ( ( pCrit->OSCritTbl[y] &= ~bitx ) == 0 )
            {
               pCrit->OSCritGrp &= ~bity;
            }
            p = ( y << 3 ) + x;
            OSTCBPrioTbl[p]->OSTCBDly = 0;
            OSTCBPrioTbl[p]->OSTCBStat &= ~OS_STAT_CRIT;
            OSRdyGrp |= bity;
            OSRdyTbl[y] |= bitx;
            pCrit->OSCritTaskNum = p;


            UCOS_EXIT_CRITICAL();
            OSSched();
            return ( OS_NO_ERR );
         }
      }

      UCOS_EXIT_CRITICAL();
      return ( OS_NO_ERR );
   }

   UCOS_EXIT_CRITICAL();
   return ( OS_CRIT_ERR );
}


BYTE OSTaskID( void )
{
   return OSTCBCur->OSTCBPrio;
}





void OSSetName(const char * name)
{

	OSTCBCur->pOSTCBName=name;
}


const char *  OSTaskName()
{

return	OSTCBCur->pOSTCBName;
}








