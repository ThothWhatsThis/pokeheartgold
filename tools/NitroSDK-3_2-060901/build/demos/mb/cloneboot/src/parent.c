/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - demos - cloneboot
  File:     parent.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: parent.c,v $
  Revision 1.9  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.8  11/21/2005 10:48:43  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.7  08/04/2005 14:18:48  yosizaki
  fix about ParentMain().

  Revision 1.6  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.5  2005/02/04 11:02:15  yasu
  Revised the copyright date

  Revision 1.4  02/02/2005 09:05:14  seiki_masashi
  Corrected ChildInfo subscript overflow

  Revision 1.3  12/03/2004 08:48:36  takano_makoto
  Revised MPB_USING_MB_EX to MBP_USING_MB_EX, and added support for the MBP_USING_PREVIOUS_DOWNLOAD switch

  Revision 1.2  11/24/2004 11:07:38  yosizaki
  add parent_begin/parent_end.

  Revision 1.1  11/15/2004 06:21:18  yosizaki
  initial update.

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


/******************************************************************************/

static void GetChannelMain(void);
static BOOL ConnectMain(u16 tgid);
static void PrintChildState(void);
static BOOL JudgeConnectableChild(WMStartParentCallback *cb);


//============================================================================
//  constant definitions
//============================================================================

/* the GGID used in this demo*/
#define WH_GGID                 SDK_MAKEGGID_SYSTEM(0x22)


/* the program information that this demo downloads*/
const MBGameRegistry mbGameList = {
    /*
     * With a clone boot, specify NULL as the program path name.
     * However, this is the specification for MBP_RegistFile() of the MBP module and
     * the argument actually passed to MB_RegisterFile() can be anything.
     */
    NULL,
    (u16 *)L"DataShareDemo",           // Game name
    (u16 *)L"DataSharing demo(cloneboot)",      // Game content description
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


/*****************************************************************************/
/* Start the definition range of the .parent section parent device exclusive region*/
#include <nitro/parent_begin.h>
/*****************************************************************************/


/*---------------------------------------------------------------------------*
  Name:         ParentMain

  Description:  Parent main routine

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ParentMain(void)
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

    while (TRUE)
    {
        OS_WaitVBlankIntr();

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
        else
        {
            // Exit the WH module and repeat
            WH_Finalize();
            (void)WH_End();
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
            BgSetMessage(PLTT_WHITE, " Push A Button to start   ");
            if (IS_PAD_TRIGGER(PAD_BUTTON_A))
            {
                BgSetMessage(PLTT_YELLOW, "Check Traffic ratio       ");
                (void)WH_StartMeasureChannel();
            }
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
            OS_WaitVBlankIntr();
            return FALSE;
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


/*****************************************************************************/
/* End the definition boundary of the .parent section parent device exclusive region*/
#include <nitro/parent_end.h>
/*****************************************************************************/
