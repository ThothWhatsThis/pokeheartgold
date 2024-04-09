/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI - demos - dmaCallback-2
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.13  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.12  11/21/2005 10:53:18  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.11  06/27/2005 11:10:12  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.10  02/28/2005 05:26:04  yosizaki
  do-indent.

  Revision 1.9  04/15/2004 06:51:12  yada
  only arranged comment

  Revision 1.8  03/08/2004 09:19:14  yada
  fixed top comment

  Revision 1.7  2004/02/24 05:25:21  yasu
  used OS_EnableIrqMask instead of OS_SetIrqMask

  Revision 1.6  2004/02/10 05:47:46  yasu
  delete macro CODE32

  Revision 1.5  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.4  01/29/2004 08:07:40  yada
  Added GX_DispOn()

  Revision 1.3  01/19/2004 01:26:59  yada
  Removed "_"  to change the asynchronous function names from xxx_Async to xxxAsync

  Revision 1.2  01/18/2004 07:33:00  yada
  Modified associate with changing GX function name

  Revision 1.1  01/18/2004 04:23:58  yada
  first release


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include "data.h"

static GXOamAttr oamBak[128];

void    ObjSet(int objNo, int x, int y, int charNo, int paletteNo);
void    VBlankIntr(void);

//---- DMA Callback
void    myDmaCallback(void *arg);

//---- Counter
static volatile u32 dmaCnt = 0;

//---- Return 0-9, A-F codes in lower 4 bits after shift (right shift) cnt for OBJ display.
inline int ObjChar(u32 cnt, int shift)
{
    u32     d = (cnt >> shift) & 0xf;
    return (int)((d < 10) ? '0' + d : 'A' + d - 10);
}

//---- DMA Number
#define DMA_NO  3

//================================================================================
/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void NitroMain()
{
    //================ Initialize
    //---- OS Initialization
    OS_Init();

    //---- GX Initialization
    GX_Init();

    //================ Settings
    //---- All Power ON
    GX_SetPower(GX_POWER_ALL);

    //----  Enable Vblank interrupt
    (void)OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();

    //---- Occurrence of Vblank setting
    (void)GX_VBlankIntr(TRUE);

    //---- VRAM Clear
    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
    (void)GX_DisableBankForLCDC();

    //---- OAM and palette clear
    MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);
    MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);


    //---- Set bankA for OBJ
    GX_SetBankForOBJ(GX_VRAM_OBJ_128_A);

    //---- Set to graphics display mode
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);

    //---- Set only OBJ display ON
    GX_SetVisiblePlane(GX_PLANEMASK_OBJ);

    //---- Used with 32K byte OBJ in 2D mapping mode
    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);

    //---- Data load
    MI_DmaCopy32(DMA_NO, sampleCharData, (void *)HW_OBJ_VRAM, sizeof(sampleCharData));
    MI_DmaCopy32(DMA_NO, samplePlttData, (void *)HW_OBJ_PLTT, sizeof(samplePlttData));

    //---- Move hidden OBJ out of screen
    MI_DmaFill32(DMA_NO, oamBak, 0xc0, sizeof(oamBak));

    //---- Started display
    OS_WaitVBlankIntr();
    GX_DispOn();

    //================ Main loop
    while (1)
    {
        //---- Wait for V-blank interrupt completion
        OS_WaitVBlankIntr();

        //---- Counter value display
        ObjSet(0, 50, 100, ObjChar(dmaCnt, 28), 2);
        ObjSet(1, 60, 100, ObjChar(dmaCnt, 24), 2);
        ObjSet(2, 70, 100, ObjChar(dmaCnt, 20), 2);
        ObjSet(3, 80, 100, ObjChar(dmaCnt, 16), 2);
        ObjSet(4, 90, 100, ObjChar(dmaCnt, 12), 2);
        ObjSet(5, 100, 100, ObjChar(dmaCnt, 8), 2);
        ObjSet(6, 110, 100, ObjChar(dmaCnt, 4), 2);
        ObjSet(7, 120, 100, ObjChar(dmaCnt, 0), 2);

        //---- OAM updating
        DC_FlushRange(oamBak, sizeof(oamBak));
        /* IO register is accessed using DMA operation, so cache Wait is not needed*/
        // DC_WaitWriteBufferEmpty();

        //---- Vblank interrupt DMA transfer, Call callback Call callback
        MI_VBlankDmaCopy32Async(DMA_NO, oamBak, (void *)HW_OAM, sizeof(oamBak), myDmaCallback,
                                (void *)0x10);
    }
}

//--------------------------------------------------------------------------------
//      set OBJ
//
void ObjSet(int objNo, int x, int y, int charNo, int paletteNo)
{
    G2_SetOBJAttr((GXOamAttr *)&oamBak[objNo],
                  x,
                  y,
                  0,
                  GX_OAM_MODE_NORMAL,
                  FALSE,
                  GX_OAM_EFFECT_NONE, GX_OAM_SHAPE_8x8, GX_OAM_COLOR_16, charNo, paletteNo, 0);
}

//--------------------------------------------------------------------------------
//    V-blank interrupt process
//
void VBlankIntr(void)
{
    //---- Interrupt check flag
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}


//--------------------------------------------------------------------------------
//    DMA callback
//
void myDmaCallback(void *arg)
{
    dmaCnt = dmaCnt + (u32)arg;
}

/*====== End of main.c ======*/
