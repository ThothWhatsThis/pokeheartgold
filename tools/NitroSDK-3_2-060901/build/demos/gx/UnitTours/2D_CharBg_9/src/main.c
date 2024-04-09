/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/2D_CharBg_9
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

  Revision 1.26  11/21/2005 10:29:04  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.25  02/28/2005 05:26:14  yosizaki
  do-indent.

  Revision 1.24  11/02/2004 09:35:17  takano_makoto
  fix typo.

  Revision 1.23  05/24/2004 10:30:13  takano_makoto
  Fixed autotest code

  Revision 1.22  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.21  04/06/2004 12:48:06  yada
  fix header comment

  Revision 1.20  2004/02/24 05:25:21  yasu
  used OS_EnableIrqMask instead of OS_SetIrqMask

  Revision 1.19  02/09/2004 01:44:47  kitani_toshikazu
  Add default setting codes for auto testing.

  Revision 1.18  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.17  02/05/2004 01:15:24  kitani_toshikazu
  added codes for autoTesing.

  Revision 1.16  01/19/2004 02:15:56  nishida_kenji
  fix comments

  Revision 1.15  01/18/2004 09:20:26  nishida_kenji
  adds DEMOStartDisplay

  Revision 1.14  01/18/2004 07:12:43  nishida_kenji
  change the place of SVC_WaitVBlankIntr

  Revision 1.13  01/09/2004 12:02:34  nishida_kenji
  use OS_SetIrqCheckFlag at VBlankIntr()

  Revision 1.12  01/07/2004 09:38:05  nishida_kenji
  revises comments

  Revision 1.11  12/25/2003 11:00:09  nishida_kenji
  converted by GX_APIChangeFrom031212-2.pl

  Revision 1.10  12/17/2003 11:35:17  nishida_kenji
  use DC_FlushRange

  Revision 1.9  12/17/2003 09:00:20  nishida_kenji
  Totally revised APIs.
  build/buildtools/GX_APIChangeFrom031212.pl would help you change your program to some extent.
  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// A sample that controls brightness:
//
// USAGE:
//   UP, DOWN   : Controls brightness
//
// HOWTO:
// 1. Set up the character/palette/screen data same as 2D_CharBG_1.
// Specify the planes and brightness by G2_SetBlendBrightness().
// You can also control brightness by G2_ChangeBlendBrightness().
//---------------------------------------------------------------------------

#include <nitro.h>
#include "DEMO.h"
#include "data.h"

void NitroMain(void)
{
    int     evy = 0;

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
    GX_LoadBG0Char(d_yuuyake_schDT, 0, sizeof(d_yuuyake_schDT));
    GX_LoadBGPltt(d_yuuyake_sclDT, 0, sizeof(d_yuuyake_sclDT));

    //---------------------------------------------------------------------------
    // Transmitting the screen data
    //---------------------------------------------------------------------------
    GX_LoadBG0Scr(d_yuuyake_sscDT, 0, sizeof(d_yuuyake_sscDT));

    //---------------------------------------------------------------------------
    // Setting up brightness control
    //
    // Specify planes and brightness(-16...+16)
    //---------------------------------------------------------------------------
    G2_SetBlendBrightness(GX_BLEND_PLANEMASK_BG0, 0);

    DEMOStartDisplay();
    //---------------------------------------------------------------------------
    // Main Loop
    //---------------------------------------------------------------------------
    while (1)
    {
        DEMOReadKey();

        if (DEMO_IS_PRESS(PAD_KEY_UP))
            evy++;
        if (DEMO_IS_PRESS(PAD_KEY_DOWN))
            evy--;

        if (evy < -16)
            evy = -16;
        if (evy > 16)
            evy = 16;

#ifdef SDK_AUTOTEST
        evy = 8;                       // default params for testing.
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(100, 0x44F02A2A);
        EXT_TestTickCounter();
#endif //SDK_AUTOTEST

        OS_WaitVBlankIntr();           // Waiting the end of V-Blank interrupt

        //---------------------------------------------------------------------------
        // Change the coefficient for brightness
        //---------------------------------------------------------------------------
        G2_ChangeBlendBrightness(evy);
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
