/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/2D_Oam_256_16
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.11  01/18/2006 02:11:21  kitase_hirotake
  do-indent

  Revision 1.10  11/21/2005 10:30:18  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.9  07/22/2005 12:53:44  kitase_hirotake
  Deleted the decrement part I forgot to delete previously.

  Revision 1.8  07/22/2005 12:38:33  kitase_hirotake
  Corrected a problem where the +control pad would not work at FINALROM

  Revision 1.7  06/27/2005 11:10:12  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.6  02/28/2005 05:26:14  yosizaki
  do-indent.

  Revision 1.5  06/02/2004 13:08:56  takano_makoto
  Change EXTPLTT enum name.

  Revision 1.4  05/27/2004 09:21:38  takano_makoto
  Added Autotest code

  Revision 1.3  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.2  04/06/2004 12:48:06  yada
  fix header comment

  Revision 1.1  03/01/2004 09:05:36  takano_makoto
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// A sample that use extended 256x16 color palette in OBJ:
// 
// This sample displays some objects on the display with extended 256x16 color
// palette.
// 
// USAGE:
//   UP, DOWN, LEFT, RIGHT      : Slide the OBJs
//   A, B                       : Scaling the OBJs
//   L, R                       : Rotate the OBJs
//   SELECT     : Switch palette Number of the OBJs
//   START      : Initialize status of the OBJs
//   
// HOWTO:
// 1. Allocate VRAM to extended palette by GX_SetBankForOBJExtPltt().
// 2. Transfer the palette data by GX_BeginLoadOBJExtPltt(), GX_LoadOBJExtPltt()
//    and GX_EndLoadOBJExtPltt().
// 3. Set GX_OAM_COLOR_256 to OAM attribute to use the extended palette.
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
//        Displays 256 color x 16 extended palette character of affine OBJ
//  Description:
//        Performs affine extension (rotate / scale) on the 256-color x 16 extended palette OBJ
//      and displays it.
//  Operation:
//        UP, DOWN: Manipulate display position
//        BUTTON_A, B : Enlarge, reduce
//        BUTTON_L, R : rotation
//        SELECT     : Switch palettes
//        START      : Initialize settings
//---------------------------------------------------------------------------
void NitroMain()
{
    int     base_pos_x = 0, base_pos_y = 0;
    int     base_pal = 0;
    int     base_pat = 0;
    fx32    scale = FX32_ONE;
    u16     rotate = 0;

    //---------------------
    // initialization
    //---------------------
    DEMOInitCommon();
    DEMOInitVRAM();
    DEMOInitDisplayOBJOnly();

    /* Allocate VRAM bank to OBJ extended palette.*/
    GX_SetBankForOBJExtPltt(GX_VRAM_OBJEXTPLTT_0_F);

    /* Load character data and palette data.*/
    GX_LoadOBJ(bitmapOBJ_256color_icg, // Transfer OBJ data to VRAM A
               0, sizeof(bitmapOBJ_256color_icg));
    {
        GX_BeginLoadOBJExtPltt();      // Prepare to transfer palette data
        GX_LoadOBJExtPltt(bitmapOBJ_256color_icl,       // Transfer palette data to VRAM
                          0, sizeof(bitmapOBJ_256color_icl));
        GX_EndLoadOBJExtPltt();        // Palette data transfer complete
    }

    DEMOStartDisplay();

    //---------------------
    //  Main loop
    //---------------------
    while (1)
    {
        int     i, j;
        int     pos_x = base_pos_x;
        int     pos_y = base_pos_y;
        int     palette = base_pal;

        /* Obtain pad information and configure operations*/
        DEMOReadKey();
#ifdef SDK_AUTOTEST                    // code for auto-test
        {
            const EXTKeys keys[8] =
                { {PAD_BUTTON_A, 10}, {PAD_BUTTON_SELECT, 1}, {PAD_BUTTON_L, 10},
            {PAD_KEY_UP | PAD_KEY_RIGHT, 20}
            };
            EXT_AutoKeys(keys, &gKeyWork.press, &gKeyWork.trigger);
        }
#endif

        if (DEMO_IS_PRESS(PAD_KEY_UP))
        {
            --base_pos_y;
            OS_Printf("pos=%d:%d rotate=%d scale=%d\n", base_pos_x, base_pos_y, rotate, scale);
        }
        else if (DEMO_IS_PRESS(PAD_KEY_DOWN))
        {
            ++base_pos_y;
            OS_Printf("pos=%d:%d rotate=%d scale=%d\n", base_pos_x, base_pos_y, rotate, scale);
        }
        if (DEMO_IS_PRESS(PAD_KEY_RIGHT))
        {
            ++base_pos_x;
            OS_Printf("pos=%d:%d rotate=%d scale=%d\n", base_pos_x, base_pos_y, rotate, scale);
        }
        else if (DEMO_IS_PRESS(PAD_KEY_LEFT))
        {
            --base_pos_x;
            OS_Printf("pos=%d:%d rotate=%d scale=%d\n", base_pos_x, base_pos_y, rotate, scale);
        }
        if (DEMO_IS_PRESS(PAD_BUTTON_L))
        {
            rotate -= 512;
            OS_Printf("pos=%d:%d rotate=%d scale=%d\n", base_pos_x, base_pos_y, rotate, scale);
        }
        else if (DEMO_IS_PRESS(PAD_BUTTON_R))
        {
            rotate += 512;
            OS_Printf("pos=%d:%d rotate=%d scale=%d\n", base_pos_x, base_pos_y, rotate, scale);
        }
        if (DEMO_IS_PRESS(PAD_BUTTON_A))
        {
            scale += 128;
            OS_Printf("pos=%d:%d rotate=%d scale=%d\n", base_pos_x, base_pos_y, rotate, scale);
        }
        else if (DEMO_IS_PRESS(PAD_BUTTON_B))
        {
            scale -= 128;
            if (scale < 0)
            {
                scale = 0;
            }
            OS_Printf("pos=%d:%d rotate=%d scale=%d\n", base_pos_x, base_pos_y, rotate, scale);
        }
        if (DEMO_IS_TRIG(PAD_BUTTON_SELECT))
        {
            if (++base_pal > 15)
            {
                base_pal = 0;
            }
            OS_Printf("base_pal=%d\n", base_pal);
        }

        if (DEMO_IS_TRIG(PAD_BUTTON_START))
        {
            base_pal = 0;
            base_pos_x = 0;
            base_pos_y = 0;
            rotate = 0;
            scale = FX32_ONE;
            OS_Printf("pos=%d:%d rotate=%d scale=%d\n", base_pos_x, base_pos_y, rotate, scale);
        }

        /* generation and configuration of the affine transformation matrix*/
        {
            MtxFx22 mtx;
            fx16    sinVal = FX_SinIdx(rotate);
            fx16    cosVal = FX_CosIdx(rotate);
            fx32    rScale = FX_Inv(scale);
            GXOamAffine *oamBuffp = (GXOamAffine *)&s_OAMBuffer[0];

            mtx._00 = (fx32)((cosVal * rScale) >> FX32_SHIFT);
            mtx._01 = (fx32)((sinVal * rScale) >> FX32_SHIFT);
            mtx._10 = -(fx32)((sinVal * rScale) >> FX32_SHIFT);
            mtx._11 = (fx32)((cosVal * rScale) >> FX32_SHIFT);
            // Set OBJ affine transformation
            G2_SetOBJAffine(oamBuffp,  // OAM buffer pointer
                            &mtx);     // 2x2 matrix for affine transformation
        }

        /* OAM configuration (configure 16 32-dot x 32-dot OBJs)*/
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++)
            {
                int     count = (i * 4) + j;
                int     pattern = ((count / 4) * 0x80) + ((count % 4) * 0x08);

                /* Configure the OAM attributes of the OBJ*/
                G2_SetOBJAttr(&s_OAMBuffer[count],      // Specify the location of OAM
                              pos_x,   // X position
                              pos_y,   // Y position
                              0,       // Priority level
                              GX_OAM_MODE_NORMAL,       // Normal OBJ
                              FALSE,   // Disable mosaic
                              GX_OAM_EFFECT_AFFINE,     // Affine effect
                              GX_OAM_SHAPE_32x32,       // OBJ size
                              GX_OAM_COLOR_256, // 256 colors
                              pattern, // Character name
                              palette, // Color palette
                              0);
                pos_x += 32;
                if (++palette > 15)
                {
                    palette = 0;
                }
            }
            pos_x = base_pos_x;
            pos_y += 32;
        }

        /* output the cache to main memory*/
        DC_FlushRange(s_OAMBuffer, sizeof(s_OAMBuffer));
        /* IO register is accessed using DMA operation, so cache Wait is not needed*/
        // DC_WaitWriteBufferEmpty();

#ifdef SDK_AUTOTEST                    // code for auto-test
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(100, 0x8B9FC4A0);
        EXT_TestTickCounter();
#endif //SDK_AUTOTEST

        /* V-blank wait*/
        OS_WaitVBlankIntr();

        /* Transfer to OAM*/
        GX_LoadOAM(s_OAMBuffer,        // Transfer OAM buffer to OAM
                   0, sizeof(s_OAMBuffer));
        MI_DmaFill32(3,                // Clear OAM buffer
                     s_OAMBuffer, 192, sizeof(s_OAMBuffer));
    }
}
