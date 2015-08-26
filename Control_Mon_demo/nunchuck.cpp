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

#include <stdint.h>
#include <multichanneli2c.h>
#include <stdio.h>
#include <ucos.h>
#include <sim.h>
#include <intcdefs.h>
#include <cfinter.h>
#include <pins.h>
#include "nunchuck.h"

#define LOGME iprintf("We made it to line %d of file %s.\n", __LINE__, __FILE__);

OS_SEM nunchuckReadSem;

void NunchuckStruct::Update()
{
    uint8_t readBuf[6];
    Nunchuck_Read(readBuf);

    stick.x = readBuf[0];
    stick.y = readBuf[1];
    accel.x = (readBuf[2] << 2) | ((readBuf[5] >> 2) & 0x3);
    accel.y = (readBuf[3] << 2) | ((readBuf[5] >> 4) & 0x3);
    accel.z = (readBuf[4] << 2) | ((readBuf[5] >> 6) & 0x3);
    button.z = !(readBuf[5] & 0x1);
    button.c = !(readBuf[5] & 0x2);
}

int NunchuckStruct::Serialize(char *buf, int maxLen)
{
    int written = 0;

    if (maxLen == 0) {
        return siprintf(buf, "{ \"stick\": { \"x\": %u, \"y\": %u }, \"accel\": { \"x\": %u, \"y\": %u, \"z\": %u }, \"button\": { \"z\": %s, \"c\": %s } }",
                stick.x, stick.y, accel.x, accel.y, accel.z,
                button.z ? "true" : "false",
                button.c ? "true" : "false"
                );

    }

    return sniprintf(buf, maxLen, "{ \"stick\": { \"x\": %u, \"y\": %u }, \"accel\": { \"x\": %u, \"y\": %u, \"z\": %u }, \"button\": { \"z\": %s, \"c\": %s } }",
            stick.x, stick.y, accel.x, accel.y, accel.z,
            button.z ? "true" : "false",
            button.c ? "true" : "false"
            );
}

INTERRUPT( NunchuckISR, 0x2400 )
{

}

void Nunchuck_Init()
{
//    OSSemInit( &nunchuckReadSem, 0 );
//    J2[39].function( PINJ2_39_I2C0_SDA);  // Set Pins to I2C
//    J2[42].function( PINJ2_42_I2C0_SCL);
//    sim2.i2c0.i2fdr = 0x3C;
//    sim2.i2c0.i2cr = 0x80;
//    sim2.i2c0.i2sr = 0x02;
//
//    //start init of Nunchuck
//    while (sim2.i2c0.i2sr & 0x20){}
//    sim2.i2c0.i2cr = 0x90;
//    sim2.i2c0.i2cr = 0xB0;
//    while ((sim2.i2c0.i2sr & 0x20)==0){}
//
//    sim2.i2c0.i2dr = 0x52<<1;
//    while ((sim2.i2c0.i2sr & 0x80)==0){}
//    sim2.i2c0.i2dr = 0x40;
//    while ((sim2.i2c0.i2sr & 0x80)==0){}
//    sim2.i2c0.i2dr = 0x00;
//    while ((sim2.i2c0.i2sr & 0x80)==0){}
//    sim2.i2c0.i2cr = 0x80;

    MultiChannel_I2CInit(0, 0x08, 0x3F);
    MultiChannel_I2CStart(0, 0x52, false);
    MultiChannel_I2CSend(0, 0x40);
    MultiChannel_I2CSend(0, 0x00);
    MultiChannel_I2CStop(0);
}

void Nunchuck_Read(uint8_t *readBuf)
{
//    LOGME
//        iprintf("isr: 0x%02X\n", sim2.i2c0.i2sr);
//    sim2.i2c0.i2cr = 0xB8;
//    while ((sim2.i2c0.i2sr & 0x20)==0){}
//    sim2.i2c0.i2dr = 0x52<<1 | 0x01;
//    while ((sim2.i2c0.i2sr & 0x80)==0){}
//    for (int i = 0; i < 6; ) {
//        readBuf[i] = sim2.i2c0.i2dr;
//        while ((sim2.i2c0.i2sr & 0x80)==0){}
//        i += 1 - (sim2.i2c0.i2sr & 0x01);
//    }
//    sim2.i2c0.i2cr = 0x80;
//    LOGME

    MultiChannel_I2CReadBuf(0, 0x52, readBuf, 6);

    for (volatile int i = 0; i < 500; i++);

//    while (sim2.i2c0.i2sr & 0x20){}
//    sim2.i2c0.i2cr = 0x90;
//    sim2.i2c0.i2cr = 0xB0;
//    while ((sim2.i2c0.i2sr & 0x20)==0){}
//
//    sim2.i2c0.i2dr = 0x52<<1;
//    while ((sim2.i2c0.i2sr & 0x80)==0){}
//    sim2.i2c0.i2dr = 0x00;
//    while ((sim2.i2c0.i2sr & 0x80)==0){}
//    sim2.i2c0.i2cr = 0x80;
//
//    LOGME
    MultiChannel_I2CStart(0, 0x52, false);
    MultiChannel_I2CSend(0, 0x00);
    MultiChannel_I2CStop(0);

    readBuf[0] = (readBuf[0] ^ 0x17) + 0x17;
    readBuf[1] = (readBuf[1] ^ 0x17) + 0x17;
    readBuf[2] = (readBuf[2] ^ 0x17) + 0x17;
    readBuf[3] = (readBuf[3] ^ 0x17) + 0x17;
    readBuf[4] = (readBuf[4] ^ 0x17) + 0x17;
    readBuf[5] = (readBuf[5] ^ 0x17) + 0x17;

}
