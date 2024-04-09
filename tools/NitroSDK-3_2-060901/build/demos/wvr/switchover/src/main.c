/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WVR - demos - switchover
  File:     main.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.4  01/18/2006 02:11:21  kitase_hirotake
  do-indent

  Revision 1.3  11/21/2005 11:08:15  kitase_hirotake
  Deleted OS_InitPrintServer

  Revision 1.2  02/28/2005 05:26:12  yosizaki
  do-indent.

  Revision 1.1  02/16/2005 12:47:54  yosizaki
  initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro.h>
#include "common.h"


/*---------------------------------------------------------------------------*
    function definitions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         InitializeAllocateSystem

  Description:  Initializes the memory allocation system within the main memory arena.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitializeAllocateSystem(void)
{
    void   *tempLo;
    OSHeapHandle hh;

    // Based on the premise that OS_Init is already called
    tempLo = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
    OS_SetArenaLo(OS_ARENA_MAIN, tempLo);
    hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
    if (hh < 0)
    {
        OS_Panic("ARM9: Fail to create heap...\n");
    }
    hh = OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
}

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  Initialization and main loop.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    /* library initialization*/
    OS_Init();
    FX_Init();
    GX_Init();

    /* Memory allocation*/
    InitializeAllocateSystem();

    {
        BOOL    bSwitch = FALSE;
        for (;; bSwitch = !bSwitch)
        {
            if (bSwitch)
            {
                GraphicMain();
            }
            else
            {
                WirelessMain();
            }
            (void)OS_DisableIrq();
            (void)OS_DisableInterrupts();

            (void)GX_DisableBankForLCDC();
            (void)GX_DisableBankForTex();
            (void)GX_DisableBankForTexPltt();
            (void)GX_DisableBankForBG();
            (void)GX_DisableBankForOBJ();
            (void)GX_DisableBankForSubBG();
            (void)GX_DisableBankForSubOBJ();

        }
    }
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
