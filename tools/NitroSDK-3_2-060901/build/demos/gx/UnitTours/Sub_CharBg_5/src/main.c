/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/Sub_CharBg_5
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.9  01/18/2006 02:11:28  kitase_hirotake
  do-indent

  Revision 1.8  11/21/2005 10:43:19  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.7  06/27/2005 11:10:12  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.6  02/28/2005 05:26:04  yosizaki
  do-indent.

  Revision 1.5  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.4  04/06/2004 12:48:06  yada
  fix header comment

  Revision 1.3  2004/02/24 05:25:21  yasu
  used OS_EnableIrqMask instead of OS_SetIrqMask

  Revision 1.2  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.1  02/02/2004 06:26:16  nishida_kenji
  Based on 2D_CharBg_5.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// A sample that flips all characters on the screen horizontally/vertically:
//
// USAGE:
//   A: Flips horizontally
//   B: Flips vertically
//
// HOWTO:
// 1. Set up the character/palette/screen data same as Sub_CharBG_1.
// 2. Set up horizontal/vertical flip bits of the screen data.
//---------------------------------------------------------------------------

#include <nitro.h>
#include "DEMO.h"
#include "data.h"

static u16 sScrnBuf[SCREEN_SIZE];      // Buffer for screen data(BG #0)

void NitroMain(void)
{
    //---------------------------------------------------------------------------
    // Initialize:
    // They enable IRQ interrupts, initialize VRAM, and set BG #0 for text mode.
    //---------------------------------------------------------------------------
    DEMOInitCommon();
    DEMOInitVRAM();
    DEMOInitDisplaySubBG0Only();

    //---------------------------------------------------------------------------
    // Transmitting the character data and the palette data
    //---------------------------------------------------------------------------
    GXS_LoadBG0Char(d_64_256_bg_schDT, 0, sizeof(d_64_256_bg_schDT));
    GXS_LoadBGPltt(d_64_256_bg_sclDT, 0, sizeof(d_64_256_bg_sclDT));

    DEMOStartDisplay();
    //---------------------------------------------------------------------------
    // Main Loop
    //---------------------------------------------------------------------------
    while (1)
    {
        int     flip_H = 0, flip_V = 0;
        int     i, j;

        DEMOReadKey();

        if (DEMO_IS_PRESS(PAD_BUTTON_A))
            flip_H = 1;
        if (DEMO_IS_PRESS(PAD_BUTTON_B))
            flip_V = 1;

        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 8; j++)
            {
                sScrnBuf[(i * 32) + j] = (u16)(flip_V << 11 | flip_H << 10 | ((i * 0x10) + j));
            }
        }
        // Store the data in the main memory, and invalidate the cache.
        DC_FlushRange(sScrnBuf, sizeof(sScrnBuf));
        /* IO register is accessed using DMA operation, so cache Wait is not needed*/
        // DC_WaitWriteBufferEmpty();

        OS_WaitVBlankIntr();           // Waiting the end of V-Blank interrupt

        GXS_LoadBG0Scr(sScrnBuf, 0, sizeof(sScrnBuf));
    }
}

//---------------------------------------------------------------------------
// V-Blank interrupt function:
//
// Interrupt handlers are registered on the interrupt table by OS_SetIRQFunction.
// OS_EnableIrqMask selects IRQ interrupts to enable, and
// OS_EnableIrq enables IRQ interrupts.
// Notice that you have to call 'OS_SetIrqCheckFlag' to check a V-Blank interrupt.
//---------------------------------------------------------------------------
void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK); // checking V-Blank interrupt
}
