/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/Sub_Oam_5
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.11  01/18/2006 02:11:28  kitase_hirotake
  do-indent

  Revision 1.10  11/21/2005 10:45:21  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.9  06/27/2005 11:10:12  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.8  02/28/2005 05:26:04  yosizaki
  do-indent.

  Revision 1.7  05/25/2004 07:00:16  takano_makoto
  added FlushRange

  Revision 1.6  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.5  04/06/2004 12:48:07  yada
  fix header comment

  Revision 1.4  2004/02/24 05:25:21  yasu
  used OS_EnableIrqMask instead of OS_SetIrqMask

  Revision 1.3  02/12/2004 09:33:51  nishida_kenji
  GXOamColor->GXOamColorMode.

  Revision 1.2  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.1  02/02/2004 06:33:25  nishida_kenji
  Based on 2D_Oam_5.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// A sample that mosaics an OBJ:
//
// USAGE:
//   UP, DOWN   :    Control the size of mosaic in vertical direction
//   LEFT, RIGHT: Control the size of mosaic in horizontal direction
//
// HOWTO:
// 1. Set up the character/palette/attribute data same as Sub_Oam_1.
// 2. Specify the size of mosaics by G2S_SetOBJMosaicSize().
//---------------------------------------------------------------------------

#include <nitro.h>
#include "DEMO.h"
#include "data.h"

static GXOamAttr sOamBak[128];         // Buffer for OAM

void NitroMain(void)
{
    int     mosaic_H = 0, mosaic_V = 0;

    //---------------------------------------------------------------------------
    // Initialize:
    // They enable IRQ interrupts, initialize VRAM, and make OBJ visible.
    //---------------------------------------------------------------------------
    DEMOInitCommon();
    DEMOInitVRAM();
    DEMOInitDisplaySubOBJOnly();

    //---------------------------------------------------------------------------
    // Transmitting the character data and the palette data
    //---------------------------------------------------------------------------
    GXS_LoadOBJ(d_64_256_obj_schDT, 0, sizeof(d_64_256_obj_schDT));
    GXS_LoadOBJPltt(d_64_256_obj_sclDT, 0, sizeof(d_64_256_obj_sclDT));

    MI_DmaFill32(3, sOamBak, 192, sizeof(sOamBak));     // let out of the screen if not display

    G2_SetOBJAttr(&sOamBak[0],         // a pointer to the attributes
                  0,                   // x
                  0,                   // y
                  0,                   // priority
                  GX_OAM_MODE_NORMAL,  // OBJ mode
                  TRUE,                // mosaic
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

        if (DEMO_IS_PRESS(PAD_KEY_DOWN))
            mosaic_V++;
        if (DEMO_IS_PRESS(PAD_KEY_UP))
            mosaic_V--;
        if (DEMO_IS_PRESS(PAD_KEY_RIGHT))
            mosaic_H++;
        if (DEMO_IS_PRESS(PAD_KEY_LEFT))
            mosaic_H--;
        if (mosaic_H > 15)
            mosaic_H = 15;
        if (mosaic_H < 0)
            mosaic_H = 0;
        if (mosaic_V > 15)
            mosaic_V = 15;
        if (mosaic_V < 0)
            mosaic_V = 0;
        if (DEMO_IS_TRIG(PAD_BUTTON_SELECT))
        {
            mosaic_H = 0;
            mosaic_V = 0;
        }

        OS_WaitVBlankIntr();           // Waiting the end of V-Blank interrupt

        //---------------------------------------------------------------------------
        // Specify the horizontal/vertical size of mosaics on OBJs
        //---------------------------------------------------------------------------
        G2S_SetOBJMosaicSize(mosaic_H, mosaic_V);

        // Store the data in the main memory, and invalidate the cache.
        DC_FlushRange(sOamBak, sizeof(sOamBak));
        /* IO register is accessed using DMA operation, so cache Wait is not needed*/
        // DC_WaitWriteBufferEmpty();

        GXS_LoadOAM(sOamBak, 0, sizeof(sOamBak));
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
