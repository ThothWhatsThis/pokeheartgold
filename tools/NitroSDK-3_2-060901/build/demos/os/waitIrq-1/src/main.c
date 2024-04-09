/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - demos - waitIrq-1
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

  Revision 1.3  03/03/2005 13:01:15  takano_makoto
  exclude any code which is available in specific compiler.

  Revision 1.2  02/28/2005 05:26:20  yosizaki
  do-indent.

  Revision 1.1  05/20/2004 00:54:45  yada
  initial release

  Revision 1.3  05/01/2004 05:50:14  yada
  OS_WaitInterrupt->OS_WaitIrq

  Revision 1.2  04/30/2004 05:02:25  yada
  added comment

  Revision 1.1  04/30/2004 04:37:56  yada
  first release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

void    VBlankIntr(void);
void    proc1(void *p1);

#define    STACK_SIZE     1024
#define    THREAD1_PRIO   17
OSThread thread1;
u32     stack1[STACK_SIZE / sizeof(u32)];

//================================================================================
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
    OS_WakeupThreadDirect(&thread1);

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
        // while waiting V-Blank, lower priority thread runs and display 'X'.
        // if V-Blank interrupt occurred, return here immediately.

        OS_WaitIrq(1, OS_IE_V_BLANK);
        OS_Printf("Idle thread n=%d\n", n);
    }

    OS_Printf("==== Finish sample.\n");
    OS_Terminate();
}

//--------------------------------------------------------------------------------
//    V-Blank interrupt handler
//
void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
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
        if (((n++) & 0xff) == 0)
        {
            OS_Printf("X");
        }
    }
}


/*====== End of main.c ======*/
