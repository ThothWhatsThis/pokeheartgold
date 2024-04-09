/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - demos - overlay
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.14  01/18/2006 02:11:21  kitase_hirotake
  do-indent

  Revision 1.13  11/21/2005 10:23:28  kitase_hirotake
  Deleted OS_InitPrintServer and OS_PrintServer.
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.12  11/17/2005 01:27:55  adachi_hiroaki
  Made MI_DMA_NUM+1 into FS_DMA_NOT_USE when setting DMA numbers

  Revision 1.11  02/28/2005 05:26:13  yosizaki
  do-indent.

  Revision 1.10  11/17/2004 05:03:05  yosizaki
  change to use FS_LoadOverlayImageAsync.

  Revision 1.9  10/18/2004 13:30:52  yosizaki
  add sample code to use overlay in different methods.

  Revision 1.8  2004/08/23 07:27:27  yasu
  Workaround for pushing A Button in IPL

  Revision 1.7  04/22/2004 03:12:35  yosizaki
  fix around OS_InitAlloc().

  Revision 1.6  04/22/2004 02:30:27  yosizaki
  add FS_LoadTable().

  Revision 1.5  04/19/2004 06:50:40  yosizaki
  fix around TEG-CARD.

  Revision 1.4  04/13/2004 12:08:02  yosizaki
  add ovaerlay to TCM.
  fix header comment.

  Revision 1.3  04/06/2004 07:00:35  yosizaki
  change FS_Init()
  change around FSOverlayID

  Revision 1.2  04/05/2004 02:49:17  yosizaki
  change argtype of FSLoadOverlay()

  Revision 1.1  04/02/2004 12:21:12  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitro/fs.h>
#include "func.h"


/* use DMA-3 for FS*/
static const u32 default_dma_no = MI_DMA_MAX_NUM;

/* overlay id in *.lcf*/
FS_EXTERN_OVERLAY(main_overlay_1);
FS_EXTERN_OVERLAY(main_overlay_2);
FS_EXTERN_OVERLAY(main_overlay_3);

/* overlay table*/
static FSOverlayID p_id[] = {
    FS_OVERLAY_ID(main_overlay_1),
    FS_OVERLAY_ID(main_overlay_2),
    FS_OVERLAY_ID(main_overlay_3),
};

/* current overlay id*/
static u32 overlay_id;
static FSOverlayID last_loaded_overlay;

static void offset_overlay_id(int i)
{
    const int overlay_max = (int)(sizeof(p_id) / sizeof(*p_id) - 1);
    i += (s32)overlay_id;
    if (i < 0)
        i = overlay_max;
    else if (i > overlay_max)
        i = 0;
    overlay_id = (u32)i;
    OS_Printf("current id = 0x%08X.\n", overlay_id);
}

/* load overlay in various method*/
static BOOL load_overlay_high(MIProcessor target, FSOverlayID id)
{
    return FS_LoadOverlay(target, id);
}

static BOOL load_overlay_middle(MIProcessor target, FSOverlayID id)
{
    FSOverlayInfo info;
    if (!FS_LoadOverlayInfo(&info, target, id))
        return FALSE;
    if (!FS_LoadOverlayImage(&info))
        return FALSE;
    FS_StartOverlay(&info);
    return TRUE;
}

static BOOL load_overlay_low(MIProcessor target, FSOverlayID id)
{
    FSOverlayInfo info;
    if (!FS_LoadOverlayInfo(&info, target, id))
        return FALSE;
    {
        FSFile  file;
        FS_InitFile(&file);
        (void)FS_LoadOverlayImageAsync(&info, &file);
        (void)FS_WaitAsync(&file);
        (void)FS_CloseFile(&file);
    }
    FS_StartOverlay(&info);
    return TRUE;
}

#define	load_overlay	load_overlay_low


/* unload module[last_loaded_overlay] and load module[overlay_id]*/
static void load_func(void)
{
    static BOOL is_loaded = FALSE;

    BOOL    ret;
    if (is_loaded)
    {
        /* unload old module*/
        ret = FS_UnloadOverlay(MI_PROCESSOR_ARM9, last_loaded_overlay);
        OS_Printf("overlay 0x%08X is unloaded.\n", last_loaded_overlay);
        SDK_ASSERT(ret);
        is_loaded = FALSE;
    }
    /* load new module.
       main_overlay_3 is in ITCM (0x01FF8000), so use CpuCopy().*/
    if (p_id[overlay_id] == FS_OVERLAY_ID(main_overlay_3))
        (void)FS_SetDefaultDMA(FS_DMA_NOT_USE);
    else
        (void)FS_SetDefaultDMA(default_dma_no);

    ret = load_overlay(MI_PROCESSOR_ARM9, p_id[overlay_id]);
    if (!ret)
    {
        OS_Printf("overlay 0x%08X is not available!\n", p_id[overlay_id]);
    }
    else
    {
        is_loaded = TRUE;
        last_loaded_overlay = p_id[overlay_id];
        OS_Printf("overlay 0x%08X is loaded.\n", p_id[overlay_id]);
    }
}

/* V-blank callback*/
static void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         InitializeAllocateSystem

  Description:  Initializes the memory allocation system within the main memory arena.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitializeAllocateSystem(void)
{
    void   *tempLo;
    OSHeapHandle hh;

    // Based on the premise that OS_Init is already called
    tempLo = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
    OS_SetArenaLo(OS_ARENA_MAIN, tempLo);
    hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
    if (hh < 0)
    {
        OS_Panic("ARM9: Fail to create heap...\n");
    }
    hh = OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
}


void NitroMain(void)
{
    /* initialize application*/
    OS_Init();
    InitializeAllocateSystem();

    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrqMask(OS_IE_FIFO_RECV);
    (void)OS_EnableIrq();

    /* Initialize file-system*/
    FS_Init(default_dma_no);

    /* always preload FS table for faster directory access.*/
    {
        u32     need_size = FS_GetTableSize();
        void   *p_table = OS_Alloc(need_size);
        SDK_ASSERT(p_table != NULL);
        (void)FS_LoadTable(p_table, need_size);
    }


    (void)GX_VBlankIntr(TRUE);

    OS_Printf("\n"
              "++++++++++++++++++++++++++++++++++++++++\n" "test : load overlay modules ... \n\n");

    {
        /* initialize default to func_1*/
        overlay_id = 0;
        load_func();

        /* address of function is specified in main.lsf*/
        OS_Printf("func_1() : addr = 0x%08X.\n", func_1);
        OS_Printf("func_2() : addr = 0x%08X.\n", func_2);
        OS_Printf("func_3() : addr = 0x%08X.\n", func_3);

        /* select and load*/
        {
            u32     btn = PAD_Read();

            for (;;)
            {

                /* wait key down*/
                u32     btn_bak, btn_down;
                btn_bak = btn;
                OS_WaitVBlankIntr();
                btn = PAD_Read();
                btn_down = (u32)(~btn_bak & btn);

                /* if A, call current module*/
                if (btn_down & PAD_BUTTON_A)
                {
                    /* if two overlays share the region,
                       FS-system can not guarantee except last-loaded one.
                       please be careful to use OVERLAYed function! */
                    if (last_loaded_overlay == p_id[0])
                        func_1();
                    else if (last_loaded_overlay == p_id[1])
                        func_2();
                    else if (last_loaded_overlay == p_id[2])
                        func_3();
                }

                /* if B, load current id*/
                else if (btn_down & PAD_BUTTON_B)
                {
                    load_func();
                }

                /* if DOWN, increment id*/
                else if (btn_down & PAD_KEY_DOWN)
                {
                    offset_overlay_id(+1);
                }

                /* if UP, decrement id*/
                else if (btn_down & PAD_KEY_UP)
                {
                    offset_overlay_id(-1);
                }

            }
        }
    }

    OS_Printf("\n" "++++++++++++++++++++++++++++++++++++++++\n" "end\n\n");

}
