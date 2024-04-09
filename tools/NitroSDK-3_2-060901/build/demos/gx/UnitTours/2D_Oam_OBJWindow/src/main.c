/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/2D_Oam_OBJWindow
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

  Revision 1.7  11/21/2005 10:32:04  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.6  06/27/2005 11:10:12  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.5  02/28/2005 05:26:14  yosizaki
  do-indent.

  Revision 1.4  05/27/2004 09:22:11  takano_makoto
  Added Autotest code

  Revision 1.3  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.2  04/06/2004 12:48:06  yada
  fix header comment

  Revision 1.1  03/01/2004 09:03:11  takano_makoto
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// A sample that use OBJ window.
// 
// This sample displays a sea view through a OBJ window.
// 
// USAGE:
//   UP, DOWN, LEFT, RIGHT      : Move the OBJ window
//   
// HOWTO:
// 1. Set up OBJ window by GX_SetVisibleWnd(GX_WNDMASK_OW)
// 2. Set inside and outside of window by G2_SetWndOutsidePlane() and
//    G2_SetWndOBJInsidePlane.
// 3. Transfer the OBJ data by GX_LoadOBJ().
// 4. Set GX_OAM_MODE_OBJWND to OAM attribute to use the OBJ window
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
//        V-blank interrupt process
//  Description:
//        Enables a flag that confirms that a V-Blank interrupt has been performed.
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
//        Display OBJ window
//  Description:
//        Display a background BG through a circular OBJ window. Outside the OBJ window
//      is displayed black.  
//        It is possible to move the OBJ window by using the pad.
//  Operation:
//        UP, DOWN   : Move the OBJ window.
//---------------------------------------------------------------------------
void NitroMain()
{
    int     pos_x = 0, pos_y = 0;

    //---------------------
    // initialization
    //---------------------
    DEMOInitCommon();
    DEMOInitVRAM();

    /* Select OAM and BG to be displayed*/
    GX_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ);

    /* OAM configuration*/
    GX_SetBankForOBJ(GX_VRAM_OBJ_128_A);        // Use VRAM-A
    // Set character OBJ mapping mode
    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);      // 2D mapping

    /* BG configuration*/
    GX_SetBankForBG(GX_VRAM_BG_128_B); // Use VRAM-B
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS,    // 2D/3D mode
                       GX_BGMODE_0,    // 
                       GX_BG0_AS_2D);  // Display 2D
    G2_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256,        // Screen size 256x256
                     GX_BG_COLORMODE_256,       // 256 colors
                     GX_BG_SCRBASE_0x0000,      // Screen offset 0x0000
                     GX_BG_CHARBASE_0x04000,    // Character offset 0x04000
                     GX_BG_EXTPLTT_01);
    G2_SetBG0Priority(0);              // BG 0 display priority to top
    G2_BG0Mosaic(FALSE);               // Do no use mosaic effect

    /* Window configuration*/
    GX_SetVisibleWnd(GX_WNDMASK_OW);   // Enable OBJ window
    G2_SetWndOutsidePlane(GX_WND_PLANEMASK_NONE,        // Disable outside window
                          FALSE);
    G2_SetWndOBJInsidePlane(GX_WND_PLANEMASK_BG0,       // Set inside of the window to BG0
                            FALSE);

    /* load OBJcharacter data*/
    GX_LoadOBJ(d_64_256_obj_schDT,     // Load OBJ character data (window shape)
               0, sizeof(d_64_256_obj_schDT));
    /* load BG character data and palette data*/
    GX_LoadBG0Char(d_natsunoumi_schDT, // Load BG0 character data
                   0, sizeof(d_natsunoumi_schDT));
    GX_LoadBGPltt(d_natsunoumi_sclDT,  // Load BG palette data
                  0, sizeof(d_natsunoumi_sclDT));
    GX_LoadBG0Scr(d_natsunoumi_sscDT,  // Load BG0 screen data
                  0, sizeof(d_natsunoumi_sscDT));

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
            const EXTKeys keys[8] =
                { {PAD_BUTTON_A, 10}, {PAD_BUTTON_L, 10}, {PAD_KEY_DOWN | PAD_KEY_RIGHT, 20}, {0,
                                                                                               0}
            };
            EXT_AutoKeys(keys, &gKeyWork.press, &gKeyWork.trigger);
        }
#endif

        if (DEMO_IS_PRESS(PAD_KEY_DOWN))
        {
            if (++pos_y > 191)
            {
                pos_y = 191;
            }
        }
        else if (DEMO_IS_PRESS(PAD_KEY_UP))
        {
            if (--pos_y < 0)
            {
                pos_y = 0;
            }
        }
        if (DEMO_IS_PRESS(PAD_KEY_RIGHT))
        {
            if (++pos_x > 255)
            {
                pos_x = 255;
            }
        }
        else if (DEMO_IS_PRESS(PAD_KEY_LEFT))
        {
            if (--pos_x < 0)
            {
                pos_x = 0;
            }
        }

        /* Configure the OAM attributes of the OBJ window*/
        G2_SetOBJAttr(&s_OAMBuffer[0], // Pointer to the OAM to use
                      pos_x,           // X position
                      pos_y,           // Y position
                      0,               // Priority level
                      GX_OAM_MODE_OBJWND,       // Set to OBJ window mode
                      FALSE,           // Disable mosaic
                      GX_OAM_EFFECT_NONE,       // No special effect
                      GX_OAM_SHAPE_64x64,       // OBJ size
                      GX_OAM_COLOR_256, // 256 colors
                      0, 0, 0);

#ifdef SDK_AUTOTEST                    // code for auto-test
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(100, 0xAE1F20B9);
        EXT_TestTickCounter();
#endif //SDK_AUTOTEST

        /* Dump the cache to main memory, and invalidate it*/
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
