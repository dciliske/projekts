/**************************************************************************//**
 *
 * Copyright 1998-2015 NetBurner, Inc.  ALL RIGHTS RESERVED
 *   Permission is hereby granted to purchasers of NetBurner Hardware
 *   to use or modify this computer program for any use as long as the
 *   resultant program is only executed on NetBurner provided hardware.
 *
 *   No other rights to use this program or it's derivatives in part or
 *   in whole are granted.
 *
 *   It may be possible to license this or other NetBurner software for
 *   use on non-NetBurner Hardware.
 *   Please contact sales@Netburner.com for more information.
 *
 *   NetBurner makes no representation or warranties
 *   with respect to the performance of this computer program, and
 *   specifically disclaims any responsibility for any damages,
 *   special or consequential, connected with the use of this program.
 *
 *---------------------------------------------------------------------
 * NetBurner, Inc.
 * 5405 Morehouse Drive
 * San Diego, California 92121
 *
 * information available at:  http://www.netburner.com
 * E-Mail info@netburner.com
 *
 * Support is available: E-Mail support@netburner.com
 *
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

uint32_t CPU_CLOCK;

typedef struct {
    uint32_t prescale;
    uint32_t baudrate_scaler;
    bool    doubleBaudRate;
    uint32_t baud;
} baudSettingsStruct;

void calcBaudSettings( baudSettingsStruct *best, uint32_t Baudrate )
{
    uint32_t Baud;
    int32_t devMin = 0x80000000;
    int32_t devCur = 0x80000000;

    baudSettingsStruct current;
    // Prescaler bits for baud rate generator
    // bits value:  0   1   2   3
    // Prescale:    2   3   5   7
    current.prescale = 0;
    best->prescale = 0;
    // Baud Rate scaler bits
    // bits value:  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    // Scaler:      2   4   6   8   16  2^5 2^6 2^7 2^8 2^9 2^A 2^B 2^C 2^D 2^E 2^F
    current.baudrate_scaler = 0;
    best->baudrate_scaler = 0;
    // Double Baud rate bit
    // Doubles the effective baudrate of module
    current.doubleBaudRate = false;
    best->doubleBaudRate = false;
    for (int j = 0; j < 2 && devMin < 0; j++, current.doubleBaudRate = true)
    {
        for (current.prescale = 0; current.prescale < 4 && devMin < 0; current.prescale++)
        {
            // the prescale values are slighly strange. Get the right one...
            uint32_t prescaleValue = (current.prescale) ? 2 : (1 + 2 * current.prescale);

            // Baud = CPU_CLOCK / 2 * (1 + doubleBaudRate)
            //          / ( 2 * prescale_value * baudrate_scaler_value)
            //  See 40.4.3.1 of Freescale reference manual (v4) for the MCF54418

            // Baud Rate prescaler value is 2
            Baud = CPU_CLOCK * (1 + current.doubleBaudRate)
                / (8 * prescaleValue); // Baud Rate prescaler * cpu_clock PS (2) * sysclock PS (always 2)
            if (Baud <= CPU_CLOCK / 3)
            {
                devCur = Baud - Baudrate;
                if (devCur > devMin && devCur <= 0)
                {
                    devMin = devCur;
                    best->prescale = current.prescale;
                    best->baudrate_scaler = 0;
                    best->doubleBaudRate = current.doubleBaudRate;
                    best->baud = Baud;
                    if (devMin == 0) break;
                }
            }

            // Baud Rate prescaler value is 4
            Baud = CPU_CLOCK * (1 + current.doubleBaudRate)
                / (16 * prescaleValue); // Baud Rate prescaler * cpu_clock PS (2) * sysclock PS (always 2)
            if (Baud <= CPU_CLOCK / 3)
            {
                devCur = Baud - Baudrate;
                if (devCur > devMin && devCur <= 0)
                {
                    devMin = devCur;
                    best->prescale = current.prescale;
                    best->baudrate_scaler = 1;
                    best->doubleBaudRate = current.doubleBaudRate;
                    best->baud = Baud;
                    if (devMin == 0) break;
                }
            }

            // Baud Rate prescaler value is 6
            Baud = CPU_CLOCK * (1 + current.doubleBaudRate)
                / (24 * prescaleValue); // Baud Rate prescaler *  cpu_clock PS (2) * sysclock PS (always 2)
            if (Baud <= CPU_CLOCK / 3)
            {
                devCur = Baud - Baudrate;
                if (devCur > devMin && devCur <= 0)
                {
                    devMin = devCur;
                    best->prescale = current.prescale;
                    best->baudrate_scaler = 2;
                    best->doubleBaudRate = current.doubleBaudRate;
                    best->baud = Baud;
                    if (devMin == 0) break;
                }
            }

            // Now that we've dealt with the non patterned low value prescalers,
            // we can loop
            for (current.baudrate_scaler = 3;
                    current.baudrate_scaler <= 0xF && devMin < 0;
                    current.baudrate_scaler++)
            {
                // clock prescaler value is 2
                Baud = CPU_CLOCK * (1 + current.doubleBaudRate)
                        / ( 4 * prescaleValue * (0x00000001 << current.baudrate_scaler));
                if (Baud <= CPU_CLOCK / 3)
                {
                    devCur = Baud - Baudrate;
                    if (devCur > devMin && devCur <= 0)
                    {
                        devMin = devCur;
                        best->prescale = current.prescale;
                        best->baudrate_scaler = current.baudrate_scaler;
                        best->doubleBaudRate = current.doubleBaudRate;
                        best->baud = Baud;
                    }
                }
            }
        }
    }
//    return best;
}

const char * find_str_end(const char *str){
    while (*str) str++;
    return str;
}


int main(int argc, const char * argv[])
{
    int i;
    int maxLen = 0;
    if (argc <= 2) {
        printf("No input\n");
        return 0;
    }

    const char * endptr = find_str_end(argv[1]);
    char * afterNum;
    CPU_CLOCK = strtol(argv[1], &afterNum, 10);
    printf("%-*s => ", maxLen, argv[i]);
    if (endptr != afterNum) {
        printf("PARSE ERROR\n");
    }
    else if ((CPU_CLOCK == 0) && (errno)) {
        printf("RANGE ERROR\n");
        errno = 0;
    }

    for (i = 2; i < argc; ++i) {
        if (strlen(argv[i]) > maxLen) {
            maxLen = strlen(argv[i]);
        }
    }

    putchar('\n');
    for (i = 2; i < argc; ++i) {
        endptr = find_str_end(argv[i]);
        uint32_t val = strtol(argv[i], &afterNum, 10);
        printf("%-*s => ", maxLen, argv[i]);
        if (endptr != afterNum) {
            printf("PARSE ERROR\n");
        }
        else if ((val == 0) && (errno)) {
            printf("RANGE ERROR\n");
            errno = 0;
        }
        else {
            baudSettingsStruct settings;
            calcBaudSettings( &settings, val );
            printf("\n");
            printf("\tBaudrate: %*d\n", maxLen+1, settings.baud);
            printf("\t\tPrescaler:   %*d\n", maxLen+1, settings.prescale);
            printf("\t\tBaud Scaler: %*d\n", maxLen+1, settings.baudrate_scaler);
            printf("\t\tDouble Baud: %*s\n", maxLen+1, settings.doubleBaudRate ? "True" : "False");
//#ifdef INPUT_HEX
//            printf("%*lld\n", maxLen+1, val);
//#else
//            printf("%#*llx\n", maxLen+1, val);
//#endif

        }
    }
    putchar('\n');

    return 0;
}
