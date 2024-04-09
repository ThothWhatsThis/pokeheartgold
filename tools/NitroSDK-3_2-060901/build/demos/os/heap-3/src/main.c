/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - demos - heap-3
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.8  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.7  02/28/2005 05:26:20  yosizaki
  do-indent.

  Revision 1.6  09/28/2004 12:22:10  yada
  not call OS_InitArena() because OS_Init() called

  Revision 1.5  04/15/2004 06:51:12  yada
  only arranged comment

  Revision 1.4  03/08/2004 08:31:58  yada
  fixed top comment

  Revision 1.3  02/12/2004 11:41:07  yada
  Worked on distribution of processing for ARM9/ARM7.

  Revision 1.2  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.1  01/07/2004 08:06:54  yada
  Initial release.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

//----------------------------------------------------------------
#define   MAIN_HEAP_SIZE    0x2000

//================================================================================
//        Sample of heap
//        Create heap for arena of main memory
//        Create discontinuous heap using OS_AllocFixed
/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    void   *heapStart[2];
    void   *nstart;
    OSHeapHandle handle[2];
    void   *area[10];

    void   *fixed;
    void   *fixedStart;
    void   *fixedEnd;

    OS_Init();
    //OS_InitArena(); /*called in OS_Init()*/
    OS_Printf("arena lo = %x\n", OS_GetMainArenaLo());
    OS_Printf("arena hi = %x\n", OS_GetMainArenaHi());


    //---- Initialize memory allocation system for MainRAM arena
    nstart = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 2);
    OS_SetMainArenaLo(nstart);



    //==== Heap 0====
    //---- Allocate region for heap from arena
    heapStart[0] = OS_AllocFromMainArenaLo(MAIN_HEAP_SIZE, 32);
    OS_Printf("heapStart (0) %x\n", heapStart[0]);

    //---- Create heap
    handle[0] =
        OS_CreateHeap(OS_ARENA_MAIN, heapStart[0], (void *)((u32)heapStart[0] + MAIN_HEAP_SIZE));
    OS_Printf("heap handle (0) %d\n", handle[0]);



    //==== Heap 1====
    //---- Allocate region for heap from arena
    heapStart[1] = OS_AllocFromMainArenaLo(MAIN_HEAP_SIZE, 32);
    OS_Printf("heapStart (1) %x\n", heapStart[1]);

    //---- Create heap
    handle[1] =
        OS_CreateHeap(OS_ARENA_MAIN, heapStart[1], (void *)((u32)heapStart[1] + MAIN_HEAP_SIZE));
    OS_Printf("heap handle (1) %d\n", handle[1]);


    //---- Get part of region of heap 1 using OS_FixedAlloc
    OS_Printf("---------------- now alloc fixed area\n");
    fixedStart = (void *)((u32)heapStart[1] + 0x300);
    fixedEnd = (void *)((u32)heapStart[1] + 0x500);
    fixed = OS_AllocFixed(OS_ARENA_MAIN, &fixedStart, &fixedEnd);
    OS_Printf("fixed area %x  (%x-%x)\n", fixed, fixedStart, fixedEnd);

    //---- Display heap information
    OS_DumpHeap(OS_ARENA_MAIN, handle[0]);
    OS_DumpHeap(OS_ARENA_MAIN, handle[1]);


    //---- Add Fixed region to heap 0
    OS_Printf("---------------- now add to heap...\n");
    OS_AddToHeap(OS_ARENA_MAIN, handle[0], fixedStart, fixedEnd);

    //---- Display heap information
    OS_DumpHeap(OS_ARENA_MAIN, handle[0]);
    OS_DumpHeap(OS_ARENA_MAIN, handle[1]);



    //---- Set current heap
    (void)OS_SetCurrentHeap(OS_ARENA_MAIN, handle[0]);


    //---- Try to allocate region    
    area[0] = OS_Alloc(0x1f00);
    area[1] = OS_Alloc(0x100);
    OS_Printf("area[0] %x\n", area[0]);
    OS_Printf("area[1] %x\n", area[1]);

    //---- Display heap information
    OS_DumpHeap(OS_ARENA_MAIN, handle[0]);
    OS_DumpHeap(OS_ARENA_MAIN, handle[1]);

    OS_Printf("==== Finish sample.\n");
    OS_Terminate();
}

/*====== End of main.c ======*/
