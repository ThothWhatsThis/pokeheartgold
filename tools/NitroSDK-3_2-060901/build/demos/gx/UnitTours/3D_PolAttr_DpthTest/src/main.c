/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/3D_PolAttr_DpthTest
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

  Revision 1.7  11/21/2005 10:34:05  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.6  02/28/2005 05:26:04  yosizaki
  do-indent.

  Revision 1.5  05/27/2004 09:24:32  takano_makoto
  Added Autotest code

  Revision 1.4  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.3  04/06/2004 12:48:06  yada
  fix header comment

  Revision 1.2  03/15/2004 02:00:23  takano_makoto
  modify comments and codes a little.

  Revision 1.1  03/01/2004 09:07:48  takano_makoto
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


//---------------------------------------------------------------------------
// A sample that set GX_POLYGON_ATTR_MISC_DEPTHTEST_DECAL in G3_PolygonAttr().
// 
// This sample displays two squares. If switch is ON, the right one is drawn
// only when the polygon's depth value equals the value in depth buffer.
// 
// USAGE:
//   START      : Switch ON/OFF of GX_POLYGON_ATTR_MISC_DEPTHTEST_DECAL
//   
//---------------------------------------------------------------------------


#include <nitro.h>
#include "DEMO.h"
#include "data.h"

//---------------------------------------------------------------------------
//  Overview:
//        Rectangle model data
//---------------------------------------------------------------------------
/* vertex data*/
static s16 s_Vertex[3 * 8] = {
    FX16_ONE, FX16_ONE, FX16_ONE,
    FX16_ONE, -FX16_ONE, FX16_ONE,
    -FX16_ONE, FX16_ONE, FX16_ONE,
    -FX16_ONE, -FX16_ONE, FX16_ONE,
};
/* normal  data*/
static VecFx10 s_Normal = GX_VECFX10(0, 0, FX32_ONE - 1);
/* texture  coordinates*/
static GXSt s_TextureCoord[] = {
    GX_ST(0, 0),
    GX_ST(0, 64 * FX32_ONE),
    GX_ST(64 * FX32_ONE, 0),
    GX_ST(64 * FX32_ONE, 64 * FX32_ONE)
};

//---------------------------------------------------------------------------
//  Overview:
//        Set vertex coordinates
//  Description:
//        Use specified vertex data to set vertex coordinates.
//  Input:
//        i_idx: Vertex data ID
//---------------------------------------------------------------------------
inline void Vertex(int i_idx)
{
    G3_Vtx(s_Vertex[i_idx * 3], s_Vertex[i_idx * 3 + 1], s_Vertex[i_idx * 3 + 2]);
}

//---------------------------------------------------------------------------
//  Overview:
//        Set normals    
//  Description:
//  
//---------------------------------------------------------------------------
inline void Normal(void)
{
    G3_Direct1(G3OP_NORMAL, s_Normal);
}

//---------------------------------------------------------------------------
//  Overview:
//        Texture coordinate settings    
//  Description:
//        
//  Input:
//        i_idx: Texture data ID
//---------------------------------------------------------------------------
inline void TextureCoord(int i_idx)
{
    G3_Direct1(G3OP_TEXCOORD, s_TextureCoord[i_idx]);
}


//---------------------------------------------------------------------------
//  Overview:
//        Draw rectangles
//  Description:
//        
//---------------------------------------------------------------------------
static void DrawRectangle(void)
{
    G3_Begin(GX_BEGIN_QUADS);          // Begin vertex list (draw with quadrilateral polygons)
    {
        TextureCoord(1);
        Normal();
        Vertex(1);
        TextureCoord(3);
        Normal();
        Vertex(3);
        TextureCoord(2);
        Normal();
        Vertex(2);
        TextureCoord(0);
        Normal();
        Vertex(0);
    }
    G3_End();                          // End of vertex list
}

//---------------------------------------------------------------------------
//  Overview:
//        V-blank interrupt process
//  Description:
//       Enables a flag that confirms that a V-Blank interrupt has been performed.
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
//        Setting GX_POLYGON_ATTR_MISC_DEPTHTEST_DECAL in G3_PolygonAttr()
//  Description:
//        Draws when the polygon depth value is the same as the depth buffer's depth value.
//              
//        The rectangles shown on the left side are normal display. The rectangles shown on
//      the right side are depth targeted polygons. When you turn the effect ON, overlapping parts of the rectangles on the right side that have the same depth as the rectangles    
//      on the left side will blink over the rectangles on the left side.    
//  Operation:
//        START      : Switches the effect ON/OFF.
//---------------------------------------------------------------------------
void NitroMain(void)
{
    u32     texAddr = 0x00000;         // Slot address of the Texture image    
    unsigned int count = 0;
    BOOL    trg = 0;
    u16     rotate = 0;

    //---------------------
    // Initialize(IRQ enable, VRAM initialize, use BG0 in 3D mode)
    //---------------------
    DEMOInitCommon();
    DEMOInitVRAM();
    DEMOInitDisplay3D();

    /* load the texture image to the texture image slot*/
    GX_BeginLoadTex();                 // Map the bank allocated to the slot to LCDC memory.
    {
        GX_LoadTex((void *)&tex_32768_64x64[0], // Load origin pointer
                   texAddr,            // Load destination slot address
                   8192);              // Load size
    }
    GX_EndLoadTex();                   // Return slot mapped to LCDC to origin    

    DEMOStartDisplay();

    OS_Printf("mssg%d:PolygonAttr_DepthTest=%s\n", count++, (trg ? "ON" : "OFF"));

    //---------------------
    //  Main loop
    //---------------------
    while (1)
    {
        G3X_Reset();
        rotate += 128;

        /* Pad input*/
        DEMOReadKey();
#ifdef SDK_AUTOTEST                    // code for auto-test
        {
            const EXTKeys keys[8] = { {0, 20}, {PAD_BUTTON_START, 20}, {0, 0} };
            EXT_AutoKeys(keys, &gKeyWork.press, &gKeyWork.trigger);
        }
#endif

        if (DEMO_IS_TRIG(PAD_BUTTON_START))
        {
            trg = (trg + 1) & 0x01;
            OS_Printf("mssg%d:PolygonAttr_DepthTest=%s\n", count++, (trg ? "ON" : "OFF"));
        }

        /* camera configuration*/
        {
            VecFx32 Eye = { 0, 0, FX32_ONE };   // Eye point position
            VecFx32 at = { 0, 0, 0 };  // Viewpoint
            VecFx32 vUp = { 0, FX32_ONE, 0 };   // Up

            G3_LookAt(&Eye, &vUp, &at, NULL);   // Eye point settings
        }

        /* light configuration (configures light color and direction)*/
        {
            G3_LightVector(GX_LIGHTID_0, 0, 0, -FX32_ONE + 1);  // Light direction
            G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));    // Light color
        }

        /* left display rectangle render configuration*/
        G3_MtxMode(GX_MTXMODE_TEXTURE);
        G3_Identity();
        // Set the matrix to Position, Vector simultaneous set mode    
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

        G3_PushMtx();                  // Push the matrix to the stack    

        /* left display rectangle rotation calculation and position configuration*/
        {
            fx16    s = FX_SinIdx(rotate);
            fx16    c = FX_CosIdx(rotate);

            G3_Translate(-FX32_ONE / 2, 0, -3 * FX32_ONE);
            G3_RotZ(s, c);
        }

        // Set material's diffuse reflection color and ambient reflection color.
        G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31),     // Diffuse reflection color
                                GX_RGB(16, 16, 16),     // Ambient reflection color
                                TRUE); // Set vertex color
        // Set material's specular reflection color and emitted light color    
        G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16),     // Specular reflection color
                                GX_RGB(0, 0, 0),        // Emitted light color    
                                FALSE); // Specular reflection not used
        // Specify texture parameters    
        G3_TexImageParam(GX_TEXFMT_DIRECT,      // Use direct texture
                         GX_TEXGEN_TEXCOORD,    // Set texture coordinate conversion mode    
                         GX_TEXSIZE_S64,        // 64 texture s size
                         GX_TEXSIZE_T64,        // 64 texture t size
                         GX_TEXREPEAT_NONE,     // No repeat
                         GX_TEXFLIP_NONE,       // No flip
                         GX_TEXPLTTCOLOR0_USE,  // Enable palette color 0 set value
                         texAddr);     // Texture address
        // Polygon attribute settings
        G3_PolygonAttr(GX_LIGHTMASK_0, // Reflect light 0
                       GX_POLYGONMODE_MODULATE, // Modulation mode
                       GX_CULL_NONE,   // Do not cull
                       0,              // Polygon ID 0
                       31,             // Alpha value
                       GX_POLYGON_ATTR_MISC_NONE);

        DrawRectangle();               // Draw rectangles

        G3_PopMtx(1);

        G3_PushMtx();

        /* right display rectangle rotation calculation and position configuration*/
        {
            fx16    s = FX_SinIdx(rotate);
            fx16    c = FX_CosIdx(rotate);

            G3_Translate(FX32_ONE / 2, 0, -3 * FX32_ONE);
            G3_RotZ(s, c);
        }
        /* right display rectangle render configuration*/
        G3_MtxMode(GX_MTXMODE_TEXTURE); // Set matrix to Texture mode
        G3_Identity();                 // Initialize current matrix to a unit matrix
        // Set the matrix to Position, Vector simultaneous set mode    
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
        // Set material's diffuse reflection color and ambient reflection color.
        G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31),     // Diffuse reflection color
                                GX_RGB(16, 16, 16),     // Ambient reflection color
                                TRUE); // Set vertex color
        // Set material's specular reflection color and emitted light color    
        G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16),     // Specular reflection color
                                GX_RGB(0, 0, 0),        // Emitted light color    
                                FALSE); // Specular reflection not used
        // Specify texture parameters    
        G3_TexImageParam(GX_TEXFMT_DIRECT,      // Use direct texture
                         GX_TEXGEN_TEXCOORD,    // Set texture coordinate conversion mode    
                         GX_TEXSIZE_S64,        // 64 texture s size
                         GX_TEXSIZE_T64,        // 64 texture t size
                         GX_TEXREPEAT_NONE,     // No repeat
                         GX_TEXFLIP_NONE,       // No flip
                         GX_TEXPLTTCOLOR0_USE,  // Enable palette color 0 set value
                         texAddr);     // Texture address

        // Depth change settings for the rectangles that are displayed on the right.
        {
            int     attr;

            if (trg)
            {
                attr = GX_POLYGON_ATTR_MISC_DEPTHTEST_DECAL;
            }
            else
            {
                attr = GX_POLYGON_ATTR_MISC_NONE;
            }
            // Polygon attribute settings
            G3_PolygonAttr(GX_LIGHTMASK_0,      // Light 0 ON
                           GX_POLYGONMODE_MODULATE,     // Modulation mode
                           GX_CULL_NONE,        // No culling
                           0,          // Polygon ID 0
                           31,         // Alpha value
                           attr);
        }

        DrawRectangle();               // Draw rectangles

        G3_PopMtx(1);

        // swapping the polygon list RAM, the vertex RAM, etc.
        G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);

        OS_WaitVBlankIntr();           // V-Blank wait

#ifdef SDK_AUTOTEST                    // code for auto-test
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(100, 0x43CB4676);
        EXT_TestTickCounter();
#endif //SDK_AUTOTEST

    }
}
