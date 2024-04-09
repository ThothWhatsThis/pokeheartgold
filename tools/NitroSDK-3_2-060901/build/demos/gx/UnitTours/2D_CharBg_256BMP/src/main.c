/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/2D_CharBg_256BMP
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.9  01/18/2006 02:11:21  kitase_hirotake
  do-indent

  Revision 1.8  11/21/2005 10:27:02  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.7  02/28/2005 05:26:14  yosizaki
  do-indent.

  Revision 1.6  2004/07/28 09:07:37  yasu
  Adjust Offset_V

  Revision 1.5  05/27/2004 09:20:25  takano_makoto
  Added Autotest code

  Revision 1.4  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.3  04/06/2004 12:48:06  yada
  fix header comment

  Revision 1.2  03/23/2004 07:07:49  takano_makoto
  Change GX_LoadBGXScr to GX_LoadBGXBmp.

  Revision 1.1  03/01/2004 08:58:36  takano_makoto
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// A sample that use 256 color BITMAP BG:
// 
// This sample draw picture on the display with 256 color palette BITMAP BG.
// 
// 
// USAGE:
//   UP, DOWN, LEFT, RIGHT      : Slide the BG
//   A, B                       : Scaling the BG
//   L, R                       : Rotate the BG
//   SELECT + [UP, DOWN, LEFT, RIGHT] : Change the center position of rotation
//   SELECT     : Switch ON/OFF that set BG area over mode,
//                                loop or transparent.
//   
// HOWTO:
// 1. Transfer the palette data by GX_LoadBGPltt().
// 2. Transfer the screen data by GX_LoadBGxScr().
// Set 256 color BITMAP mode by G2_SetBGxControl256Bmp().
//---------------------------------------------------------------------------


#include <nitro.h>
#include "DEMO.h"
#include "data.h"
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
//        Displays an affine extended 256 color bit map BG
//  Description:
//        Affine converts (rotate / scale) and displays the 256-color bitmap BG
//      that are used for drawing.    
//
//        Use BG surface 3 with BGMODE number 3.
//        Perform configurations with G2_SetBG3Control256Bmp(), and transfer the
//         character data to the specified location.
//        Transfers screen data to the specified location.
//  Operation:
//        PLUS_KEY         : BG surface offset manipulation.    
//        SELECT + PLUS_KEY: Manipulate center position for rotate, enlarge reduce.    
//        BUTTON_A, B : Enlarge, reduce
//        BUTTON_L, R : Rotate
//        SELECT     : Area over process switch
//        START      : Initialize settings
//---------------------------------------------------------------------------
void NitroMain(void)
{
    int     trg = 0;
    int     offset_H = 0;
    int     offset_V = 30;
    fx32    scale = FX32_ONE;
    s16     x_0 = 0, y_0 = 0;
    s16     x_1 = 0, y_1 = 0;
    u16     rotate = 0;

    //---------------------
    // initialization
    //---------------------
    DEMOInitCommon();
    DEMOInitVRAM();

    /* BG configuration*/
    GX_SetBankForBG(GX_VRAM_BG_256_AB); // Allocate VRAM-A, B banks to BG

    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS,    // 2D/3D mode
                       GX_BGMODE_3,    // BGMODE 3
                       GX_BG0_AS_2D);  // 2D display to BG0

    GX_SetBGScrOffset(GX_BGSCROFFSET_0x00000);  // Set screen offset value
    GX_SetBGCharOffset(GX_BGCHAROFFSET_0x10000);        // Set character offset

    GX_SetVisiblePlane(GX_PLANEMASK_BG3);       // Select BG3 for display
    G2_SetBG3Priority(0);              // Set BG3 priority to top.
    G2_BG3Mosaic(FALSE);               // Do not apply mosaic to BG3.

    /* Read palette data (transmit the BG palette data to VRAM)*/
    GX_LoadBGPltt((const void *)bitmapBG_256color_Palette,      // Pointer to transfer origin
                  0,                   // Offset
                  PALETTE_SIZE);       // size

    /* Transfer bitmap data to BG3*/
    GX_LoadBG3Bmp(bitmapBG_256color_Texel, 0, SCREEN_SIZE);

    DEMOStartDisplay();

    //---------------------
    //  Main loop
    //---------------------
    while (1)
    {
        MtxFx22 mtx;

        /* read and operate the pad information*/
        DEMOReadKey();
#ifdef SDK_AUTOTEST                    // code for auto-test
        {
            const EXTKeys keys[8] =
                { {PAD_BUTTON_A, 10}, {PAD_BUTTON_L, 10}, {PAD_KEY_UP | PAD_KEY_RIGHT, 20}, {0,
                                                                                             0}
            };
            EXT_AutoKeys(keys, &gKeyWork.press, &gKeyWork.trigger);
        }
#endif

        if (DEMO_IS_PRESS(PAD_BUTTON_SELECT))
        {
            if (DEMO_IS_PRESS(PAD_KEY_UP))
            {
                y_0 -= 2;
            }
            if (DEMO_IS_PRESS(PAD_KEY_DOWN))
            {
                y_0 += 2;
            }
            if (DEMO_IS_PRESS(PAD_KEY_RIGHT))
            {
                x_0 += 2;
            }
            if (DEMO_IS_PRESS(PAD_KEY_LEFT))
            {
                x_0 -= 2;
            }
        }
        else
        {
            if (DEMO_IS_PRESS(PAD_KEY_UP))
            {
                offset_V += 2;
            }
            if (DEMO_IS_PRESS(PAD_KEY_DOWN))
            {
                offset_V -= 2;
            }
            if (DEMO_IS_PRESS(PAD_KEY_RIGHT))
            {
                offset_H -= 2;
            }
            if (DEMO_IS_PRESS(PAD_KEY_LEFT))
            {
                offset_H += 2;
            }
        }
        if (DEMO_IS_PRESS(PAD_BUTTON_A))
        {
            scale += 2 << (FX32_SHIFT - 8);
        }
        if (DEMO_IS_PRESS(PAD_BUTTON_B))
        {
            scale -= 2 << (FX32_SHIFT - 8);
        }
        if (DEMO_IS_PRESS(PAD_BUTTON_L))
        {
            rotate -= 256;
        }
        if (DEMO_IS_PRESS(PAD_BUTTON_R))
        {
            rotate += 256;
        }
        if (DEMO_IS_TRIG(PAD_BUTTON_SELECT))
        {
            trg = (trg + 1) & 0x01;
            OS_Printf("area_over=%d\n", trg);
        }

        if (DEMO_IS_TRIG(PAD_BUTTON_START))
        {
            offset_H = 0;
            offset_V = 0;
            x_0 = 32;
            y_0 = 32;
            scale = 1 << FX32_SHIFT;
            rotate = 0;
        }

        /* Set BG3 to a screen size of 256x256 dots and a 256-color bitmapB*/
        {
            GXBGAreaOver area_over = (trg ? GX_BG_AREAOVER_REPEAT : GX_BG_AREAOVER_XLU);

            // Set BG3 to 256 color bitmap BG    
            //   Screen size              : Screen size 256 x 256 dots
            //   Area over process      : Determine with area_over
            //   Screen base block: 0x00000
            G2_SetBG3Control256Bmp(GX_BG_SCRSIZE_256BMP_256x256,
                                   area_over, GX_BG_BMPSCRBASE_0x00000);
        }

        /* Generate affine conversion matrix*/
        {
            fx16    sinVal = FX_SinIdx(rotate);
            fx16    cosVal = FX_CosIdx(rotate);
            fx32    rScale = FX_Inv(scale);

            mtx._00 = (fx32)((cosVal * rScale) >> FX32_SHIFT);
            mtx._01 = (fx32)((sinVal * rScale) >> FX32_SHIFT);
            mtx._10 = -(fx32)((sinVal * rScale) >> FX32_SHIFT);
            mtx._11 = (fx32)((cosVal * rScale) >> FX32_SHIFT);
        }

#ifdef SDK_AUTOTEST                    // code for auto-test
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(100, 0x7A05F2E8);
        EXT_TestTickCounter();
#endif //SDK_AUTOTEST

        /* V-Blank wait*/
        OS_WaitVBlankIntr();

        /* configure the affine conversion that is applied to the BG3 surface*/
        G2_SetBG3Affine(&mtx,          // Conversion matrix    
                        x_0,           // Rotate center point (X) coordinate    
                        y_0,           // Rotate center point (Y) coordinate    
                        offset_H,      // Coordinate before rotation (X)    
                        offset_V);     // Coordinate before rotation (Y)    
    }
}
