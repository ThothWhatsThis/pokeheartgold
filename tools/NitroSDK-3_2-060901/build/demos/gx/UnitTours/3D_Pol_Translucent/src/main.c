/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/3D_Pol_Translucent
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.28  01/18/2006 02:11:26  kitase_hirotake
  do-indent

  Revision 1.27  11/21/2005 10:38:59  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.26  02/28/2005 05:26:19  yosizaki
  do-indent.

  Revision 1.25  11/02/2004 09:38:50  takano_makoto
  fix typo.

  Revision 1.24  05/24/2004 10:35:24  takano_makoto
  Fixed autotest code

  Revision 1.23  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.22  04/06/2004 12:48:07  yada
  fix header comment

  Revision 1.21  2004/02/24 05:25:21  yasu
  used OS_EnableIrqMask instead of OS_SetIrqMask

  Revision 1.20  02/09/2004 01:44:47  kitani_toshikazu
  Add default setting codes for auto testing.

  Revision 1.19  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.18  02/05/2004 01:15:24  kitani_toshikazu
  added codes for autoTesing.

  Revision 1.17  01/22/2004 02:03:25  nishida_kenji
  swap G3_SwapBuffers and SVC_WaitVBlankIntr

  Revision 1.16  01/19/2004 02:15:56  nishida_kenji
  fix comments

  Revision 1.15  01/18/2004 09:20:26  nishida_kenji
  adds DEMOStartDisplay

  Revision 1.14  01/18/2004 08:19:33  nishida_kenji
  change the place of SVC_WaitVBlankIntr

  Revision 1.13  01/09/2004 12:02:34  nishida_kenji
  use OS_SetIrqCheckFlag at VBlankIntr()

  Revision 1.12  01/07/2004 09:38:05  nishida_kenji
  revises comments

  Revision 1.11  01/06/2004 12:54:09  nishida_kenji
  revise comments

  Revision 1.10  12/25/2003 11:00:10  nishida_kenji
  converted by GX_APIChangeFrom031212-2.pl

  Revision 1.9  12/17/2003 09:00:20  nishida_kenji
  Totally revised APIs.
  build/buildtools/GX_APIChangeFrom031212.pl would help you change your program to some extent.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// A sample that use translucent polygons:
//
// There are a simple translucent cube and an opaque square behind it.
//
// HOWTO:
// Enable alpha blending by G3X_AlphaBlend(TRUE).
// Alpha can be set by PolygonAttr().
//---------------------------------------------------------------------------

#include <nitro.h>
#include "DEMO.h"

void NitroMain(void)
{
    u16     Rotate = 0;                // for rotating cubes(0-65535)

    //---------------------------------------------------------------------------
    // Initialize:
    // They enable IRQ interrupts, initialize VRAM, and set BG #0 for 3D mode.
    //---------------------------------------------------------------------------
    DEMOInitCommon();
    DEMOInitVRAM();
    DEMOInitDisplay3D();

    // Enable alpha blending
    G3X_AlphaBlend(TRUE);

    DEMOStartDisplay();
    while (1)
    {
        G3X_Reset();
        Rotate += 256;

        //---------------------------------------------------------------------------
        // Set up camera matrix
        //---------------------------------------------------------------------------
        {
            VecFx32 Eye = { 0, 0, FX32_ONE };   // Eye position
            VecFx32 at = { 0, 0, 0 };  // Viewpoint
            VecFx32 vUp = { 0, FX32_ONE, 0 };   // Up

            G3_LookAt(&Eye, &vUp, &at, NULL);
        }

        G3_PushMtx();

        //---------------------------------------------------------------------------
        // Draw a translucent cube
        //---------------------------------------------------------------------------
        G3_PushMtx();

        {
            fx16    s = FX_SinIdx(Rotate);
            fx16    c = FX_CosIdx(Rotate);
            G3_Translate(0, 0, -5 * FX32_ONE);

            G3_RotX(s, c);
            G3_RotY(s, c);
            G3_RotZ(s, c);
        }

        G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31),     // diffuse
                                GX_RGB(16, 16, 16),     // ambient
                                TRUE   // use diffuse as vtx color if TRUE
            );

        G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16),     // specular
                                GX_RGB(0, 0, 0),        // emission
                                FALSE  // use shininess table if TRUE
            );

        G3_PolygonAttr(GX_LIGHTMASK_NONE,       // no lights
                       GX_POLYGONMODE_MODULATE, // modulation mode
                       GX_CULL_BACK,   // cull back
                       0,              // polygon ID(0 - 63)
                       24,             // alpha(0 - 31)
                       0               // OR of GXPolygonAttrMisc's value
            );

        G3_Begin(GX_BEGIN_QUADS);

        {
            G3_Vtx(FX16_ONE, -FX16_ONE, FX16_ONE);
            G3_Vtx(FX16_ONE, FX16_ONE, FX16_ONE);
            G3_Vtx(-FX16_ONE, FX16_ONE, FX16_ONE);
            G3_Vtx(-FX16_ONE, -FX16_ONE, FX16_ONE);

            G3_Vtx(-FX16_ONE, -FX16_ONE, -FX16_ONE);
            G3_Vtx(-FX16_ONE, FX16_ONE, -FX16_ONE);
            G3_Vtx(FX16_ONE, FX16_ONE, -FX16_ONE);
            G3_Vtx(FX16_ONE, -FX16_ONE, -FX16_ONE);

            G3_Vtx(-FX16_ONE, -FX16_ONE, FX16_ONE);
            G3_Vtx(-FX16_ONE, FX16_ONE, FX16_ONE);
            G3_Vtx(-FX16_ONE, FX16_ONE, -FX16_ONE);
            G3_Vtx(-FX16_ONE, -FX16_ONE, -FX16_ONE);

            G3_Vtx(FX16_ONE, -FX16_ONE, -FX16_ONE);
            G3_Vtx(FX16_ONE, FX16_ONE, -FX16_ONE);
            G3_Vtx(FX16_ONE, FX16_ONE, FX16_ONE);
            G3_Vtx(FX16_ONE, -FX16_ONE, FX16_ONE);

            G3_Vtx(-FX16_ONE, FX16_ONE, -FX16_ONE);
            G3_Vtx(-FX16_ONE, FX16_ONE, FX16_ONE);
            G3_Vtx(FX16_ONE, FX16_ONE, FX16_ONE);
            G3_Vtx(FX16_ONE, FX16_ONE, -FX16_ONE);

            G3_Vtx(-FX16_ONE, -FX16_ONE, FX16_ONE);
            G3_Vtx(-FX16_ONE, -FX16_ONE, -FX16_ONE);
            G3_Vtx(FX16_ONE, -FX16_ONE, -FX16_ONE);
            G3_Vtx(FX16_ONE, -FX16_ONE, FX16_ONE);
        }

        G3_End();

        G3_PopMtx(1);

        //---------------------------------------------------------------------------
        // Draw opaque squares
        //---------------------------------------------------------------------------
        G3_PushMtx();

        // Rotate and translate a square
        G3_Translate(0, 0, -10 << FX32_SHIFT);
        {
            fx16    s = FX_SinIdx(Rotate);
            fx16    c = FX_CosIdx(Rotate);

            G3_RotZ(s, c);
        }

        G3_MaterialColorDiffAmb(GX_RGB(31, 0, 0),       // diffuse
                                GX_RGB(16, 16, 16),     // ambient
                                TRUE   // use diffuse as vtx color if TRUE
            );

        G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16),     // specular
                                GX_RGB(0, 0, 0),        // emission
                                FALSE  // use shininess table if TRUE
            );

        G3_PolygonAttr(GX_LIGHTMASK_NONE,       // no lights
                       GX_POLYGONMODE_MODULATE, // modulation mode
                       GX_CULL_BACK,   // cull back
                       0,              // polygon ID(0 - 63)
                       31,             // alpha(0 - 31)
                       0               // OR of GXPolygonAttrMisc's value
            );

        G3_Begin(GX_BEGIN_QUADS);
        {
            G3_Vtx(FX16_ONE << 1, 0, FX16_ONE << 1);
            G3_Vtx(FX16_ONE << 1, FX16_ONE << 1, FX16_ONE << 1);
            G3_Vtx(0, FX16_ONE << 1, FX16_ONE << 1);
            G3_Vtx(0, 0, FX16_ONE << 1);
        }
        G3_End();

        G3_MaterialColorDiffAmb(GX_RGB(0, 31, 0),       // diffuse
                                GX_RGB(16, 16, 16),     // ambient
                                TRUE   // use diffuse as vtx color if TRUE
            );

        G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16),     // specular
                                GX_RGB(0, 0, 0),        // emission
                                FALSE  // use shininess table if TRUE
            );

        G3_PolygonAttr(GX_LIGHTMASK_NONE,       // no lights
                       GX_POLYGONMODE_MODULATE, // modulation mode
                       GX_CULL_BACK,   // cull back
                       0,              // polygon ID(0 - 63)
                       31,             // alpha(0 - 31)
                       0               // OR of GXPolygonAttrMisc's value
            );

        G3_Begin(GX_BEGIN_QUADS);

        {
            G3_Vtx(0, 0, FX16_ONE << 1);
            G3_Vtx(0, FX16_ONE << 1, FX16_ONE << 1);
            G3_Vtx(-FX16_ONE << 1, FX16_ONE << 1, FX16_ONE << 1);
            G3_Vtx(-FX16_ONE << 1, 0, FX16_ONE << 1);
        }

        G3_End();

        G3_MaterialColorDiffAmb(GX_RGB(0, 0, 31),       // diffuse
                                GX_RGB(16, 16, 16),     // ambient
                                TRUE   // use diffuse as vtx color if TRUE
            );

        G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16),     // specular
                                GX_RGB(0, 0, 0),        // emission
                                FALSE  // use shininess table if TRUE
            );

        G3_PolygonAttr(GX_LIGHTMASK_NONE,       // no lights
                       GX_POLYGONMODE_MODULATE, // modulation mode
                       GX_CULL_BACK,   // cull back
                       0,              // polygon ID(0 - 63)
                       31,             // alpha(0 - 31)
                       0               // OR of GXPolygonAttrMisc's value
            );

        G3_Begin(GX_BEGIN_QUADS);
        {
            G3_Vtx(0, -FX16_ONE << 1, FX16_ONE << 1);
            G3_Vtx(0, 0, FX16_ONE << 1);
            G3_Vtx(-FX16_ONE << 1, 0, FX16_ONE << 1);
            G3_Vtx(-FX16_ONE << 1, -FX16_ONE << 1, FX16_ONE << 1);
        }
        G3_End();

        G3_MaterialColorDiffAmb(GX_RGB(31, 31, 0),      // diffuse
                                GX_RGB(16, 16, 16),     // ambient
                                TRUE   // use diffuse as vtx color if TRUE
            );

        G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16),     // specular
                                GX_RGB(0, 0, 0),        // emission
                                FALSE  // use shininess table if TRUE
            );

        G3_PolygonAttr(GX_LIGHTMASK_NONE,       // no lights
                       GX_POLYGONMODE_MODULATE, // modulation mode
                       GX_CULL_BACK,   // cull back
                       0,              // polygon ID(0 - 63)
                       31,             // alpha(0 - 31)
                       0               // OR of GXPolygonAttrMisc's value
            );

        G3_Begin(GX_BEGIN_QUADS);

        {
            G3_Vtx(FX16_ONE << 1, -FX16_ONE << 1, FX16_ONE << 1);
            G3_Vtx(FX16_ONE << 1, 0, FX16_ONE << 1);
            G3_Vtx(0, 0, FX16_ONE << 1);
            G3_Vtx(0, -FX16_ONE << 1, FX16_ONE << 1);
        }

        G3_End();

        G3_PopMtx(1);

        G3_PopMtx(1);

        // swapping the polygon list RAM, the vertex RAM, etc.
        G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);

#ifdef SDK_AUTOTEST
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(100, 0x2F2518C7);
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
