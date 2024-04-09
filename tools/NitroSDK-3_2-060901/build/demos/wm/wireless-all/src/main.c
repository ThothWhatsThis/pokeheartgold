/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - wireless-all
  File:     main.c

  Copyright 2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.2  04/10/2006 13:37:12  yosizaki
  beautify source code indent.

  Revision 1.1  04/10/2006 13:07:32  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include "common.h"

/*
 * a sample application that implements all of the following wireless functionality
 *
 * - DS Download Play clone booting
 * - data sharing
 * - using the file system via a wireless connection
 *
 * Because the MB library samples use the multiboot functionality,
 * development tools of the same communications environment (wired or wireless) are required.
 * The mb_child.bin program in the * $NitroSDK/bin/ARM9-TS/Release/ directory
 * is a demo that provides the same functionality as
 * that of the DS system.
 * Load this binary on the other tools as you would as demo program,
 * and run them together.
 *
 */

/******************************************************************************/
/* function*/

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  Main routine

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    /* common system initialization*/
    InitCommon();

    /* overall status transitioning*/
    for (;;)
    {
        /* DS Download Play from the parent*/
        if (!MB_IsMultiBootChild())
        {
            ExecuteDownloadServer();
        }
        /* wireless initialization and parent/child startup processes*/
        if (!MB_IsMultiBootChild())
        {
#if !defined(MBP_USING_MB_EX)
            (void)WH_Initialize();
#endif
            WaitWHState(WH_SYSSTATE_IDLE);
            StartWirelessParent();
        }
        else
        {
            (void)WH_Initialize();
            WaitWHState(WH_SYSSTATE_IDLE);
            StartWirelessChild();
        }
        /* data sharing main processes*/
        ExecuteDataSharing();
        (void)WH_End();
        WaitWHState(WH_SYSSTATE_STOP);
    }

    /* the controls do not extend beyond this point*/
    OS_Terminate();
}
