/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - demos - waitIrq-2
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.7  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.6  03/03/2005 13:02:59  takano_makoto
  exclude any code which is available in specific compiler.

  Revision 1.5  02/28/2005 05:26:20  yosizaki
  do-indent.

  Revision 1.4  12/08/2004 05:18:04  yada
  use timer interrupt instead of key interrupt

  Revision 1.3  08/06/2004 12:45:46  yada
  change key interrupt logic enum

  Revision 1.2  08/06/2004 05:14:04  yada
  PAD_SetIrq -> PADi_SetIrq, PAD_ClearIrq -> PADi_ClearIrq

  Revision 1.1  05/20/2004 00:54:45  yada
  initial release

  Revision 1.2  04/30/2004 05:02:25  yada
  added comment

  Revision 1.1  04/30/2004 04:37:56  yada
  first release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

void    TimerIntr(void);
void    proc1(void *p1);
void    proc2(void *p1);

#define    STACK_SIZE     1024
#define    THREAD1_PRIO   17
#define    THREAD2_PRIO   18
OSThread thread1;
OSThread thread2;
u32     stack1[STACK_SIZE / sizeof(u32)];
u32     stack2[STACK_SIZE / sizeof(u32)];

/* ----------------
 * Make one of the defines below valid.
 *     Then, check how their operations differ.   */

//#define WAIT_TIMER
#define WAIT_ANY

/*  The two threads thread1 and thread2 stop and wait for the IRQ.
 *  If WAIT_TIMER is enabled, only thread1 will run.    
 *  If WAIT_ANY is enabled, both thread1 and thread2 will run.
 *  This is because for WAIT_TIMER, after thread1 restores from OS_WaitIrq() due to a timer interrupt,
 *  thread1 again goes into OS_WaitIrq() after the interrupt check flag is cleared.
 *  proc2 will go into an operable state, but since the key interrupt check flag is already cleared,
 *  it cannot get out of OS_WaitIrq().
 *
 *  In case of WAIT_ANY, the moment interrupt occurs, thread1 and thread2 become executable.
 *   So, when thread1 comes for the next OS_WaitAnyIrq(),
 *  thread2 can run.
 */

// Check if either one was enabled
#if (defined(WAIT_TIMER) && defined(WAIT_ANY)) || (!defined(WAIT_TIMER) && !defined(WAIT_ANY))
*****ERROR.define either from WAIT_TIMER or WAIT_ANY.
#endif
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void NitroMain()
{
    OS_Init();
    OS_StartTimer(OS_TIMER_0, 0xff, OS_TIMER_PRESCALER_1024);

    //---- timer interrupt setting
    (void)OS_SetIrqFunction(OS_IE_TIMER0, TimerIntr);
    (void)OS_EnableIrqMask(OS_IE_TIMER0);
    (void)OS_EnableIrq();

    //---- create and wakeup threads
    OS_InitThread();
    OS_CreateThread(&thread1, proc1, NULL, stack1 + STACK_SIZE / sizeof(u32), STACK_SIZE,
                    THREAD1_PRIO);
    OS_CreateThread(&thread2, proc2, NULL, stack2 + STACK_SIZE / sizeof(u32), STACK_SIZE,
                    THREAD2_PRIO);
    OS_WakeupThreadDirect(&thread1);
    OS_WakeupThreadDirect(&thread2);

    OS_Printf("please push any button.\n");

    //---- idle loop
    while (1)
    {
        OS_SleepThread(NULL);
        // never wake up
    }

    // never reach
}

//--------------------------------------------------------------------------------
//    Timer interrupt handler
//
void TimerIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_TIMER0);
}

//--------------------------------------------------------------------------------
//    proc1
//
void proc1(void *p1)
{
#pragma unused(p1)
    int     n = 0;

    while (1)
    {
        OS_Printf("thread 1\n");
#ifdef WAIT_TIMER
        OS_WaitIrq(TRUE, OS_IE_TIMER0);
#endif
#ifdef WAIT_ANY
        OS_WaitAnyIrq();
#endif
    }
}

//--------------------------------------------------------------------------------
//    proc2
//
void proc2(void *p1)
{
#pragma unused(p1)
    int     n = 0;
    while (1)
    {
        OS_Printf("thread 2\n");
#ifdef WAIT_TIMER
        OS_WaitIrq(TRUE, OS_IE_TIMER0);
#endif
#ifdef WAIT_ANY
        OS_WaitAnyIrq();
#endif
    }
}


/*====== End of main.c ======*/
