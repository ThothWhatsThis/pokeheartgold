/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - demos - sleep-1
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.11  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.10  11/21/2005 10:55:34  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.9  06/27/2005 11:10:12  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.8  02/28/2005 05:26:04  yosizaki
  do-indent.

  Revision 1.7  09/30/2004 09:32:35  yada
  exchange OS_InitAlarm() and OS_InitTick()

  Revision 1.6  09/30/2004 05:08:02  yada
  add OS_InitAlarm().  to remove it was mistake

  Revision 1.5  09/28/2004 12:25:26  yada
  not call OS_InitAlarm() because OS_Init() called

  Revision 1.4  04/15/2004 06:51:12  yada
  only arranged comment

  Revision 1.3  03/30/2004 08:38:22  yada
  Minor Revisions

  Revision 1.2  03/08/2004 08:31:58  yada
  fixed top comment

  Revision 1.1  03/03/2004 09:19:42  yada
  firstRelease

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include "data.h"

static GXOamAttr oamBak[128];

void    VBlankIntr(void);
void    ObjSet(int objNo, int x, int y, int charNo, int paletteNo);


//---- Return 0-9, A-F code for OBJ display
inline int ObjChar(u32 cnt, int shift)
{
    u32     d = (cnt >> shift) & 0xf;
    return (int)((d < 10) ? '0' + d : 'A' + d - 10);
}


//---- Definitions, declarations for sub threads
#define STACK_SIZE  1024
#define THREAD_PRIO 20

OSThread thread;
void    proc(void *arg);

u64     stack[STACK_SIZE / sizeof(u64)];


//---- Display counter
static u32 count1;                     // For launch threads
static u32 count2;                     // For sub threads

//================================================================================
//   Counter for launch threads is incremented and displayed.
//   If you press the A button, this thread will sleep for 1 second.
//   During that time a sub thread will start.
//   Normally a sub thread has a low priority, so it will not run.
/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void NitroMain()
{
    //================ Initialize
    OS_Init();

    OS_InitTick();
    OS_InitAlarm();
    OS_InitThread();

    GX_Init();

    //---- Start sub thread
    OS_CreateThread(&thread, proc, NULL, stack + STACK_SIZE / sizeof(u64), STACK_SIZE, THREAD_PRIO);
    OS_WakeupThreadDirect(&thread);


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
    MI_DmaCopy32(3, sampleCharData, (void *)HW_OBJ_VRAM, sizeof(sampleCharData));
    MI_DmaCopy32(3, samplePlttData, (void *)HW_OBJ_PLTT, sizeof(samplePlttData));

    //---- Move hidden OBJ out of screen
    MI_DmaFill32(3, oamBak, 0xc0, sizeof(oamBak));

    GX_DispOn();

    //================ Main loop
    while (1)
    {
        u16     keyData;

        //---- Wait for V-blank interrupt completion
        OS_WaitVBlankIntr();

        //---- Load pad data
        keyData = PAD_Read();

        //---- Increase count
        count1++;

        //---- Display count
        ObjSet(0, 50, 100, ObjChar(count1, 28), 2);
        ObjSet(1, 60, 100, ObjChar(count1, 24), 2);
        ObjSet(2, 70, 100, ObjChar(count1, 20), 2);
        ObjSet(3, 80, 100, ObjChar(count1, 16), 2);
        ObjSet(4, 90, 100, ObjChar(count1, 12), 2);
        ObjSet(5, 100, 100, ObjChar(count1, 8), 2);
        ObjSet(6, 110, 100, ObjChar(count1, 4), 2);
        ObjSet(7, 120, 100, ObjChar(count1, 0), 2);

        ObjSet(8, 140, 100, ObjChar(count2, 28), 2);
        ObjSet(9, 150, 100, ObjChar(count2, 24), 2);
        ObjSet(10, 160, 100, ObjChar(count2, 20), 2);
        ObjSet(11, 170, 100, ObjChar(count2, 16), 2);
        ObjSet(12, 180, 100, ObjChar(count2, 12), 2);
        ObjSet(13, 190, 100, ObjChar(count2, 8), 2);
        ObjSet(14, 200, 100, ObjChar(count2, 4), 2);
        ObjSet(15, 210, 100, ObjChar(count2, 0), 2);


        //---- Sleep one second if the A button is pressed
        if (keyData & PAD_BUTTON_A)
        {
            OS_Sleep(1000);
        }
    }
}

//--------------------------------------------------------------------------------
//  set OBJ
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
//    
//
void proc(void *arg)
{
#pragma unused( arg )

    while (1)
    {
        //---- Wait for V-blank interrupt completion
        OS_WaitVBlankIntr();

        //---- Increase count
        count2++;
    }
}

/*====== End of main.c ======*/
