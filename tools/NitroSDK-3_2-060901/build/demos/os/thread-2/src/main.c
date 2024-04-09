/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - demos - thread-2
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.12  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.11  03/03/2005 12:57:56  takano_makoto
  exclude any codes which are available in a specific compiler.

  Revision 1.10  02/28/2005 05:26:20  yosizaki
  do-indent.

  Revision 1.9  04/15/2004 06:51:12  yada
  only arranged comment

  Revision 1.8  03/08/2004 08:31:58  yada
  fixed top comment

  Revision 1.7  02/26/2004 05:51:46  yada
  Added argument for starting function to OS_CreateThread().
  Revisions that related to that.

  Revision 1.6  02/19/2004 01:04:43  yada
  added OS_Init()

  Revision 1.5  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.4  12/25/2003 07:32:21  yada
  Changes as per unified type rules

  Revision 1.3  12/11/2003 11:53:36  yada
  modified by change of name   OS_CreateThreadQueue() -> OS_InitThreadQueue()

  Revision 1.2  11/30/2003 06:33:34  yada
  changed OS_CreateThread() function    

  Revision 1.1  2003/11/26 01:38:01  yasu
  Initial version.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

#define STACK_SIZE      1024

void    proc1(void *p1);
void    proc2(void *p1);

OSThread thread1;
OSThread thread2;
OSThreadQueue wakeup_signal;


u64     stack1[STACK_SIZE / sizeof(u64)];
u64     stack2[STACK_SIZE / sizeof(u64)];

#define         THREAD1_PRIO    1
#define         THREAD2_PRIO    2


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
    OS_InitThreadQueue(&wakeup_signal);
    OS_CreateThread(&thread1, proc1, NULL, stack1 + STACK_SIZE / sizeof(u64), STACK_SIZE,
                    THREAD1_PRIO);
    OS_CreateThread(&thread2, proc2, NULL, stack2 + STACK_SIZE / sizeof(u64), STACK_SIZE,
                    THREAD2_PRIO);
    OS_WakeupThreadDirect(&thread1);
    OS_WakeupThreadDirect(&thread2);

    for (n = 0; n < 5; n++)
    {
        OS_Printf("Idle\n");
        OS_WakeupThread(&wakeup_signal);
    }

    OS_Printf("==== Finish sample.\n");
    OS_Terminate();
}

//--------------------------------------------------------------------------------
//    proc1
//
void proc1(void *p1)
{
#pragma unused(p1)
    while (1)
    {
        OS_Printf("Thread1\n");
        OS_SleepThread(&wakeup_signal);
    }
}

//--------------------------------------------------------------------------------
//    proc2
//
void proc2(void *p1)
{
#pragma unused(p1)
    while (1)
    {
        OS_Printf("Thread2\n");
        OS_SleepThread(&wakeup_signal);
    }
}

/*====== End of main.c ======*/
