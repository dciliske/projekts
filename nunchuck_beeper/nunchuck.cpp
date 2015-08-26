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
#include "nunchuck.h"

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

void Nunchuck_Init()
{
    MultiChannel_I2CInit(0, 0x08, 0x3F);
    MultiChannel_I2CStart(0, 0x52, false);
    MultiChannel_I2CSend(0, 0x40);
    MultiChannel_I2CSend(0, 0x00);
    MultiChannel_I2CStop(0);
}

void Nunchuck_Read(uint8_t *readBuf)
{
    MultiChannel_I2CReadBuf(0, 0x52, readBuf, 6);

    for (volatile int i = 0; i < 500; i++);

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
