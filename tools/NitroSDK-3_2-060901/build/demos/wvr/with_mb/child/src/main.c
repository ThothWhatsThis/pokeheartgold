/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WVR - demos
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.7  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.6  11/21/2005 11:08:27  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.5  02/28/2005 05:26:23  yosizaki
  do-indent.

  Revision 1.4  02/18/2005 12:08:32  seiki_masashi
  warning measures

  Revision 1.3  02/16/2005 10:39:42  terui
  Simplified wvr include header notation

  Revision 1.2  02/09/2005 09:23:21  terui
  Made changed to explicitly specify VRAM ID when calling WVR_StartUp.  

  Revision 1.1  01/14/2005 06:25:50  terui
  Moved from tests to demos.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitro/wm.h>
#include <nitro/mb.h>

#include "common.h"
#include "disp.h"
#include "gmain.h"
#include "wh.h"

#include <nitro/wvr.h>

//============================================================================
//  Prototype Declaration
//============================================================================

static void ModeConnect(void);         // Begins connection to parent.
static void ModeError(void);           // Error display screen
static void ModeWorking(void);         // Busy screen
static void ChildReceiveCallback(WMmpRecvBuf *data);

static void StartUpCallback(void *arg, WVRResult result);


//============================================================================
//  Variable definitions
//============================================================================

static u32 gFrame;                     // Frame counter

static WMBssDesc gMBParentBssDesc ATTRIBUTE_ALIGN(32);

static volatile u8 startCheck;


//============================================================================
//  function definitions
//============================================================================

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  Child main routine

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{

    // Initialize screen, OS
    CommonInit();

    // Check if this machine is a child that was booted from multiboot.    
    if (!MB_IsMultiBootChild())
    {
        OS_Panic("not found Multiboot child flag!\n");
    }

    //--------------------------------------------------------------
    // If started with multiboot, will reset and disconnect.    
    // After booting, the booted parent's BssDesc will be saved.    
    // Use this info to reconnect to the parent.    
    // If you extract the mac address from the BssDesc and connect using the mac address    
    // from the parent's scan, there should be no problem. But if you use the saved    
    // BssDesc to connect to the parent, you must make sure that communication size and    
    // transfer mode match in the parent and child.    
    //--------------------------------------------------------------

    /* 
     * Gets parent info used for reconnecting to the parent.                       
     * The WMBssDesc used for connection must be aligned to 32 bytes.
     * When reconnecting with the mac address and not rescanning,    
     * make sure the max send size and KS/CS flags match on the parents and children.    
     * If it connects after rescanning, these values can all be zero.    
     */
    MB_ReadMultiBootParentBssDesc(&gMBParentBssDesc, WH_PARENT_MAX_SIZE,        // Maximum parent transfer size
                                  WH_CHILD_MAX_SIZE,    // Maximum child transfer size
                                  0,   // Key sharing flag
                                  0);  // Continuous transfer mode flag

    // When connecting without rescanning the parent, the tgid on the
    // parent and child must match.
    // After restarting the parent, to prevent connections from unrelated IPL, you must    
    // change the tgid and the child must change its tgid to match.     
    // In this demo, the parent and child tgids have increments of 1.    
    gMBParentBssDesc.gameInfo.tgid++;

    // Initialize screen
    DispInit();
    // Initialize the heap
    InitAllocateSystem();

    // Enable interrupts
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();

    /*================================================================*/
    // Start activating the wireless library
    startCheck = 0;
    if (WVR_RESULT_OPERATING != WVR_StartUpAsync(GX_VRAM_ARM7_128_D, StartUpCallback, NULL))
    {
        OS_TPanic("WVR_StartUpAsync failed. \n");
    }
    while (!startCheck)
    {
    }
    /*================================================================*/

    GInitDataShare();

    //********************************
    // Initialize Wireless
    (void)WH_Initialize();
    //********************************

    // LCD display start
    GX_DispOn();
    GXS_DispOn();

    // debug string output
    OS_TPrintf("MB child: Simple DataSharing demo started.\n");

    // empty call for getting key input data (strategy for pressing A button in IPL)
    ReadKey();

    // Main loop
    for (gFrame = 0; TRUE; gFrame++)
    {
        // get key input data
        ReadKey();

        // Clears the screen
        BgClear();

        // Direct to process based on communication status
        switch (WH_GetSystemState())
        {
        case WH_SYSSTATE_CONNECT_FAIL:
            {
                // If WM_StartConnect() fails, the WM internal state will be invalid.    
                // Reset to IDLE state by calling WM_Reset.    
                WH_Reset();
            }
            break;
        case WH_SYSSTATE_IDLE:
            {
                static  retry = 0;
                enum
                {
                    MAX_RETRY = 5
                };

                if (retry < MAX_RETRY)
                {
                    ModeConnect();
                    retry++;
                    break;
                }
                // If connection to parent is not possible after MAX_RETRY, display ERROR
            }
        case WH_SYSSTATE_ERROR:
            ModeError();
            break;
        case WH_SYSSTATE_BUSY:
        case WH_SYSSTATE_SCANNING:
            ModeWorking();
            break;

        case WH_SYSSTATE_CONNECTED:
        case WH_SYSSTATE_KEYSHARING:
        case WH_SYSSTATE_DATASHARING:
            {
                BgPutString(8, 1, 0x2, "Child mode");
                GStepDataShare(gFrame);
                GMain();
            }
            break;
        }

        // Display of signal reception strength
        {
            int     level;
            level = WH_GetLinkLevel();
            BgPrintStr(31, 23, 0xf, "%d", level);
        }

        // Waiting for the V-blank
        OS_WaitVBlankIntr();
    }
}

/*---------------------------------------------------------------------------*
  Name:         StartUpCallback

  Description:  Callback function to be notified end of async processing in wireless operation control
                library.

  Arguments:    arg     -   Argument specified when WVR_StartUpAsync is called. Not used.
                result  -   The processing results from async function.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void StartUpCallback(void *arg, WVRResult result)
{
#pragma unused( arg )

    if (result != WVR_RESULT_SUCCESS)
    {
        OS_TPanic("WVR_StartUpAsync error.[%08xh]\n", result);
    }
    startCheck = 1;
}

/*---------------------------------------------------------------------------*
  Name:         ModeConnect

  Description:  connection start

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static void ModeConnect(void)
{
#define USE_DIRECT_CONNECT

    // When connecting directly without parent rescan.
#ifdef USE_DIRECT_CONNECT
    //********************************
    (void)WH_ChildConnect(WH_CONNECTMODE_DS_CHILD, &gMBParentBssDesc);
    // WH_ChildConnect(WH_CONNECTMODE_MP_CHILD, &gMBParentBssDesc, TRUE);
    // WH_ChildConnect(WH_CONNECTMODE_KS_CHILD, &gMBParentBssDesc, TRUE);
    //********************************
#else
    WH_SetGgid(gMBParentBssDesc.gameInfo.ggid);
    // When using parent rescan    
    //********************************
    (void)WH_ChildConnectAuto(WH_CONNECTMODE_DS_CHILD, gMBParentBssDesc.bssid,
                              gMBParentBssDesc.channel);
    // WH_ChildConnect(WH_CONNECTMODE_MP_CHILD, &gMBParentBssDesc, TRUE);
    // WH_ChildConnect(WH_CONNECTMODE_KS_CHILD, &gMBParentBssDesc, TRUE);
    //********************************
#endif
}

/*---------------------------------------------------------------------------*
  Name:         ModeError

  Description:  Error acknowledge screen display   

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static void ModeError(void)
{
    BgPrintStr(5, 10, 0x1, "======= ERROR! =======");

    if (WH_GetLastError() == WM_ERRCODE_OVER_MAX_ENTRY)
    {
        BgPrintStr(5, 13, 0xf, "OVER_MAX_ENTRY");
    }
}

/*---------------------------------------------------------------------------*
  Name:         ModeWorking

  Description:  Displays working screen    

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static void ModeWorking(void)
{
    BgPrintStr(9, 11, 0xf, "Now working...");

    if (IS_PAD_TRIGGER(PAD_BUTTON_START))
    {
        //********************************
        (void)WH_Finalize();
        //********************************
    }
}
