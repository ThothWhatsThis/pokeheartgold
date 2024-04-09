/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - demos - heap-1
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.9  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.8  02/28/2005 05:26:20  yosizaki
  do-indent.

  Revision 1.7  09/28/2004 12:22:10  yada
  not call OS_InitArena() because OS_Init() called

  Revision 1.6  04/15/2004 06:51:12  yada
  only arranged comment

  Revision 1.5  03/08/2004 08:31:58  yada
  fixed top comment

  Revision 1.4  02/12/2004 11:41:07  yada
  Worked on distribution of processing for ARM9/ARM7.

  Revision 1.3  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.2  01/09/2004 09:18:31  yada
  Changed display of ending part

  Revision 1.1  01/07/2004 06:39:50  yada
  Initial release.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

//----------------------------------------------------------------
#define   MAIN_HEAP_SIZE    0x2000

//================================================================================
//        Comprehensive sample of heap
//        Create a heap for the main memory arena,
//        and attempt region allocation / deallocation.
/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    void   *heapStart;
    void   *nstart;
    OSHeapHandle handle;
    void   *area[10];

    OS_Init();
    //OS_InitArena(); /*called in OS_Init()*/
    OS_Printf("arena lo = %x\n", OS_GetMainArenaLo());
    OS_Printf("arena hi = %x\n", OS_GetMainArenaHi());


    //---- Initialize memory allocation system for MainRAM arena
    nstart = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 2);
    OS_SetMainArenaLo(nstart);

    //---- Allocate region for heap from arena
    heapStart = OS_AllocFromMainArenaLo(MAIN_HEAP_SIZE, 32);
    OS_Printf("heapStart %x\n", heapStart);

    //---- Create heap
    handle = OS_CreateHeap(OS_ARENA_MAIN, heapStart, (void *)((u32)heapStart + MAIN_HEAP_SIZE));
    OS_Printf("heap handle %d\n", handle);

    //---- Set current heap
    (void)OS_SetCurrentHeap(OS_ARENA_MAIN, handle);

    //---- Try to allocate region    
    area[0] = OS_Alloc(32);
    area[1] = OS_Alloc(32);
    area[2] = OS_Alloc(32);
    area[3] = OS_Alloc(64);

    OS_Printf("area[0] %x\n", area[0]);
    OS_Printf("area[1] %x\n", area[1]);
    OS_Printf("area[2] %x\n", area[2]);
    OS_Printf("area[3] %x\n", area[3]);

    //---- Display heap information
    OS_DumpHeap(OS_ARENA_MAIN, handle);


    //---- Try to free region    
    OS_Free(area[1]);
    OS_Free(area[3]);

    //---- Display heap information
    OS_DumpHeap(OS_ARENA_MAIN, handle);

    OS_Printf("==== Finish sample.\n");
    OS_Terminate();
}

/*====== End of main.c ======*/
