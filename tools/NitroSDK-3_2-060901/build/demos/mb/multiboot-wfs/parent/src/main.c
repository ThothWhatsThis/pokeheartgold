/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - demos - multiboot-wfs
  File:     main.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.2  08/09/2005 14:55:09  yosizaki
  fix comment

  Revision 1.1  06/23/2005 10:06:03  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
    This is a  sample program for the child during the wireless multibooting to use the file system by
	operating WBT.

    HOW TO:
        1. To start communications as a parent, press the A button.
           When a child is found in the surrounding area with the same multiboot-wfs demo, communication
           automatically begins. Communication with up to 15 children is possible at the same time.
        2. For info on various functions during connection, refer to parent/child screens
           and the comments in the source code.
 *---------------------------------------------------------------------------*/


#include <nitro.h>
#include <nitro/wm.h>
#include <nitro/wbt.h>
#include <nitro/fs.h>

#include    "mbp.h"
#include    "wfs.h"
#include    "wc.h"

#include    "util.h"
#include    "common.h"


/*---------------------------------------------------------------------------*
    Function Definitions
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  Initialization and main loop.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    /* Initialize render framework for sample*/
    UTIL_Init();

    /*
     * Initializes the file system.
     * The available DMA channel can be specified for the parent.
     * This is simply ignored with the child as it is not used.
     */
    FS_Init(FS_DMA_NOT_USE);

    /* Starts the LCD display*/
    GX_DispOn();
    GXS_DispOn();

    /* State transition main loop*/
    for (;;)
    {
        /* If previous WFS is already running, stop it once.*/
        if (WFS_GetStatus() != WFS_STATE_STOP)
        {
            WFS_End();
            WcFinish();
        }

        /* Start DS download play*/
        ModeMultiboot();

        /* Set the mode and start the parent*/
        ModeSelect();

        /* Wait until parent startup is complete*/
        ModeWorking();

        /* If started, display parent screen*/
        ModeParent();

        /* Wait until parent termination is complete*/
        ModeWorking();

        /* If there is an error, stop here.*/
        ModeError();
    }
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
