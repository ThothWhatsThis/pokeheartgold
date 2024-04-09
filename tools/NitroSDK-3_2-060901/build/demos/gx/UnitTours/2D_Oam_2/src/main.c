/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/2D_Oam_2
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.37  01/18/2006 02:11:21  kitase_hirotake
  do-indent

  Revision 1.36  11/21/2005 10:30:02  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.35  06/27/2005 11:10:12  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.34  02/28/2005 05:26:04  yosizaki
  do-indent.

  Revision 1.33  11/02/2004 09:35:31  takano_makoto
  fix typo.

  Revision 1.32  05/24/2004 10:30:32  takano_makoto
  Fixed autotest code

  Revision 1.31  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.30  04/06/2004 12:48:06  yada
  fix header comment

  Revision 1.29  2004/02/24 05:25:21  yasu
  used OS_EnableIrqMask instead of OS_SetIrqMask

  Revision 1.28  02/12/2004 09:33:51  nishida_kenji
  GXOamColor->GXOamColorMode.

  Revision 1.27  02/09/2004 01:44:47  kitani_toshikazu
  Add default setting codes for auto testing.

  Revision 1.26  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.25  02/05/2004 01:15:24  kitani_toshikazu
  added codes for autoTesing.

  Revision 1.24  01/19/2004 02:15:56  nishida_kenji
  fix comments

  Revision 1.23  01/18/2004 09:20:26  nishida_kenji
  adds DEMOStartDisplay

  Revision 1.22  01/18/2004 07:33:33  nishida_kenji
  change the place of SVC_WaitVBlankIntr

  Revision 1.21  01/14/2004 11:24:30  nishida_kenji
  add G2_OBJMosaic,
  remove GXOamMosaic

  Revision 1.20  01/09/2004 12:02:34  nishida_kenji
  use OS_SetIrqCheckFlag at VBlankIntr()

  Revision 1.19  01/07/2004 09:38:05  nishida_kenji
  revises comments

  Revision 1.18  12/25/2003 11:00:09  nishida_kenji
  converted by GX_APIChangeFrom031212-2.pl

  Revision 1.17  12/17/2003 12:21:36  nishida_kenji
  use DC_FlushRange

  Revision 1.16  12/17/2003 09:00:20  nishida_kenji
  Totally revised APIs.
  build/buildtools/GX_APIChangeFrom031212.pl would help you change your program to some extent.
  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// A sample that rotates an OBJ:
//
// USAGE:
//   LEFT, L: Turn left
//   RIGHT,R: Turn right
//
// HOWTO:
// 1. Set up the character/palette/attribute data same as 2D_Oam_1.
// 2. Set up a matrix by G2_SetOBJAffine().
//---------------------------------------------------------------------------

#include <nitro.h>
#include "DEMO.h"
#include "data.h"

static GXOamAttr sOamBak[128];         // buffer for OAM

void NitroMain(void)
{
    u16     rotate = 0;

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

    DEMOStartDisplay();
    //---------------------------------------------------------------------------
    // Main Loop
    //---------------------------------------------------------------------------
    while (1)
    {
        DEMOReadKey();

        if (DEMO_IS_PRESS(PAD_KEY_RIGHT))
            rotate += 256;
        if (DEMO_IS_PRESS(PAD_KEY_LEFT))
            rotate -= 256;
        if (DEMO_IS_PRESS(PAD_BUTTON_R))
            rotate += 256;
        if (DEMO_IS_PRESS(PAD_BUTTON_L))
            rotate -= 256;
        if (DEMO_IS_TRIG(PAD_BUTTON_SELECT))
            rotate = 0;

        G2_SetOBJAttr(&sOamBak[0],     // a pointer to the attributes
                      60,              // x
                      35,              // y
                      0,               // priority
                      GX_OAM_MODE_NORMAL,       // OBJ mode
                      FALSE,           // mosaic
                      GX_OAM_EFFECT_AFFINE,     // flip/affine/no display/affine(double)
                      GX_OAM_SHAPE_64x64,       // size and shape
                      GX_OAM_COLORMODE_256,     // OBJ character data are in 256-color format
                      0,               // character name
                      0,               // color param
                      0                // affine param
            );

        {
            fx16    sinVal, cosVal;
            MtxFx22 mtx;
            GXOamAffine *tmp = (GXOamAffine *)(&sOamBak[0]);

            sinVal = FX_SinIdx(rotate);
            cosVal = FX_CosIdx(rotate);
            mtx._00 = cosVal;
            mtx._01 = sinVal;
            mtx._10 = -sinVal;
            mtx._11 = cosVal;
            G2_SetOBJAffine(tmp, &mtx);
        }

        // Store the data in the main memory, and invalidate the cache.
        DC_FlushRange(sOamBak, sizeof(sOamBak));
        /* IO register is accessed using DMA operation, so cache Wait is not needed*/
        // DC_WaitWriteBufferEmpty();

#ifdef SDK_AUTOTEST
        rotate = 4098;                 // default params for testing.
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(100, 0x2F6D8647);
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
