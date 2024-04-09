/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/3D_BoxTest
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

  Revision 1.8  11/21/2005 10:33:02  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.7  02/28/2005 05:26:04  yosizaki
  do-indent.

  Revision 1.6  07/29/2004 06:50:14  takano_makoto
  Add PolygonAttr setting before BoxTest.

  Revision 1.5  05/27/2004 09:23:45  takano_makoto
  Added Autotest code

  Revision 1.4  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.3  04/06/2004 12:48:06  yada
  fix header comment

  Revision 1.2  03/15/2004 01:58:41  takano_makoto
  modify comments and programs a little.

  Revision 1.1  03/01/2004 09:04:35  takano_makoto
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// A sample that use Box test.
// 
// This sample displays a cube and a rolling sphere.
// If the sphere contact with the cube, it is displayed normally.
// The other case it is displayed by wire frame.
// 
//---------------------------------------------------------------------------


#include <nitro.h>
#include "DEMO.h"
#include "data.h"
//---------------------------------------------------------------------------
//  Overview:
//  V-blank interrupt process
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
//        Do the box test
//  Description:
//        When the sphere of a rotating wire frame touches a surface of the enclosing cube, the box test    
//      result will be drawn as normal.    
//        
//        Using orthogonal projection, within a space, establish a space for culling with the box test.    
//        Create a cube that has the same size as the sphere with respect to the current clip coordinate matrix that was established by orthogonal projection.    
//         Use this to perform the box test.    
//        Next, convert to a perspective projection, and use the wire frame to draw the previously
//         established culling space.
//        Then, when drawing a sphere, based on the results of the previously performed box test,    
//         switch between normal draw and wire frame draw.    
//---------------------------------------------------------------------------
void NitroMain(void)
{
    u16     Rotate = 0;                // for rotating cubes(0-65535)

    //---------------------
    // initialization
    //---------------------
    DEMOInitCommon();
    DEMOInitVRAM();

    G3X_Init();                        // Initialize 3D graphics
    G3X_InitTable();                   // Initialize graphic table
    G3X_InitMtxStack();                // Initialize matrix stack

    /* 3D graphics configuration*/
    GX_SetVisiblePlane(GX_PLANEMASK_BG0);       // Use BG number 0

    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS,    // 2D/3D mode
                       GX_BGMODE_4,    // Set BGMODE to 4
                       GX_BG0_AS_3D);  // Use BG's 0 as 3D
    G2_SetBG0Priority(0);              // BG 0 display priority to top

    G3X_SetShading(GX_SHADING_HIGHLIGHT);       // Set shading mode
    G3X_AntiAlias(TRUE);               // Enable antialiasing

    G3_SwapBuffers(GX_SORTMODE_AUTO,   // Swap buffer settings
                   GX_BUFFERMODE_W);
    // Clear color settings
    G3X_SetClearColor(GX_RGB(0, 0, 0), // clear color
                      0,               // clear alpha
                      0x7fff,          // clear depth
                      63,              // clear polygon ID
                      FALSE);          // fog

    G3_ViewPort(0, 0, 255, 191);       // Viewport settings

    DEMOStartDisplay();

    //---------------------
    //  Main loop
    //---------------------
    while (1)
    {
        int     sphere_alpha;

        G3X_Reset();
        Rotate += 256;

        /* Orthogonal projection settings*/
        {
            fx32    pos1_x = -((5 * FX32_ONE) / 2);
            fx32    pos1_y = ((5 * FX32_ONE) / 2);
            fx32    pos2_x = ((5 * FX32_ONE) / 2);
            fx32    pos2_y = -((5 * FX32_ONE) / 2);
            fx32    near = (5 * FX32_ONE) / 2;
            fx32    far = ((5 * FX32_ONE) / 2) + (5 * FX32_ONE);

            G3_MtxMode(GX_MTXMODE_PROJECTION);  // Set the matrix to projection mode
            G3_Identity();             // Initialize current matrix to a unit matrix
            // Orthogonal projection settings
            G3_Ortho(pos1_y,           // up    pos Y
                     pos2_y,           // down  pos Y
                     pos1_x,           // left  pos X
                     pos2_x,           // right pos X
                     near,             // near
                     far,              // far
                     NULL);

            G3_StoreMtx(0);
        }

        /* box test*/
        {
            G3_MtxMode(GX_MTXMODE_TEXTURE);     // Set matrix to Texture mode
            G3_Identity();             // Initialize current matrix to a unit matrix
            // Set the matrix to Position, Vector simultaneous set mode    
            G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
            G3_PushMtx();
            {
                GXBoxTestParam box;
                fx16    s = FX_SinIdx(Rotate);
                fx16    c = FX_CosIdx(Rotate);
                s32     result;

                G3_Translate(0, 0, -11 * FX32_ONE);
                G3_RotY(s, c);
                G3_Translate(0, 0, 6 * FX32_ONE);
                G3_RotX(s, c);
                G3_RotY(s, c);
                G3_RotZ(s, c);

                // Set FAR clip and 1-dot polygon rendering to ON
                // before the BoxTest
                G3_PolygonAttr(GX_LIGHTMASK_0,
                               GX_POLYGONMODE_MODULATE,
                               GX_CULL_NONE,
                               0,
                               0,
                               GX_POLYGON_ATTR_MISC_FAR_CLIPPING | GX_POLYGON_ATTR_MISC_DISP_1DOT);
                G3_Begin(GX_BEGIN_TRIANGLES);
                G3_End();

                box.x = -7200;
                box.y = -7200;
                box.z = -7200;
                box.width = 7200 * 2;
                box.height = 7200 * 2;
                box.depth = 7200 * 2;
                G3_BoxTest(&box);      // Do the box test

                while (G3X_GetBoxTestResult(&result) != 0)
                {
                    // Loop to wait for box text results
                    ;
                }
                sphere_alpha = (result ? 31 : 0);
                OS_Printf("result %d\n", result);
            }
            G3_PopMtx(1);
        }

        /* Perspective projection settings*/
        G3_MtxMode(GX_MTXMODE_PROJECTION);      // Set the matrix to projection mode
        G3_Identity();                 // Initialize current matrix to a unit matrix
        {
            fx32    right = FX32_ONE;
            fx32    top = FX32_ONE * 3 / 4;
            fx32    near = FX32_ONE;
            fx32    far = FX32_ONE * 400;
            // Perspective projection settings
            G3_Perspective(FX32_SIN30, // Sine   FOVY
                           FX32_COS30, // Cosine FOVY
                           FX32_ONE * 4 / 3,    // Aspect
                           near,       // Near
                           far,        // Far
                           NULL);

            G3_StoreMtx(0);
        }

        /* camera configuration*/
        {
            VecFx32 Eye = { 0, 8 * FX32_ONE, FX32_ONE };        // Eye point position
            VecFx32 at = { 0, 0, -5 * FX32_ONE };       // Viewpoint
            VecFx32 vUp = { 0, FX32_ONE, 0 };   // Up

            G3_LookAt(&Eye, &vUp, &at, NULL);   // Eye point settings
        }

        /* Light settings*/
        {
            G3_LightVector(GX_LIGHTID_0, 0, -FX32_ONE + 1, 0);  // Light color
            G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));    // Light direction
        }

        G3_MtxMode(GX_MTXMODE_TEXTURE); // Set matrix to Texture mode
        G3_Identity();                 // Initialize current matrix to a unit matrix
        // Set the matrix to Position, Vector simultaneous set mode    
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

        /* generate and render box*/
        {
            G3_PushMtx();
            G3_Translate(0, 0, -5 * FX32_ONE);  // Set box position
            // Set material's diffuse reflection color and ambient reflection color.
            G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31), // Diffuse reflection color
                                    GX_RGB(16, 16, 16), // Ambient reflection color
                                    FALSE);     // Do not set vertex color
            // Set material's specular reflection color and emitted light color    
            G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16), // Specular reflection color
                                    GX_RGB(0, 0, 0),    // Emitted light color    
                                    FALSE);     // Do not use specular reflection table.
            // Specify texture parameters    
            G3_TexImageParam(GX_TEXFMT_NONE,    // No texture
                             GX_TEXGEN_NONE,    // No texture coordinate conversion    
                             GX_TEXSIZE_S8,
                             GX_TEXSIZE_T8,
                             GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0);
            // Polygon attribute settings
            G3_PolygonAttr(GX_LIGHTMASK_0,      // Light 0 ON
                           GX_POLYGONMODE_MODULATE,     // Modulation mode
                           GX_CULL_NONE,        // No culling
                           1,          // Polygon ID 1
                           0,          // Alpha value
                           GX_POLYGON_ATTR_MISC_NONE);
            G3_Begin(GX_BEGIN_QUADS);  // Begin vertex list
            // (declaration to draw with quadrilateral polygons)
            {
                G3_Normal(0, 1 << 9, 0);
                G3_Vtx(((5 * FX16_ONE) / 2), ((5 * FX16_ONE) / 2), ((5 * FX16_ONE) / 2));
                G3_Vtx(((5 * FX16_ONE) / 2), ((5 * FX16_ONE) / 2), -((5 * FX16_ONE) / 2));
                G3_Vtx(-((5 * FX16_ONE) / 2), ((5 * FX16_ONE) / 2), -((5 * FX16_ONE) / 2));
                G3_Vtx(-((5 * FX16_ONE) / 2), ((5 * FX16_ONE) / 2), ((5 * FX16_ONE) / 2));

                G3_Normal(0, -1 << 9, 0);
                G3_Vtx(((5 * FX16_ONE) / 2), -((5 * FX16_ONE) / 2), ((5 * FX16_ONE) / 2));
                G3_Vtx(((5 * FX16_ONE) / 2), -((5 * FX16_ONE) / 2), -((5 * FX16_ONE) / 2));
                G3_Vtx(-((5 * FX16_ONE) / 2), -((5 * FX16_ONE) / 2), -((5 * FX16_ONE) / 2));
                G3_Vtx(-((5 * FX16_ONE) / 2), -((5 * FX16_ONE) / 2), ((5 * FX16_ONE) / 2));

                G3_Normal(1 << 9, 0, 0);
                G3_Vtx(((5 * FX16_ONE) / 2), -((5 * FX16_ONE) / 2), -((5 * FX16_ONE) / 2));
                G3_Vtx(((5 * FX16_ONE) / 2), -((5 * FX16_ONE) / 2), ((5 * FX16_ONE) / 2));
                G3_Vtx(((5 * FX16_ONE) / 2), ((5 * FX16_ONE) / 2), ((5 * FX16_ONE) / 2));
                G3_Vtx(((5 * FX16_ONE) / 2), ((5 * FX16_ONE) / 2), -((5 * FX16_ONE) / 2));

                G3_Normal(-1 << 9, 0, 0);
                G3_Vtx(-((5 * FX16_ONE) / 2), -((5 * FX16_ONE) / 2), ((5 * FX16_ONE) / 2));
                G3_Vtx(-((5 * FX16_ONE) / 2), -((5 * FX16_ONE) / 2), -((5 * FX16_ONE) / 2));
                G3_Vtx(-((5 * FX16_ONE) / 2), ((5 * FX16_ONE) / 2), -((5 * FX16_ONE) / 2));
                G3_Vtx(-((5 * FX16_ONE) / 2), ((5 * FX16_ONE) / 2), ((5 * FX16_ONE) / 2));
            }
            G3_End();                  // End of vertex list
            G3_PopMtx(1);
        }

        /* generate and render sphere*/
        {
            G3_PushMtx();              // Matrix to stack
            {                          // Calculate move position
                fx16    s = FX_SinIdx(Rotate);
                fx16    c = FX_CosIdx(Rotate);

                G3_Translate(0, 0, -11 * FX32_ONE);
                G3_RotY(s, c);
                G3_Translate(0, 0, 6 * FX32_ONE);
                G3_RotX(s, c);
                G3_RotY(s, c);
                G3_RotZ(s, c);
            }
            // Set material's diffuse reflection color and ambient reflection color.
            G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31), // Diffuse reflection color
                                    GX_RGB(16, 16, 16), // Ambient reflection color
                                    FALSE);     // Do not set vertex color
            // Set material's specular reflection color and emitted light color    
            G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16), // Specular reflection color
                                    GX_RGB(0, 0, 0),    // Emitted light color    
                                    FALSE);     // Do not use specular reflection table    
            // Specify texture parameters    
            G3_TexImageParam(GX_TEXFMT_NONE,    // No texture
                             GX_TEXGEN_NONE,    // No texture coordinate conversion    
                             GX_TEXSIZE_S8,
                             GX_TEXSIZE_T8,
                             GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0);
            // Set polygon related attribute values
            G3_PolygonAttr(GX_LIGHTMASK_0,      // Light 0 ON
                           GX_POLYGONMODE_MODULATE,     // Modulation mode
                           GX_CULL_NONE,        // No culling
                           1,          // Polygon ID 1
                           sphere_alpha,        // Alpha value
                           GX_POLYGON_ATTR_MISC_NONE);

            OS_Printf("sphere_alpha=%d\n", sphere_alpha);
            // Draw sphere
            G3_Begin(GX_BEGIN_TRIANGLES);/* Begin vertex list
                                                   (Declaration to draw with triangular polygons)*/
            {
                MI_SendGXCommand(3, sphere1, sphere_size);
            }
            G3_End();                  // End of vertex list
            G3_PopMtx(1);
        }

        // swapping the polygon list RAM, the vertex RAM, etc.
        G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);

        /* V-Blank wait*/
        OS_WaitVBlankIntr();

#ifdef SDK_AUTOTEST
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(100, 0x1959EE87);
        EXT_TestTickCounter();
#endif //SDK_AUTOTEST

    }
}
