/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/3D_PolAttr_1Dot
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

  Revision 1.8  11/21/2005 10:33:52  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.7  02/28/2005 05:26:04  yosizaki
  do-indent.

  Revision 1.6  2004/07/27 08:02:40  yasu
  Avoid warning of padding

  Revision 1.5  05/27/2004 09:24:21  takano_makoto
  Added Autotest code

  Revision 1.4  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.3  04/06/2004 12:48:06  yada
  fix header comment

  Revision 1.2  03/15/2004 02:00:17  takano_makoto
  modify comments and codes a little.

  Revision 1.1  03/01/2004 09:07:03  takano_makoto
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// A sample that control the display of 1dot polygon by depth value.
// 
// This sample displays three cubes, and change scale and depth by pad.
// If cubes looks less than 1dot, set visible or invisible by press A.
// 
// USAGE:
//   UP, DOWN   : Change the limit depth value that 1dot polygon is visible.
//   L, R                       : Change the scale of object.
//   A: Switch ON/OFF that control visible of 1dot polygon.
//   
//---------------------------------------------------------------------------

#include <nitro.h>
#include "DEMO.h"
#include "data.h"

//---------------------------------------------------------------------------
//  Overview:
//        Cube model data
//---------------------------------------------------------------------------
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
//        Set vertex color
//---------------------------------------------------------------------------
inline void Color(void)
{
    G3_Color(GX_RGB(31, 31, 31));
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
//        Cube drawing information type
//  Description:
//---------------------------------------------------------------------------
#pragma  warn_padding off
typedef struct
{
    int     trg;
    fx32    scale;
    fx32    depth;
    u32     texAddr;
    u16     rotate;
    // [PADDING 2 BYTE HERE]
}
cubu_c;
#pragma  warn_padding reset


//---------------------------------------------------------------------------
//  Overview:
//        Draw cube
//  Description:
//
//  Input:
//        *p : Cube information pointer
//        x : X position
//        y: Y position
//        z: Z position
//---------------------------------------------------------------------------
static void Cube(cubu_c * p, fx32 x, fx32 y, fx32 z)
{
    G3_PushMtx();

    /* cube rotation and position configuration*/
    {
        fx16    s = FX_SinIdx(p->rotate);
        fx16    c = FX_CosIdx(p->rotate);

        G3_Translate(x, y, z);

        G3_RotX(s, c);
        G3_RotY(s, c);
        G3_RotZ(s, c);

        G3_Scale(p->scale, p->scale, p->scale);
    }

    /* cube render configuration*/
    // Set material's diffuse reflection color and ambient reflection color.
    G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31), // Diffuse reflection color
                            GX_RGB(16, 16, 16), // Ambient reflection color
                            TRUE);     // Set vertex color
    // Set material's specular reflection color and emitted light color    
    G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16), // Specular reflection color
                            GX_RGB(0, 0, 0),    // Emitted light color    
                            FALSE);    // Specular reflection not used
    // Polygon attribute settings

    // Specify texture parameters    
    G3_TexImageParam(GX_TEXFMT_DIRECT, // Use direct texture
                     GX_TEXGEN_TEXCOORD,        // Set texture coordinate conversion mode    
                     GX_TEXSIZE_S64,   // 64 texture s size
                     GX_TEXSIZE_T64,   // 64 texture t size
                     GX_TEXREPEAT_NONE, // No repeat
                     GX_TEXFLIP_NONE,  // No flip
                     GX_TEXPLTTCOLOR0_USE,      // Enable palette color 0 set value
                     p->texAddr);      // Texture address

    {
        int     attr;

        if (p->trg)
        {
            attr = GX_POLYGON_ATTR_MISC_DISP_1DOT;
        }
        else
        {
            attr = GX_POLYGON_ATTR_MISC_NONE;
        }
        // Polygon attribute settings
        G3_PolygonAttr(GX_LIGHTMASK_NONE,       // Do not reflect light
                       GX_POLYGONMODE_MODULATE, // Modulation mode
                       GX_CULL_NONE,   // No culling
                       0,              // Polygon ID 0
                       31,             // Alpha value
                       attr);          // 1 dot polygon display setting
    }
    // 
    G3X_SetDisp1DotDepth(p->depth);    // 1 dot polygon display border depth value    

    G3_Begin(GX_BEGIN_QUADS);          // Start vertex list (draw with quadrilateral polygons)
    {
        TextureCoord(1);
        Color();
        Vertex(2);
        TextureCoord(0);
        Color();
        Vertex(0);
        TextureCoord(2);
        Color();
        Vertex(4);
        TextureCoord(3);
        Color();
        Vertex(6);

        TextureCoord(1);
        Color();
        Vertex(7);
        TextureCoord(0);
        Color();
        Vertex(5);
        TextureCoord(2);
        Color();
        Vertex(1);
        TextureCoord(3);
        Color();
        Vertex(3);

        TextureCoord(1);
        Color();
        Vertex(6);
        TextureCoord(0);
        Color();
        Vertex(4);
        TextureCoord(2);
        Color();
        Vertex(5);
        TextureCoord(3);
        Color();
        Vertex(7);

        TextureCoord(1);
        Color();
        Vertex(3);
        TextureCoord(0);
        Color();
        Vertex(1);
        TextureCoord(2);
        Color();
        Vertex(0);
        TextureCoord(3);
        Color();
        Vertex(2);

        TextureCoord(1);
        Color();
        Vertex(5);
        TextureCoord(0);
        Color();
        Vertex(4);
        TextureCoord(2);
        Color();
        Vertex(0);
        TextureCoord(3);
        Color();
        Vertex(1);

        TextureCoord(1);
        Color();
        Vertex(6);
        TextureCoord(0);
        Color();
        Vertex(7);
        TextureCoord(2);
        Color();
        Vertex(3);
        TextureCoord(3);
        Color();
        Vertex(2);
    }
    G3_End();                          // End of vertex list

    G3_PopMtx(1);
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
//        Setting GX_POLYGON_ATTR_MISC_DISP_1DOT in G3_PolygonAttr()
//        (Use depth value to suppress 1 dot polygon display)
//  Description:
//        Use depth value to suppress 1 dot polygon display.
//
//        When the sample starts up, press BUTTON_L
//         of the ESS-ID.
//        The scale value becomes 0. Three objects are displayed as 1 dot.    
//        Press up and down on the +Control Pad to increase or decrease the depth value.    
//         This will cause deep objects to blink in order from the left.    
//  Operation:
//        BUTTON_L, R : Object scale value
//        UP, DOWN: Increase, decrease object depth value    
//        START      : Return to initial value
//        SELECT     : Use position test to update depth to near value    
//---------------------------------------------------------------------------
void NitroMain(void)
{
    cubu_c  cubu;

    cubu.trg = 1;
    cubu.scale = FX32_ONE;
    cubu.depth = 16384;
    cubu.texAddr = 0x01000;            // Slot address of the Texture image    
    cubu.rotate = 0;

    /*
     * Initialize (IRQ enable, VRAM initialize, use BG0 in 3D mode)
     */
    DEMOInitCommon();
    DEMOInitVRAM();
    DEMOInitDisplay3D();

    /* load the texture image to the texture image slot*/
    GX_BeginLoadTex();                 // Map the bank allocated to the slot to LCDC memory.
    {
        GX_LoadTex((void *)&tex_32768_64x64[0], // Load origin pointer
                   cubu.texAddr,       // Load destination slot address
                   8192);              // Load size
    }
    GX_EndLoadTex();                   // Return slot mapped to LCDC to origin    

    DEMOStartDisplay();

    while (1)
    {
        G3X_Reset();
        cubu.rotate += 256;

        DEMOReadKey();
#ifdef SDK_AUTOTEST                    // code for auto-test
        {
            const EXTKeys keys[8] =
                { {PAD_BUTTON_L, 80}, {PAD_KEY_DOWN, 8}, {PAD_BUTTON_A, 1}, {0, 0} };
            EXT_AutoKeys(keys, &gKeyWork.press, &gKeyWork.trigger);
        }
#endif

        if (DEMO_IS_PRESS(PAD_BUTTON_R))
        {
            cubu.scale += 64;
            OS_Printf("Scale=%d Depth=%d\n", cubu.scale, cubu.depth);
        }
        else if (DEMO_IS_PRESS(PAD_BUTTON_L))
        {
            cubu.scale -= 64;
            if (cubu.scale < 0)
            {
                cubu.scale = 0;
            }
            OS_Printf("Scale=%d Depth=%d\n", cubu.scale, cubu.depth);
        }
        if (DEMO_IS_PRESS(PAD_KEY_UP))
        {
            cubu.depth += 256;
            if (cubu.depth > 0xffffff)
            {
                cubu.depth = 0xffffff;
            }
            OS_Printf("Scale=%d Depth=%d\n", cubu.scale, cubu.depth);
        }
        else if (DEMO_IS_PRESS(PAD_KEY_DOWN))
        {
            cubu.depth -= 256;
            if (cubu.depth < 0)
            {
                cubu.depth = 0;
            }
            OS_Printf("Scale=%d Depth=%d\n", cubu.scale, cubu.depth);
        }
        if (DEMO_IS_TRIG(PAD_BUTTON_A))
        {
            cubu.trg = (cubu.trg + 1) & 0x01;
            OS_Printf("PolygonAttr_1DotPoly=%s\n", (cubu.trg ? "ON" : "OFF"));
        }
        if (DEMO_IS_TRIG(PAD_BUTTON_START))
        {
            cubu.scale = FX32_ONE;
            cubu.depth = 16384;
            OS_Printf("Scale=%d Depth=%d\n", cubu.scale, cubu.depth);
        }
        if (DEMO_IS_TRIG(PAD_BUTTON_SELECT))
        {
            fx16    s = FX_SinIdx(cubu.rotate);
            fx16    c = FX_CosIdx(cubu.rotate);

            G3_Translate(-FX32_ONE, 0, -4 * FX32_ONE);

            G3_RotX(s, c);
            G3_RotY(s, c);
            G3_RotZ(s, c);

            G3_Scale(cubu.scale, cubu.scale, cubu.scale);

            {
                VecFx32 m;
                fx32    w;

                G3_PositionTest(FX16_ONE, FX16_ONE, FX16_ONE);
                while (G3X_GetPositionTestResult(&m, &w))
                {
                    // Wait for position test
                }

                OS_Printf("Position Test : Pos(%d, %d, %d) W(%d)\n", m.x, m.y, m.z, w);
                cubu.depth = w;
            }
        }

        /* camera configuration*/
        {
            VecFx32 Eye = { 0, 0, FX32_ONE };   // Eye position
            VecFx32 at = { 0, 0, 0 };  // Viewpoint
            VecFx32 vUp = { 0, FX32_ONE, 0 };   // Up

            G3_LookAt(&Eye, &vUp, &at, NULL);   // Eye point settings
        }

        G3_MtxMode(GX_MTXMODE_TEXTURE); // Set matrix to Texture mode
        G3_Identity();                 // Initialize current matrix to a unit matrix
        // Set the matrix to Position, Vector simultaneous set mode    
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

        /* Draw cube*/
        Cube(&cubu, FX32_ONE, 0, -2 * FX32_ONE);
        Cube(&cubu, 0, 0, -3 * FX32_ONE);
        Cube(&cubu, -FX32_ONE, 0, -4 * FX32_ONE);

        // swapping the polygon list RAM, the vertex RAM, etc.
        G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);

        /* V-blank wait*/
        OS_WaitVBlankIntr();

#ifdef SDK_AUTOTEST                    // for auto test
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(150, 0xA00DF599);
        EXT_TestTickCounter();
#endif //SDK_AUTOTEST

    }
}
