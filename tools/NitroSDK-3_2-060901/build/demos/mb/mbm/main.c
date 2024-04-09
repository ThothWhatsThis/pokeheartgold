/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - demos - multiboot
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.8  01/18/2006 02:11:28  kitase_hirotake
  do-indent

  Revision 1.7  12/26/2005 00:24:14  kitase_hirotake
  Deleted PXI_InitFifo

  Revision 1.6  11/21/2005 10:50:34  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.5  02/28/2005 05:26:19  yosizaki
  do-indent.

  Revision 1.4  11/24/2004 06:15:01  yosizaki
  rename MB_StartParentEx/MB_EndEx.

  Revision 1.3  10/21/2004 06:28:00  takano_makoto
  comments added

  Revision 1.2  09/28/2004 09:58:37  takano_makoto
  Revised the channel_setting to volatile

  Revision 1.1  09/28/2004 06:50:04  takano_makoto
  initial update.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//--------------------------------------------------------------------
// Channel determination sample when multibooting    
// 
// Since the measurement of the radio usage rate is only used in the wireless IDLE state,    
// the channel must be determined as a preprocess before starting MB when    
// be called every frame.
//
// After this sample's MBM_MeasureChannel() is called when the wireless power is in an OFF state,    
// it will return the results with a callback after internally looking for an optimal channel    
// and returning to the wireless power OFF state.    
// 
// (Note: There are also ways of using MB_StartParentFromIdle and    
//    MB_EndToIdle added in NITRO-SDK2.0rc2)    
//--------------------------------------------------------------------


#include <nitro.h>

#include "mb_measure_channel.h"


static void my_callback(MBMCallback * cb);
static vu16 channel_setting = 0;
static int prog_state;


/******************************************************************************/
/* function*/

/* V-blank interrupt process*/
static void VBlankIntr(void)
{
    //---- Interrupt check flag
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/* Main*/
void NitroMain()
{
    /* OS  initialization*/
    OS_Init();
    OS_InitThread();
    OS_InitTick();
    OS_InitAlarm();
    FX_Init();

    /* GX  initialization*/
    GX_Init();
    GX_DispOff();
    GXS_DispOff();

    {                                  /* memory allocation initialization*/
        OSHeapHandle heapHandle;       // Heap handle
        enum
        { MAIN_HEAP_SIZE = 0x80000 };
        void   *heapStart, *nstart;
        nstart = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 16);
        OS_SetMainArenaLo(nstart);
        heapStart = OS_AllocFromMainArenaLo((u32)MAIN_HEAP_SIZE, 32);
        heapHandle =
            OS_CreateHeap(OS_ARENA_MAIN, heapStart, (void *)((u32)heapStart + MAIN_HEAP_SIZE));
        if (heapHandle < 0)
        {
            OS_Panic("ARM9: Fail to create heap...\n");
        }
        heapHandle = OS_SetCurrentHeap(OS_ARENA_MAIN, heapHandle);
    }

    /* V-blank interrupt configuration*/
    (void)OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrqMask(OS_IE_FIFO_RECV);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
    (void)GX_VBlankIntr(TRUE);

    /* Starts display*/
    GX_DispOn();
    GXS_DispOn();
    G2_SetBG0Offset(0, 0);

    channel_setting = 0;

    {
        void   *wm_sys_buf_ptr = OS_Alloc(WM_SYSTEM_BUF_SIZE);
        MBM_MeasureChannel((u8 *)wm_sys_buf_ptr, my_callback);
        while (channel_setting == 0)
        {
            OS_WaitVBlankIntr();
        }
        OS_Free(wm_sys_buf_ptr);
    }

    /* Main loop*/
    for (;;)
    {
        OS_WaitVBlankIntr();
    }
}


static void my_callback(MBMCallback * cb)
{
    channel_setting = 1;
    if (cb->errcode == MBM_MEASURE_SUCCESS)
    {
        OS_TPrintf("measure channel = %d\n", cb->channel);
    }
    else
    {
        OS_TPrintf("get measure channel fail\n");
    }
}
