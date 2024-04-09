/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/3D_PolAttr_FARClip
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.10  01/18/2006 02:11:21  kitase_hirotake
  do-indent

  Revision 1.9  11/21/2005 10:34:33  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.8  02/28/2005 05:26:04  yosizaki
  do-indent.

  Revision 1.7  11/02/2004 08:23:24  takano_makoto
  fix comment.

  Revision 1.6  2004/09/14 01:50:40  yasu
  Fixed backslashes at end of line.

  Revision 1.5  05/27/2004 09:24:52  takano_makoto
  Added Autotest code

  Revision 1.4  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.3  04/06/2004 12:48:06  yada
  fix header comment

  Revision 1.2  03/15/2004 02:01:25  takano_makoto
  modify comments and codes a little.

  Revision 1.1  03/01/2004 09:09:15  takano_makoto
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// A sample that set GX_POLYGON_ATTR_MISC_FAR_CLIPP in G3_PolygonAttr().
//
// This sample displays two cubes.
// The left one is normal setting and the right one is used far clipping.
// If polygon cross the far face, the right one is clipped.
//
// USAGE:
//   UP, DOWN   : Move z position of the object.
//   START      : Reset position of the object.
//
//---------------------------------------------------------------------------

#include <nitro.h>
#include "DEMO.h"
#include "data.h"

//---------------------------------------------------------------------------
//  Overview:
//        Cube model data
//---------------------------------------------------------------------------
/*  */
/* vertex data*/
static s16 s_Vertex[3 * 8] = {
    FX16_ONE, FX16_ONE, FX16_ONE,
    FX16_ONE, FX16_ONE, -FX16_ONE,
    FX16_ONE, -FX16_ONE, FX16_ONE,
    FX16_ONE, -FX16_ONE, -FX16_ONE,
    -FX16_ONE, FX16_ONE, FX16_ONE,
    -FX16_ONE, FX16_ONE, -FX16_ONE,
    -FX16_ONE, -FX16_ONE, FX16_ONE,
    -FX16_ONE, -FX16_ONE, -FX16_ONE
};

/* normal  data*/
static VecFx10 s_Normal[6] = {
    GX_VECFX10(0, 0, FX32_ONE - 1),
    GX_VECFX10(0, FX32_ONE - 1, 0),
    GX_VECFX10(FX32_ONE - 1, 0, 0),
    GX_VECFX10(0, 0, -FX32_ONE + 1),
    GX_VECFX10(0, -FX32_ONE + 1, 0),
    GX_VECFX10(-FX32_ONE + 1, 0, 0)
};

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
    G3_Vtx(s_Vertex[i_idx * 3],        // Set vertex coordinates
           s_Vertex[i_idx * 3 + 1], s_Vertex[i_idx * 3 + 2]);
}

//---------------------------------------------------------------------------
//  Overview:
//        Set normals    
//  Description:
//        
//  Input:
//        i_idx: Normal data ID    
//---------------------------------------------------------------------------
inline void Normal(int i_idx)
{
    G3_Direct1(G3OP_NORMAL, s_Normal[i_idx]);
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
//        Draw cube
//---------------------------------------------------------------------------
static void DrawCube(void)
{
    G3_Begin(GX_BEGIN_QUADS);          // Begin vertex list (draw with quadrilateral polygons)
    {
        TextureCoord(1);
        Normal(0);
        Vertex(2);
        TextureCoord(0);
        Normal(0);
        Vertex(0);
        TextureCoord(2);
        Normal(0);
        Vertex(4);
        TextureCoord(3);
        Normal(0);
        Vertex(6);

        TextureCoord(1);
        Normal(3);
        Vertex(7);
        TextureCoord(0);
        Normal(3);
        Vertex(5);
        TextureCoord(2);
        Normal(3);
        Vertex(1);
        TextureCoord(3);
        Normal(3);
        Vertex(3);

        TextureCoord(1);
        Normal(5);
        Vertex(6);
        TextureCoord(0);
        Normal(5);
        Vertex(4);
        TextureCoord(2);
        Normal(5);
        Vertex(5);
        TextureCoord(3);
        Normal(5);
        Vertex(7);

        TextureCoord(1);
        Normal(2);
        Vertex(3);
        TextureCoord(0);
        Normal(2);
        Vertex(1);
        TextureCoord(2);
        Normal(2);
        Vertex(0);
        TextureCoord(3);
        Normal(2);
        Vertex(2);

        TextureCoord(1);
        Normal(1);
        Vertex(5);
        TextureCoord(0);
        Normal(1);
        Vertex(4);
        TextureCoord(2);
        Normal(1);
        Vertex(0);
        TextureCoord(3);
        Normal(1);
        Vertex(1);

        TextureCoord(1);
        Normal(4);
        Vertex(6);
        TextureCoord(0);
        Normal(4);
        Vertex(7);
        TextureCoord(2);
        Normal(4);
        Vertex(3);
        TextureCoord(3);
        Normal(4);
        Vertex(2);
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
//        Clip far surface intersect polygons for G3_PolygonAttr()    
//  Description:
//          The cubes displayed on the left are not far surface clipped.    
//          The cubes on the right are far surface clipped.    
//          The viewing volume far value is extremely small compared to the normal viewing volume setting.    
//          This is to make the clipping process more apparent.    
//  Operation:
//        UP, DOWN: Manipulate the object's Z coordinate position.
//        START      : Return to initial position.    
//---------------------------------------------------------------------------
void NitroMain(void)
{
    int     pos_z = -5;
    u16     rotate = 0;
    u32     texAddr = 0x01000;         // Slot address of the Texture image    

    //---------------------
    // Initialize(IRQ enable, VRAM initialize, use BG0 for 3D mode)
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

    /* Perspective projection settings*/
    {
        fx32    right = FX32_ONE;
        fx32    top = FX32_ONE * 3 / 4;
        fx32    near = FX32_ONE;
        fx32    far = FX32_ONE * 6;
        // Perspective projection settings
        G3_Perspective(FX32_SIN30,     // Sine   FOVY
                       FX32_COS30,     // Cosine FOVY
                       FX32_ONE * 4 / 3,        // Aspect
                       near,           // Near
                       far,            // Far
                       NULL);

        G3_StoreMtx(0);                // Save the matrix in stack number 0    
    }

    DEMOStartDisplay();

    //---------------------
    //  Main loop
    //---------------------
    while (1)
    {
        G3X_Reset();
        rotate += 256;

        /* Process pad input*/
        DEMOReadKey();

        if (DEMO_IS_TRIG(PAD_KEY_DOWN))
        {
            if (++pos_z > -3)
            {
                pos_z = -3;
            }
            OS_Printf("Pos_Z=%d\n", pos_z);
        }
        else if (DEMO_IS_TRIG(PAD_KEY_UP))
        {
            if (--pos_z < -7)
            {
                pos_z = -7;
            }
            OS_Printf("Pos_Z=%d\n", pos_z);
        }
        if (DEMO_IS_TRIG(PAD_BUTTON_START))
        {
            pos_z = -5;
            OS_Printf("Pos_Z=%d\n", pos_z);
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
            G3_LightVector(GX_LIGHTID_0, 0, 0, -FX32_ONE + 1);
            G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
        }

        G3_MtxMode(GX_MTXMODE_TEXTURE); // Set matrix to Texture mode
        G3_Identity();                 // Initialize current matrix to a unit matrix
        // Set the matrix to Position, Vector simultaneous set mode    
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

        G3_PushMtx();                  // Push the matrix to the stack    

        /* right display calculate the cube rotation and set its position*/
        {
            fx16    s = FX_SinIdx(rotate);
            fx16    c = FX_CosIdx(rotate);

            G3_Translate(FX32_ONE * 2, 0, pos_z * FX32_ONE);
            G3_RotX(s, c);
            G3_RotY(s, c);
            G3_RotZ(s, c);
        }

        /* right display cube render configuration*/
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
        G3_PolygonAttr(GX_LIGHTMASK_0, // Light 0 ON
                       GX_POLYGONMODE_MODULATE, // Modulation mode
                       GX_CULL_NONE,   // No culling
                       0,              // Polygon ID 0
                       31,             // Alpha value
                       GX_POLYGON_ATTR_MISC_FAR_CLIPPING);

        DrawCube();                    // Draw cube

        G3_PopMtx(1);

        G3_PushMtx();
        /* left display cube rotation calculation and position configuration*/
        {
            fx16    s = FX_SinIdx(rotate);
            fx16    c = FX_CosIdx(rotate);

            G3_Translate(-FX32_ONE * 2, 0, pos_z * FX32_ONE);

            G3_RotX(s, c);
            G3_RotY(s, c);
            G3_RotZ(s, c);
        }

        G3_MtxMode(GX_MTXMODE_TEXTURE); // Set matrix to Texture mode
        G3_Identity();                 // Initialize current matrix to a unit matrix
        // Set the matrix to Position, Vector simultaneous set mode    
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

        /* left display cube render configuration*/
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
        G3_PolygonAttr(GX_LIGHTMASK_0, // Light 0 ON
                       GX_POLYGONMODE_MODULATE, // Modulation mode
                       GX_CULL_NONE,   // No cull back
                       0,              // Polygon ID 0
                       31,             // Alpha value
                       GX_POLYGON_ATTR_MISC_NONE);

        DrawCube();                    // Draw cube

        G3_PopMtx(1);

        // swapping the polygon list RAM, the vertex RAM, etc.
        G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);

        /* V-Blank wait*/
        OS_WaitVBlankIntr();

#ifdef SDK_AUTOTEST                    // code for auto-test
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(100, 0x9F1292DC);
        EXT_TestTickCounter();
#endif //SDK_AUTOTEST
    }
}
