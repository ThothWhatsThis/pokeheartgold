/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - demos - cloneboot
  File:     main.c

  Copyright 2003-2005 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.4  2005/02/28 05:26:02  yosizaki
  do-indent.

  Revision 1.3  2004/11/24 11:07:05  yosizaki
  change ParentIdentifier to ParentMain.

  Revision 1.2  2004/11/24 10:17:32  yasu
  Untabify

  Revision 1.1  2004/11/15 06:21:18  yosizaki
  initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include "common.h"

/*
 * Sample application that uses clone boot.  
 *
 * To use the multiboot functionality, the MB library samples require several development  
 * machines having the same communication environment (wired or wireless).  
 * Since the mb_child.bin program in the $NitroSDK/bin/ARM9-TS/Release/ directory
 * is a sample that provides the same functionality as the  
 * multiboot child on the final production model.  
 * Load this binary into another machine using the same method used with
 * the sample program and run them together.
 *
 */

/******************************************************************************/



//============================================================================
//   Function Definitions
//============================================================================

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  Main routine

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    /*
     * This sample uses the multiboot-Model sample demo as-is,  
     * and processes differently depending on whether the  
     * machine has a DS download play child program or not.  
     *
     * With clone boot, the main differences between the parent and child environments are:
     The child cannot access the card;
     The code exclusive to the parent must be under 8KB.  
     The procedure given for wireless communications is where the
     * parent device and child device are different.
     *
     * If you follow the above guidelines and create a wrapper  * that fits your application, you can create an efficient program that  
     * uses the same code for most parent and child processes, and  
     * saves card ROM space while supporting both individual and  * multiplayer games.  
     If the parent and child machines do not have  * any shared elements for DS download play, then you will  
     * not be able to do this.  
     */
    if (!MB_IsMultiBootChild())
    {
        ParentMain();
    }
    else
    {
        ChildMain();
    }

    /* the controls do not extend beyond this point */
}

