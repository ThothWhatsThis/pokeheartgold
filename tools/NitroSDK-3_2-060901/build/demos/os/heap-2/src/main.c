/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - demos - heap-2
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.10  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.9  02/28/2005 05:26:20  yosizaki
  do-indent.

  Revision 1.8  09/28/2004 12:22:10  yada
  not call OS_InitArena() because OS_Init() called

  Revision 1.7  04/15/2004 06:51:12  yada
  only arranged comment

  Revision 1.6  04/14/2004 00:13:30  yada
  Added message for when stopping by debug build

  Revision 1.5  03/08/2004 08:31:58  yada
  fixed top comment

  Revision 1.4  02/12/2004 11:41:07  yada
  Worked on distribution of processing for ARM9/ARM7.

  Revision 1.3  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.2  01/09/2004 09:18:55  yada
  Just modified comment

  Revision 1.1  01/07/2004 07:34:19  yada
  Initial release.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

//----------------------------------------------------------------
#define   MAIN_HEAP_SIZE    0x2000

//================================================================================
//        Sample of heap
//        Create heap for arena of main memory
//        Create several heap, and destroy half    
//        Try to allocate from destroyed heap.    
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

    OS_Init();
    //OS_InitArena(); /*called in OS_Init()*/
    OS_Printf("arena lo = %x\n", OS_GetMainArenaLo());
    OS_Printf("arena hi = %x\n", OS_GetMainArenaHi());


    //---- Initialize memory allocation system for MainRAM arena
    nstart = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 2);
    OS_SetMainArenaLo(nstart);

    //---- Allocate region for heap from arena
    heapStart[0] = OS_AllocFromMainArenaLo(MAIN_HEAP_SIZE, 32);
    OS_Printf("heapStart (0) %x\n", heapStart[0]);

    //---- Create heap 0
    handle[0] =
        OS_CreateHeap(OS_ARENA_MAIN, heapStart[0], (void *)((u32)heapStart[0] + MAIN_HEAP_SIZE));
    OS_Printf("heap handle (0) %d\n", handle[0]);

    //---- Allocate region for heap from arena
    heapStart[1] = OS_AllocFromMainArenaLo(MAIN_HEAP_SIZE, 32);
    OS_Printf("heapStart (1) %x\n", heapStart[1]);

    //---- Create heap 1
    handle[1] =
        OS_CreateHeap(OS_ARENA_MAIN, heapStart[1], (void *)((u32)heapStart[1] + MAIN_HEAP_SIZE));
    OS_Printf("heap handle (1) %d\n", handle[1]);

    //---- Set current heap (heap 0)
    (void)OS_SetCurrentHeap(OS_ARENA_MAIN, handle[0]);

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
    OS_DumpHeap(OS_ARENA_MAIN, handle[0]);
    OS_DumpHeap(OS_ARENA_MAIN, handle[1]);


    //---- Try to free region    
    OS_Free(area[1]);
    OS_Free(area[3]);

    //---- Display heap information
    OS_DumpHeap(OS_ARENA_MAIN, handle[0]);
    OS_DumpHeap(OS_ARENA_MAIN, handle[1]);

    //---- Destroy heap 0
    OS_DestroyHeap(OS_ARENA_MAIN, handle[0]);
    OS_Printf("---------------- heap 0 destroyed\n");

    //---- Display heap information
    OS_DumpHeap(OS_ARENA_MAIN, handle[0]);
    OS_DumpHeap(OS_ARENA_MAIN, handle[1]);

    //---- Try to allocate region from destroyed heap 0(should be failure)    
#ifdef SDK_DEBUG
    OS_Printf
        ("**** Next 'OS_AllocFromHeap' will be stopped by assertion in case of DEBUG-build. ***\n");
#endif
    area[4] = OS_AllocFromHeap(OS_ARENA_MAIN, 0, 64);
    OS_Printf("\narea[4] %x\n", area[4]);

    //---- Create heap again (heap 0)
    handle[0] =
        OS_CreateHeap(OS_ARENA_MAIN, heapStart[0], (void *)((u32)heapStart[0] + MAIN_HEAP_SIZE));
    OS_Printf("---------------- heap 0 re-created\n");
    OS_Printf("heap handle (0) %d\n", handle[0]);

    //---- Allocate region
    area[5] = OS_AllocFromHeap(OS_ARENA_MAIN, 0, 64);
    OS_Printf("\narea[5] %x\n", area[5]);

    //---- Display heap information
    OS_DumpHeap(OS_ARENA_MAIN, handle[0]);
    OS_DumpHeap(OS_ARENA_MAIN, handle[1]);

    OS_Printf("==== Finish sample.\n");
    OS_Terminate();
}

/*====== End of main.c ======*/
