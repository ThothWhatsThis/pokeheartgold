/*---------------------------------------------------------------------------*
  Project:  Project:  NitroSDK - MB - demos - multiboot-wfs
  File:     common.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: common.c,v $
  Revision 1.2  11/21/2005 10:52:47  kitase_hirotake
  Deleted OS_PrintServer
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr.

  Revision 1.1  06/23/2005 09:05:57  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    "common.h"
#include    "wfs.h"
#include    "wc.h"


/*****************************************************************************/
/* constant*/

/* Game information*/
const WMParentParam def_parent_param[1] = {
    {
     NULL, 0, 0,
     GGID_WBT_FS,
     0x0000,                           // Temporary ID (incremented by 1 each time)
     1,                                // Entry permit flag
     15,                               // Maximum number of connected children
     0,                                // Multiboot flag
     0,                                // Key sharing
     1,                                // Continuous send flag
     200,                              // Beacon interval
     {0, 0, 0, 0},                     // User name
     {0, 0, 0, 0, 0, 0, 0, 0},         // Game name
     1,                                // Connect channel (Does MeasureChannel at start time)
     WC_PARENT_DATA_SIZE_MAX,          // Size of parent send data
     WC_CHILD_DATA_SIZE_MAX / 16       // Size of child send data
     }
};


/*****************************************************************************/
/* function*/

/*---------------------------------------------------------------------------*
  Name:         StateCallbackForWFS

  Description:  WFS callback function.
                This is called when the state became WFS_STATE_READY.
                WFS_GetStatus() can be used at an arbitrary position to make a determination
                without receiving notification with this callback.

  Arguments:    arg       User-defined argument specified in the callback.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void StateCallbackForWFS(void *arg)
{
    (void)arg;
    switch (WFS_GetStatus())
    {
    case WFS_STATE_READY:
        OS_TPrintf("WFS ready.\n");
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         AllocatorForWFS

  Description:  Dynamic allocation function for memory specified in WFS.

  Arguments:    arg       User-defined argument specified in the allocator.
                size      Required size if requesting memory allocation.
                ptr       If NULL, allocates memory. Otherwise, requests release.

  Returns:      If ptr is NULL, the amount of memory in size is allocated and its pointer returned.
                If not, the ptr memory is released.
                If released, the return value is simply ignored.
 *---------------------------------------------------------------------------*/
void   *AllocatorForWFS(void *arg, u32 size, void *ptr)
{
    (void)arg;
    if (!ptr)
        return OS_Alloc(size);
    else
    {
        OS_Free(ptr);
        return NULL;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WCCallbackForWFS

  Description:  Callback that receives status notification from the WC's wireless automatic drive.

  Arguments:    arg     - Callback pointer for the notification source WM function.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCCallbackForWFS(WMCallback *arg)
{
    WCStatus wcStatus = WcGetStatus();
    switch (arg->apiid)
    {
    case WM_APIID_START_MP:
        {                              /* begin MP state*/
            WMStartMPCallback *cb = (WMStartMPCallback *)arg;
            switch (cb->state)
            {
            case WM_STATECODE_MP_START:
                /*
                 * In order to notify of a transition to MP_PARENT state or MP_CHILD state,
                 * call WFS_Start().
                 * By taking this notification as a turning point, in order to transfer blocks, WFS
                 * internally begins calling the  WM_SetMPDataToPort() function.
                 */
                WFS_Start();
                break;
            }
        }
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         InitFrameLoop

  Description:  Initialization for game frame loop.

  Arguments:    p_key           Key information structure to initialize.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void InitFrameLoop(KeyInfo * p_key)
{
    ClearLine();
    ClearString();
    KeyRead(p_key);
}

/*---------------------------------------------------------------------------*
  Name:         WaitForNextFrame

  Description:  Wait until next drawing frame.

  Arguments:    p_key           Key information structure to update.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WaitForNextFrame(KeyInfo * p_key)
{
    FlushLine();
    OS_WaitVBlankIntr();
    ClearString();
    KeyRead(p_key);
}

/*---------------------------------------------------------------------------*
  Name:         ModeError

  Description:  Processing in error display screen

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ModeError(void)
{
    KeyInfo key[1];

    InitFrameLoop(key);
    while (WcGetStatus() == MODE_ERROR)
    {
        WaitForNextFrame(key);
        PrintString(5, 10, COLOR_RED, "======= ERROR! =======");
        PrintString(5, 13, COLOR_WHITE, " Fatal error occured.");
        PrintString(5, 14, COLOR_WHITE, "Please reboot program.");
    }
}

/*---------------------------------------------------------------------------*
  Name:         ModeWorking

  Description:  Processing in busy screen

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ModeWorking(void)
{
    KeyInfo key[1];

    InitFrameLoop(key);
    while (WcGetStatus() == MODE_BUSY)
    {
        WaitForNextFrame(key);
        PrintString(9, 11, COLOR_WHITE, "Now working...");
        if (key->trg & PAD_BUTTON_START)
        {
            WcEnd();
        }
    }
}


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
