/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - demos - multiboot-Model
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.32  03/13/2006 06:39:01  yosizaki
  add test of MB_SetUserVolatData..

  Revision 1.31  2006/01/18 02:11:29  kitase_hirotake
  do-indent

  Revision 1.30  11/21/2005 10:51:51  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.29  04/11/2005 08:50:25  yosizaki
  change to use WM_GetNextTgid().

  Revision 1.28  03/08/2005 10:29:04  yosizaki
  change to initialize TGID at random.

  Revision 1.27  02/28/2005 05:26:19  yosizaki
  do-indent.

  Revision 1.26  02/04/2005 11:02:55  yasu
  Revised the copyright date

  Revision 1.25  02/02/2005 09:05:14  seiki_masashi
  Corrected ChildInfo subscript overflow

  Revision 1.24  2004/12/28 09:34:44  pair2
  Corrected the framework regarding the redundant calls of WH_Initialize()

  Revision 1.23  12/03/2004 08:12:58  takano_makoto
  Revised MPB_USING_MB_EX to MBP_USING_MB_EX, and added support for the MBP_USING_PREVIOUS_DOWNLOAD switch

  Revision 1.22  11/12/2004 09:01:20  yosizaki
  change mb_parent to /wireless_shared/mbp

  Revision 1.21  11/10/2004 01:12:28  takano_makoto
  Only 14 children were displayed. Fixed.    

  Revision 1.20  11/02/2004 18:08:07  takano_makoto
  fix comments.

  Revision 1.19  10/28/2004 01:31:09  takano_makoto
  Moved wmhigh to wh under wireless_shared.

  Revision 1.18  10/27/2004 02:06:41  takano_makoto
  Revised to increment tgid after a restart.

  Revision 1.17  10/25/2004 06:29:56  takano_makoto
  Added a setting for maximum connected children.

  Revision 1.16  10/21/2004 00:42:42  yosizaki
  add SHARED_WH switch.

  Revision 1.15  10/18/2004 11:44:06  yosizaki
  change to use MB_StartParentEx/MB_EndEx (switch)

  Revision 1.14  10/08/2004 01:27:10  takano_makoto
  Fixed bug in displayed child number offset.

  Revision 1.13  10/05/2004 09:45:29  terui
  Standardized MAC addresses as type u8 arrays.

  Revision 1.12  10/05/2004 07:49:36  takano_makoto
  Revised so that child Accept processes in mb_parent.c callback.

  Revision 1.11  10/05/2004 03:58:49  takano_makoto
  Make small revisions to comments.

  Revision 1.10  10/05/2004 02:15:05  takano_makoto
  Revised so that system waits for entry state and starts simultaneous download for all children.  

  Revision 1.9  09/30/2004 08:28:26  yosizaki
  add SDK_MAKEGGID_SYSTEM macro.

  Revision 1.8  09/27/2004 08:35:49  takano_makoto
  Fixed the problem where channel number changed when restarting parent device.

  Revision 1.7  09/21/2004 07:26:06  takano_makoto
  comments added

  Revision 1.4  09/17/2004 10:36:11  takano_makoto
  Made a change to location where BeaconPeriod was adjusted through the standard function srand.

  Revision 1.3  09/17/2004 06:11:59  takano_makoto
  Changed so that user information is fetched through MBP_GetChildInfo().

  Revision 1.2  09/17/2004 02:20:23  sato_masaki
  TODO : Added reply comments for TODO.

  Revision 1.1  09/16/2004 14:06:41  takano_makoto
  Initial Upload

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro.h>
#include <nitro/wm.h>
#include <nitro/mb.h>

#include "mbp.h"
#include "common.h"
#include "disp.h"
#include "gmain.h"
#include "wh.h"

/*
 * Sample application that reconnects following multiboot.  
 *
 * Because the MB library samples use the multiboot functionality,
 * development tools of the same communications environment (wired or wireless) are required.
 * The mb_child.bin program in the * $NitroSDK/bin/ARM9-TS/Release/ directory
 * The mb_child.bin program is a sample that provides the same functionality
 * as a multiboot child on the retail console.
 * Load this binary on the other tools as you would as sample program,
 * and run them together.
 * 
 */

/******************************************************************************/

static void WaitPressButton(void);
static void GetChannelMain(void);
static BOOL ConnectMain(u16 tgid);
static void PrintChildState(void);
static BOOL JudgeConnectableChild(WMStartParentCallback *cb);
static void OnPreSendMBVolat(u32 ggid);


//============================================================================
//  constant definitions
//============================================================================

/* the GGID used in this demo*/
#define WH_GGID                 SDK_MAKEGGID_SYSTEM(0x21)


/* the program information that this demo downloads*/
const MBGameRegistry mbGameList = {
    "/child.srl",                      // Child binary code
    (u16 *)L"DataShareDemo",           // Game name
    (u16 *)L"DataSharing demo",        // Game content description
    "/data/icon.char",                 // Icon character data
    "/data/icon.plt",                  // Icon palette data
    WH_GGID,                           // GGID
    MBP_CHILD_MAX + 1,                 // Max. number of players, including parents
};



//============================================================================
//   Variable definitions
//============================================================================

static s32 gFrame;                     // Frame counter

//-----------------------
// For keeping communication pathways
//-----------------------
static u16 sChannel = 0;
static const MBPChildInfo *sChildInfo[MBP_CHILD_MAX];


//============================================================================
//   function definitions
//============================================================================

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  Main routine

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    u16     tgid = 0;

    // Initialize screen, OS
    CommonInit();
    // Initialize screen
    DispInit();
    // Initialize the heap
    InitAllocateSystem();

    // Set data in WH
    WH_SetGgid(WH_GGID);

    // Enable interrupts
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();

    DispOn();

    /*
     * When the DS download play child device is identical to the parent device TGID, the parent device information does not update.
     * Therefore consideration is necessary on the DS download play parent device
     * so that different TGID values are assigned on each startup.
     * Normally, if the WM_GetNextTgid() function is used for this,
     * you can get a unique value based on RTC.
     */
    tgid = WM_GetNextTgid();

    while (TRUE)
    {
        // Wait for A button to be pressed
        WaitPressButton();

        // Search for a channel with little traffic
        GetChannelMain();

        /*
         * Normally, a different value is set to tgid every time the parent starts.
         * Note that when reconnecting to a multiboot child, if the parent is not started with the same tgid,
         * the connection is not possible unless a rescan is performed.
         * If scan is performed again before reconnecting, there is no need to
         * save tgid.
         */
        // Multiboot distribution
        if (ConnectMain(++tgid))
        {
            // Multiboot child startup is successful
            break;
        }
    }

    //--------------
    // Following multiboot, the child is reset and communication is temporarily disconnected.
    // In the existing condition, the parent needs to end the communication from MB_End().
    // Start the communication from the beginning while parent and children are completely disconnected.
    // 
    // At this time, the aids of the children will be shuffled. If necessary,
    // Save the aid and MAC address combination of before multiboot, and
    // link to a new aid when reconnecting.
    //--------------


    // Set buffer for data sharing communication
    GInitDataShare();

#if !defined(MBP_USING_MB_EX)
    (void)WH_Initialize();
#endif

    // Set the function to determine connected children.
    WH_SetJudgeAcceptFunc(JudgeConnectableChild);

    /* Main routine*/
    for (gFrame = 0; TRUE; gFrame++)
    {
        OS_WaitVBlankIntr();

        ReadKey();

        BgClear();

        switch (WH_GetSystemState())
        {
        case WH_SYSSTATE_IDLE:
            /* ----------------
             * When reconnecting a child to the same parent without rescanning,
             * the parent needs to have the same tgid and channel as the child.
             * In this demo, the child can connect without rescanning by having both parent and child use
             * the same channel as that used in multiboot and incrementing the tgid used for multiboot by one.
             * 
             * When specifying the MAC address to rescan, there will not be any problems if
             * the tgids and channels are not the same.
             * ---------------- */
            (void)WH_ParentConnect(WH_CONNECTMODE_DS_PARENT, (u16)(tgid + 1), sChannel);
            break;

        case WH_SYSSTATE_CONNECTED:
        case WH_SYSSTATE_KEYSHARING:
        case WH_SYSSTATE_DATASHARING:
            {
                BgPutString(8, 1, 0x2, "Parent mode");
                GStepDataShare(gFrame);
                GMain();
            }
            break;
        }
    }
}



/*---------------------------------------------------------------------------*
  Name:         WaitPressButton

  Description:  Function that waits in loop until A button is pressed.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WaitPressButton(void)
{
    while (TRUE)
    {
        OS_WaitVBlankIntr();
        ReadKey();
        BgClear();
        BgSetMessage(PLTT_WHITE, " Push A Button to start   ");
        if (IS_PAD_TRIGGER(PAD_BUTTON_A))
        {
            return;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         GetChannelMain

  Description:  Thoroughly checks the radio wave usage to find the channel to use.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void GetChannelMain(void)
{

    /*-----------------------------------------------*
     * After thoroughly checking the radio wave usage of the channel,
     * select the channel that is least used.
     * To call WM_MeasureChannel(), the state must be in the IDLE.
     * The function cannot be used in multiboot state because it does not remain in the IDLE state.
     * First call WM_Initialize to check the radio wave usage and end with WM_End, and
     * then call MB_Init again.
     *-----------------------------------------------*/
    (void)WH_Initialize();

    while (TRUE)
    {
        ReadKey();
        BgClear();
        BgSetMessage(PLTT_YELLOW, " Search Channel ");

        switch (WH_GetSystemState())
        {
            //-----------------------------------------
            // Initialization complete
        case WH_SYSSTATE_IDLE:
            (void)WH_StartMeasureChannel();
            break;
            //-----------------------------------------
            // Complete channel search
        case WH_SYSSTATE_MEASURECHANNEL:
            {
                sChannel = WH_GetMeasureChannel();
#if !defined(MBP_USING_MB_EX)
                (void)WH_End();
#else
                /* Proceed to the multiboot process while maintaining the IDLE state*/
                return;
#endif
            }
            break;
            //-----------------------------------------
            // End WM
        case WH_SYSSTATE_STOP:
            /* proceed to the multiboot process once WM_End completes*/
            return;
            //-----------------------------------------
            // Busy
        case WH_SYSSTATE_BUSY:
            break;
            //-----------------------------------------
            // Error occurred
        case WH_SYSSTATE_ERROR:
            (void)WH_Reset();
            break;
            //-----------------------------------------
        default:
            OS_Panic("Illegal State\n");
        }
        OS_WaitVBlankIntr();           // Wait for completion of v blank interrupt
    }
}


/*---------------------------------------------------------------------------*
  Name:         ConnectMain

  Description:  Connect with multiboot.

  Arguments:    tgid        Specifies the tgid for booting as parent.

  Returns:      If transfer to the child is successful, returns TRUE;
                If transfer fails or is cancelled, FALSE.
 *---------------------------------------------------------------------------*/
static BOOL ConnectMain(u16 tgid)
{
    MBP_Init(mbGameList.ggid, tgid);

    while (TRUE)
    {
        ReadKey();

        BgClear();

        BgSetMessage(PLTT_YELLOW, " MB Parent ");

        switch (MBP_GetState())
        {
            //-----------------------------------------
            // Idle state
        case MBP_STATE_IDLE:
            {
                MBP_Start(&mbGameList, sChannel);
                /* Transmission test of use definition data*/
                MB_SetSendVolatCallback(OnPreSendMBVolat, MB_SEND_VOLAT_CALLBACK_TIMMING_BEFORE);
            }
            break;

            //-----------------------------------------
            // Accepting entry from the child
        case MBP_STATE_ENTRY:
            {
                BgSetMessage(PLTT_WHITE, " Now Accepting            ");

                if (IS_PAD_TRIGGER(PAD_BUTTON_B))
                {
                    // Cancel multiboot with B Button
                    MBP_Cancel();
                    break;
                }

                // If there is at least one child in entry, start is possible.
                if (MBP_GetChildBmp(MBP_BMPTYPE_ENTRY) ||
                    MBP_GetChildBmp(MBP_BMPTYPE_DOWNLOADING) ||
                    MBP_GetChildBmp(MBP_BMPTYPE_BOOTABLE))
                {
                    BgSetMessage(PLTT_WHITE, " Push START Button to start   ");

                    if (IS_PAD_TRIGGER(PAD_BUTTON_START))
                    {
                        // Start download
                        MBP_StartDownloadAll();
                    }
                }
            }
            break;

            //-----------------------------------------
            // Program distribution process
        case MBP_STATE_DATASENDING:
            {

                // If everyone has completed download, start is possible.
                if (MBP_IsBootableAll())
                {
                    // Start boot
                    MBP_StartRebootAll();
                }
            }
            break;

            //-----------------------------------------
            // Reboot process
        case MBP_STATE_REBOOTING:
            {
                BgSetMessage(PLTT_WHITE, " Rebooting now                ");
            }
            break;

            //-----------------------------------------
            // Reconnect process
        case MBP_STATE_COMPLETE:
            {
                // Once all members have successfully connected, the multi-boot processing ends,
                // and restarts the wireless as an normal parent.
                BgSetMessage(PLTT_WHITE, " Reconnecting now             ");

                OS_WaitVBlankIntr();
                return TRUE;
            }
            break;

            //-----------------------------------------
            // Error occurred
        case MBP_STATE_ERROR:
            {
                // Cancel communications.
                MBP_Cancel();
            }
            break;

            //-----------------------------------------
            // Communication cancellation processing
        case MBP_STATE_CANCEL:
            // None
            break;

            //-----------------------------------------
            // Communications abnormal end
        case MBP_STATE_STOP:
#ifdef MBP_USING_MB_EX
            switch (WH_GetSystemState())
            {
            case WH_SYSSTATE_IDLE:
                (void)WH_End();
                break;
            case WH_SYSSTATE_BUSY:
                break;
            case WH_SYSSTATE_STOP:
                return FALSE;
            default:
                OS_Panic("illegal state\n");
            }
#else
            return FALSE;
#endif
        }

        // Display child state
        PrintChildState();

        OS_WaitVBlankIntr();           // Wait for completion of v blank interrupt
    }
}


/*---------------------------------------------------------------------------*
  Name:         PrintChildState

  Description:  Displays child information on screen.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void PrintChildState(void)
{
    static const char *STATE_NAME[] = {
        "NONE       ",
        "CONNECTING ",
        "REQUEST    ",
        "ENTRY      ",
        "DOWNLOADING",
        "BOOTABLE   ",
        "BOOTING    ",
    };
    enum
    {
        STATE_DISP_X = 2,
        INFO_DISP_X = 15,
        BASE_DISP_Y = 2
    };

    u16     i;

    /* display list of children*/
    for (i = 1; i <= MBP_CHILD_MAX; i++)
    {
        const MBPChildInfo *childInfo;
        MBPChildState childState = MBP_GetChildState(i);
        const u8 *macAddr;

        SDK_ASSERT(childState < MBP_CHILDSTATE_NUM);

        // State display
        BgPutString(STATE_DISP_X, i + BASE_DISP_Y, PLTT_WHITE, STATE_NAME[childState]);

        // User information display
        childInfo = MBP_GetChildInfo(i);
        macAddr = MBP_GetChildMacAddress(i);

        if (macAddr != NULL)
        {
            BgPrintStr(INFO_DISP_X, i + BASE_DISP_Y, PLTT_WHITE,
                       "%02x%02x%02x%02x%02x%02x",
                       macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
        }
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
    u16     playerNo;

    /*  Search the MAC address for the aid of when multibooting the child of cb->aid*/
    playerNo = MBP_GetPlayerNo(cb->macAddress);

    OS_TPrintf("MB child(%d) -> DS child(%d)\n", playerNo, cb->aid);

    if (playerNo == 0)
    {
        return FALSE;
    }

    sChildInfo[playerNo - 1] = MBP_GetChildInfo(playerNo);
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         OnPreSendMBVolat

  Description:  Callback function used for notification before a parent sends an MB beacon.

  Arguments:    ggid    GGID of game information to send.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void OnPreSendMBVolat(u32 ggid)
{
    /*
     * When multiple games are registered and you are switching between values set for individual games,
     * the game information is identified by using the ggid argument, as shown below.
     */
    if (ggid == mbGameList.ggid)
    {
        /*
         * The max size of transmittable user definition data is 8 bytes.
         * In this example, incremental 64-bit values are sent.
         * Note that the child may not always be able to immediately receive dynamically updated data.
         *
         */
        static u64 count ATTRIBUTE_ALIGN(8);
        SDK_COMPILER_ASSERT(sizeof(count) <= MB_USER_VOLAT_DATA_SIZE);
        SDK_COMPILER_ASSERT(MB_USER_VOLAT_DATA_SIZE == 8);
        ++count;
        MB_SetUserVolatData(ggid, (u8*)&count, sizeof(count));
    }
}
