/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - demos - multiboot-PowerSave
  File:     main.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.3  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.2  11/21/2005 10:52:07  kitase_hirotake
  Deleted OS_InitPrintServer

  Revision 1.1  04/28/2005 07:15:24  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro.h>


#include "common.h"
#include "dispfunc.h"

/*
 * A sample of the parent in Single-Card Play that clearly changes the settings for power save mode.
 *
 * Because the MB library samples use the wireless functionality,
 * development tools of the same communications environment (wired or wireless) are required.
 * The mb_child.bin program in the * $NitroSDK/bin/ARM9-TS/Rom/ directory
 * is a sample that provides the same features as the Single-Card Play child
 * of the DS system.
 * Load this binary on the other tools as you would as demo program,
 * and run them together.
 */


/*****************************************************************************/
/* function*/

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  V-blank interrupt handler to update images

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    BgUpdate();
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  Application's main entry

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    /* OS  initialization*/
    OS_Init();
    OS_InitTick();
    OS_InitAlarm();
    FX_Init();

    /* GX  initialization*/
    GX_Init();
    GX_DispOff();
    GXS_DispOff();

    /* memory allocation initialization*/
    {
        OSHeapHandle hh;
        static const u32 MAIN_HEAP_SIZE = 0x80000;
        void   *heapStart;
        void   *nstart = OS_InitAlloc(OS_ARENA_MAIN,
                                      OS_GetMainArenaLo(), OS_GetMainArenaHi(), 16);
        OS_SetMainArenaLo(nstart);
        heapStart = OS_AllocFromMainArenaLo(MAIN_HEAP_SIZE, 32);
        hh = OS_CreateHeap(OS_ARENA_MAIN, heapStart, (void *)((u32)heapStart + MAIN_HEAP_SIZE));
        if (hh < 0)
        {
            OS_Panic("ARM9: Fail to create heap...\n");
        }
        (void)OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
    }

    /* FS initialization*/
    FS_Init(FS_DMA_NOT_USE);
    {
        u32     need_size = FS_GetTableSize();
        void   *p_table = OS_Alloc(need_size);
        SDK_ASSERT(p_table != NULL);
        (void)FS_LoadTable(p_table, need_size);
    }

    /* Initialize drawing setting*/
    {
        GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
        MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
        (void)GX_DisableBankForLCDC();
        MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);
        MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);
        MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);
        MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);
        BgInit();
    }

    /* V-blank interrupt configuration*/
    (void)OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrqMask(OS_IE_FIFO_RECV);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
    (void)GX_VBlankIntr(TRUE);

    /* Main loop*/
    for (;;)
    {
        MenuMode();
        ParentMode();
    }

}
