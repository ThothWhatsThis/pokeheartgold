/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - simple-1
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.39  04/10/2006 04:50:11  kitase_hirotake
  changed so that the number of data sharing children would reference WH_CHILD_MAX, not WM_NUM_MAX_CHILD

  Revision 1.38  01/18/2006 02:11:21  kitase_hirotake
  do-indent

  Revision 1.37  11/25/2005 01:35:20  seiki_masashi
  Changed the structure name from GameInfo to MyGameInfo

  Revision 1.36  11/24/2005 04:27:58  adachi_hiroaki
  comments added

  Revision 1.35  11/21/2005 11:07:00  kitase_hirotake
  Deleted OS_InitPrintServer, OS_PrintServer and PXI_Init
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.34  11/21/2005 00:41:10  adachi_hiroaki
  Made the parent device selection screen display the number of players joining and their nicknames

  Revision 1.33  11/11/2005 07:25:42  yosizaki
  fix sub-BG chrbase setting.

  Revision 1.32  06/27/2005 11:10:13  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.31  04/07/2005 04:35:20  seiki_masashi
  Fixed bug in channel selection

  Revision 1.30  02/28/2005 05:26:12  yosizaki
  do-indent.

  Revision 1.29  02/18/2005 14:23:18  seiki_masashi
  Revised Copyright notation

  Revision 1.28  02/18/2005 12:08:23  seiki_masashi
  warning measures

  Revision 1.27  12/01/2004 07:24:08  sasakis
  Reviewed comments and fixed the lobby wait bug after recovering error.

  Revision 1.26  11/08/2004 02:47:11  takano_makoto
  Changed the return value of scanCallback. Added WH_EndScan.

  Revision 1.25  11/05/2004 09:16:48  sasakis
  Improved the behavior of windows, and arranged codes.

  Revision 1.24  11/05/2004 04:27:40  sasakis
  Added the lobby screen and parent selection screen, and made the necessary modifications (such as scan related).

  Revision 1.23  11/02/2004 07:12:55  sasakis
  Arranged codes.

  Revision 1.22  11/02/2004 01:07:34  sasakis
  Made a few fixes in addition to restoring a section with some meaningless code

  Revision 1.21  11/01/2004 09:47:21  seiki_masashi
  Deleted unnecessary strings.

  Revision 1.20  11/01/2004 09:42:17  seiki_masashi
  Changed sSendBuf and sRecvBuf from 256 bytes to 512 bytes.

  Revision 1.19  11/01/2004 04:11:44  sasakis
  Added code, etc to take care of extremely slow processing. (provisional version includes code that is being worked on)

  Revision 1.18  10/29/2004 02:06:37  takano_makoto
  Changed WH_ChildConnectAuto interface.

  Revision 1.17  10/28/2004 08:10:19  takano_makoto
  Changed the WH_Connect function name

  Revision 1.16  10/27/2004 03:47:00  takano_makoto
  Changed MeasureChannel to start manually

  Revision 1.15  10/22/2004 07:35:18 AM  sasakis
  Support for shared wh.

  Revision 1.14  10/22/2004 06:58:26  sasakis
  Added radio emission icon.

  Revision 1.13  10/21/2004 00:43:34  yosizaki
  add SHARED_WH switch.

  Revision 1.12  10/06/2004 05:11:35  sasakis
  Addition of a graph display, etc.

  Revision 1.11  09/30/2004 06:19:57  seiki_masashi
  Changed an indent

  Revision 1.10  09/30/2004 06:16:07  seiki_masashi
  Changed the call back location of WM_StepDataSharing.

  Revision 1.9  09/24/2004 09:43:35  sasakis
  Made several small bug fixes.

  Revision 1.8  09/24/2004 06:12:35  sasakis
  Added a manual channel selection feature.

  Revision 1.7  09/22/2004 09:39:33  sasakis
  Added a debug screen.

  Revision 1.6  09/17/2004 09:13:30  sasakis
  Modified to allow selection of the least crowded channel
  by using MeasureChannel when it is a parent. (conditions are not yet determined.)

  Revision 1.5  09/15/2004 10:23:40  sasakis
  Added a radio reception strength icon, modification to make the data size variable, etc.

  Revision 1.4  09/10/2004 08:44:52  sasakis
  Added a guideline compliance process, made error-related reconsiderations, etc.

  Revision 1.3  09/08/2004 08:55:35  sasakis
  Slightly strengthened the disconnect process.

  Revision 1.2  09/08/2004 01:52:52  sasakis
  Error / disconnection related improvements.

  Revision 1.1  09/07/2004 04:59:42  sasakis
  Initial version registration.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/*
  dataShare-Model

  It has not been made into a game format, but this is a sample program that actively implemented
  the features that may be required (or that seems to be required) for the actual game application.
  Specifically, one of
  
  1) Lobby screen
  2) Parent selection screen
  3) Measures against processing failure (synchronized)
  4) Error display and return
  
  are implemented.
  The operation can be done with:

  +Control Pad: Menu selection
  A Button: Select
  B Button: Cancel

  Besides the above-mentioned operations, for debugging
  
  L Button: Increase the burden count
  R Button: Decrease the burden count
  
  are also available.
*/

#include <nitro.h>
#include <nitro/wm.h>
#include <string.h>

#include "main.h"
#include "font.h"
#include "print.h"
#include "key.h"
#include "graphics.h"
#include "wh.h"
#include "menu.h"
#include "unicode.h"

/* the GGID used in this demo*/
#define WH_GGID           SDK_MAKEGGID_SYSTEM(0x13)
#define GRAPH_TOTAL_FRAME 60

typedef struct GraphData_
{
    u16     level;
    s16     data;
}
GraphData;

static u16 sForcedChannel = 0;
static u16 sTgid = 0;
static int sSysMode = 0;
static int sSysModeStep = 0;

static s32 sFrame = 0;
static u8 sSendBuf[512] ATTRIBUTE_ALIGN(32);
static u8 sRecvBuf[512] ATTRIBUTE_ALIGN(32);
static BOOL sRecvFlag[WM_NUM_MAX_CHILD + 1];
static MyGameInfo sGameInfo;

static WMKeySet sKeySet;

static GraphData sGraphData[1 + WM_NUM_MAX_CHILD][GRAPH_TOTAL_FRAME];
static PRScreen sInfoScreen ATTRIBUTE_ALIGN(32);
static PRScreen sDebugScreen ATTRIBUTE_ALIGN(32);

static int sBeaconCount = 0;
static int sNeedWait = 0;
static BOOL sGraphEnabled = 0;

static WMBssDesc sBssDesc[ITEM_NUM_MAX];
static Window sRoleMenuWindow;
static Window sSelectChannelWindow;
static Window sSelectParentWindow;
static Window sLobbyWindow;
static Window sErrorWindow;
static Window sBusyWindow;
static Window sOptionWindow;
static Window sWaitWindow;

extern const unsigned char wlicon_image[];
extern const unsigned short wlicon_palette[];

static void TraceWH(const char *fmt, ...)
{
    va_list vlist;
    va_start(vlist, fmt);
    PR_VPrintString(&sDebugScreen, fmt, vlist);
    va_end(vlist);
}

static void printString(const char *fmt, ...)
{
    va_list vlist;
    va_start(vlist, fmt);
    PR_VPrintString(&sInfoScreen, fmt, vlist);
    va_end(vlist);
}

BOOL isDataReceived(int index)
{
    return sRecvFlag[index];
}

ShareData *getRecvData(int index)
{
    return (ShareData *) (&(sRecvBuf[index * sizeof(ShareData)]));
}

ShareData *getSendData(void)
{
    return (ShareData *) sSendBuf;
}

PRScreen *getInfoScreen(void)
{
    return &sInfoScreen;
}

void changeSysMode(int s)
{
    if (sSysMode == s)
    {
        return;
    }

    OS_Printf("sysmode = %d\n", s);
    sSysMode = s;
    sSysModeStep = 0;
}

static void indicateCallback(void *arg)
{
    WMIndCallback *cb;
    cb = (WMIndCallback *)arg;
    if (cb->state == WM_STATECODE_BEACON_RECV)
    {
        sBeaconCount = 2;
    }
}

static void scanCallback(WMBssDesc *bssdesc)
{
    char    buf[ITEM_LENGTH_MAX];
    char    nickName[10 * 2 + 1];
    u8      entryCount;
    int     i;

    // Checks whether userGameInfo is assumed
    if (bssdesc->gameInfoLength != 0 && bssdesc->gameInfo.userGameInfoLength == sizeof(MyGameInfo))
    {
        const MyGameInfo *pMyGameInfo = (MyGameInfo *) bssdesc->gameInfo.userGameInfo;
        MI_CpuCopy8(pMyGameInfo->nickName, nickName, (u32)pMyGameInfo->nickNameLength * 2);
        entryCount = pMyGameInfo->entryCount;
    }
    else
    {
        (void)STD_CopyLString(nickName, "unknown", sizeof(nickName));
        entryCount = 0;
    }

    // investigates the parents already found
    for (i = 0; i < sSelectParentWindow.itemnum; ++i)
    {
        if (WM_IsBssidEqual(sBssDesc[i].bssid, bssdesc->bssid))
        {
            break;
        }
    }

    // Create the menu item for parent selection.
    (void)OS_SNPrintf(buf, ITEM_LENGTH_MAX,
                      "[%d]channel%d %s %d/%d", i + 1, bssdesc->channel, nickName, entryCount,
                      WH_CHILD_MAX);

    if (i < sSelectParentWindow.itemnum)
    {
        // It's already registered in the menu, so it's over-written
        setItemToWindow(&sSelectParentWindow, buf, i);
    }
    else
    {
        // This is a newly found parent, so the information is remembered and added to the menu.
        sBssDesc[sSelectParentWindow.itemnum] = *bssdesc;
        addItemToWindow(&sSelectParentWindow, buf);

        WH_PrintBssDesc(bssdesc);
    }
}

static void gameinfoCallback(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;
}

static void updateGameInfo(BOOL isEntry)
{
    ++sGameInfo.periodicalCount;
    sGameInfo.entryCount = (u8)(MATH_CountPopulation(WH_GetBitmap()) - 1);

    (void)WM_SetGameInfo(gameinfoCallback, (u16 *)&sGameInfo, sizeof(sGameInfo), WH_GGID, sTgid,
                         (u8)(isEntry ? WM_ATTR_FLAG_ENTRY : 0));
}

static void forceSpinWait(void)
{
    // Process to create the process failure state randomly
    // by using OS_SpinWait .

    static int waitcycle = 0;

    if (getKeyInfo()->cnt & PAD_BUTTON_L)
    {
        waitcycle += 4000;
        // OS_Printf("wait = %d\n", waitcycle);

    }
    else if (getKeyInfo()->cnt & PAD_BUTTON_R)
    {
        waitcycle -= 4000;
        if (waitcycle < 0)
        {
            waitcycle = 0;
        }
        // OS_Printf("wait = %d\n", waitcycle);
    }

    OS_SpinWait((u32)waitcycle);
}

static void ModeSelectRole(void)
{
    static const char *menuitems[] = {
        "Start (Parent mode)",
        "Start (Child mode)",
        "Option",
        NULL
    };

    if (sSysModeStep == 0)
    {
        sRoleMenuWindow.selected = 0;
        setupWindow(&sRoleMenuWindow, 16, 16, WIN_FLAG_SELECTABLE, 24, 24, 16);
        if (sRoleMenuWindow.itemnum == 0)
        {
            int     i;
            for (i = 0; menuitems[i] != NULL; ++i)
            {
                addItemToWindow(&sRoleMenuWindow, menuitems[i]);
            }
        }
        openWindow(&sRoleMenuWindow);
    }

    if (sRoleMenuWindow.state == WIN_STATE_CLOSED)
    {
        if (sRoleMenuWindow.selected < 0)
        {
            openWindow(&sRoleMenuWindow);
            return;
        }

        switch (sRoleMenuWindow.selected)
        {
        case 0:
            if (sForcedChannel == 0)
            {
                // Obtains the best channel based on the usage rate and makes a connection.
                (void)WH_StartMeasureChannel();

            }
            else
            {
                sTgid++;

                // userGameInfo is updated in the entry reception state.
                updateGameInfo(TRUE);

                // Destroys cached parent information.
                MI_CpuClear8(sBssDesc, sizeof(sBssDesc));

                // Use the channel that was selected manually to start the connection.
                (void)WH_ParentConnect(WH_CONNECTMODE_DS_PARENT, sTgid, sForcedChannel);
            }
            changeSysMode(SYSMODE_LOBBY);
            break;

        case 1:
            {
                // Searches for a parent.
                static const u8 ANY_PARENT[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
                enum
                { ALL_CHANNEL = 0 };

                initWindow(&sSelectParentWindow);
                setupWindow(&sSelectParentWindow, 16, 16, WIN_FLAG_SELECTABLE, 8 * 2, 8, 16);
                (void)WH_StartScan(scanCallback, ANY_PARENT, ALL_CHANNEL);
                changeSysMode(SYSMODE_SCAN_PARENT);
            }
            break;

        case 2:
            // To option screen.
            changeSysMode(SYSMODE_OPTION);
            break;

        default:
            break;
        }
    }
}

static void ModeOption(void)
{
    // Option screen process

    if (sSysModeStep == 0)
    {
        initWindow(&sOptionWindow);
        setupWindow(&sOptionWindow, 16, 16, WIN_FLAG_SELECTABLE, 8, 8, 16);
        addItemToWindow(&sOptionWindow, "Select channel (parent)");

        if (sGraphEnabled)
        {
            addItemToWindow(&sOptionWindow, "Disable beacon graph");
        }
        else
        {
            addItemToWindow(&sOptionWindow, "Enable beacon graph");
        }

        openWindow(&sOptionWindow);
        return;
    }

    if (sOptionWindow.state == WIN_STATE_CLOSED)
    {
        if (sOptionWindow.selected < 0)
        {
            // Cancelled.
            changeSysMode(SYSMODE_SELECT_ROLE);
            return;
        }

        if (sOptionWindow.selected == 0)
        {
            // To the channel selection screen
            changeSysMode(SYSMODE_SELECT_CHANNEL);

        }
        else if (sOptionWindow.selected == 1)
        {
            sGraphEnabled = sGraphEnabled ? FALSE : TRUE;
            changeSysMode(SYSMODE_SELECT_ROLE);
        }
    }
}

static void ModeLobby(void)
{
    // Lobby screen process

    u16     bmap;
    int     i;

    updateGameInfo(TRUE);

    if (sSysModeStep == 0)
    {
        initWindow(&sLobbyWindow);
        setupWindow(&sLobbyWindow, 16, 16, WIN_FLAG_NONE, 8, 8, 16);

        for (i = 0; i < WH_CHILD_MAX; ++i)
        {
            addItemToWindow(&sLobbyWindow, "");
        }

        addItemToWindow(&sLobbyWindow, "");
        addItemToWindow(&sLobbyWindow, "Push A to start");

        openWindow(&sLobbyWindow);
        return;
    }

    bmap = WH_GetBitmap();
    for (i = 0; i < WH_CHILD_MAX; ++i)
    {
        if (bmap & (1 << i))
        {
            (void)OS_SNPrintf(sLobbyWindow.item[i], ITEM_LENGTH_MAX, "[%02d] - entry", i);
        }
        else
        {
            (void)OS_SNPrintf(sLobbyWindow.item[i], ITEM_LENGTH_MAX, "[%02d] - waiting", i);
        }
    }

    if (sLobbyWindow.state == WIN_STATE_CLOSED)
    {
        getSendData()->command = SHARECMD_NONE;
        if (sLobbyWindow.selected < 0)
        {
            WH_Finalize();
            changeSysMode(SYSMODE_SELECT_ROLE);
            return;
        }

        changeSysMode(SYSMODE_PARENT);
    }
}

static void ModeLobbyWait(void)
{
    // Process of a child waiting in the lobby.
    // Waits until a parent sends a start signal.

    if (sSysModeStep == 0)
    {
        initWindow(&sWaitWindow);
        setupWindow(&sWaitWindow, 32, 56, WIN_FLAG_NOCONTROL, 8, 8, 8);
        addItemToWindow(&sWaitWindow, "\\2Accepted.");
        addItemToWindow(&sWaitWindow, "\\2Waiting for parent...");
        openWindow(&sWaitWindow);
        return;
    }

    if (getRecvData(0)->command == SHARECMD_EXITLOBBY)
    {
        closeWindow(&sWaitWindow);
        changeSysMode(SYSMODE_CHILD);
    }
}

static void ModeSelectChannel(void)
{
    static u16 channelList[15];        // Auto select + maximum 14 channels
    // Channel selection screen
    if (sSysModeStep == 0)
    {
        setupWindow(&sSelectChannelWindow, 16, 16, WIN_FLAG_SELECTABLE, 16, 12, 16);

        if (sSelectChannelWindow.itemnum == 0)
        {
            u16     pattern;
            int     i, j;
            for (i = 0; i < 14; i++)
            {
                channelList[i] = 0;
            }
            pattern = WH_GetAllowedChannel();
            addItemToWindow(&sSelectChannelWindow, "Auto select");
            for (i = 1, j = 1; i <= 14; ++i)
            {
                if (pattern & (1 << (i - 1)))
                {
                    char    buf[ITEM_LENGTH_MAX];
                    (void)OS_SNPrintf(buf, ITEM_LENGTH_MAX, "Channel %d", i);
                    channelList[j] = (u16)i;
                    ++j;
                    addItemToWindow(&sSelectChannelWindow, buf);
                }
            }
        }

        openWindow(&sSelectChannelWindow);
    }

    if (sSelectChannelWindow.state == WIN_STATE_CLOSED)
    {
        if (sSelectChannelWindow.selected >= 0)
        {
            sForcedChannel = channelList[sSelectChannelWindow.selected];
        }

        // Return to the role selection screen.
        changeSysMode(SYSMODE_SELECT_ROLE);
    }
}

static void ModeSelectParent(void)
{
    // Display parent on the list and select it.
    // In this state, WH is scanning a parent, and if a new parent was found during scanning,
    // it will be added/reflected to the menu.

    if (sSysModeStep == 0)
    {
        openWindow(&sSelectParentWindow);
    }

    // Did the user close the parent search screen?
    if ((sSelectParentWindow.state == WIN_STATE_CLOSED))
    {
        if (WH_GetSystemState() == WH_SYSSTATE_SCANNING)
        {
            // If the parent is currently scanning, scan is finished for a moment.
            (void)WH_EndScan();
            return;
        }

        if (WH_GetSystemState() == WH_SYSSTATE_IDLE)
        {
            if (sSelectParentWindow.selected < 0)
            {
                WH_Finalize();
                changeSysMode(SYSMODE_SELECT_ROLE);
                return;
            }
            // If not scanning, given that the user has selected a parent, data sharing starts.
            (void)WH_ChildConnect(WH_CONNECTMODE_DS_CHILD,
                                  &(sBssDesc[sSelectParentWindow.selected]));
            changeSysMode(SYSMODE_LOBBYWAIT);
        }
    }
}

static void ModeError(void)
{
    // Error state
    if (sSysModeStep == 0)
    {
        initWindow(&sErrorWindow);
        setupWindow(&sErrorWindow, 16, 16, WIN_FLAG_NONE, 8, 8, 16);

        addItemToWindow(&sErrorWindow, "\\1Error has occured!");

        if (WH_GetLastError() == WM_ERRCODE_OVER_MAX_ENTRY)
        {
            addItemToWindow(&sErrorWindow, "\\4Rejected\n");
        }

        if (WH_GetLastError() == WH_ERRCODE_DISCONNECTED)
        {
            addItemToWindow(&sErrorWindow, "\\4Disconnected by parent\n");
        }

        if (WH_GetLastError() == WH_ERRCODE_PARENT_NOT_FOUND)
        {
            addItemToWindow(&sErrorWindow, "\\4Parent not found\n");
        }

        if (WH_GetLastError() == WH_ERRCODE_LOST_PARENT)
        {
            addItemToWindow(&sErrorWindow, "\\4Lost parent\n");
        }

        addItemToWindow(&sErrorWindow, "");
        addItemToWindow(&sErrorWindow, "\\fPush A to reset");

        closeAllWindow();
        openWindow(&sErrorWindow);
    }

    if (sErrorWindow.state == WIN_STATE_CLOSED)
    {
        WH_Finalize();
        getRecvData(0)->command = SHARECMD_NONE;
        changeSysMode(SYSMODE_SELECT_ROLE);
    }
}

static void printShareData(void)
{
    s32     i;
    ShareData *sd;

    sd = getSendData();
    printString("\\2Send:     0x%04x 0x%04x\n", sd->key, sd->count & 0xffff);

    printString("\\4Receive:\n");
    for (i = 1; i < (WM_NUM_MAX_CHILD + 1); i++)
    {
        if (sRecvFlag[i])
        {
            sd = getRecvData(i);
            printString("\\4Child%02d:  0x%04x 0x%04x\n", i, sd->key, sd->count & 0xffff);

        }
        else
        {
            printString("No child\n");
        }
    }
}

static void ModeParent(void)
{
    printString("\n  \\fParent mode\n\n");
    printShareData();
}

static void ModeChild(void)
{
    ShareData *sd;

    printString("\n  \\fChild mode\n\n");
    printShareData();

    sd = (ShareData *) getRecvData(0);
    printString("\\4Parent:   0x%04x 0x%04x\n", sd->key, sd->count & 0xffff);
}

static void VBlankIntr(void)
{
    /*
       Some of the other samples perform StepDataSharing
       here (with VBlankIntr function).
       This sample also performed StepDataSharing here before.

       The reason it was set this way was that WM_StepDataSharing (and WM_GetKeySet)
       have to be called "before the start of
       MP communications in that frame."
       V count which SDK prepares for the default MP communication is set to 240 for a child, and
       260 for a parent. Therefore, setting immediately after V Blank is going to have the least difficulties.
       (Though it is not yet implemented at the time this document was written,
       we plan to have the V-count at MP communications startup configurable during initialization.

       Normally, for 30fps games, code is required to limit the amount of StepDS
       calls to once per two frames.
       In that case, when WM_ERRCODE_NO_DATASET was returned (if the extremely slow processing occurred),
       set to wait for 1 frame instead of 2 frames.
       Otherwise, when there is one frame difference between the parent and child, it cannot be corrected.
     */

    updateKeys();
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

static void initAllocSystem(void)
{
    void   *tempLo;
    OSHeapHandle hh;

    tempLo = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
    OS_SetArenaLo(OS_ARENA_MAIN, tempLo);
    hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
    if (hh < 0)
    {
        OS_Panic("ARM9: Fail to create heap...\n");
    }
    hh = OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
}

static void drawPowerGraph(void)
{
    static const GXRgb linecolor[4] = {
        GX_RGB(15, 0, 0),              // dark red (dead)
        GX_RGB(31, 31, 0),             // yellow
        GX_RGB(0, 31, 0),              // green
        GX_RGB(20, 31, 20),            // light green
    };

    int     midx, ringidx;

    ringidx = (sFrame % GRAPH_TOTAL_FRAME);

    for (midx = 0; midx < WM_NUM_MAX_CHILD + 1; ++midx)
    {
        sGraphData[midx][ringidx].data = (s16)getRecvData(midx)->data;
        sGraphData[midx][ringidx].level = (u16)getRecvData(midx)->level;
    }

    G3_PolygonAttr(GX_LIGHTMASK_NONE,
                   GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 0, 31, GX_POLYGON_ATTR_MISC_DISP_1DOT);
    G3_TexImageParam(GX_TEXFMT_NONE,
                     GX_TEXGEN_NONE,
                     GX_TEXSIZE_S16,
                     GX_TEXSIZE_T16, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0);

    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    G3_PushMtx();
    G3_Identity();
    G3_Translate(0, 0, -FX16_ONE * 4);

    G3_Begin(GX_BEGIN_TRIANGLES);

    for (midx = 1; midx < WM_NUM_MAX_CHILD + 1; ++midx)
    {
        int     basey, ys, ye, gi, x, level;
        basey = ((WM_NUM_MAX_CHILD / 2 - midx) * 9 + 6) * FX16_ONE / 64;

        for (gi = 0; gi < GRAPH_TOTAL_FRAME; ++gi)
        {
            int     ri;
            ri = (ringidx - gi);
            if (ri < 0)
            {
                ri += GRAPH_TOTAL_FRAME;
            }

            ys = sGraphData[midx][ri].data;
            level = sGraphData[midx][ri].level;

            ++ri;
            if (ri >= GRAPH_TOTAL_FRAME)
            {
                ri -= GRAPH_TOTAL_FRAME;
            }

            ye = sGraphData[midx][ri].data;

            x = -(gi - GRAPH_TOTAL_FRAME / 2) * 3;
            x *= FX16_ONE / 64;
            ys = ys * FX16_ONE / 64 + basey;
            ye = ye * FX16_ONE / 64 + basey;

            G3_Color(linecolor[level]);

            G3_Vtx((fx16)x, (fx16)ys, 0);
            G3_Vtx((fx16)(x + FX16_ONE / 64 / 2), (fx16)(ys + FX16_ONE / 64), 0);
            G3_Vtx((fx16)(x + 3 * FX16_ONE / 64), (fx16)ye, 0);
        }
    }

    G3_End();
    G3_PopMtx(1);
}

static void drawPowerIcon(void)
{
    // GUIDELINE
    // Display the radio receiving strength icon.
    setupPseudo2DCamera();

    G3_PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGONMODE_DECAL, GX_CULL_NONE, 0, 31, 0);
    G3_PushMtx();
    G3_MtxMode(GX_MTXMODE_TEXTURE);
    G3_Identity();

    G3_TexImageParam(GX_TEXFMT_PLTT16,
                     GX_TEXGEN_TEXCOORD,
                     GX_TEXSIZE_S16,
                     GX_TEXSIZE_T16,
                     GX_TEXREPEAT_NONE,
                     GX_TEXFLIP_NONE,
                     GX_TEXPLTTCOLOR0_USE, (u32)(0x2000 + WM_GetLinkLevel() * 16 * 16 / 2));
    G3_TexPlttBase(0x2000, GX_TEXFMT_PLTT16);
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    drawPseudo2DTexQuad(224, 160, 16, 16, 16, 16);
}

static void drawRadioIcon(void)
{
    // GUIDELINE
    // Display the radio wave strength icon.
    int     i;
    G3_TexPlttBase(0x2000, GX_TEXFMT_PLTT16);
    G3_TexImageParam(GX_TEXFMT_PLTT16,
                     GX_TEXGEN_TEXCOORD,
                     GX_TEXSIZE_S16,
                     GX_TEXSIZE_T16,
                     GX_TEXREPEAT_NONE,
                     GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0x2000 + 4 * 16 * 16 / 2);

    for (i = 0; i < 2; ++i)
    {
        drawPseudo2DTexQuad(16,
                            12 + i * 24 + ((i == sRoleMenuWindow.selected) ? (sFrame / 15 & 1) : 0),
                            16, 16, 16, 16);
    }
}

static void updateShareData(void)
{
    if (WH_GetSystemState() == WH_SYSSTATE_DATASHARING)
    {
        if (WH_StepDS(sSendBuf))
        {
            u16     i;
            for (i = 0; i < WH_CHILD_MAX + 1; ++i)
            {
                u8     *adr;
                ShareData *sd;

                adr = (u8 *)WH_GetSharedDataAdr(i);
                sd = (ShareData *) & (sRecvBuf[i * sizeof(ShareData)]);

                if (adr != NULL)
                {
                    MI_CpuCopy8(adr, sd, sizeof(ShareData));
                    sRecvFlag[i] = TRUE;

                }
                else
                {
                    sd->level = 0;
                    sd->data = 0;
                    sRecvFlag[i] = FALSE;
                }
            }
            for(; i < WM_NUM_MAX_CHILD + 1; ++i)
            {
                ShareData *sd;
                
                sd = (ShareData *) & (sRecvBuf[i * sizeof(ShareData)]);
                
                sd->level = 0;
                sd->data = 0;
                sRecvFlag[i] = FALSE;
            }
            sNeedWait = FALSE;

        }
        else
        {
            u16     i;
            for (i = 0; i < WM_NUM_MAX_CHILD + 1; ++i)
            {
                sRecvFlag[i] = FALSE;
            }

            sNeedWait = TRUE;
        }

    }
    else
    {
        u16     i;
        for (i = 0; i < WM_NUM_MAX_CHILD + 1; ++i)
        {
            sRecvFlag[i] = FALSE;
        }

        sNeedWait = FALSE;
    }
}

static void packSendData(void)
{
    ShareData *senddata;

    if (sNeedWait)
    {
        return;
    }

    senddata = getSendData();
    senddata->command = (sSysMode == SYSMODE_LOBBY) ? SHARECMD_NONE : SHARECMD_EXITLOBBY;

    senddata->level = (u16)WM_GetLinkLevel();

    senddata->data = 0;
    senddata->key = getKeyInfo()->cnt;
    senddata->count = sFrame & 0xffff;

    if (sBeaconCount != 0)
    {
        senddata->data += sBeaconCount * senddata->level;

        if (sBeaconCount > 0)
        {
            sBeaconCount = -sBeaconCount + 1;
        }
        else
        {
            sBeaconCount = -sBeaconCount - 1;
        }
    }
}

static void mainLoop(void)
{
    for (sFrame = 0; TRUE; sFrame++)
    {
        int     whstate;
        int     prevmode;

        whstate = WH_GetSystemState();
        prevmode = sSysMode;

        switch (whstate)
        {
        case WH_SYSSTATE_ERROR:
            // WH state has the priority if an error occurred.
            changeSysMode(SYSMODE_ERROR);
            break;

        case WH_SYSSTATE_MEASURECHANNEL:
            {
                u16     channel = WH_GetMeasureChannel();
                sTgid++;
                (void)WH_ParentConnect(WH_CONNECTMODE_DS_PARENT, sTgid, channel);
            }
            break;

        default:
            break;
        }

        PR_ClearScreen(&sInfoScreen);

        // Load test.
        forceSpinWait();

        switch (sSysMode)
        {
        case SYSMODE_SELECT_ROLE:
            // Role (parent or child) selection screen
            ModeSelectRole();
            break;

        case SYSMODE_SELECT_CHANNEL:
            // Channel selection screen
            ModeSelectChannel();
            break;

        case SYSMODE_LOBBY:
            // Lobby screen
            ModeLobby();
            break;

        case SYSMODE_LOBBYWAIT:
            // Lobby standby screen
            ModeLobbyWait();
            break;

        case SYSMODE_OPTION:
            // Option screen
            ModeOption();
            break;

        case SYSMODE_SCAN_PARENT:
        case SYSMODE_SELECT_PARENT:
            // Parent selection screen
            ModeSelectParent();
            break;

        case SYSMODE_ERROR:
            // Error report screen
            ModeError();
            break;

        case SYSMODE_PARENT:
            // Parent mode screen
            ModeParent();
            break;

        case SYSMODE_CHILD:
            // Child mode screen
            ModeChild();
            break;

        default:
            break;
        }

        if ((whstate == WH_SYSSTATE_BUSY)
            || ((whstate == WH_SYSSTATE_SCANNING) && (sSelectParentWindow.itemnum == 0)))
        {
            sBusyWindow.state = WIN_STATE_OPENED;

        }
        else
        {
            sBusyWindow.state = WIN_STATE_CLOSED;
        }

        updateAllWindow();
        drawAllWindow();

        drawPowerIcon();

        if ((sSysMode == SYSMODE_PARENT) || (sSysMode == SYSMODE_CHILD))
        {
            if (sGraphEnabled)
            {
                drawPowerGraph();
            }
        }

        if ((sSysMode == SYSMODE_SELECT_ROLE) && (sRoleMenuWindow.state == WIN_STATE_OPENED))
        {
            drawRadioIcon();
        }

        G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);

        if (!sNeedWait && ((sSysMode == SYSMODE_PARENT) || (sSysMode == SYSMODE_CHILD)))
        {
            // With actual game, character update process and such
            // are going to be performed here.
        }

        DC_FlushRange(sInfoScreen.screen, sizeof(u16) * PR_SCREEN_SIZE);
        /* IO register is accessed using DMA operation, so cache Wait is not needed*/
        // DC_WaitWriteBufferEmpty();
        GX_LoadBG2Scr(sInfoScreen.screen, 0, sizeof(u16) * PR_SCREEN_SIZE);

        DC_FlushRange(sDebugScreen.screen, sizeof(u16) * PR_SCREEN_SIZE);
        /* IO register is accessed using DMA operation, so cache Wait is not needed*/
        // DC_WaitWriteBufferEmpty();
        GXS_LoadBG2Scr(sDebugScreen.screen, 0, sizeof(u16) * PR_SCREEN_SIZE);

        OS_WaitVBlankIntr();

        updateShareData();
        packSendData();

        if (prevmode == sSysMode)
        {
            ++sSysModeStep;
        }
    }
}

void NitroMain(void)
{
    OS_Init();
    FX_Init();

    initGraphics();

    GX_SetBankForBG(GX_VRAM_BG_256_AB);
    GX_SetBankForBGExtPltt(GX_VRAM_BGEXTPLTT_01_F);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256,
                         GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000);

    GX_BeginLoadTex();
    GX_LoadTex(wlicon_image, 0x2000, 16 * 16 * 5);
    GX_EndLoadTex();

    GX_BeginLoadTexPltt();
    GX_LoadTexPltt(wlicon_palette, 0x2000, 32);
    GX_EndLoadTexPltt();

    GX_LoadBG2Char(d_CharData, 0, sizeof(d_CharData));
    GX_LoadBGPltt(d_PaletteData, 0, sizeof(d_PaletteData));

    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    GX_SetBankForSubBGExtPltt(GX_VRAM_SUB_BGEXTPLTT_0123_H);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256,
                          GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000);

    GXS_SetGraphicsMode(GX_BGMODE_0);
    GXS_SetVisiblePlane(GX_PLANEMASK_BG2);
    G2S_SetBG2Priority(0);
    G2S_BG2Mosaic(FALSE);

    GXS_LoadBG2Char(d_CharData, 0, sizeof(d_CharData));
    GXS_LoadBGPltt(d_PaletteData, 0, sizeof(d_PaletteData));

    initAllocSystem();

    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)GX_VBlankIntr(TRUE);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();

    sInfoScreen.scroll = FALSE;
    sDebugScreen.scroll = TRUE;

    {
        // Gets owner information
        OSOwnerInfo MyInfo;
        OS_GetOwnerInfo(&MyInfo);
        sGameInfo.nickNameLength = (u8)MyInfo.nickNameLength;
        ExUTF16_LEtoSJIS_BE(sGameInfo.nickName, MyInfo.nickName, sGameInfo.nickNameLength);
    }

    // Initialize Wireless
    (void)WH_Initialize();
    (void)WM_SetIndCallback(indicateCallback);

    // WH initial setting
    WH_SetGgid(WH_GGID);
    WH_SetDebugOutput(TraceWH);

    GX_DispOn();
    GXS_DispOn();

    initWindow(&sRoleMenuWindow);
    initWindow(&sSelectChannelWindow);
    initWindow(&sSelectParentWindow);
    initWindow(&sLobbyWindow);
    initWindow(&sErrorWindow);
    initWindow(&sOptionWindow);
    initWindow(&sWaitWindow);
    initWindow(&sBusyWindow);

    setupWindow(&sBusyWindow, 64, 80, WIN_FLAG_NOCONTROL, 8, 8, 16);
    addItemToWindow(&sBusyWindow, "\\2Working...");

    // Countermeasure for pressing A button at IPL
    updateKeys();

    // initCharacter();
    changeSysMode(SYSMODE_SELECT_ROLE);
    mainLoop();
}
