/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/3D_Pol_TexSRT
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.23  01/18/2006 02:11:24  kitase_hirotake
  do-indent

  Revision 1.22  11/21/2005 10:38:07  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.21  02/28/2005 05:26:16  yosizaki
  do-indent.

  Revision 1.20  11/02/2004 09:38:36  takano_makoto
  fix typo.

  Revision 1.19  05/24/2004 10:35:01  takano_makoto
  Fixed autotest code

  Revision 1.18  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.17  04/06/2004 12:48:07  yada
  fix header comment

  Revision 1.16  2004/02/24 05:25:21  yasu
  used OS_EnableIrqMask instead of OS_SetIrqMask

  Revision 1.15  2004/02/10 06:08:47  nishida_kenji
  used MTX_TransApply44.

  Revision 1.14  2004/02/10 05:29:15  nishida_kenji
  fixed bugs at texture matrix making.

  Revision 1.13  02/09/2004 01:44:47  kitani_toshikazu
  Add default setting codes for auto testing.

  Revision 1.12  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.11  02/05/2004 01:15:24  kitani_toshikazu
  added codes for autoTesing.

  Revision 1.10  01/30/2004 11:34:10  kitani_toshikazu
  removed Tab codes.

  Revision 1.9  01/30/2004 11:15:14  kitani_toshikazu
  added DEMO_Set3DDefaultShininessTable() to control  specular reflection.

  Revision 1.8  01/28/2004 06:20:40  kitani_toshikazu
  adjusted light intensity

  Revision 1.7  01/27/2004 09:00:04  nishida_kenji
  moves G3_LightVector to follow G3_LookAt.

  Revision 1.6  01/22/2004 02:03:25  nishida_kenji
  swap G3_SwapBuffers and SVC_WaitVBlankIntr

  Revision 1.5  01/19/2004 02:15:56  nishida_kenji
  fix comments

  Revision 1.4  01/18/2004 09:20:26  nishida_kenji
  adds DEMOStartDisplay

  Revision 1.3  01/18/2004 08:19:33  nishida_kenji
  change the place of SVC_WaitVBlankIntr

  Revision 1.2  01/13/2004 00:00:28  nishida_kenji
  small fix (scale)

  Revision 1.1  01/10/2004 07:46:33  nishida_kenji
  added a sample for texture SRT animation

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// A sample for texture SRT animation:
//
// USAGE:
//   Up, Down, Left, Right: move the texture
//   A, B                       : change the scale of the texture
//   L, R                       : rotate the texture
//
// HOWTO:
// Set up the texture matrix.
// Specify GX_TEXGEN_TEXCOORD as a parameter of G3_TexImageParam.
//
// Notice that the translation must be multiplied by 16 in advance,
// or use G3_LoadTexMtxTexCoord() instead.
//---------------------------------------------------------------------------

#include <nitro.h>
#include "DEMO.h"
#include "tex_4plett.h"

void NitroMain(void)
{
    u32     myTexAddr = 0x1000;        // texture image address in texture image slots
    u32     myTexPlttAddr = 0x1000;    // texture palette address in texture palette slots

    fx32    texScale = FX32_ONE >> 1;
    fx32    texX = 0;
    fx32    texY = 0;
    u16     texRotate = 0;

    //---------------------------------------------------------------------------
    // Initialize:
    // They enable IRQ interrupts, initialize VRAM, and set BG #0 for 3D mode.
    //---------------------------------------------------------------------------
    DEMOInitCommon();
    DEMOInitVRAM();
    DEMOInitDisplay3D();


    //---------------------------------------------------------------------------
    // Download the texture images:
    //
    // Transfer the texture data on the main memory to the texture image slots.
    //---------------------------------------------------------------------------
    GX_BeginLoadTex();                 // map the texture image slots onto LCDC address space
    {
        GX_LoadTex((void *)&tex_4plett64x64[0], // a pointer to the texture data on the main memory (4 bytes aligned)
                   myTexAddr,          // an offset address in the texture image slots
                   1024                // the size of the texture(s) (in bytes)
            );
    }
    GX_EndLoadTex();                   // restore the texture image slots

    //---------------------------------------------------------------------------
    // Download the texture palettes:
    //
    // Transfer the texture palette data on the main memory to the texture palette slots.
    //---------------------------------------------------------------------------
    GX_BeginLoadTexPltt();             // map the texture palette slots onto LCDC address space
    {
        GX_LoadTexPltt((void *)&pal_4plett[0],  // a pointer to the texture data on the main memory (4 bytes aligned)
                       myTexPlttAddr,  // an offset address in the texture palette slots
                       8               // the size of the texture palette(s) (in bytes)
            );
    }
    GX_EndLoadTexPltt();               // restore the texture palette slots

    DEMOStartDisplay();
    while (1)
    {
        MtxFx43 camera;

        DEMOReadKey();
        if (DEMO_IS_PRESS(PAD_KEY_UP))
            texY += FX32_ONE >> 2;
        if (DEMO_IS_PRESS(PAD_KEY_DOWN))
            texY -= FX32_ONE >> 2;
        if (DEMO_IS_PRESS(PAD_KEY_RIGHT))
            texX -= FX32_ONE >> 2;
        if (DEMO_IS_PRESS(PAD_KEY_LEFT))
            texX += FX32_ONE >> 2;
        if (DEMO_IS_PRESS(PAD_BUTTON_L))
            texRotate += 256;
        if (DEMO_IS_PRESS(PAD_BUTTON_R))
            texRotate -= 256;
        if (DEMO_IS_PRESS(PAD_BUTTON_A))
            texScale += FX32_ONE >> 4;
        if (DEMO_IS_PRESS(PAD_BUTTON_B))
            texScale -= FX32_ONE >> 4;
        if (texScale <= 0)
            texScale = FX32_ONE >> 4;

        G3X_Reset();

        //---------------------------------------------------------------------------
        // Set up a camera matrix
        //---------------------------------------------------------------------------
        {
            VecFx32 Eye = { 0, 0, 3 * FX32_ONE };       // Eye position
            VecFx32 at = { 0, 0, 0 };  // Viewpoint
            VecFx32 vUp = { 0, FX32_ONE, 0 };   // Up
            G3_LookAt(&Eye, &vUp, &at, &camera);
        }

        //---------------------------------------------------------------------------n
        // Set up light colors and direction.
        // Notice that light vector is transformed by the current vector matrix
        // immediately after LightVector command is issued.
        //
        // GX_LIGHTID_0: white, downward
        //---------------------------------------------------------------------------
        G3_LightVector(GX_LIGHTID_0, FX16_SQRT1_3, -FX16_SQRT1_3, -FX16_SQRT1_3);
        G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));

        G3_PushMtx();

        //---------------------------------------------------------------------------
        // Set up the texture matrix for SRT animation
        //---------------------------------------------------------------------------
        {
            fx16    s = FX_SinIdx(texRotate);
            fx16    c = FX_CosIdx(texRotate);
            fx32    rScale = FX_Inv(texScale);

            //---------------------------------------------------------------------------
            // Initialize a texture matrix
            //---------------------------------------------------------------------------
            G3_MtxMode(GX_MTXMODE_TEXTURE);
            G3_Identity();
#if 1
            //---------------------------------------------------------------------------
            // Scale, Rotation, and Translation on the texture matrix:
            //
            // Notice that 3rd row and 4th row must be multiplied by 16.
            //---------------------------------------------------------------------------
            G3_Translate(32 * FX32_ONE, 32 * FX32_ONE, 0);      // move the origin to the center of the texture
            G3_Translate(texX, texY, 0);        // translate
            G3_RotZ(s, c);             // rotate counterclockwise
            G3_Scale(rScale, rScale, FX32_ONE); // scale
            G3_Translate(-32 * FX32_ONE, -32 * FX32_ONE, 0);    // move the center of the texture to the origin
            {
                MtxFx44 mtx;
                MTX_Identity44(&mtx);
                mtx._22 = 16 * FX32_ONE;
                mtx._33 = 16 * FX32_ONE;
                G3_MultMtx44(&mtx);
            }
#else
            //---------------------------------------------------------------------------
            // CPU version
            //---------------------------------------------------------------------------
            {

                MtxFx44 mtx, tmp;
                MTX_Identity44(&mtx);

                mtx._30 = 32 * FX32_ONE + texX;
                mtx._31 = 32 * FX32_ONE + texY;

                MTX_RotZ44(&tmp, s, c);
                MTX_Concat44(&tmp, &mtx, &mtx);
                MTX_ScaleApply44(&mtx, &mtx, rScale, rScale, FX32_ONE);

                MTX_TransApply44(&mtx, &mtx, -32 * FX32_ONE, -32 * FX32_ONE, 0);

                //---------------------------------------------------------------------------
                // Load a texture matrix(in the case of GX_TEXGEN_TEXCOORD):
                //
                // Internaly, 3rd and 4th row are multiplied by 16 before
                // the argument matrix is set to the current texture matrix.
                //---------------------------------------------------------------------------
                G3_LoadTexMtxTexCoord(&mtx);    // Translation is internally multiplied by 16.
            }
#endif
            G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

        }

        G3_TexImageParam(GX_TEXFMT_PLTT4,       // use 4 colors palette texture
                         GX_TEXGEN_TEXCOORD,    // use normal source
                         GX_TEXSIZE_S64,        // 64 pixels
                         GX_TEXSIZE_T64,        // 64 pixels
                         GX_TEXREPEAT_ST,       // repeat st
                         GX_TEXFLIP_NONE,       // no flip
                         GX_TEXPLTTCOLOR0_USE,  // use color 0 of the palette
                         myTexAddr     // the offset of the texture image
            );

        G3_TexPlttBase(myTexPlttAddr,  // the offset of the texture palette
                       GX_TEXFMT_PLTT4 // 4 colors palette texture
            );

        // Set the material color (diffuse, ambient, specular) as basic white
        DEMO_Set3DDefaultMaterial(TRUE, TRUE);
        DEMO_Set3DDefaultShininessTable();

        G3_PolygonAttr(GX_LIGHTMASK_0, // Light #0 is on
                       GX_POLYGONMODE_MODULATE, // modulation mode
                       GX_CULL_BACK,   // cull back
                       0,              // polygon ID(0 - 63)
                       31,             // alpha(0 - 31)
                       0               // OR of GXPolygonAttrMisc's value
            );

        G3_Begin(GX_BEGIN_QUADS);

        {
            G3_TexCoord(0, 0);
            G3_Vtx(-FX16_ONE, FX16_ONE, 0);
            G3_TexCoord(0, 64 * FX32_ONE);
            G3_Vtx(-FX16_ONE, -FX16_ONE, 0);
            G3_TexCoord(64 * FX32_ONE, 64 * FX32_ONE);
            G3_Vtx(FX16_ONE, -FX16_ONE, 0);
            G3_TexCoord(64 * FX32_ONE, 0);
            G3_Vtx(FX16_ONE, FX16_ONE, 0);
        }
        G3_End();

        G3_PopMtx(1);

        // swapping the polygon list RAM, the vertex RAM, etc.
        G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);

#ifdef SDK_AUTOTEST
        // default value for testing.
        texX = FX32_ONE >> 2 * 15;
        texY = FX32_ONE >> 2 * 25;
        texRotate = 256 * 8;
        texScale = FX32_ONE - (FX32_ONE >> 4) * 3;

        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(100, 0xB17E279F);
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
