/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - demos - thread-4
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.14  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.13  11/21/2005 10:56:02  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.12  03/03/2005 13:00:06  takano_makoto
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

  Revision 1.7  02/24/2004 00:05:20  yada
  (none)

  Revision 1.6  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.5  02/02/2004 05:34:12  yada
  OS_SetIrqMask() -> OS_EnableIrqMask()

  Revision 1.4  01/18/2004 06:49:20  yada
  (none)

  Revision 1.3  01/09/2004 10:53:49  yada
  rewrote set function for interrupt check flag

  Revision 1.2  12/25/2003 07:32:21  yada
  Changes as per unified type rules

  Revision 1.1  2003/12/16 06:30:36  yasu
  Sample of thread switch processing by thread-4 interrupt

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

void    VBlankIntr(void);

#define    STACK_SIZE     1024
#define    THREAD1_PRIO   1
#define    THREAD2_PRIO   2

void    proc1(void *p1);
void    proc2(void *p1);

OSThread thread1;
OSThread thread2;

u32     stack1[STACK_SIZE / sizeof(u32)];
u32     stack2[STACK_SIZE / sizeof(u32)];

//================================================================================
//      Sample of thread switch using interrupt
//      Can make thread sleep/wake up in IRQ handler.
//      However, actual thread switching is postponed until IRQ routine ends.
/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void NitroMain()
{
    s32     n;

    OS_Init();

    //================ Start thread
    OS_InitThread();
    OS_CreateThread(&thread1, proc1, NULL, stack1 + STACK_SIZE / sizeof(u32), STACK_SIZE,
                    THREAD1_PRIO);
    OS_CreateThread(&thread2, proc2, NULL, stack2 + STACK_SIZE / sizeof(u32), STACK_SIZE,
                    THREAD2_PRIO);
    OS_WakeupThreadDirect(&thread1);
    OS_WakeupThreadDirect(&thread2);

    //================ Settings
#ifdef SDK_ARM9
    //---- Initialize graphics
    GX_Init();
#endif // SDK_ARM9

    //----  Enable Vblank interrupt
    (void)OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();

    //---- Occurrence of Vblank setting
    (void)GX_VBlankIntr(TRUE);

    //================ Main loop
    for (n = 0; n < 5; n++)
    {
        //---- Wait for V-blank interrupt completion
        OS_WaitVBlankIntr();

        OS_Printf("Idle\n");
    }

    OS_Printf("==== Finish sample.\n");
    OS_Terminate();
}

//--------------------------------------------------------------------------------
//    V-Blank interrupt processing(processing in IRQ)
//
void VBlankIntr(void)
{
    //---- Interrupt check flag
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);

    //---- Start thread 1
    OS_WakeupThreadDirect(&thread1);
}

//--------------------------------------------------------------------------------
//    proc1
//
void proc1(void *p1)
{
#pragma unused(p1)
    while (1)
    {
        OS_Printf("------- Thread1 sleep\n");
        OS_SleepThread(NULL);
        OS_Printf("------- Thread1 wakeup\n");

        //---- Start thread 2
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
        OS_Printf("--------------- Thread2 sleep\n");
        OS_SleepThread(NULL);
        OS_Printf("--------------- Thread2 wakeup\n");
    }
}

/*====== End of main.c ======*/
