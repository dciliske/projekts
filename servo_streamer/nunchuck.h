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

void Nunchuck_Read(uint8_t *readBuf);
void Nunchuck_Init();

struct AccelStruct {
    uint16_t x;
    uint16_t y;
    uint16_t z;
};

struct StickStruct {
    uint8_t x;
    uint8_t y;
};

struct ButtonStruct {
    bool z;
    bool c;
};

struct NunchuckStruct {
    AccelStruct accel;
    StickStruct stick;
    ButtonStruct button;

    inline void Init() { Nunchuck_Init(); }
    void Update();
    int Serialize(char * buf, int maxLen = 0);
};
