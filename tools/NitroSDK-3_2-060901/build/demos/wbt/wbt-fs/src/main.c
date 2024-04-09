/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WBT - demos - wbt-fs
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.20  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.19  11/21/2005 11:06:11  kitase_hirotake
  Deleted OS_InitPrintServer, OS_PrintServer and PXI_Init
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.18  06/10/2005 06:31:30  yosizaki
  fix around ModeSelect().

  Revision 1.17  05/09/2005 04:49:23  yosizaki
  fix about call of WFS_End().

  Revision 1.16  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.15  10/31/2004 23:53:26  yosizaki
  delete WC_DEFAULT_CHANNEL (abandoned symbol)

  Revision 1.14  10/28/2004 13:10:48  yosizaki
  change to use block-transfer callback of wc.

  Revision 1.13  10/25/2004 10:20:52  yosizaki
  move to parent.c and child.c

  Revision 1.12  10/05/2004 10:22:36  yosizaki
  change packet-size within 5600[us].

  Revision 1.11  09/29/2004 07:27:52  yosizaki
  change WFS_Init.

  Revision 1.10  09/29/2004 07:02:00  yosizaki
  merge from branch.

  Revision 1.9  09/15/2004 09:10:57  yosizaki
  add Seek&Read.
  add some sample test.

  Revision 1.8  09/09/2004 14:09:03  yosizaki
  add sample of WFS_GetCurrentDownloadProgress().

  Revision 1.7  09/09/2004 07:38:31  yosizaki
  fix timing of FS_LoadTable.

  Revision 1.6  09/09/2004 05:05:01  yosizaki
  total reform.

  Revision 1.5  09/06/2004 08:46:12  yosizaki
  add routine to check result of FS_ReadFile().

  Revision 1.4  09/06/2004 06:54:09  yosizaki
  small change. (debug output)

  Revision 1.3  09/04/2004 13:26:58  yosizaki
  add test of overlay.

  Revision 1.2  09/04/2004 01:45:41  yosizaki
  add some debug-prints.

  Revision 1.1  09/03/2004 01:11:58  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
    This is a  sample program for the child during the wireless multibooting to use the file system by
	operating WBT.

    HOWTO:
        1. To start communications as a parent, press the A button.
           When a child is found in the surrounding area with the same wbt-fs demo, communication
           automatically begins. Communication with up to 15 children is possible at the same time.
        2. To start communications as a child, press the B button.
           When a parent with the same wbt-fs demo is found in the surrounding area, communication
           automatically begins.
        3. For info on various functions during connection, refer to parent/child screens
           and the comments in the source code.
 *---------------------------------------------------------------------------*/


#include <nitro.h>
#include <nitro/wm.h>
#include <nitro/wbt.h>
#include <nitro/fs.h>

#include    "wfs.h"

#include    "util.h"
#include    "wc.h"
#include    "func.h"
#include    "common.h"



/*---------------------------------------------------------------------------*
    Internal Constant Definitions
 *---------------------------------------------------------------------------*/

/* wireless communication parameters*/

#define		WC_PARENT_DATA_SIZE_MAX		128
#define		WC_CHILD_DATA_SIZE_MAX		256

#if	!defined(SDK_MAKEGGID_SYSTEM)
#define SDK_MAKEGGID_SYSTEM(num)    (0x003FFF00 | (num))
#endif
#define GGID_WBT_FS                 SDK_MAKEGGID_SYSTEM(0x31)

// Game information
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
     // Reserves
     }
};


/* screen transition mode*/
enum
{
    MODE_SELECT,
    MODE_ERROR,
    MODE_BUSY,
    MODE_PARENT,
    MODE_CHILD,
    MODE_MAX
};


/*---------------------------------------------------------------------------*
    internal variable definitions
 *---------------------------------------------------------------------------*/

/* If the child is selected even once, FS will be replaced, and it will not become the parent*/
static BOOL is_child = FALSE;


/*---------------------------------------------------------------------------*
    Internal function definitions
 *---------------------------------------------------------------------------*/


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
  Name:         WCCallbackForWFS

  Description:  Callback that receives status notification from the WC's wireless automatic drive.

  Arguments:    arg     - Callback pointer for the notification source WM function.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WCCallbackForWFS(WMCallback *arg)
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
  Name:         NitroMain

  Description:  Initialization and main loop.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    /* the different types of initialization*/
    OS_Init();
    OS_InitTick();

    FX_Init();
    GX_Init();
    GX_DispOff();
    GXS_DispOff();

    /* Initialize render framework for sample*/
    UTIL_Init();

    /*
     * Initializes the file system.
     * The available DMA channel can be specified for the parent.
     * This is simply ignored with the child as it is not used.
     */
    FS_Init(FS_DMA_NOT_USE);

    /* LCD display start*/
    GX_DispOn();
    GXS_DispOn();

    {                                  /* Main loop*/

        static KeyInfo key[1];
        int     frame_count;

        int     cur_mode = MODE_MAX;

        KeyRead(key);
        for (frame_count = 0;; ++frame_count)
        {
            KeyRead(key);

            ClearString();
            PrintString(1, 22, COLOR_WHITE, "frame:%d", frame_count);

            {                          /* determine screen transition*/
                int     new_mode = MODE_ERROR;
                switch (WcGetStatus())
                {
                case WC_STATUS_READY:
                    {
                        /* If previous WFS is already running, stop it once.*/
                        if (WFS_GetStatus() != WFS_STATE_STOP)
                        {
                            WFS_End();
                            WcFinish();
                            /* WM, WC and WFS have completed transition to pre-initialization state*/
                        }
                    }
                    new_mode = MODE_SELECT;
                    break;
                case WC_STATUS_BUSY:
                    new_mode = MODE_BUSY;
                    break;
                case WC_STATUS_PARENT:
                    new_mode = MODE_PARENT;
                    break;
                case WC_STATUS_CHILD:
                    new_mode = MODE_CHILD;
                    break;
                case WC_STATUS_ERROR:
                    new_mode = MODE_ERROR;
                    break;
                }
                if (cur_mode != new_mode)
                {
                    cur_mode = new_mode;
                    ClearLine();
                }
            }

            {                          /* screen process*/
                switch (cur_mode)
                {
                case MODE_SELECT:
                    ModeSelect(key);
                    break;
                case MODE_ERROR:
                    ModeError(key);
                    break;
                case MODE_BUSY:
                    ModeWorking(key);
                    break;
                case MODE_PARENT:
                    ModeParent(key);
                    break;
                case MODE_CHILD:
                    ModeChild(key);
                    break;
                }
                FlushLine();
            }

            OS_WaitVBlankIntr();
        }
    }
}


/*---------------------------------------------------------------------------*
  Name:         ModeSelect

  Description:  Process in parent/child selection screen

  Arguments:    key         Key input

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ModeSelect(const KeyInfo * p_key)
{
    if (!is_child)
    {
        PrintString(3, 10, COLOR_WHITE, "Press A to connect as PARENT");
    }
    PrintString(3, 12, COLOR_WHITE, "Press B to connect as CHILD");

    /*
     * In order for WFS to run using WM_SetPortCallback(),
     * call WFS_Init() after WM_Initialize().
     */
    if (!is_child && (p_key->trg & PAD_BUTTON_A))
    {
        /*
         * Table load is effective only with a parent.
         *  Inside WFS, the child holds the parent file system
         * table information in dynamically-allocated memory.
         * Therefore, loading this table again has no meaning.
         */
        static BOOL table_loaded = FALSE;
        if (!table_loaded)
        {
            u32     need_size = FS_GetTableSize();
            void   *p_table = OS_Alloc(need_size);
            SDK_ASSERT(p_table != NULL);
            table_loaded = FS_LoadTable(p_table, need_size);
            if (!table_loaded && p_table)
            {
                OS_Free(p_table);
            }
        }
        WcInit(def_parent_param, FALSE, /* data-sharing is not used*/
               TRUE,                   /* block-transfer is used*/
               1 /* non care in this demo (data-sharing frame period)*/ );
        WcSetBlockTransferCallback(WCCallbackForWFS);
        WcStartParent();
        /*
         * By default the parent starts with the following settings.
         * (If you want to change the settings, they can be set to any timing.)
         */
        WFS_InitParent(port_wbt, StateCallbackForWFS, AllocatorForWFS, NULL,
                       parent_packet_max, NULL, TRUE);
        WFS_EnableSync(0);
    }
    else if (p_key->trg & PAD_BUTTON_B)
    {
        is_child = TRUE;
        WcInit(def_parent_param, FALSE, /* data-sharing is not used*/
               TRUE,                   /* block-transfer is used*/
               1 /* non care in this demo (data-sharing frame period)*/ );
        WcSetBlockTransferCallback(WCCallbackForWFS);
        WcStartChild();
        WFS_InitChild(port_wbt, StateCallbackForWFS, AllocatorForWFS, NULL);
    }
}

/*---------------------------------------------------------------------------*
  Name:         ModeError

  Description:  Processing in error display screen

  Arguments:    key         Key input

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ModeError(const KeyInfo * p_key)
{
    PrintString(5, 10, COLOR_RED, "======= ERROR! =======");
    PrintString(5, 13, COLOR_WHITE, " Fatal error occured.");
    PrintString(5, 14, COLOR_WHITE, "Please reboot program.");

    (void)p_key;
}

/*---------------------------------------------------------------------------*
  Name:         ModeWorking

  Description:  Processing in busy screen

  Arguments:    key         Key input

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ModeWorking(const KeyInfo * p_key)
{
    PrintString(9, 11, COLOR_WHITE, "Now working...");

    if (p_key->trg & PAD_BUTTON_START)
    {
        WcEnd();
    }
}


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
