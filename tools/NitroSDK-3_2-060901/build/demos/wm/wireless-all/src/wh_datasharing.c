/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - wireless-all
  File:     wh_datasharing.c

  Copyright 2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wh_datasharing.c,v $
  Revision 1.2  04/10/2006 13:37:12  yosizaki
  beautify source code indent.

  Revision 1.1  04/10/2006 13:07:32  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include "common.h"


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
static void StateCallbackForWFS(void *arg)
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
static void *AllocatorForWFS(void *arg, u32 size, void *ptr)
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
  Name:         JudgeConnectableChild

  Description:  Function that determines if the child is connectable during reconnect

  Arguments:    cb      Information for the child that tries connection

  Returns:      If connection is accepted, TRUE.
                If not accepted, FALSE.
 *---------------------------------------------------------------------------*/
static BOOL JudgeConnectableChild(WMStartParentCallback *cb)
{
    /*  Search the MAC address for the aid of when multibooting the child of cb->aid*/
    u16     playerNo = MBP_GetPlayerNo(cb->macAddress);
    OS_TPrintf("MB child(%d) -> DS child(%d)\n", playerNo, cb->aid);
    return (playerNo != 0);
}

/*---------------------------------------------------------------------------*
  Name:         StartWirelessParent

  Description:  begins parent wireless communication

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void StartWirelessParent(void)
{
    /* begins WFS initialization and connection processes*/
    WFS_InitParent(PORT_WFS, StateCallbackForWFS, AllocatorForWFS,
                   NULL, WH_PARENT_MAX_SIZE, NULL, TRUE);
    WH_SetJudgeAcceptFunc(JudgeConnectableChild);
    (void)WH_ParentConnect(WH_CONNECTMODE_DS_PARENT, WM_GetNextTgid(), GetCurrentChannel());
}

/*---------------------------------------------------------------------------*
  Name:         StartWirelessChild

  Description:  begins child wireless communication

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void StartWirelessChild(void)
{
    for (;;)
    {
        /* begins WFS initialization, parent search and connection processes*/
        WaitWHState(WH_SYSSTATE_IDLE);
        WFS_InitChild(PORT_WFS, StateCallbackForWFS, AllocatorForWFS, NULL);
        WH_SetGgid(WH_GGID);
        (void)WH_ChildConnectAuto(WH_CONNECTMODE_DS_CHILD,
                                  (const u8 *)MB_GetMultiBootParentBssDesc()->bssid, 0);
        while ((WH_GetSystemState() == WH_SYSSTATE_BUSY) ||
               (WH_GetSystemState() == WH_SYSSTATE_SCANNING))
        {
            (void)WaitNextFrame();
            PrintString(9, 11, 0xf, "Now working...");
            if (IS_PAD_TRIGGER(PAD_BUTTON_START))
            {
                (void)WH_Finalize();
            }
        }
        /* retries when connection fails*/
        if (WH_GetSystemState() == WH_SYSSTATE_CONNECT_FAIL)
        {
            WH_Reset();
            WaitWHState(WH_SYSSTATE_IDLE);
        }
        /* end here for unexpected errors*/
        else if (WH_GetSystemState() == WH_SYSSTATE_ERROR)
        {
            for (;;)
            {
                (void)WaitNextFrame();
                PrintString(5, 10, 0x1, "======= ERROR! =======");
                if (WH_GetLastError() == WM_ERRCODE_OVER_MAX_ENTRY)
                {
                    PrintString(5, 13, 0xf, "OVER_MAX_ENTRY");
                }
            }
        }
        else
        {
            break;
        }
    }

}
