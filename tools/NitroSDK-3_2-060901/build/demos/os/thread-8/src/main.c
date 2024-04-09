/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - demos - thread-8
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.4  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.3  07/22/2005 01:12:06  adachi_hiroaki
  Pad processing has been split into different threads

  Revision 1.2  07/12/2005 06:42:05  adachi_hiroaki
  Changed specifications for destructor support and so that the system operates smartly even when SDK_THREAD_INFINITY is not present.

  Revision 1.1  07/12/2005 01:57:17  adachi_hiroaki
  initial release


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

#define STACK_SIZE 1536
#define MAIN_HEAP_SIZE 0x100000
#define MESSAGE_QUEUE_SIZE 64

#ifndef SDK_THREAD_INFINITY
const int g_nThreadMax = OS_THREAD_MAX_NUM;
#else
const int g_nThreadMax = 1024;
#endif

OSThreadQueue g_threadQueue;

OSMessage g_msgBuffer[MESSAGE_QUEUE_SIZE];
OSMessageQueue g_msgQueue;

BOOL    g_bExit = FALSE;
int     g_nTotalThreadCount = 0;

void    proc1(void *);
void    proc2(void *);
void    proc3(void *);


void    AllocThreadStorage(OSThread **ppThread, void **ppStackTop);
void    FreeThreadStorage(OSThread *pThread);
void    ManageThreadStorage(void *);
void    ReadPad(void *);

void    RequestFree(void *);

OSThread *SpawnThread(void (*func) (void *), u32 prio);
void    PrepareAlloc();

void NitroMain(void)
{

    OSThread *pThreadManage = NULL;
    int     nThreadStart;

    /* initialization*/
    OS_Init();
    OS_InitThread();
    OS_InitThreadQueue(&g_threadQueue);
    OS_InitMessageQueue(&g_msgQueue, &g_msgBuffer[0], MESSAGE_QUEUE_SIZE);

    /* Initialization required for OS_Sleep()*/
    OS_InitTick();
    OS_InitAlarm();
    (void)OS_EnableIrq();

    PrepareAlloc();

    OS_Printf("==== sample start\n");
    OS_Printf("infinity thread support: ");
#ifdef SDK_THREAD_INFINITY
    OS_Printf("yes\n");
#else
    OS_Printf("no\n");
#endif

    (void)OS_SetThreadPriority(OS_GetCurrentThread(), 31);

    /* Create thread for management*/
    pThreadManage = SpawnThread(ManageThreadStorage, 12);
    OS_Printf("ManageThreadStorage id=%d\n", pThreadManage->id);

    /* Remember number of initial threads (including management thread)*/
    nThreadStart = OS_GetNumberOfThread();

    /*  Create thread to read pad data*/
    (void)SpawnThread(ReadPad, 8);

    /* Create the beginning of a thread that is going to split*/
    (void)SpawnThread(proc1, 16);

    /* Exit if the current number of threads does not match the initial number of threads*/
    while (nThreadStart < OS_GetNumberOfThread())
    {
        (void)OS_DisableScheduler();
        OS_WakeupThread(&g_threadQueue);
        (void)OS_EnableScheduler();
        OS_RescheduleThread();
    }
    /* Terminate management thread*/

#ifdef SDK_THREAD_INFINITY
    OS_KillThread(pThreadManage, NULL);
    OS_JoinThread(pThreadManage);
#else
    OS_DestroyThread(pThreadManage);
    OS_JoinThread(pThreadManage);
    FreeThreadStorage(pThreadManage);
#endif

    OS_Printf("==== Finish sample.\n");
    OS_Terminate();
}

/* Allocate memory required for the thread*/
void AllocThreadStorage(OSThread **ppThread, void **ppStackTop)
{
    /* Check free memory so that warning will not occur*/
    if (OS_GetMaxFreeSize(OS_ARENA_MAIN, OS_CURRENT_HEAP_HANDLE) < STACK_SIZE
        || OS_GetTotalFreeSize(OS_ARENA_MAIN,
                               OS_CURRENT_HEAP_HANDLE) < STACK_SIZE + sizeof(OSThread))
    {
        *ppThread = NULL;
        *ppStackTop = NULL;
        return;
    }
    *ppThread = OS_Alloc(sizeof(OSThread));
    *ppStackTop = OS_Alloc(STACK_SIZE);
}

/* Free memory used to store thread*/
void FreeThreadStorage(OSThread *pThread)
{
    SDK_NULL_ASSERT(pThread);
    OS_Free((void *)pThread->stackTop);
    OS_Free(pThread);
}

/* Allocate the memory required by the thread and create the thread*/
OSThread *SpawnThread(void (*func) (void *), u32 prio)
{
    OSThread *pThread = NULL;
    void   *pStackTop = NULL;
    AllocThreadStorage(&pThread, &pStackTop);
    if (!pThread || !pStackTop || OS_GetNumberOfThread() >= g_nThreadMax)
    {
        return NULL;
    }
    OS_CreateThread(pThread, func, NULL, (void *)((u8 *)pStackTop + STACK_SIZE), STACK_SIZE, prio);

#ifdef SDK_THREAD_INFINITY
    OS_SetThreadDestructor(pThread, RequestFree);
#endif

    /* Although the system is put into execution wait status, context switching is not enforced*/
    (void)OS_DisableScheduler();
    OS_WakeupThreadDirect(pThread);
    (void)OS_EnableScheduler();

    ++g_nTotalThreadCount;
    OS_Printf("spawn     : id=%08x, number=%d(total %d)\n", pThread->id, OS_GetNumberOfThread(),
              g_nTotalThreadCount);
    return pThread;
}

/* Prepare the heap*/
void PrepareAlloc()
{
    void   *heapStart = NULL;
    void   *nstart = NULL;
    OSHeapHandle handle;
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
}

/* Management thread: Free the memory that was being used after a received OSThread* thread has terminated*/
void ManageThreadStorage(void *)
{
    OSMessage message;
    OSThread *pThread = NULL;
    while (TRUE)
    {
        (void)OS_ReceiveMessage(&g_msgQueue, &message, OS_MESSAGE_BLOCK);
        pThread = (OSThread *)message;
        OS_JoinThread(pThread);
        OS_Printf("terminate : id=%08x, number=%d\n", pThread->id, OS_GetNumberOfThread());
        FreeThreadStorage(pThread);
    }
}

/*  */
void ReadPad(void *)
{
    u32     nPadResult = 0, nPadResultPrev = 0;
    while (!g_bExit)
    {
        nPadResultPrev = nPadResult;
        nPadResult = PAD_Read();
        if ((nPadResult ^ nPadResultPrev) & nPadResult & PAD_BUTTON_A)
        {
            /* Display thread dump using the A button*/
            OS_DumpThreadList();
        }
        if (nPadResult & PAD_BUTTON_A)
        {
            /* Pause while the A button is held down*/
            continue;
        }
        else if (nPadResult & PAD_BUTTON_B)
        {
            /* Quit thread creation*/
            g_bExit = TRUE;
        }
        OS_Sleep(100);
    }
#ifndef SDK_THREAD_INFINITY
    RequestFree(NULL);
#endif
}

/* Have the management thread free memory used by its own thread*/
void RequestFree(void *)
{
    OSMessage message = (OSMessage)OS_GetCurrentThread();
    (void)OS_SendMessage(&g_msgQueue, message, OS_MESSAGE_NOBLOCK);
}

//--------------------------------------------------------------------------------
//    proc1
//
void proc1(void *)
{
    OSMessage message = (OSMessage)OS_GetCurrentThread();
    int     i;
    for (i = 0; i < 32; ++i)
    {
        OS_SleepThread(&g_threadQueue);
    }
    if (!g_bExit)
    {
        (void)SpawnThread(proc2, 16);
        (void)SpawnThread(proc3, 16);
    }
#ifndef SDK_THREAD_INFINITY
    RequestFree(NULL);
#endif
}

//--------------------------------------------------------------------------------
//    proc2
//
void proc2(void *)
{
    OSMessage message = (OSMessage)OS_GetCurrentThread();
    int     i;
    for (i = 0; i < 4; ++i)
    {
        OS_SleepThread(&g_threadQueue);
    }
    if (!g_bExit)
    {
        (void)SpawnThread(proc1, 16);
        (void)SpawnThread(proc3, 16);
    }
#ifndef SDK_THREAD_INFINITY
    RequestFree(NULL);
#endif
}

//--------------------------------------------------------------------------------
//    proc3
//
void proc3(void *)
{
    OSMessage message = (OSMessage)OS_GetCurrentThread();
    int     i;
    for (i = 0; i < 64; ++i)
    {
        OS_SleepThread(&g_threadQueue);
    }
    if (!g_bExit)
    {
        (void)SpawnThread(proc1, 16);
        (void)SpawnThread(proc2, 16);
    }
#ifndef SDK_THREAD_INFINITY
    RequestFree(NULL);
#endif
}

/*====== End of main.c ======*/
