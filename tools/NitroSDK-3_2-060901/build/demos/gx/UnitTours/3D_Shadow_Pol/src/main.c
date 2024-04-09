/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/3D_Shadow_Pol
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.9  01/18/2006 02:11:26  kitase_hirotake
  do-indent

  Revision 1.8  11/21/2005 10:39:35  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.7  02/28/2005 05:26:04  yosizaki
  do-indent.

  Revision 1.6  11/02/2004 08:23:30  takano_makoto
  fix comment.

  Revision 1.5  05/27/2004 09:25:01  takano_makoto
  Added Autotest code

  Revision 1.4  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.3  04/06/2004 12:48:06  yada
  fix header comment

  Revision 1.2  03/15/2004 02:01:34  takano_makoto
  modify comments and codes a little.

  Revision 1.1  03/01/2004 09:09:37  takano_makoto
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


//---------------------------------------------------------------------------
// A sample that use shadow polygon.
// 
// This sample displays a sphere and ground.
// On the ground, the sphere's shadow is drawn.
//   
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
//        Use shadow polygons for shadow display    
//  Description:
//        Use cylinders to create the shadow of a sphere on the ground. At this time prepare 2 identical cylinders.    
//         Prepare a cylinder for masking.    
//        The polygons of this cylinder have shadow polygon, polygon attributes (PolygonAttr).    
//         Set to draw only the back side.    
//        Next, in the same coordinates as above, prepare a cylinder for drawing. The polygons of this cylinder have    
//         shadow polygon, polygon attributes (PolygonAttr). Set to draw only the back side.    
//           At this time give vertex color to the cylinder used for draw.
//        When this cylinder intersects the polygons that are the ground, a shadow will be displayed    
//         in the area of intersection of the cylinder and the ground surface.    
//           At this time the color of the displayed shadow will be the color of the shadow polygons    
//         that are used for drawing.    
//---------------------------------------------------------------------------
void NitroMain(void)
{
    u16     Rotate = 0;                // for rotating sphere (0-65535)

    //---------------------
    // Initialize(IRQ enable, VRAM initialize, use BG0 for 3D mode)
    //---------------------
    DEMOInitCommon();
    DEMOInitVRAM();
    DEMOInitDisplay3D();

    G3X_AlphaBlend(TRUE);              // Enable alpha blending

    DEMOStartDisplay();

    //---------------------
    //  Main loop
    //---------------------
    while (1)
    {
        G3X_Reset();
        Rotate += 256;

        /* camera configuration*/
        {
            VecFx32 Eye = { 0, 15 * FX32_ONE, -15 * FX32_ONE }; // Eye point position
            VecFx32 at = { 0, 0, 0 };  // Viewpoint
            VecFx32 vUp = { 0, FX32_ONE, 0 };   // Up

            G3_LookAt(&Eye, &vUp, &at, NULL);   // Eye point settings
        }

        /* light configuration (configures light color and direction)*/
        {
            G3_LightVector(GX_LIGHTID_0, 0, -FX32_ONE + 1, 0);
            G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
        }

        G3_MtxMode(GX_MTXMODE_TEXTURE); // Set matrix to Texture mode
        G3_Identity();                 // Initialize current matrix to a unit matrix
        // Set the matrix to Position, Vector simultaneous set mode    
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

        /* landscape render configuration*/
        G3_PushMtx();
        // Set material's diffuse reflection color and ambient reflection color.
        G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31),     // Diffuse reflection color
                                GX_RGB(16, 16, 16),     // Ambient reflection color
                                FALSE); // Do not use vertex color

        // Set material's specular reflection color and emitted light color    
        G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16),     // Specular reflection color
                                GX_RGB(0, 0, 0),        // Emitted light color    
                                FALSE); // Specular reflection not used
        // Specify texture parameters    
        G3_TexImageParam(GX_TEXFMT_NONE,        // Do not use texture
                         GX_TEXGEN_NONE,
                         GX_TEXSIZE_S8,
                         GX_TEXSIZE_T8,
                         GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0);
        // Polygon attribute settings
        G3_PolygonAttr(GX_LIGHTMASK_0, // Light 0 ON
                       GX_POLYGONMODE_MODULATE, // Modulation mode
                       GX_CULL_NONE,   // No cull back
                       2,              // Polygon ID 2
                       31,             // Alpha value
                       GX_POLYGON_ATTR_MISC_NONE);

        G3_Scale(10 * FX32_ONE, 0, 10 * FX32_ONE);

        G3_Begin(GX_BEGIN_QUADS);      // Start vertex list (draw with quadrilateral polygons)
        G3_Normal(0, 1 << 9, 0);
        G3_Vtx(FX16_ONE, 0, FX16_ONE);
        G3_Vtx(FX16_ONE, 0, -FX16_ONE);
        G3_Vtx(-FX16_ONE, 0, -FX16_ONE);
        G3_Vtx(-FX16_ONE, 0, FX16_ONE);
        G3_End();                      // End of vertex list

        G3_PopMtx(1);


        G3_PushMtx();
        /* sphere rotation calculation and position configuration*/
        {
            fx16    s = FX_SinIdx(Rotate);
            fx16    c = FX_CosIdx(Rotate);

            G3_RotY(s, c);
            G3_Translate(0, 5 * FX32_ONE, -5 * FX32_ONE);
            G3_RotX(s, c);
            G3_RotY(s, c);
            G3_RotZ(s, c);
        }

        /* sphere render configuration*/
        // Set material's diffuse reflection color and ambient reflection color.
        G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31),     // Diffuse reflection color
                                GX_RGB(16, 16, 16),     // Ambient reflection color
                                FALSE); // Do not use vertex color

        // Set material's specular reflection color and emitted light color    
        G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16),     // Specular reflection color
                                GX_RGB(0, 0, 0),        // Emitted light color    
                                FALSE); // Specular reflection not used
        // Specify texture parameters    
        G3_TexImageParam(GX_TEXFMT_NONE,        // Do not use texture
                         GX_TEXGEN_NONE,
                         GX_TEXSIZE_S8,
                         GX_TEXSIZE_T8,
                         GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0);
        // Polygon attribute settings
        G3_PolygonAttr(GX_LIGHTMASK_0, // Light 0 ON
                       GX_POLYGONMODE_MODULATE, // Modulation mode
                       GX_CULL_BACK,   // Do cull back
                       1,              // Polygon ID 1
                       31,             // Alpha value
                       GX_POLYGON_ATTR_MISC_NONE);

        G3_Begin(GX_BEGIN_TRIANGLES);  // Begin vertex list (use triangular polygons)
        {
            MI_SendGXCommand(3, sphere1, sphere_size);
        }
        G3_End();                      // End of vertex list

        G3_PopMtx(1);

        /* shadow polygon position configuration for mask*/
        G3_PushMtx();
        {
            fx16    s = FX_SinIdx(Rotate);
            fx16    c = FX_CosIdx(Rotate);

            G3_RotY(s, c);
            G3_Translate(0, 0, -5 * FX32_ONE);
        }

        /* shadow polygon render configuration for mask*/
        // Set material's diffuse reflection color and ambient reflection color.
        G3_MaterialColorDiffAmb(GX_RGB(0, 0, 0),        // Diffuse reflection color
                                GX_RGB(0, 0, 0),        // Ambient reflection color
                                FALSE); // Do not use vertex color
        // Set material's specular reflection color and emitted light color    
        G3_MaterialColorSpecEmi(GX_RGB(0, 0, 0),        // Specular reflection color
                                GX_RGB(0, 0, 0),        // Emitted light color    
                                FALSE); // Specular reflection not used
        // Specify texture parameters    
        G3_TexImageParam(GX_TEXFMT_NONE,        // Do not use texture
                         GX_TEXGEN_NONE,
                         GX_TEXSIZE_S8,
                         GX_TEXSIZE_T8,
                         GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0);
        // Polygon attribute settings
        G3_PolygonAttr(GX_LIGHTMASK_0, // Do not reflect light
                       GX_POLYGONMODE_SHADOW,   // Shadow polygon mode
                       GX_CULL_FRONT,  // Do cull front
                       0,              // Polygon ID 0
                       0x08,           // Alpha value
                       GX_POLYGON_ATTR_MISC_NONE);

        // Draw mask (cylinder)
        G3_Begin(GX_BEGIN_TRIANGLES);  // Begin vertex list (use triangular polygons)
        {
            MI_SendGXCommand(3, cylinder1, cylinder_size);
        }
        G3_End();                      // End of vertex list

        /* shadow polygon render configuration for rendering*/
        // Set material's diffuse reflection color and ambient reflection color.
        G3_MaterialColorDiffAmb(GX_RGB(0, 0, 0),        // Diffuse reflection color
                                GX_RGB(0, 0, 0),        // Ambient reflection color
                                TRUE); // Use vertex color
        // Set material's specular reflection color and emitted light color    
        G3_MaterialColorSpecEmi(GX_RGB(0, 0, 0),        // Specular reflection color
                                GX_RGB(0, 0, 0),        // Emitted light color    
                                FALSE); // Specular reflection not used
        // Specify texture parameters    
        G3_TexImageParam(GX_TEXFMT_NONE,        // Do not use texture
                         GX_TEXGEN_NONE,
                         GX_TEXSIZE_S8,
                         GX_TEXSIZE_T8,
                         GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0);
        // Polygon attribute settings
        G3_PolygonAttr(GX_LIGHTMASK_0, // Do not reflect light
                       GX_POLYGONMODE_SHADOW,   // Shadow polygon mode
                       GX_CULL_BACK,   // Do cull back
                       1,              // Polygon ID 1
                       0x08,           // Alpha value
                       GX_POLYGON_ATTR_MISC_NONE);      // 

        // Draw shadow (cylinder)
        G3_Begin(GX_BEGIN_TRIANGLES);  // Begin vertex list (use triangular polygons)
        {
            MI_SendGXCommand(3, cylinder1, cylinder_size);
        }
        G3_End();                      // End of vertex list

        G3_PopMtx(1);

        // swapping the polygon list RAM, the vertex RAM, etc.
        G3_SwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_W);

        /* Waiting for the V-blank*/
        OS_WaitVBlankIntr();

#ifdef SDK_AUTOTEST                    // code for auto-test
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(100, 0x38FFDE1A);
        EXT_TestTickCounter();
#endif //SDK_AUTOTEST
    }
}
