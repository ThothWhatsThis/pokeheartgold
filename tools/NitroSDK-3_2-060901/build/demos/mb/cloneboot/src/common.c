/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - demos - cloneboot
  File:     common.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: common.c,v $
  Revision 1.3  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.2  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.1  11/15/2004 06:21:18  yosizaki
  initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include "common.h"
#include "disp.h"


static void VBlankIntr(void);

/*
 * The common features used on this demo as a whole    
 */
static u16 padPress;
static u16 padTrig;

/*---------------------------------------------------------------------------*
  Name:         ReadKey

  Description:  Processing for key reading

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void ReadKey(void)
{
    u16     currData = PAD_Read();

    padTrig = (u16)(~padPress & currData);
    padPress = currData;
}

/*---------------------------------------------------------------------------*
  Name:         GetPressKey

  Description:  Get down key    

  Arguments:    None

  Returns:      Get bitmap for down key    
 *---------------------------------------------------------------------------*/
u16 GetPressKey(void)
{
    return padPress;
}


/*---------------------------------------------------------------------------*
  Name:         GetTrigKey

  Description:  Get key trigger

  Arguments:    None

  Returns:      Get bitmap for key trigger    
 *---------------------------------------------------------------------------*/
u16 GetTrigKey(void)
{
    return padTrig;
}


/*---------------------------------------------------------------------------*
  Name:         CommonInit

  Description:  Performs common initialization    

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void CommonInit(void)
{
    /* OS  initialization*/
    OS_Init();
    OS_InitTick();
    OS_InitAlarm();
    FX_Init();

    /* GX  initialization*/
    GX_Init();
    GX_DispOff();
    GXS_DispOff();

    /* V-blank interrupt configuration*/
    (void)OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrqMask(OS_IE_FIFO_RECV);
    (void)GX_VBlankIntr(TRUE);

    {                                  /* FS initialization*/
        static u32 fs_tablework[0x100 / 4];
        FS_Init(FS_DMA_NOT_USE);
        (void)FS_LoadTable(fs_tablework, sizeof(fs_tablework));
    }

    // Dummy call of read key    
    ReadKey();
}


/*---------------------------------------------------------------------------*
  Name:         InitAllocateSystem

  Description:  Initializes the memory allocation system within the main memory arena.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void InitAllocateSystem(void)
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
  Name:         VBlankIntr

  Description:  Get key trigger

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    DispVBlankFunc();

    //---- Interrupt check flag
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}
