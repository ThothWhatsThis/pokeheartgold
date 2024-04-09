/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - demos - thread-3
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.13  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.12  03/03/2005 12:59:09  takano_makoto
  exclude any code which is available in specific compiler.

  Revision 1.11  02/28/2005 05:26:20  yosizaki
  do-indent.

  Revision 1.10  04/15/2004 06:51:12  yada
  only arranged comment

  Revision 1.9  03/08/2004 08:31:58  yada
  fixed top comment

  Revision 1.8  02/26/2004 05:51:46  yada
  Added argument for starting function to OS_CreateThread().
  Revisions that related to that.

  Revision 1.7  02/12/2004 11:41:07  yada
  Worked on distribution of processing for ARM9/ARM7.

  Revision 1.6  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.5  12/25/2003 07:32:21  yada
  Changes as per unified type rules

  Revision 1.4  11/30/2003 06:34:00  yada
  changed OS_CreateThread() function    

  Revision 1.3  11/26/2003 02:33:37  yada
  (none)

  Revision 1.2  11/26/2003 02:08:58  yada
  (none)

  Revision 1.1  11/26/2003 02:04:03  yada
  Initial release.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

#define STACK_SIZE  1024

void    proc1(void *p1);
void    proc2(void *p1);

OSThread thread1;
OSThread thread2;

OSMessage mesgBuffer[10];
OSMessageQueue mesgQueue;


u64     stack1[STACK_SIZE / sizeof(u64)];
u64     stack2[STACK_SIZE / sizeof(u64)];

#define   THREAD1_PRIO  1
#define   THREAD2_PRIO  2

//================================================================================
//  Sample of thread switch using message queue
/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    s32     n;

    OS_InitMessageQueue(&mesgQueue, &mesgBuffer[0], 10);

    OS_InitThread();
    OS_CreateThread(&thread1, proc1, NULL, stack1 + STACK_SIZE / sizeof(u64), STACK_SIZE,
                    THREAD1_PRIO);
    OS_CreateThread(&thread2, proc2, NULL, stack2 + STACK_SIZE / sizeof(u64), STACK_SIZE,
                    THREAD2_PRIO);
    OS_WakeupThreadDirect(&thread1);
    OS_WakeupThreadDirect(&thread2);

    for (n = 0; n < 5; n++)
    {
        OSMessage message;

        OS_Printf("Idle\n");

        message = (OSMessage)0x12345;
        OS_Printf("send message = %x\n", message);
        (void)OS_SendMessage(&mesgQueue, message, OS_MESSAGE_BLOCK);
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
        OSMessage message;

        (void)OS_ReceiveMessage(&mesgQueue, &message, OS_MESSAGE_BLOCK);
        OS_Printf("recv message = %x\n", message);

        OS_WakeupThreadDirect(&thread2);
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
        OS_SleepThread(NULL);
    }
}

/*====== End of main.c ======*/
