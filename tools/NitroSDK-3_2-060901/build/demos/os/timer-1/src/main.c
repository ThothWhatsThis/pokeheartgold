/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - demos - timer-1
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.18  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.17  11/21/2005 10:56:23  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.16  06/27/2005 11:10:12  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.15  02/28/2005 05:26:04  yosizaki
  do-indent.

  Revision 1.14  04/15/2004 06:51:12  yada
  only arranged comment

  Revision 1.13  03/30/2004 08:38:22  yada
  Minor Revisions

  Revision 1.12  03/08/2004 08:31:58  yada
  fixed top comment

  Revision 1.11  2004/02/10 05:48:12  yasu
  delete macro CODE32

  Revision 1.10  02/05/2004 10:50:13  yada
  Made timer reset process to be performed

  Revision 1.9  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.8  02/02/2004 05:34:12  yada
  OS_SetIrqMask() -> OS_EnableIrqMask()

  Revision 1.7  01/29/2004 08:07:40  yada
  Added GX_DispOn()

  Revision 1.6  01/18/2004 07:29:23  yada
  Modified associate with changing GX function name

  Revision 1.5  01/18/2004 06:47:24  yada
  added OS_Init

  Revision 1.4  01/14/2004 11:25:47  nishida_kenji
  add G2_OBJMosaic,
  remove GXOamMosaic

  Revision 1.3  01/09/2004 10:53:49  yada
  rewrote set function for interrupt check flag

  Revision 1.2  01/08/2004 12:02:24  yada
  flushed the cache before OAM data transfer

  Revision 1.1  01/08/2004 06:32:45  yada
  Initial release.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include "data.h"

static GXOamAttr oamBak[128];

void    VBlankIntr(void);
void    ObjSet(int objNo, int x, int y, int charNo, int paletteNo);
void    TimerSet(void);
void    TimerIntr(void);

static volatile u32 timerCnt = 0;

//---- Return 0-9, A-F code for OBJ display
inline int ObjChar(u32 cnt, int shift)
{
    u32     d = (cnt >> shift) & 0xf;
    return (int)((d < 10) ? '0' + d : 'A' + d - 10);
}

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
    GX_SetBankForOBJ(GX_VRAM_OBJ_128_A);        // Assign bank A onto OBJ

    //---- Set to graphics display mode
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);

    //---- Set only OBJ display ON
    GX_SetVisiblePlane(GX_PLANEMASK_OBJ);

    //---- Used with 32K byte OBJ in 2D mapping mode
    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);


    //---- Data load
    MI_DmaCopy32(3, sampleCharData, (void *)HW_OBJ_VRAM, sizeof(sampleCharData));
    MI_DmaCopy32(3, samplePlttData, (void *)HW_OBJ_PLTT, sizeof(samplePlttData));

    //---- Move hidden OBJ out of screen
    MI_DmaFill32(3, oamBak, 0xc0, sizeof(oamBak));

    //---- Started display
    OS_WaitVBlankIntr();
    GX_DispOn();

    //================ Main loop
    //---- Timer interrupt setting
    TimerSet();

    while (1)
    {
        u16     keyData;

        //---- Wait for V-blank interrupt completion
        OS_WaitVBlankIntr();

        //---- Load pad data
        keyData = PAD_Read();

        //---- Counter value display
        ObjSet(0, 50, 100, ObjChar(timerCnt, 28), 2);
        ObjSet(1, 60, 100, ObjChar(timerCnt, 24), 2);
        ObjSet(2, 70, 100, ObjChar(timerCnt, 20), 2);
        ObjSet(3, 80, 100, ObjChar(timerCnt, 16), 2);
        ObjSet(4, 90, 100, ObjChar(timerCnt, 12), 2);
        ObjSet(5, 100, 100, ObjChar(timerCnt, 8), 2);
        ObjSet(6, 110, 100, ObjChar(timerCnt, 4), 2);
        ObjSet(7, 120, 100, ObjChar(timerCnt, 0), 2);

        //---- Enable/disable timer interrupt by button
        if (keyData & PAD_BUTTON_A)
        {
            (void)OS_EnableIrqMask(OS_IE_TIMER0);
        }
        if (keyData & PAD_BUTTON_B)
        {
            (void)OS_DisableIrqMask(OS_IE_TIMER0);
        }
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
    //---- OAM updating
    DC_FlushRange(oamBak, sizeof(oamBak));
    /* IO register is accessed using DMA operation, so cache Wait is not needed*/
    // DC_WaitWriteBufferEmpty();
    MI_DmaCopy32(3, oamBak, (void *)HW_OAM, sizeof(oamBak));

    //---- Interrupt check flag
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

//--------------------------------------------------------------------------------
//              Timer setting
//
void TimerSet(void)
{
    //---- Set interrupt routine for Timer0
    OS_SetIrqFunction(OS_IE_TIMER0, TimerIntr);

    //---- Enable interrupt for Timer0
    (void)OS_EnableIrqMask(OS_IE_TIMER0);

    //---- Start Timer0
    OS_StartTimer(OS_TIMER_0, 1000, OS_TIMER_PRESCALER_1024);
}

//--------------------------------------------------------------------------------
//              Timer interrupt processing
//
void TimerIntr(void)
{
    timerCnt = timerCnt + 1;

    //---- Interrupt check flag
    OS_SetIrqCheckFlag(OS_IE_TIMER0);

    //---- Disable Timer0 once
    OS_StopTimer(OS_TIMER_0);

    //---- Reset Timer
    TimerSet();
}

/*====== End of main.c ======*/
