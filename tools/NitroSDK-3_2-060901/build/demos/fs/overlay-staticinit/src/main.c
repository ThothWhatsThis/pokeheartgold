/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - demos - overlay-staticinit
  File:     main.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.4  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.3  11/21/2005 10:23:49  kitase_hirotake
  Deleted OS_InitPrintServer

  Revision 1.2  02/28/2005 05:26:03  yosizaki
  do-indent.

  Revision 1.1  01/19/2005 07:44:45  yosizaki
  initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro.h>

/* libraries being expressly used*/
#include <nitro/os.h>
#include <nitro/fs.h>
#include <nitro/pad.h>
#include <nitro/spi.h>


/* routine for screen rendering*/
#include "draw.h"

/* interface for each mode*/
#include "mode.h"


/*****************************************************************************/
/* constant*/

/* constants for automatic sampling*/
#define     SAMPLING_FREQUENCE      4  // Touch panel sampling frequence per frame
#define     SAMPLING_BUFSIZE        (SAMPLING_FREQUENCE + 1)    // AutoSampling buffer size
#define     SAMPLING_START_VCOUNT   0  // base v-count value in auto sampling.


/*****************************************************************************/
/* variable*/

/* interface for each mode*/
BOOL    (*UpdateFrame) (int frame_count, const InputData * input, int player_count,
                        int own_player_id);
void    (*DrawFrame) (int frame);
void    (*EndFrame) (FSOverlayID *p_next_mode);

/* automatic sampling buffer for the touch panel*/
static TPData tp_auto_buf[SAMPLING_BUFSIZE];


/*****************************************************************************/
/* function*/

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  Default V-blank processing

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         InitApp

  Description:  Initialization for key areas of application.    

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitApp(void)
{
    /* OS initialization*/
    OS_Init();
    OS_InitThread();
    OS_InitTick();
    /* arena initialization*/
    {
        OSHeapHandle hh;
        void   *tmp = OS_InitAlloc(OS_ARENA_MAIN,
                                   OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
        OS_SetArenaLo(OS_ARENA_MAIN, tmp);
        hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
        if (hh < 0)
            OS_Panic("ARM9: Fail to create heap...\n");
        hh = OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
    }
    /* Enable interrupts*/
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    /* initialize the touch panel*/
    {
        TPCalibrateParam calibrate;
        TP_Init();
        if (TP_GetUserInfo(&calibrate))
            TP_SetCalibrateParam(&calibrate);
        (void)TP_RequestAutoSamplingStart(SAMPLING_START_VCOUNT, SAMPLING_FREQUENCE, tp_auto_buf,
                                          SAMPLING_BUFSIZE);
    }
    /* initialize the file system*/
    FS_Init(FS_DMA_NOT_USE);
}

/*---------------------------------------------------------------------------*
  Name:         GetInput

  Description:  Gets the current input state.

  Arguments:    input             Location that stores the info obtained.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void GetInput(InputData * input)
{
    if (input)
    {
        const u16 hold_bak = input->hold_bits;
        u16     hold_bits;
        /* update the touch panel information*/
        const TPData *const cur_tp = tp_auto_buf + TP_GetLatestIndexInAuto();
        if (cur_tp->touch == TP_TOUCH_OFF)
            input->tp.touch = TP_TOUCH_OFF;
        else if (cur_tp->validity == TP_VALIDITY_VALID)
            TP_GetCalibratedPoint(&input->tp, cur_tp);
        /* update the pad information (and the stylus touch bit)*/
        hold_bits = (u16)(PAD_Read() | (input->tp.touch ? PAD_BUTTON_TP : 0));
        input->push_bits = (u16)(~hold_bak & hold_bits);
        input->hold_bits = hold_bits;
        input->release_bits = (u16)(hold_bak & ~hold_bits);
    }
}

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  Main entry point to application.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    /* shared variables for screen transitions*/
    enum
    { PLAYER_MAX = 1 };                /* max number of players (considering the communications match-up, etc.)*/
    int     player_count;              /* total number of players*/
    int     own_player_id;             /* own player number*/
    InputData input[PLAYER_MAX];       /* input information array*/
    FSOverlayID cur_mode;              /* previous mode*/
    FSOverlayID prev_mode;             /* current mode*/
    FS_EXTERN_OVERLAY(top_menu);       /* initial mode (top menu)*/

    /*
     * Initialize application framework and screen transition state.
     */
    {
        InitApp();
        InitDraw();
        SetClearColor(RGB555_CLEAR);
        cur_mode = prev_mode = FS_OVERLAY_ID(top_menu);
        UpdateFrame = NULL;
        DrawFrame = NULL;
        EndFrame = NULL;
        MI_CpuClear8(input, sizeof(input));
        player_count = 1;
        own_player_id = 0;
    }

    /*
     * Create modes for overlay and loop through them.    
     */
    for (;;)
    {

        /*
         * Start current mode.
         * Complete load and initialize with NitroStaticInit().    
         * Required interfaces set themselves.    
         * Mode termination conditions are returned by current mode with UpdateFrame.
         */
        {
            int     frame = 0;
            if (!FS_LoadOverlay(MI_PROCESSOR_ARM9, cur_mode))
            {
                OS_TPanic("failed to load specified mode(%08X)", cur_mode);
            }
            GetInput(&input[own_player_id]);
            for (;;)
            {
                GetInput(&input[own_player_id]);
                if (!UpdateFrame(frame, input, player_count, own_player_id))
                    break;
                DrawBegin();
                DrawFrame(frame);
                DrawEnd();
                ++frame;
            }
        }

        /*
         * Terminates current mode and moves to next mode.    
         * If current mode did not explicitly set next mode,    
         * return to previous mode.    
         */
        {
            FSOverlayID next_mode = prev_mode;
            EndFrame(&next_mode);
            if (!FS_UnloadOverlay(MI_PROCESSOR_ARM9, cur_mode))
            {
                OS_TPanic("failed to unload specified mode(%08X)", cur_mode);
            }
            prev_mode = cur_mode;
            cur_mode = next_mode;
        }

    }

}
