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

#include <predef.h>
#include <ctype.h>
#include <multichanneli2c.h>
#include <pins.h>
#include <sim.h>
#include <cfinter.h>
#include <intcdefs.h>
#include <stdio.h>
#include <ucos.h>

#define LIDAR_LITE_ADDR 0x62
#define RegisterMeasure     0x00          // Register to write to initiate ranging.
#define MeasureValue        0x04          // Value to initiate ranging.
#define RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.
static OS_SEM LLSem;

INTERRUPT(LL_PIT_ISR, 0x2300)
{
    sim2.pit[2].pcsr = 0x0004;
    OSSemPost(&LLSem);
}

void LLInit(int i2c_chan)
{
    switch (i2c_chan) {
        case 0:
            J2[39].function(PINJ2_39_I2C0_SDA);
            J2[42].function(PINJ2_42_I2C0_SCL);
            break;
        case 1:
            J2[41].function(PINJ2_41_I2C1_SDA);
            J2[44].function(PINJ2_44_I2C1_SCL);
            break;
        case 4:
            J2[3].function(PINJ2_3_I2C4_SDA);
            J2[4].function(PINJ2_4_I2C4_SCL);
            break;
        case 5:
            J2[21].function(PINJ2_21_I2C5_SDA);
            J2[22].function(PINJ2_22_I2C5_SCL);
            break;
    }

    MultiChannel_I2CInit(i2c_chan);
    SETUP_PIT2_ISR(LL_PIT_ISR, 3);
}

uint16_t LLRead(int i2c_chan)
{
    uint8_t startCode = 0x04;
    uint16_t dist;
    uint8_t rv;
    uint8_t buf[4];
    buf[0] = RegisterMeasure;
    buf[1] = MeasureValue;

    rv = MultiChannel_I2CSendBuf(i2c_chan, LIDAR_LITE_ADDR, buf, 2);
//    iprintf("rv: %u\n", rv);
//    MultiChannel_I2CStart(i2c_chan, LIDAR_LITE_ADDR, I2C_START_WRITE);
//    MultiChannel_I2CSend(i2c_chan, RegisterMeasure);
//    MultiChannel_I2CSend(i2c_chan, MeasureValue);
//    MultiChannel_I2CStop(i2c_chan);
    sim2.pit[2].pcsr = 0x071E; // prescale by 128
    sim2.pit[2].pmr = 0x5F5E; // count for 25msec
    sim2.pit[2].pcsr |= 0x0001; // start
    buf[0] = RegisterHighLowB;
    OSSemPend(&LLSem, 2);
    rv = MultiChannel_I2CSendBuf(i2c_chan, LIDAR_LITE_ADDR, buf, 1);
//    iprintf("rv: %u\n", rv);
//    MultiChannel_I2CStart(i2c_chan, LIDAR_LITE_ADDR, I2C_START_WRITE);
//    MultiChannel_I2CSend(i2c_chan, RegisterHighLowB);
//    MultiChannel_I2CStop(i2c_chan);
    rv = MultiChannel_I2CReadBuf(i2c_chan, LIDAR_LITE_ADDR, (uint8_t *)&dist, 2);
//    iprintf("rv: %u\n", rv);

    return dist;
}
