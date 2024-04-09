/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/2D_Oam_Translucent
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.8  01/18/2006 02:11:21  kitase_hirotake
  do-indent

  Revision 1.7  11/21/2005 10:32:19  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.6  06/27/2005 11:10:12  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.5  02/28/2005 05:26:04  yosizaki
  do-indent.

  Revision 1.4  05/27/2004 09:22:22  takano_makoto
  Added Autotest code

  Revision 1.3  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.2  04/06/2004 12:48:06  yada
  fix header comment

  Revision 1.1  03/01/2004 09:04:12  takano_makoto
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// A sample that use OBJ with alpha blending effect.
// 
// This sample displays two spheres on the display.
// One of the sphere is transparent.
// 
// USAGE:
//   UP, DOWN   : Change rate of transparent.
//   
// HOWTO:
// 1. Set GX_OAM_MODE_XLU to OAM attribute to use transparent OBJ.
// 2. Set transparent object by G2_SetBlendAlpha().
//---------------------------------------------------------------------------

#include <nitro.h>
#include "DEMO.h"
#include "data.h"
//---------------------------------------------------------------------------
//  Overview:
//        OAM buffer region
//---------------------------------------------------------------------------
static GXOamAttr s_OAMBuffer[128];

//---------------------------------------------------------------------------
//  Overview:
//  V-blank interrupt process
//  Description:
//  Enables a flag that confirms that a V-Blank interrupt has been performed.
//
//        Initialize common (DEMOInitCommon()) performs the following:
//        * Select IRQ interrupt (OS_SetIrqMask()).
//        * Register this function that performs IRQ interrupt (OS_SetIRQFunction()).
//        * Enable IRQ interrupt (OS_EnableIrq()).
//        This function is called during V-Blank.
//---------------------------------------------------------------------------
void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK); // Sets a VBlank interrupt confirmation flag
}

//---------------------------------------------------------------------------
//  Overview:
//        Display translucent OBJ
//  Description:
//        Displays translucent OBJ characters
//        OBJs displayed on the left are normal display. OBJs displayed on the right are translucent.
//      OBJ.
//  Operation:
//        UP, DOWN: Blending factor of the translucent OBJ can be changed.
//---------------------------------------------------------------------------
void NitroMain()
{
    int     eva = 12;

    //---------------------
    // initialization
    //---------------------
    DEMOInitCommon();
    DEMOInitVRAM();
    DEMOInitDisplayOBJOnly();

    /* load character data and palette data*/
    GX_LoadOBJ(d_64_256_obj_schDT, 0, sizeof(d_64_256_obj_schDT));
    GX_LoadOBJPltt(d_64_256_obj_sclDT, 0, sizeof(d_64_256_obj_sclDT));

    DEMOStartDisplay();

    //---------------------
    //  Main loop
    //---------------------
    while (1)
    {
        /* load pad information*/
        DEMOReadKey();
#ifdef SDK_AUTOTEST                    // code for auto-test
        {
            const EXTKeys keys[8] = { {PAD_KEY_DOWN, 5}, {0, 0} };
            EXT_AutoKeys(keys, &gKeyWork.press, &gKeyWork.trigger);
        }
#endif

        if (DEMO_IS_PRESS(PAD_KEY_UP))
        {
            if (++eva > 0x1f)
            {
                eva = 0x1f;
            }
            OS_Printf("eva=%d\n", eva);
        }
        else if (DEMO_IS_PRESS(PAD_KEY_DOWN))
        {
            if (--eva < 0x00)
            {
                eva = 0x00;
            }
            OS_Printf("eva=%d\n", eva);
        }

        /* Configure the normal OAM attributes of the OBJ*/
        G2_SetOBJAttr(&s_OAMBuffer[0], // Use the first OAM
                      0,               // X position
                      0,               // Y position
                      0,               // Priority level
                      GX_OAM_MODE_NORMAL,       // Normal OBJ
                      FALSE,           // Disable mosaic
                      GX_OAM_EFFECT_NONE,       // No special effect
                      GX_OAM_SHAPE_64x64,       // OBJ size
                      GX_OAM_COLOR_256, // 256 colors
                      0,               // Character name
                      0, 0);

        /* Configure the OAM attributes of the translucent OBJ*/
        G2_SetOBJAttr(&s_OAMBuffer[1], // Use 2nd OAM
                      64,              // X position
                      0,               // Y position
                      0,               // Priority level
                      GX_OAM_MODE_XLU, // Translucent OBJ
                      FALSE,           // Disable mosaic
                      GX_OAM_EFFECT_NONE,       // No special effect
                      GX_OAM_SHAPE_64x64,       // OBJ size
                      GX_OAM_COLOR_256, // 256 colors
                      0,               // Character name
                      0, 0);
        /* alpha blending configuration*/
        G2_SetBlendAlpha(GX_BLEND_PLANEMASK_NONE,       // First target plane not specified
                         GX_BLEND_PLANEMASK_BD, // Second target plane uses backdrop
                         eva,          // Set blending factor for first plane
                         0);           // 

#ifdef SDK_AUTOTEST
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(100, 0x80C7021F);
        EXT_TestTickCounter();
#endif //SDK_AUTOTEST

        /* output the cache to main memory*/
        DC_FlushRange(s_OAMBuffer, sizeof(s_OAMBuffer));
        /* IO register is accessed using DMA operation, so cache Wait is not needed*/
        // DC_WaitWriteBufferEmpty();

        /* V-blank wait*/
        OS_WaitVBlankIntr();

        /* Transfer to OAM*/
        GX_LoadOAM(s_OAMBuffer,        // Transfer OAM buffer to OAM
                   0, sizeof(s_OAMBuffer));
        MI_DmaFill32(3,                // Clear OAM buffer
                     s_OAMBuffer, 192, sizeof(s_OAMBuffer));
    }
}
