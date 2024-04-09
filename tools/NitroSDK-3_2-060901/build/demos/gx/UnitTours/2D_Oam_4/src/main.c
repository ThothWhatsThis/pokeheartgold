/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/2D_Oam_4
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.32  01/18/2006 02:11:21  kitase_hirotake
  do-indent

  Revision 1.31  11/21/2005 10:30:47  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.30  06/27/2005 11:10:12  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.29  02/28/2005 05:26:04  yosizaki
  do-indent.

  Revision 1.28  11/02/2004 09:35:47  takano_makoto
  fix typo.

  Revision 1.27  05/24/2004 10:30:51  takano_makoto
  Fixed autotest code

  Revision 1.26  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.25  04/06/2004 12:48:06  yada
  fix header comment

  Revision 1.24  2004/02/24 05:25:21  yasu
  used OS_EnableIrqMask instead of OS_SetIrqMask

  Revision 1.23  02/12/2004 09:33:51  nishida_kenji
  GXOamColor->GXOamColorMode.

  Revision 1.22  02/09/2004 01:44:47  kitani_toshikazu
  Add default setting codes for auto testing.

  Revision 1.21  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.20  02/05/2004 01:15:24  kitani_toshikazu
  added codes for autoTesing.

  Revision 1.19  01/19/2004 02:15:56  nishida_kenji
  fix comments

  Revision 1.18  01/18/2004 09:20:26  nishida_kenji
  adds DEMOStartDisplay

  Revision 1.17  01/18/2004 07:33:33  nishida_kenji
  change the place of SVC_WaitVBlankIntr

  Revision 1.16  01/14/2004 11:24:30  nishida_kenji
  add G2_OBJMosaic,
  remove GXOamMosaic

  Revision 1.15  01/13/2004 08:27:24  nishida_kenji
  simplification

  Revision 1.14  01/09/2004 12:02:34  nishida_kenji
  use OS_SetIrqCheckFlag at VBlankIntr()

  Revision 1.13  01/07/2004 09:38:05  nishida_kenji
  revises comments

  Revision 1.12  12/25/2003 11:00:09  nishida_kenji
  converted by GX_APIChangeFrom031212-2.pl

  Revision 1.11  12/17/2003 12:21:36  nishida_kenji
  use DC_FlushRange

  Revision 1.10  12/17/2003 09:00:20  nishida_kenji
  Totally revised APIs.
  build/buildtools/GX_APIChangeFrom031212.pl would help you change your program to some extent.
  $NoKeywords: $
 *---------------------------------------------------------------------------*/
//---------------------------------------------------------------------------
// A sample that flips OBJs:
//
// USAGE:
//   A: Flips horizontally
//   B: Flips vertically
//
// HOWTO:
// 1. Set up the character/palette/attribute data same as 2D_Oam_1.
// 2. Flip horizontally/vertically by setting corresponding fields.
//---------------------------------------------------------------------------
#include <nitro.h>
#include "DEMO.h"
#include "data.h"

static GXOamAttr sOamBak[128];         // Buffer for OAM

void NitroMain(void)
{
    int     flip_H = 0, flip_V = 0;

    //---------------------------------------------------------------------------
    // Initialize:
    // They enable IRQ interrupts, initialize VRAM, and make OBJ visible.
    //---------------------------------------------------------------------------
    DEMOInitCommon();
    DEMOInitVRAM();
    DEMOInitDisplayOBJOnly();

    //---------------------------------------------------------------------------
    // Transmitting the character data and the palette data
    //---------------------------------------------------------------------------
    GX_LoadOBJ(d_64_256_obj_schDT, 0, sizeof(d_64_256_obj_schDT));
    GX_LoadOBJPltt(d_64_256_obj_sclDT, 0, sizeof(d_64_256_obj_sclDT));

    MI_DmaFill32(3, sOamBak, 192, sizeof(sOamBak));     // let out of the screen if not display

    G2_SetOBJAttr(&sOamBak[0],         // a pointer to the attributes
                  0,                   // x
                  0,                   // y
                  0,                   // priority
                  GX_OAM_MODE_NORMAL,  // OBJ mode
                  FALSE,               // mosaic
                  GX_OAM_EFFECT_NONE,  // flip/affine/no display/affine(double)
                  GX_OAM_SHAPE_64x64,  // size and shape
                  GX_OAM_COLORMODE_256, // OBJ character data are in 256-color format
                  0,                   // character name
                  0,                   // color param
                  0                    // affine param
        );

    DEMOStartDisplay();
    //---------------------------------------------------------------------------
    // Main Loop
    //---------------------------------------------------------------------------
    while (1)
    {
        DEMOReadKey();

        if (DEMO_IS_TRIG(PAD_BUTTON_A))
        {
            ++flip_H;
            flip_H %= 2;
        }
        if (DEMO_IS_TRIG(PAD_BUTTON_B))
        {
            ++flip_V;
            flip_V %= 2;
        }

        {
            GXOamEffect effect = (GXOamEffect)0;
            if (flip_H)
                effect |= GX_OAM_EFFECT_FLIP_H;
            if (flip_V)
                effect |= GX_OAM_EFFECT_FLIP_V;

#ifdef SDK_AUTOTEST
            effect = (GXOamEffect)(GX_OAM_EFFECT_FLIP_H | GX_OAM_EFFECT_FLIP_V);        // default params for testing.
#endif //SDK_AUTOTEST


            //---------------------------------------------------------------------------
            // G2_SetOBJEffect:
            // it gives effects such as flip/scale and rotation.
            //---------------------------------------------------------------------------
            G2_SetOBJEffect(&sOamBak[0], effect, 0      // select affine params if GX_OAM_EFFECT_AFFINE or GX_OAM_EFFECT_AFFINE_DOUBLE selected
                );
        }

        // Store the data in the main memory, and invalidate the cache.
        DC_FlushRange(sOamBak, sizeof(sOamBak));
        /* IO register is accessed using DMA operation, so cache Wait is not needed*/
        // DC_WaitWriteBufferEmpty();

#ifdef SDK_AUTOTEST
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(100, 0x9975CFB4);
        EXT_TestTickCounter();
#endif //SDK_AUTOTEST

        OS_WaitVBlankIntr();           // Waiting the end of V-Blank interrupt

        GX_LoadOAM(sOamBak, 0, sizeof(sOamBak));
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
