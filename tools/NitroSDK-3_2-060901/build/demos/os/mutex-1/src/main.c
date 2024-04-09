/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - demos - mutex-1
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.23  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.22  11/21/2005 10:54:50  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.21  06/27/2005 11:10:12  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.20  03/03/2005 12:55:41  takano_makoto
  exclude any codes which are available in a specific compiler.

  Revision 1.19  02/28/2005 05:26:04  yosizaki
  do-indent.

  Revision 1.18  02/18/2005 05:33:47  takano_makoto
  made revisions so that OS_Init is called before GX_Init

  Revision 1.17  06/18/2004 13:08:29  terui
  Swap font data.

  Revision 1.16  06/08/2004 00:19:49  yada
  add '#pragma unused()' for FINALROM

  Revision 1.15  04/15/2004 06:51:12  yada
  only arranged comment

  Revision 1.14  03/08/2004 08:31:58  yada
  fixed top comment

  Revision 1.13  02/26/2004 09:42:45  yada
  Added argument for starting function to OS_CreateThread().
  Revisions that related to that.

  Revision 1.12  2004/02/10 05:47:46  yasu
  delete macro CODE32

  Revision 1.11  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.10  02/02/2004 05:34:12  yada
  OS_SetIrqMask() -> OS_EnableIrqMask()

  Revision 1.9  01/18/2004 07:29:23  yada
  Modified associate with changing GX function name

  Revision 1.8  01/18/2004 06:49:20  yada
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

void    VBlankIntr(void);
void    ObjSet(int objNo, int x, int y, int charNo, int paletteNo);

static GXOamAttr oamBak[128];
extern const u16 samplePlttData[16][16];
extern const u32 sampleCharData[8 * 0x100];

//---- thread
#define    STACK_SIZE     1024
#define    THREAD1_PRIO   1
#define    THREAD2_PRIO   2
OSThread thread1;
OSThread thread2;
u64     stack1[STACK_SIZE / sizeof(u64)];
u64     stack2[STACK_SIZE / sizeof(u64)];
void    proc1(void *p1);
void    proc2(void *p1);

OSMutex mutex;

void    common(u32 val);

//================================================================================
/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    //================ Initialize
    OS_Init();
    //---- GX Initialization
    GX_Init();

    //================ Create Mutex
    OS_InitMutex(&mutex);

    //================ Start thread
    OS_InitThread();
    OS_CreateThread(&thread1, proc1, NULL, stack1 + STACK_SIZE / sizeof(u64), STACK_SIZE,
                    THREAD1_PRIO);
    OS_CreateThread(&thread2, proc2, NULL, stack2 + STACK_SIZE / sizeof(u64), STACK_SIZE,
                    THREAD2_PRIO);
    OS_WakeupThreadDirect(&thread1);
    OS_WakeupThreadDirect(&thread2);

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
    GX_SetBankForLCDC(0);

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

    //================ Data load
    MI_DmaCopy32(3, sampleCharData, (void *)HW_OBJ_VRAM, sizeof(sampleCharData));
    MI_DmaCopy32(3, samplePlttData, (void *)HW_OBJ_PLTT, sizeof(samplePlttData));

    GX_DispOn();

    //================ Main loop
    while (1)
    {
        u16     keyData;

        //---- Wait for V-blank interrupt completion
        OS_WaitVBlankIntr();

        //---- Move hidden OBJ out of screen
        MI_DmaFill32(3, oamBak, 0xc0, sizeof(oamBak));

        //---- Load pad data
        keyData = PAD_Read();

        //---- Display pad information as OBJ
        ObjSet(0, 180, 95, 'A', (keyData & PAD_BUTTON_A) ? 1 : 2);
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

    //---- Reset interrupt request flag
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);

    //---- Start thread 1
    OS_WakeupThreadDirect(&thread1);
    OS_WakeupThreadDirect(&thread2);
}

//--------------------------------------------------------------------------------
//    Thread 1
//
void proc1(void *p1)
{
#pragma unused(p1)
    while (1)
    {
        common(1);
        OS_SleepThread(NULL);
    }
}

//--------------------------------------------------------------------------------
//    Thread 2
//
void proc2(void *p1)
{
#pragma unused(p1)
    while (1)
    {
        common(2);
    }
}

//--------------------------------------------------------------------------------
//    Common
//
void common(u32 val)
{
#ifdef SDK_FINALROM
#pragma unused( val )
#endif

    OS_Printf("common IN thread=%d\n", val);
    OS_LockMutex(&mutex);

    OS_Printf("<Critical routine> thread=%d\n", val);

    //---- Wait for A Button trigger
    while (!(PAD_Read() & PAD_BUTTON_A))
    {
        OS_SleepThread(NULL);
    }
    while (PAD_Read() & PAD_BUTTON_A)
    {
        OS_SleepThread(NULL);
    }

    OS_Printf("common OUT thread=%d\n", val);
    OS_UnlockMutex(&mutex);
}

/*====== End of main.c ======*/
