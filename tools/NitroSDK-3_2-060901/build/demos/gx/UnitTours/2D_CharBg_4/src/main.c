/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/2D_CharBg_4
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.27  01/18/2006 02:11:21  kitase_hirotake
  do-indent

  Revision 1.26  11/21/2005 10:27:47  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.25  06/27/2005 11:10:12  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.24  02/28/2005 05:26:03  yosizaki
  do-indent.

  Revision 1.23  11/02/2004 09:34:48  takano_makoto
  fix typo.

  Revision 1.22  05/24/2004 10:29:36  takano_makoto
  Fixed autotest code

  Revision 1.21  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.20  04/06/2004 12:48:06  yada
  fix header comment

  Revision 1.19  2004/02/24 05:25:21  yasu
  used OS_EnableIrqMask instead of OS_SetIrqMask

  Revision 1.18  02/09/2004 01:44:47  kitani_toshikazu
  Add default setting codes for auto testing.

  Revision 1.17  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.16  02/05/2004 01:15:24  kitani_toshikazu
  added codes for autoTesing.

  Revision 1.15  01/19/2004 02:15:56  nishida_kenji
  fix comments

  Revision 1.14  01/18/2004 09:20:26  nishida_kenji
  adds DEMOStartDisplay

  Revision 1.13  01/18/2004 07:12:43  nishida_kenji
  change the place of SVC_WaitVBlankIntr

  Revision 1.12  01/09/2004 12:02:34  nishida_kenji
  use OS_SetIrqCheckFlag at VBlankIntr()

  Revision 1.11  01/07/2004 09:38:05  nishida_kenji
  revises comments

  Revision 1.10  12/26/2003 02:43:27  nishida_kenji
  fix comments

  Revision 1.9  12/25/2003 11:00:09  nishida_kenji
  converted by GX_APIChangeFrom031212-2.pl

  Revision 1.8  12/17/2003 11:35:17  nishida_kenji
  use DC_FlushRange

  Revision 1.7  12/17/2003 09:00:20  nishida_kenji
  Totally revised APIs.
  build/buildtools/GX_APIChangeFrom031212.pl would help you change your program to some extent.
  $NoKeywords: $
 *---------------------------------------------------------------------------*/
//---------------------------------------------------------------------------
// A sample that scrolls BG #0(text mode):
//
// USAGE:
//   UP, DOWN, LEFT, RIGHT      : Scrolls a sphere
//   SELECT     :                a sphere to the initial position
//
// HOWTO:
// 1. Set up the character/palette/screen data same as 2D_CharBG_1.
// 2. Specify an offset by G2_SetBGxOffset().
//---------------------------------------------------------------------------

#include <nitro.h>
#include "DEMO.h"
#include "data.h"

static u16 sScrnBuf[SCREEN_SIZE];      // Buffer for screen data(BG #0)

void NitroMain(void)
{
    int     offset_H = 0, offset_V = 0;

    //---------------------------------------------------------------------------
    // Initialize:
    // They enable IRQ interrupts, initialize VRAM, and set BG #0 for text mode.
    //---------------------------------------------------------------------------
    DEMOInitCommon();
    DEMOInitVRAM();
    DEMOInitDisplayBG0Only();

    //---------------------------------------------------------------------------
    // Transmitting the character data and the palette data
    //---------------------------------------------------------------------------
    GX_LoadBG0Char(d_64_256_bg_schDT, 0, sizeof(d_64_256_bg_schDT));
    GX_LoadBGPltt(d_64_256_bg_sclDT, 0, sizeof(d_64_256_bg_sclDT));

    {
        int     i, j;
        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 8; j++)
            {
                sScrnBuf[(i * 32) + j] = (u16)((i * 0x10) + j);
            }
        }
    }

    // Store the data in the main memory, and invalidate the cache.
    DC_FlushRange(sScrnBuf, sizeof(sScrnBuf));
    /* IO register is accessed using DMA operation, so cache Wait is not needed*/
    // DC_WaitWriteBufferEmpty();

    // DMA Transfer to BG #0 screen
    GX_LoadBG0Scr(sScrnBuf, 0, sizeof(sScrnBuf));

    DEMOStartDisplay();
    //---------------------------------------------------------------------------
    // Main Loop
    //---------------------------------------------------------------------------
    while (1)
    {
        DEMOReadKey();

        if (DEMO_IS_PRESS(PAD_KEY_UP))
            offset_V += 2;
        if (DEMO_IS_PRESS(PAD_KEY_DOWN))
            offset_V -= 2;
        if (DEMO_IS_PRESS(PAD_KEY_RIGHT))
            offset_H -= 2;
        if (DEMO_IS_PRESS(PAD_KEY_LEFT))
            offset_H += 2;
        if (DEMO_IS_TRIG(PAD_BUTTON_SELECT))
        {
            offset_H = 0;
            offset_V = 0;
        }

#ifdef SDK_AUTOTEST
        offset_H = 10;
        offset_V = 15;                 // default params for testing.
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(100, 0xAF23A638);
        EXT_TestTickCounter();
#endif //SDK_AUTOTEST

        OS_WaitVBlankIntr();           // Waiting the end of V-Blank interrupt

        // Setting the offset for BG #0 plane
        G2_SetBG0Offset(offset_H, offset_V);
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
