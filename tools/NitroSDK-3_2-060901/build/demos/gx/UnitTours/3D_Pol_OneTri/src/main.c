/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/3D_Pol_OneTri
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.32  01/18/2006 02:11:22  kitase_hirotake
  do-indent

  Revision 1.31  11/21/2005 10:36:00  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.30  02/28/2005 05:26:15  yosizaki
  do-indent.

  Revision 1.29  11/02/2004 09:37:17  takano_makoto
  fix typo.

  Revision 1.28  05/24/2004 10:32:52  takano_makoto
  Fixed autotest code

  Revision 1.27  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.26  04/06/2004 12:48:07  yada
  fix header comment

  Revision 1.25  2004/02/24 05:25:21  yasu
  used OS_EnableIrqMask instead of OS_SetIrqMask

  Revision 1.24  02/09/2004 01:44:47  kitani_toshikazu
  Add default setting codes for auto testing.

  Revision 1.23  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.22  02/05/2004 01:15:24  kitani_toshikazu
  added codes for autoTesing.

  Revision 1.21  01/29/2004 01:16:16  nishida_kenji
  Remove GX_BlankScr and use GX_DispOn/GX_DispOff instead.

  Revision 1.20  01/22/2004 02:03:25  nishida_kenji
  swap G3_SwapBuffers and SVC_WaitVBlankIntr

  Revision 1.19  01/19/2004 02:15:56  nishida_kenji
  fix comments

  Revision 1.18  01/18/2004 09:20:26  nishida_kenji
  adds DEMOStartDisplay

  Revision 1.17  01/18/2004 08:19:33  nishida_kenji
  change the place of SVC_WaitVBlankIntr

  Revision 1.16  01/09/2004 12:02:34  nishida_kenji
  use OS_SetIrqCheckFlag at VBlankIntr()

  Revision 1.15  01/05/2004 12:05:03  nishida_kenji
  revises comments

  Revision 1.14  12/25/2003 11:00:10  nishida_kenji
  converted by GX_APIChangeFrom031212-2.pl

  Revision 1.13  12/17/2003 09:00:20  nishida_kenji
  Totally revised APIs.
  build/buildtools/GX_APIChangeFrom031212.pl would help you change your program to some extent.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include "DEMO.h"

//---------------------------------------------------------------------------
// A sample for drawing a triangle:
//
// This sample simply draws a triangle.
// It does not even set camera.
// Also, it does not use DEMOInit(), which sets up 3D hardware.
//---------------------------------------------------------------------------
static void drawTriangle()
{
    G3_PolygonAttr(GX_LIGHTMASK_NONE,  // no lights
                   GX_POLYGONMODE_MODULATE,     // modulation mode
                   GX_CULL_NONE,       // cull none
                   0,                  // polygon ID(0 - 63)
                   31,                 // alpha(0 - 31)
                   0                   // OR of GXPolygonAttrMisc's value
        );

    G3_Begin(GX_BEGIN_TRIANGLES);
    {
        G3_Color(GX_RGB(31, 16, 16));
        G3_Vtx(0, FX16_ONE, 0);
        G3_Color(GX_RGB(16, 31, 16));
        G3_Vtx(-FX16_ONE, -FX16_ONE, 0);
        G3_Color(GX_RGB(16, 16, 31));
        G3_Vtx(FX16_ONE, -FX16_ONE, 0);
    }
    G3_End();
}


void NitroMain()
{
    //---------------------------------------------------------------------------
    // Initialization:
    // They set up interrupt, VRAM banks only.
    //---------------------------------------------------------------------------
    DEMOInitCommon();
    DEMOInitVRAM();
    // This sample set the basic 3D settings, DEMOInit() not called.

    //---------------------------------------------------------------------------
    // Initialize 3D hardware
    //---------------------------------------------------------------------------
    G3X_Init();

    //---------------------------------------------------------------------------
    // Initialize the matrix stacks and the matrix stack pointers
    //---------------------------------------------------------------------------
    G3X_InitMtxStack();

    //---------------------------------------------------------------------------
    // After the geometry engine or the rendering one is enabled,
    // you must issue SwapBuffers geometry command once.
    //---------------------------------------------------------------------------
    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);

    //---------------------------------------------------------------------------
    // Set up graphics mode.
    // GX_DISPMODE_GRAPHICS must be specified if you display 2D or 3D graphics.
    // GX_BG0_AS_3D must be specified if you use BG0 as 3D graphics.
    //---------------------------------------------------------------------------
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS,    // Graphics mode
                       GX_BGMODE_0,    // BG mode is 0
                       GX_BG0_AS_3D);  // Set BG0 as 3D

    GX_DispOff();

    //---------------------------------------------------------------------------
    // Set BG0 visible
    //---------------------------------------------------------------------------
    GX_SetVisiblePlane(GX_PLANEMASK_BG0);

    //---------------------------------------------------------------------------
    // Set priority of BG0 plane
    //---------------------------------------------------------------------------
    G2_SetBG0Priority(0);

    //---------------------------------------------------------------------------
    // Shading mode is toon (toon shading is not used in this sample)
    //---------------------------------------------------------------------------
    G3X_SetShading(GX_SHADING_TOON);

    //---------------------------------------------------------------------------
    // No alpha tests
    //---------------------------------------------------------------------------
    G3X_AlphaTest(FALSE, 0);

    //---------------------------------------------------------------------------
    // Enable alpha blending
    //---------------------------------------------------------------------------
    G3X_AlphaBlend(TRUE);

    //---------------------------------------------------------------------------
    // Enable anti aliasing
    //---------------------------------------------------------------------------
    G3X_AntiAlias(TRUE);

    //---------------------------------------------------------------------------
    // No edge marking
    //---------------------------------------------------------------------------
    G3X_EdgeMarking(FALSE);

    //---------------------------------------------------------------------------
    // No fog
    //---------------------------------------------------------------------------
    G3X_SetFog(FALSE, (GXFogBlend)0, (GXFogSlope)0, 0);

    //---------------------------------------------------------------------------
    // Set up clear color, depth, and polygon ID.
    //---------------------------------------------------------------------------
    G3X_SetClearColor(0, 0, 0x7fff, 63, FALSE);

    //---------------------------------------------------------------------------
    // Viewport
    //---------------------------------------------------------------------------
    G3_ViewPort(0, 0, 255, 191);

    //---------------------------------------------------------------------------
    // Start display
    //---------------------------------------------------------------------------
    OS_WaitVBlankIntr();               // Waiting the end of V-Blank interrupt
    GX_DispOn();

    while (1)
    {
        //---------------------------------------------------------------------------
        // Reset the states of 3D graphics
        //---------------------------------------------------------------------------
        G3X_Reset();

        //---------------------------------------------------------------------------
        // In this sample, the camera matrix and the projection matrix are set as
        // an identity matrices for simplicity.
        //---------------------------------------------------------------------------
        G3_MtxMode(GX_MTXMODE_PROJECTION);
        G3_Identity();
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
        G3_Identity();

        drawTriangle();

        // swapping the polygon list RAM, the vertex RAM, etc.
        G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);

#ifdef SDK_AUTOTEST
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(100, 0xFA8A9506);
        EXT_TestTickCounter();
#endif //SDK_AUTOTEST

        OS_WaitVBlankIntr();           // Waiting the end of V-Blank interrupt
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
