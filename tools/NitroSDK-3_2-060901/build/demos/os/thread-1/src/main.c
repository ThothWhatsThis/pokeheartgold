/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - demos - thread-1
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.19  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.18  02/28/2005 05:26:20  yosizaki
  do-indent.

  Revision 1.17  06/08/2004 00:20:49  yada
  add '#pragma unused()' for FINALROM

  Revision 1.16  04/15/2004 06:51:12  yada
  only arranged comment

  Revision 1.15  03/08/2004 08:31:58  yada
  fixed top comment

  Revision 1.14  02/26/2004 09:43:08  yada
  Added argument for starting function to OS_CreateThread().
  Revisions that related to that.

  Revision 1.13  02/26/2004 05:51:46  yada
  Added argument for starting function to OS_CreateThread().
  Revisions that related to that.

  Revision 1.12  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.11  01/16/2004 11:49:39  yada
  added OS_Init()

  Revision 1.10  2003/12/29 04:27:24  yasu
  workaround for OS_Warning, OS_Panic

  Revision 1.9  12/25/2003 07:32:21  yada
  Changes as per unified type rules

  Revision 1.8  2003/12/10 10:51:16  yasu
  restricted number of output lines to console    

  Revision 1.7  11/30/2003 06:28:40  yada
  changed OS_CreateThread() function    

  Revision 1.6  2003/11/26 01:28:15  yasu
  changed to demo using OS_WakeupThreadDirect    

  Revision 1.5  2003/11/25 13:32:42  yasu
  changed to use ThreadQueue

  Revision 1.4  2003/11/25 11:19:06  yasu
  deleted debug comment

  Revision 1.3  2003/11/25 06:32:40  yasu
  bug fixed

  Revision 1.2  11/25/2003 05:56:39  yada
  (none)

  Revision 1.1  2003/11/25 04:33:22  yasu
  initial version additions

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

#define	STACK_SIZE	1024

void    proc1(void *arg);
void    proc2(void *arg);

OSThread thread1;
OSThread thread2;

u64     stack1[STACK_SIZE / sizeof(u64)];
u64     stack2[STACK_SIZE / sizeof(u64)];

#define		THREAD1_PRIO	10
#define		THREAD2_PRIO	11

//================================================================================
/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    s32     n;

    OS_Init();
    OS_InitThread();
    OS_CreateThread(&thread1, proc1, (void *)0x111, stack1 + STACK_SIZE / sizeof(u64), STACK_SIZE,
                    THREAD1_PRIO);
    OS_CreateThread(&thread2, proc2, (void *)0x222, stack2 + STACK_SIZE / sizeof(u64), STACK_SIZE,
                    THREAD2_PRIO);

    OS_DumpThreadList();


    OS_WakeupThreadDirect(&thread1);
    OS_WakeupThreadDirect(&thread2);

    for (n = 0; n < 5; n++)
    {
        OS_Printf("Idle\n");
        OS_WakeupThreadDirect(&thread2);
    }

    OS_Printf("==== Finish sample.\n");
    OS_Terminate();
}

//--------------------------------------------------------------------------------
//    proc1
//
void proc1(void *arg)
{
#ifdef SDK_FINALROM
#pragma unused( arg )
#endif
    while (1)
    {
        OS_Printf("Thread1 (%x)\n", (u32)arg);
        OS_SleepThread(NULL);
    }
}

//--------------------------------------------------------------------------------
//    proc2
//
void proc2(void *arg)
{
#ifdef SDK_FINALROM
#pragma unused( arg )
#endif
    while (1)
    {
        OS_Printf("Thread2 (%x)\n", (u32)arg);
        OS_SleepThread(NULL);
        OS_WakeupThreadDirect(&thread1);
    }
}

/*====== End of main.c ======*/
