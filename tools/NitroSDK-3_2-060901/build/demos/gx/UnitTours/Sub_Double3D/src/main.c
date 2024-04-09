/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/Sub_Double3D
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.20  01/18/2006 02:11:28  kitase_hirotake
  do-indent

  Revision 1.19  11/21/2005 10:44:16  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.18  10/07/2005 09:02:12  okubata_ryoma
  Minor revisions

  Revision 1.17  10/07/2005 05:22:48  okubata_ryoma
  Corrected comment

  Revision 1.16  10/07/2005 05:11:45  okubata_ryoma
  Corrected comment

  Revision 1.14  08/03/2005 02:03:11  okubata_ryoma
  Fixed wrap-around problem again.

  Revision 1.13  07/07/2005 06:16:11  okubata_ryoma
  Fixed wrap-around problem with objects.

  Revision 1.10  07/07/2005 04:47:46  okubata_ryoma
  Minor revisions

  Revision 1.8  06/27/2005 11:10:12  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.7  02/28/2005 05:26:19  yosizaki
  do-indent.

  Revision 1.6  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.5  04/06/2004 12:48:08  yada
  fix header comment

  Revision 1.4  2004/02/24 10:48:30  yasu
  fix initial value of "flag"

  Revision 1.3  2004/02/24 05:25:21  yasu
  used OS_EnableIrqMask instead of OS_SetIrqMask

  Revision 1.2  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.1  02/04/2004 11:11:04  nishida_kenji
  new

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// A sample that displays moving 3D models on both LCDs.
//
// Both LCDs display a different rotating 3D cube.
// The sub 2D engine displays the captured image rendered by the main engine,
// making use of direct color bitmap BG/bitmap OBJ.
// Note that the output LCDs are swapped every frame.
//
// HOWTO:
// 1. Set up OAM for the sub 2D engine, to display captured images correctly.
// 2. (Frame 2N):   Assign VRAM-C to BG of the sub 2D engine,
//                  and display the captured image on VRAM-C
//                  through direct color bitmap BG.
//                  Capture the 3D plane and store it on VRAM-D.
//    (Frame 2N+1): Assign VRAM-D to OBJ of the sub 2D engine,
//                  and display the captured image on VRAM-D
//                  through bitmap OBJs.
//                  Capture the 3D plane and store it on VRAM-C.
//---------------------------------------------------------------------------

#include <nitro.h>
#include "DEMO.h"

static GXOamAttr sOamBak[128];
u16     keyData;

BOOL    flip_flag = TRUE;              // Flip switch flag
BOOL    swap = FALSE;                  // SwapBuffers execution flag

s16     gCubeGeometry[3 * 8] = {
    FX16_ONE, FX16_ONE, FX16_ONE,
    FX16_ONE, FX16_ONE, -FX16_ONE,
    FX16_ONE, -FX16_ONE, FX16_ONE,
    FX16_ONE, -FX16_ONE, -FX16_ONE,
    -FX16_ONE, FX16_ONE, FX16_ONE,
    -FX16_ONE, FX16_ONE, -FX16_ONE,
    -FX16_ONE, -FX16_ONE, FX16_ONE,
    -FX16_ONE, -FX16_ONE, -FX16_ONE
};

GXRgb   gCubeColor[8] = {
    GX_RGB(31, 31, 31),
    GX_RGB(31, 31, 0),
    GX_RGB(31, 0, 31),
    GX_RGB(31, 0, 0),
    GX_RGB(0, 31, 31),
    GX_RGB(0, 31, 0),
    GX_RGB(0, 0, 31),
    GX_RGB(0, 0, 0)
};

static void Color(int idx)
{
    G3_Color(gCubeColor[idx]);
}

static void Vtx(int idx)
{
    G3_Vtx(gCubeGeometry[idx * 3], gCubeGeometry[idx * 3 + 1], gCubeGeometry[idx * 3 + 2]);
}

static void Quad(int idx0, int idx1, int idx2, int idx3)
{
    Vtx(idx0);
    Vtx(idx1);
    Vtx(idx2);
    Vtx(idx3);
}

static void ColVtxQuad(int idx0, int idx1, int idx2, int idx3)
{
    Color(idx0);
    Vtx(idx0);
    Color(idx1);
    Vtx(idx1);
    Color(idx2);
    Vtx(idx2);
    Color(idx3);
    Vtx(idx3);
}

static void drawLeftCube(u16 Rotate)
{
    G3_PushMtx();

    // Rotate and translate
    G3_Translate(-3 << (FX32_SHIFT - 1), 0, 0);
    {
        fx16    s = FX_SinIdx(Rotate);
        fx16    c = FX_CosIdx(Rotate);

        G3_RotX(s, c);
        G3_RotY(s, c);
        G3_RotZ(s, c);
    }

    G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31), // diffuse
                            GX_RGB(16, 16, 16), // ambient
                            FALSE      // use diffuse as vtx color if TRUE
        );

    G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16), // specular
                            GX_RGB(0, 0, 0),    // emission
                            FALSE      // use shininess table if TRUE
        );

    G3_PolygonAttr(GX_LIGHTMASK_NONE,  // disable lights
                   GX_POLYGONMODE_MODULATE,     // modulation mode
                   GX_CULL_BACK,       // cull back
                   0,                  // polygon ID(0 - 63)
                   31,                 // alpha(0 - 31)
                   0                   // OR of GXPolygonAttrMisc's value
        );

    //---------------------------------------------------------------------------
    // Draw a cube:
    // Specify different colors for the planes.
    //---------------------------------------------------------------------------

    G3_Begin(GX_BEGIN_QUADS);

    {
        Color(3);
        Quad(2, 0, 4, 6);

        Color(4);
        Quad(7, 5, 1, 3);

        Color(5);
        Quad(6, 4, 5, 7);

        Color(2);
        Quad(3, 1, 0, 2);

        Color(6);
        Quad(5, 4, 0, 1);

        Color(1);
        Quad(6, 7, 3, 2);
    }

    G3_End();

    G3_PopMtx(1);
}

static void drawRightCube(u16 Rotate)
{
    G3_PushMtx();

    // Rotate and translate
    G3_Translate(3 << (FX32_SHIFT - 1), 0, 0);

    {
        fx16    s = FX_SinIdx(Rotate);
        fx16    c = FX_CosIdx(Rotate);

        G3_RotX(s, c);
        G3_RotY(s, c);
        G3_RotZ(s, c);
    }

    G3_MaterialColorDiffAmb(GX_RGB(31, 31, 31), // diffuse
                            GX_RGB(16, 16, 16), // ambient
                            FALSE      // use diffuse as vtx color if TRUE
        );

    G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16), // specular
                            GX_RGB(0, 0, 0),    // emission
                            FALSE      // use shininess table if TRUE
        );

    G3_PolygonAttr(GX_LIGHTMASK_NONE,  // disable lights
                   GX_POLYGONMODE_MODULATE,     // modulation mode
                   GX_CULL_BACK,       // cull back
                   0,                  // polygon ID(0 - 63)
                   31,                 // alpha(0 - 31)
                   0                   // OR of GXPolygonAttrMisc's value
        );

    //---------------------------------------------------------------------------
    // Draw a cube:
    // Specify different colors for the vertices.
    //---------------------------------------------------------------------------
    G3_Begin(GX_BEGIN_QUADS);
    {
        ColVtxQuad(2, 0, 4, 6);
        ColVtxQuad(7, 5, 1, 3);
        ColVtxQuad(6, 4, 5, 7);
        ColVtxQuad(3, 1, 0, 2);
        ColVtxQuad(5, 4, 0, 1);
        ColVtxQuad(6, 7, 3, 2);
    }
    G3_End();

    G3_PopMtx(1);

}

static void setupFrame2N(void)
{

    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);

    (void)GX_ResetBankForSubOBJ();
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    GX_SetBankForLCDC(GX_VRAM_LCDC_D);
    GX_SetCapture(GX_CAPTURE_SIZE_256x192,
                  GX_CAPTURE_MODE_A,
                  GX_CAPTURE_SRCA_3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_D_0x00000, 16, 0);

    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
    GX_SetVisiblePlane(GX_PLANEMASK_BG0);
    G2_SetBG0Priority(0);

    GXS_SetGraphicsMode(GX_BGMODE_5);
    GXS_SetVisiblePlane(GX_PLANEMASK_BG2);
    G2S_SetBG2ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256,
                           GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x00000);
    G2S_SetBG2Priority(0);
    G2S_BG2Mosaic(FALSE);
}

static void setupFrame2N_1(void)
{
    GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
    (void)GX_ResetBankForSubBG();
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);
    GX_SetBankForLCDC(GX_VRAM_LCDC_C);
    GX_SetCapture(GX_CAPTURE_SIZE_256x192,
                  GX_CAPTURE_MODE_A,
                  GX_CAPTURE_SRCA_3D, (GXCaptureSrcB)0, GX_CAPTURE_DEST_VRAM_C_0x00000, 16, 0);

    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
    GX_SetVisiblePlane(GX_PLANEMASK_BG0);
    G2_SetBG0Priority(0);

    GXS_SetGraphicsMode(GX_BGMODE_5);
    GXS_SetVisiblePlane(GX_PLANEMASK_OBJ);
}

static void setupSubOAM(void)
{
    int     i;
    int     x, y;
    int     idx = 0;

    GXS_SetOBJVRamModeBmp(GX_OBJVRAMMODE_BMP_2D_W256);

    for (i = 0; i < 128; ++i)
    {
        sOamBak[i].attr01 = 0;
        sOamBak[i].attr23 = 0;
    }

    for (y = 0; y < 192; y += 64)
    {
        for (x = 0; x < 256; x += 64, idx++)
        {
            G2_SetOBJAttr(&sOamBak[idx],
                          x,
                          y,
                          0,
                          GX_OAM_MODE_BITMAPOBJ,
                          FALSE,
                          GX_OAM_EFFECT_NONE,
                          GX_OAM_SHAPE_64x64, GX_OAM_COLOR_16, (y / 8) * 32 + (x / 8), 15, 0);
        }
    }

    DC_FlushRange(&sOamBak[0], sizeof(sOamBak));
    /* IO register is accessed using DMA operation, so cache Wait is not needed*/
    // DC_WaitWriteBufferEmpty();
    GXS_LoadOAM(&sOamBak[0], 0, sizeof(sOamBak));

}

//---------------------------------------------------------------------------
// Enables the SwapBuffers execution flag.
//---------------------------------------------------------------------------
static void SetSwapBuffersflag(void)
{
    OSIntrMode old = OS_DisableInterrupts();    // interrupts prohibited
    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
    swap = TRUE;
    (void)OS_RestoreInterrupts(old);
}


void NitroMain(void)
{
//#define CPU_TEST                      // Puts a load on the CPU (for testing)
//#define GE_TEST                      // Puts a load on the Geometry Engine (for testing)

    u16     Rotate = 0;                // for rotating cubes(0-65535)

    int     i = 0;

#if defined(CPU_TEST) || defined(GE_TEST)
    MATHRandContext32 rnd;             // Variable to generate random number that applies load to CPU (For testing)
#endif
    //---------------------------------------------------------------------------
    // Initialize:
    // They enable IRQ interrupts, initialize VRAM, and set BG #0 for 3D mode.
    //---------------------------------------------------------------------------
    DEMOInitCommon();
    DEMOInitVRAM();


    G3X_Init();
    G3X_InitTable();
    G3X_InitMtxStack();
    G3X_AntiAlias(TRUE);
    setupSubOAM();

    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
    G3X_SetClearColor(GX_RGB(0, 0, 0), // clear color
                      0,               // clear alpha
                      0x7fff,          // clear depth
                      63,              // clear polygon ID
                      FALSE            // fog
        );

    G3_ViewPort(0, 0, 255, 191);       // Viewport

    //---------------------------------------------------------------------------
    // Set up the projection matrix
    //---------------------------------------------------------------------------
    {
        fx32    right = FX32_ONE;
        fx32    top = FX32_ONE * 3 / 4;
        fx32    near = FX32_ONE;
        fx32    far = FX32_ONE * 400;

        //---------------------------------------------------------------------------
        // Switch MTXMODE to GX_MTXMODE_PROJECTION, and
        // set a projection matrix onto the current projection matrix on the matrix stack
        //---------------------------------------------------------------------------
        G3_Perspective(FX32_SIN30, FX32_COS30,  // sine and cosine of FOVY
                       FX32_ONE * 4 / 3,        // aspect
                       near,           // near
                       far,            // far
                       NULL            // a pointer to a matrix if you use it
            );

        G3_StoreMtx(0);
    }

    DEMOStartDisplay();

#if defined(CPU_TEST) || defined(GE_TEST)
    MATH_InitRand32(&rnd, OS_GetVBlankCount()); // Generates a random number that applies load to CPU (For testing)
#endif

    while (1)
    {
        keyData = PAD_Read();


        G3X_Reset();

        Rotate += 256;

        //---------------------------------------------------------------------------
        // Set up camera matrix
        //---------------------------------------------------------------------------
        {
            VecFx32 Eye = { 0, 0, FX32_ONE * 5 };       // Eye Position
            VecFx32 at = { 0, 0, 0 };  // Viewpoint
            VecFx32 vUp = { 0, FX32_ONE, 0 };   // Up

            G3_LookAt(&Eye, &vUp, &at, NULL);
        }
        G3_PushMtx();

        if (flip_flag)
            drawRightCube(Rotate);
        else
            drawLeftCube(Rotate);
        G3_PopMtx(1);

#ifdef GE_TEST
        while (GX_GetVCount() != 191)
        {;
        }
#endif

#ifdef CPU_TEST
        // Puts a load on the CPU with a random number (For testing)
        if (keyData & PAD_BUTTON_R)
        {
            OS_SpinWait(MATH_Rand32(&rnd, 1000000));
        }
#endif

#ifdef GE_TEST
        // Puts a load on the Geometry Engine with a random number (For testing)
        while (i <= MATH_Rand32(&rnd, 1000000))
        {
            G3_PushMtx(1);
            G3_PopMtx(1);
            i++;
        }
#endif

        SetSwapBuffersflag();          // Enables the SwapBuffers execution flag.

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
/*
---------------------------------------------------------------------------------------------------

(Below) An explanation of the operations of the SwapBuffers command and the Geometry Engine.

When the SwapBuffers command calls the G3_SwapBuffers function, it is stored in the Geometry command FIFO,
  and regardless of when the SwapBuffers command was called from the Geometry command FIFO,
  the buffer swap operation is mounted when the next VBlank starts.
  (Basically, it can only be mounted when a VBlank starts.)
- Therefore, if an operation such as drawing slips into a VBlank interval,
  and the SwapBuffers command was unable to mount when the VBlank started,
  it waits for about one frame until the VBlank in the next frames starts and the Geometry Engine remains busy.
  During this time, operations like drawing graphics or swapping cannot be done, so the same image as in the last frame is displayed.


s: Run SwapBuffers command
S : Mount the buffer swap operation
w: Wait from when the SwapBuffers command was run until the buffer swap operation begins.
G : The Geometry Engine is busy drawing, etc.

                          |
-VBlank(end)------------------------------------------
               +-----+    |     +-----+
               |     |    |     |     |
               |     |    |     |     |
               |  G  |    |     |  G  |
               |     |    |     |     |
               |     |    |     |     |
               |     |    |     |     |
               |     |    |     |     |
               |     |    |     |     |
               +-----+    |     |     |
               |  s  |    |     |     |
               +-----+    |     |     |
               |  w  |    |     |     |
-VBlank(start)-+-----+----+-----+     +-----------
  * 784(cycle) |  S  |    |     |     |            * The number of CPU cycles taken by this swap operation is 784 cycles.
               +-----+    |     +-----+           
    * check ->           |     |  s  |            * This check part uses the G3X_IsGeometryBusy function
                          |     +-----+              to determine whether the swap operation has finished.    
-VBlank(end)--------------+-----+     +---------
                          |     |     |
                          |     |     |
                          |     |     |
                          |     |     |
                          |     |     |
                          |     |     |
                          |     |  w  |
                          |     |     |
                          |     |     |
                          |     |     |
                          |     |     |
                          |     |     |
                          |     |     |
                          |     |     |
                          |     |     |
-VBlank(start)------------+-----+-----+----------
                          |     |  S  |
                          |     +-----+
                          |
                          |
-VBlank(end)--------------+-----------------------


* Conditions that generate problems
    - If the upper and lower screens are swapped even though the buffer swap operation has not completed within a VBlank,
      both screens will display the same image.

To avoid the (above) problem, you must do the following within the VBlank:
    - Confirm that the next image has finished being drawn before entering a VBlank.
      If the drawing is finished, the buffer swap operation will be mounted at the start of the next VBlank.
    - Confirm that the buffer swap operation has completed within the VBlank.
      If the swap operation is finished, switch the upper and lower screens.

    Therefore, the following 2 points must be checked:
    - The next drawing is completed before entering a VBlank. (Assuring that the swap operation will happen at the start of the VBlank)
    - The buffer swap operation completes within the VBlank.
     

[] Operations definitely being carried out in a VBlank:

        * if(GX_GetVCount() <= 193)               The swap operation doesn't even take 1 line,
                                                   so if line 193 has been reached at this point,
                                                   it can be determined that the swap operation has ended
                                                   and it is not necessary to wait 784 cycles with the OS_SpinWait function.
                                                   
        * OS_SpinWait(784)                        Waits 784 cycles.
        
        * if (!G3X_IsGeometryBusy() && swap)      If the SwapBuffers command has been run before the VBlank
                                                   and the Geometry Engine is not busy.
                                                   (Is not running a buffer swap operation or drawing graphics.)
                                                
[] Meaning of the operation
    In the buffer swap operation (swaps data referenced by the rendering engine),
    784 (392 x 2) CPU cycles are necessary,
    so if the buffer swap operation starts at the start of a VBlank,
    it can be determined that the buffer swap operation has completed after 784 cycles.

    If the geometry engine is busy after waiting 784 cycles from the beginning of a VBlank,
        it means the graphics drawing operation from before the VBlank is running slowly and the buffer swap operation has not yet completed.
        -> In this case, neither the swap operation nor a switch between the upper and lower screens should be run.
    If the geometry engine is not busy after waiting 784 cycles from the beginning of a VBlank,
        both the buffer swap operation and the graphics drawing operation are finished.
        -> Since the drawing and swap operations are both finished, you should switch the upper and lower screens.

If you absolutely think the 784 cycles in a VBlank are a waste,
add an operation that is longer than 784 cycles before OS_SpinWait(784).

Or, problems can be avoided with the following, though it cannot be used when not even one polygon is being drawn:

void VBlankIntr(void)
{
    if ( (G3X_GetVtxListRamCount() == 0) && swap )
    {
        if (flag)  // flip switch
        {
            setupFrame2N_1();
        }
        else
        {
            setupFrame2N();
        }


  
With this method, when the swap buffer operation mounts at the start of a VBlank,
it can be determined that the swap buffer operation was mounted
by checking that the upper value stored in upper RAM is 0 using the G3X_GetVtxListRamCount function.
(The function that returns the upper value stored in upper RAM.)


Caution: If you run an unusually long interrupt operation before a VBlank interrupt,
      and the VBlank starts late causing an unusually short VBlank (3 lines or fewer),
      there is a chance that the upper and lower screens will be switched or just that frame will display the previous image.

---------------------------------------------------------------------------------------------------
*/
#define SPIN_WAIT                      // Switches between the method using the OS_SpinWait function and the method not using it.

    // Image drawing also checks whether the swap operation for the buffer has completed.
#ifdef SPIN_WAIT

    if (GX_GetVCount() <= 193)
    {
        OS_SpinWait(784);
    }
    if (!G3X_IsGeometryBusy() && swap)

#else

    if ((G3X_GetVtxListRamCount() == 0) && swap)

#endif
        // If the drawing and swap operations are both finished, the upper and lower screens switch.
    {
        if (flip_flag)                 // flip switch (operation to switch the upper and lower screens)
        {
            setupFrame2N_1();
        }
        else
        {
            setupFrame2N();
        }
        swap = FALSE;
        flip_flag = !flip_flag;
    }
    OS_SetIrqCheckFlag(OS_IE_V_BLANK); // checking V-Blank interrupt
}
