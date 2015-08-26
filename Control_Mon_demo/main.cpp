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
#include <string.h>
#include <HiResTimer.h>
#include <sim.h>
#include "servo.h"
#include "nunchuck.h"
#include "stepper.h"
#include "encoder.h"
#include "lidar_lite.h"

#define STEPPER_PIN_COUNT 4
#define STEP_RATE 100
#define STEPPER_MAX_SPEED 100
#define STEPPER_SEQ_SIZE 4
#define REPORT_BUF_SIZE 8192
#define INCOMING_BUF_SIZE 8192

const char AppName[] = "Control_Mon_demo";
extern http_wshandler *TheWSHandler;
extern OS_CRIT TCP_critical_section;


extern "C" {
    void UserMain( void * pd );
}

int ws_fd = -1;
OS_SEM SockReadySem;
Servo serv_x(25, false);
Servo serv_y(27, true);
NunchuckStruct nun;
int stepSpeed, stepDir;
bool stepForever;
uint16_t lidarRange = 1;
OS_CRIT I2CLock;
bool haltRanging;
OS_SEM resumeRanging;

bool keyUp;

extern volatile StepperControl Steppers[];
StepperConfig stepper = {
                            { J2[20], J2[24], J2[19], J2[23] },
                            STEPPER_PIN_COUNT,
//                            { 0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09 },
//                            { 0x09, 0x05, 0x06, 0x0A },
                            { 0x0A, 0x06, 0x05, 0x09 },
                            STEPPER_SEQ_SIZE,
                        };
OS_SEM stepSem;

char ReportBuffer[REPORT_BUF_SIZE];
char IncomingBuffer[INCOMING_BUF_SIZE];


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
    if (httpstricmp(url, "CONTROLLER")) {
        if (ws_fd < 0) {
            int rv = WSUpgrade( req, sock );
            if (rv >= 0) {
                ws_fd = rv;
                NB::WebSocket::ws_setoption(ws_fd, WS_SO_TEXT);
                OSSemPost( &SockReadySem );
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

void SendConfigReport(int ws_fd)
{
    SMPoolPtr pq;

    int dataLen = nun.Serialize((char *)pq->pData, ETHER_BUFFER_SIZE);

    dataLen = siprintf(ReportBuffer,
        "{ \"stepper\": { \"speed\": %d, \"dir\": %d, \"forever\": %s, "
        "\"rem\": %d, \"taken\": %ld }, \"range\": %u, \"nun\":  %s }",
        stepSpeed, stepDir, (stepForever ? "true" : "false"),
        Steppers[1].stepsRem, Steppers[1].stepsTaken,
        lidarRange, (char *)pq->pData);

    writeall(ws_fd, ReportBuffer, dataLen);
}

void ReportTask( void * pd )
{
    while (1) {
        ShowTaskTimes();
        iprintf("\n");

        OSTimeDly(TICKS_PER_SECOND*5);
    }
}

static void ParseInput( char *buf )
{
    char foreverBuf[16];
    int32_t stepCount;
    sscanf( buf,
            "{\"stepper\":{\"speed\":%d,\"dir\":%d,\"forever\":%5c,"
            "\"rem\":%ld,\"taken\":%*ld},\"range\":%*d,\"nun\":%*s}",
            &stepSpeed, &stepDir, foreverBuf, &stepCount
        );
    if (stepSpeed > STEPPER_MAX_SPEED) {
        stepSpeed = STEPPER_MAX_SPEED;
    }
    if (strcmp(foreverBuf, "true,") == 0) {
        iprintf("true\n");
        stepForever = true;
        StartStepper( 1, stepDir, stepSpeed );
    }
    else if (strcmp(foreverBuf, "false") == 0) {
        iprintf("false\n");
        stepForever = false;
        MoveStepper( 1, stepCount, stepDir, stepSpeed );
    }
}

static int ConsumeSocket( char c, bool &inStr, bool &strEscape )
{
    switch (c) {
    case '\\':
        if (!inStr) {
            return 0; // no change to openCount
        }
        strEscape = !strEscape;
        break;
    case '"':
        if (!strEscape) { inStr = !inStr; }
        else            { strEscape = false; }
        break;
    case '{':
        if (!strEscape) { return 1; }
        else            { strEscape = false; }
        break;
    case '}':
        if (!strEscape) { return -1; }
        else            { strEscape = false; }
        break;
    default:
        if (strEscape)  { strEscape = false; }
        break;
    }

    return 0;
}

void InputTask( void * pd )
{
    fd_set read_fds, error_fds;
    int index = 0, openCount = 0;
    bool inString = false, strEscape = false;
    while (1) {
        if (ws_fd > 0) {
            FD_SET( ws_fd, &read_fds );
            FD_SET( ws_fd, &error_fds );
            if (select(1, &read_fds, NULL, &error_fds, 0)) {
                if (FD_ISSET(ws_fd, &error_fds)) {
                    close(ws_fd);
                    ws_fd = -1;
                }
                if (FD_ISSET(ws_fd, &read_fds)) {
                    while (dataavail(ws_fd) && (index < INCOMING_BUF_SIZE)) {
                        read(ws_fd, IncomingBuffer + index, 1);
                        openCount += ConsumeSocket( IncomingBuffer[index], inString, strEscape );
                        index++;
                        if (openCount == 0) {
                            break;
                        }
                    }
                }
                if ((index == 0) && (openCount == 0)) {
                    IncomingBuffer[index] = '\0';
                    OSTimeDly(4);
                    ParseInput(IncomingBuffer);
                    index = 0;
                }
            }
        }
        else {
            OSSemPend( &SockReadySem, 0 );
        }
    }
}

extern OS_SEM LLSem;
void RangingTask( void * pd )
{
    uint16_t lastRead = 0;
    uint16_t thisRead = 0;
    while (1) {

        OSCritEnter( &I2CLock, 0 );
        if (LLRead(1, thisRead) == 0) {
            OSCritLeave( &I2CLock );
            iprintf("range: %u\n", lidarRange);
        }
        else {
            OSCritLeave( &I2CLock );
        }
        if (((thisRead > lastRead) && ((thisRead - lastRead) < (2*lastRead)))
                || ((thisRead <= lastRead) && ((lastRead - thisRead) >= (lastRead/2)))) {
           lidarRange = thisRead;
        }
        lastRead = thisRead;
        sim2.pit[2].pcsr = 0x071E; // prescale by 128
        sim2.pit[2].pmr = 0x5F5E*8; // count for 25msec
        sim2.pit[2].pcsr |= 0x0001; // start
        OSSemPend(&LLSem, 16);
        if (haltRanging) {
            haltRanging = false;
            OSSemPend( &resumeRanging, 0 );
        }
//        iprintf("semcount: %d\n", LLSem.OSSemCnt);
    }
}

void StepperISR( int timerNum );

void EncodeStep(int dir)
{
    int neg = dir < 0 ? -1 : 1;
    if (neg == -1) {
        dir = -dir;
    }
    if (stepForever) {
        if (neg == stepDir) {
            stepSpeed += dir;
        }
        else {
            stepSpeed -= dir;
        }
        if (stepSpeed <= 0) {
            stepDir = -stepDir;
            stepSpeed = (-stepSpeed) + 1;
        }
        if (stepSpeed > STEPPER_MAX_SPEED) {
            stepSpeed = STEPPER_MAX_SPEED;
        }
        StartStepper(1, stepDir, stepSpeed);
    }
    else {
        if (keyUp) {
            int stepCount = (stepSpeed/10) ? stepSpeed/10 : 1;
            if (neg == stepDir) {
                Steppers[1].stepsRem += stepCount;
                Steppers[1].done = false;
                ConfigureStepperTimer( 1, stepSpeed );
            }
            else {
                uint32_t tmp = Steppers[1].stepsRem;

                if (tmp >= stepCount) {
                    Steppers[1].stepsRem = tmp - stepCount;
                    Steppers[1].done = false;
                    ConfigureStepperTimer( 1, stepSpeed );
                }
                else {
                    Steppers[1].stepsRem = 0;
                    stepDir = -stepDir;
                    ChangeDir( 1, stepDir );
                    Steppers[1].done = false;
                    ConfigureStepperTimer( 1, stepSpeed );
                }
            }
            // If we're not stepping, restart the pit
            if (((sim2.pit[1].pcsr & 0x0001) == 0) && (Steppers[1].stepsRem)) {
                Steppers[1].done = false;
                ConfigureStepperTimer( 1, stepSpeed );
                StepperISR(1);
            }
        }
        else {
            if (neg == stepDir) {
                stepSpeed += dir;
            }
            else {
                stepSpeed -= dir;
            }
            if (stepSpeed <= 0) {
                stepDir = -stepDir;
                stepSpeed = (-stepSpeed) + 1;
                ChangeDir( 1, stepDir );
            }
            if (stepSpeed > STEPPER_MAX_SPEED) {
                stepSpeed = STEPPER_MAX_SPEED;
            }
            ChangeStepRate( 1, stepSpeed );
        }
    }
}

void ButtonChange(bool buttonState)
{
    keyUp = buttonState;
}

void ButtonDoubleClick()
{
    stepForever = !stepForever;
    Steppers[1].runForever = stepForever;
    if (stepForever) {
        StartStepper(1, stepDir, stepSpeed);
    }
    haltRanging;
}

void UserMain( void * pd )
{

    initWithWeb();
    Nunchuck_Init(); // FIXME
    LLInit(1);
    TheWSHandler = MyDoWSUpgrade;

    stepper.Init();
    ConfigureStepper( &stepper, 1 );
    stepSpeed = 100;
    stepDir = 1;
    StartStepper( 1, stepDir, stepSpeed );
    stepForever = true;
    OSSemInit( &SockReadySem, 0 );
    OSCritInit( &I2CLock );

    iprintf("Application: %s\r\nNNDK Revision: %s\r\n",AppName,GetReleaseTag());
    OSTimeDly(2);
    OSSimpleTaskCreate(ReportTask, 5);
    OSSimpleTaskCreate(InputTask, MAIN_PRIO-1);
    OSSimpleTaskCreate(RangingTask, 2);

    EnableEncoder(EncodeStep, ButtonChange, ButtonDoubleClick );

    ClearTaskTimes();
    SMPoolPtr pp;

    while(1) {
        fd_set error_fds;
        FD_ZERO( &error_fds );
        OSCritEnter( &I2CLock, 0 );
        nun.Update();
        OSCritLeave( &I2CLock );

        serv_x.SetPos(nun.stick.x);
        serv_y.SetPos(nun.stick.y);

//        iprintf("%lu\na_x: %4.4d - a_y: %4.4d - a_z: %4.4d\ns_x: %3.3d - s_y: %3.3d - b_z: %d - b_c: %d\n\n",
//            TimeTick, nun.accel.x, nun.accel.y, nun.accel.z,
//            nun.stick.x, nun.stick.y,
//            nun.button.z, nun.button.c);
//        iprintf("range: %u\n", lidarRange);
            if (ws_fd > 0){
                SendConfigReport(ws_fd);
            }

    }
}
