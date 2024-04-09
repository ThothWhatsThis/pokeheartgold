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
  Revision 1.1  06/23/2005 09:05:39  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*
    This is a  sample program so a child, during the wireless multibooting, can use the file system by
	operating WBT.

    HOW TO:
        1. To start communications as a child, press the B button.
           When a parent with the same wbt-fs demo is found in the surrounding area, communication
           automatically begins with that parent. 
        2. For info on various functions during connection, refer to parent/child screens  
           and the comments in the source code.
 *---------------------------------------------------------------------------*/



#include <nitro.h>
#include <nitro/wm.h>
#include <nitro/wbt.h>
#include <nitro/fs.h>

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
     * A parent can specify the available DMA channel.
     * This is simply ignored with the child as it is not used.
     */

    FS_Init(FS_DMA_NOT_USE);

    /* Starts the LCD display*/
    GX_DispOn();
    GXS_DispOn();

    /* Main loop*/
    for (;;)
    {
        /* If previous WFS is already running, stop it once. */
        if (WFS_GetStatus() != WFS_STATE_STOP)
        {
            WFS_End();
            WcFinish();
        }

        /* Begin child startup process.*/
        ModeSelect();

        /* Wait until child startup is complete*/
        ModeWorking();

        /* If started, display child screen*/
        ModeChild();

        /* If there is an error, stop here.*/
        ModeError();
    }
}


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/


