/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/3D_PolAttr_DpthUpdate
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

  Revision 1.8  11/21/2005 10:34:19  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.7  02/28/2005 05:26:14  yosizaki
  do-indent.

  Revision 1.6  11/02/2004 08:23:17  takano_makoto
  fix comment.

  Revision 1.5  05/27/2004 09:24:41  takano_makoto
  Added Autotest code

  Revision 1.4  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.3  04/06/2004 12:48:07  yada
  fix header comment

  Revision 1.2  03/15/2004 02:00:32  takano_makoto
  modify comments and codes a little.

  Revision 1.1  03/01/2004 09:08:29  takano_makoto
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// A sample that set GX_POLYGON_ATTR_MISC_XLU_DEPTH_UPDATE in G3_PolygonAttr().
// The depth buffer is updated when the transparent polygon is drawn.
// 
// This sample displays a transparent cube and pole. If switch is ON,
// the cube is drawn with GX_POLYGON_ATTR_MISC_XLU_DEPTH_UPDATE parameter.
// 
// USAGE:
//   UP, DOWN   : Change alpha blend parameter.
//   START      : Switch ON/OFF of GX_POLYGON_ATTR_MISC_XLU_DEPTH_UPDATE.
//   
//---------------------------------------------------------------------------

#include <nitro.h>
#include "DEMO.h"

//---------------------------------------------------------------------------
//  Overview:
//        Cube model data
//---------------------------------------------------------------------------
/* vertex data*/
static const s16 s_Vertex[3 * 8] = {
    FX16_ONE, FX16_ONE, FX16_ONE,
    FX16_ONE, FX16_ONE, -FX16_ONE,
    FX16_ONE, -FX16_ONE, FX16_ONE,
    FX16_ONE, -FX16_ONE, -FX16_ONE,
    -FX16_ONE, FX16_ONE, FX16_ONE,
    -FX16_ONE, FX16_ONE, -FX16_ONE,
    -FX16_ONE, -FX16_ONE, FX16_ONE,
    -FX16_ONE, -FX16_ONE, -FX16_ONE
};

//---------------------------------------------------------------------------
//        Set vertex coordinates
//  Description:
//        Use specified vertex data to set vertex coordinates.
//  Input:
//        i_idx: Vertex data ID
//---------------------------------------------------------------------------
static void Vertex(int i_idx)
{
    G3_Vtx(s_Vertex[i_idx * 3], s_Vertex[i_idx * 3 + 1], s_Vertex[i_idx * 3 + 2]);
}

//---------------------------------------------------------------------------
//  Overview:
//        Generate a cube surface
//  Description:
//        Generate one cube surface
//  Input:
//        i_idx0-i_id3 : Structural vertex data ID
//---------------------------------------------------------------------------
inline void Quad(int i_idx0, int i_idx1, int i_idx2, int i_idx3)
{
    Vertex(i_idx0);
    Vertex(i_idx1);
    Vertex(i_idx2);
    Vertex(i_idx3);
}


//---------------------------------------------------------------------------
//  Overview:
//        Draw cube (no normals)    
//  Description:
// 
//---------------------------------------------------------------------------
static void DrawCube(void)
{
    G3_Begin(GX_BEGIN_QUADS);          // Beginning of vertex list (draw with quadrilateral polygons)
    {
        Quad(2, 0, 4, 6);
        Quad(7, 5, 1, 3);
        Quad(6, 4, 5, 7);
        Quad(3, 1, 0, 2);
        Quad(5, 4, 0, 1);
        Quad(6, 7, 3, 2);
    }
    G3_End();                          // End of vertex list
}


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
//        Sets TR_MISC_XLU_DEPTH_UPDATE in G3_PolygonAttr()
//       (Updates the depth buffer when drawing translucent polygons)
//  Description:
//        The semi-transparent cube with fog applied that is the target of the dept test
//      orbits the periphery of the normal display rectangle.
//        If depth value updating is OFF, the influence of the fog on the depth value
//      will not be shown on the semi-transparent cube. When the depth value update is ON, the influence of fog will be reflected.    
//  Operation:
//        START      : Switch depth value update ON/OFF
//       UP, DOWN: Increase and decrease the object's alpha value.
//---------------------------------------------------------------------------
void NitroMain(void)
{
    unsigned int count = 0;
    int     alpha = 0x0a;
    BOOL    trg = 1;
    u16     Rotate = 0;

    //---------------------
    // Initialize(IRQ enable, VRAM initialize, use BG0 for 3D mode)
    //---------------------
    DEMOInitCommon();
    DEMOInitVRAM();
    DEMOInitDisplay3D();

    /* fog configuration*/
    {
        u32     fog_table[8];
        int     i;

        // Clear color settings
        G3X_SetClearColor(GX_RGB(0, 0, 0),      // Clear color
                          0,           // Clear alpha value
                          0x7fff,      // Clear depth value
                          63,          // Clear polygon ID
                          TRUE);       // Use fog
        // Fog attribute settings
        G3X_SetFog(TRUE,               // Enable fog
                   GX_FOGBLEND_COLOR_ALPHA,     // Apply fog to color and alpha
                   GX_FOGSLOPE_0x2000, // Fog gradient settings
                   0x5800);            // Fog calculation depth value
        G3X_SetFogColor(GX_RGB(31, 31, 31), 0); // Fog color settings

        // Fog table settings (increasing the values thickens the fog)
        for (i = 0; i < 8; i++)
        {
            fog_table[i] = (u32)(((i * 16) << 0) |
                                 ((i * 16 + 4) << 8) |
                                 ((i * 16 + 8) << 16) | ((i * 16 + 12) << 24));
        }
        G3X_SetFogTable(&fog_table[0]);
    }

    G3X_AlphaBlend(TRUE);              // Enable alpha blending

    /* Swap rendering engine reference data groups    
       (Notice that in Z buffer mode and W buffer mode
       depth values are different)) */
    G3_SwapBuffers(GX_SORTMODE_AUTO,   // Auto sort mode
                   GX_BUFFERMODE_Z);   // Buffering mode using Z values

    DEMOStartDisplay();

    OS_Printf("mssg%d:PolygonAttr_DepthUpdate=%s\n", count++, (trg ? "ON" : "OFF"));

    //---------------------
    //  Main loop
    //---------------------
    while (1)
    {
        G3X_Reset();
        Rotate += 256;

        /* Obtain pad information and configure operations*/
        DEMOReadKey();
#ifdef SDK_AUTOTEST                    // code for auto-test
        {
            const EXTKeys keys[8] = { {PAD_KEY_UP, 10}, {0, 1}, {PAD_KEY_UP, 10}, {0, 1},
            {PAD_KEY_UP, 10}, {0, 1}, {PAD_KEY_UP, 10}, {0, 0}
            };
            EXT_AutoKeys(keys, &gKeyWork.press, &gKeyWork.trigger);
        }
#endif

        if (DEMO_IS_TRIG(PAD_KEY_UP))
        {
            if (++alpha > 31)
            {
                alpha = 31;
            }
            OS_Printf("mssg%d:alpha=%d\n", count++, alpha);
        }
        else if (DEMO_IS_TRIG(PAD_KEY_DOWN))
        {
            if (--alpha < 0)
            {
                alpha = 0;
            }
            OS_Printf("mssg%d:alpha=%d\n", count++, alpha);
        }
        if (DEMO_IS_TRIG(PAD_BUTTON_START))
        {
            trg = (trg + 1) & 0x01;
            OS_Printf("mssg%d:PolygonAttr_DepthUpdate=%s\n", count++, (trg ? "ON" : "OFF"));
        }

        /* camera configuration*/
        {
            VecFx32 Eye = { 0, 0, FX32_ONE * 8 };       // Eye point position
            VecFx32 at = { 0, 0, 0 };  // Viewpoint
            VecFx32 vUp = { 0, FX32_ONE, 0 };   // Up

            G3_LookAt(&Eye, &vUp, &at, NULL);   // Eye point settings
        }

        G3_MtxMode(GX_MTXMODE_TEXTURE); // Set matrix to Texture mode
        G3_Identity();                 // Initialize current matrix to a unit matrix
        // Set the matrix to Position, Vector simultaneous set mode    
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

        G3_PushMtx();                  // Push the matrix to the stack    

        /* calculate the cube rotation and set its position*/
        {
            fx16    s = FX_SinIdx(Rotate);
            fx16    c = FX_CosIdx(Rotate);

            G3_RotY(s, c);
            G3_Translate(0, 0, 5 * FX32_ONE);
            G3_RotX(s, c);
            G3_RotZ(s, c);
        }

        /* cube render configuration (perform fog blending)*/
        // Set material's diffuse reflection color, and ambient reflection color    
        G3_MaterialColorDiffAmb(GX_RGB(31, 0, 0),       // Diffuse reflection color
                                GX_RGB(16, 16, 16),     // Ambient reflection color
                                TRUE); // Set vertex color
        // Set material's specular reflection color, and emitted color    
        G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16),     // Specular reflection color
                                GX_RGB(0, 0, 0),        // Emitted light color    
                                FALSE); // Specular reflection not used

        // Cube polygon attribute (alpha and depth) settings
        {
            int     attr;

            if (trg)
            {
                attr = GX_POLYGON_ATTR_MISC_XLU_DEPTH_UPDATE;
            }
            else
            {
                attr = GX_POLYGON_ATTR_MISC_NONE;
            }
            attr |= GX_POLYGON_ATTR_MISC_FOG;
            // Polygon attribute settings
            G3_PolygonAttr(GX_LIGHTMASK_NONE,   // No light influence
                           GX_POLYGONMODE_MODULATE,     // Modulation mode
                           GX_CULL_BACK,        // Do cull back
                           0,          // Polygon ID 0
                           alpha,      // Set alpha value
                           attr);
        }

        DrawCube();                    // Draw cube

        G3_PopMtx(1);

        /* rectangle render configuration (do not perform fog blending)*/
        G3_PushMtx();
        // Set material's diffuse reflection color, and ambient reflection color    
        G3_MaterialColorDiffAmb(GX_RGB(0, 0, 31),       // Diffuse reflection color
                                GX_RGB(16, 16, 16),     // Ambient reflection color
                                TRUE); // Set vertex color
        // Set material's specular reflection color, and emitted color    
        G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16),     // Specular reflection color
                                GX_RGB(0, 0, 0),        // Emitted light color    
                                FALSE); // Specular reflection not used
        // Polygon attribute settings
        G3_PolygonAttr(GX_LIGHTMASK_NONE,       // No light influence
                       GX_POLYGONMODE_MODULATE, // Modulation mode
                       GX_CULL_BACK,   // Do cull back
                       0,              // Polygon ID 0
                       31,             // Alpha value
                       0);

        G3_Begin(GX_BEGIN_QUADS);      // Begin vertex list (draw with quadrilateral polygons)
        {
            G3_Vtx(FX32_ONE << 2, -FX32_ONE << 2, 0);
            G3_Vtx(FX32_ONE << 2, FX32_ONE << 2, 0);
            G3_Vtx(-FX32_ONE, FX32_ONE << 2, 0);
            G3_Vtx(-FX32_ONE, -FX32_ONE << 2, 0);
        }
        G3_End();                      // End of vertex list

        G3_PopMtx(1);

        // swapping the polygon list RAM, the vertex RAM, etc.
        G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_Z);

        /* Waiting for the V-blank*/
        OS_WaitVBlankIntr();

#ifdef SDK_AUTOTEST                    // code for auto-test
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(100, 0x2A787000);
        EXT_TestTickCounter();
#endif //SDK_AUTOTEST
    }
}
