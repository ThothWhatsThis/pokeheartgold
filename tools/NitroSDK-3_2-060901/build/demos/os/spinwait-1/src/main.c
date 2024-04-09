/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - demos - spinwait-1
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.6  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.5  02/28/2005 05:26:20  yosizaki
  do-indent.

  Revision 1.4  04/15/2004 06:51:12  yada
  only arranged comment

  Revision 1.3  04/06/2004 02:18:05  yada
  OS_Halt() -> OS_Terminate()

  Revision 1.2  03/08/2004 08:31:58  yada
  fixed top comment

  Revision 1.1  2004/02/27 01:55:51  yasu
  added demo for OS_SpinWait

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

#define TicksToCPUCycles( tick )        ((tick)*64*2)

//================================================================================
/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void NitroMain()
{
    OSTick  begin, end;
    u32     n;

    OS_Init();
    OS_InitTick();

    n = 1000;
    for (n = 1000; n <= 1000000; n *= 2)
    {
        begin = OS_GetTick();
        OS_SpinWait(n);
        end = OS_GetTick();

        OS_Printf("OS_SpinWait(%d)  Wait=%lld cycles\n", n, TicksToCPUCycles(end - begin));
    }

    OS_Printf("==== Finish sample.\n");
    OS_Terminate();
}

/*====== End of main.c ======*/
