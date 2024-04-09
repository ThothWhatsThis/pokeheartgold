/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - demos - exceptionDisplay-1
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.20  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.19  08/10/2005 05:01:19  yada
  remove OS_InitThread()

  Revision 1.18  06/27/2005 11:10:12  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.17  02/28/2005 05:26:19  yosizaki
  do-indent.

  Revision 1.16  2004/10/04 00:34:45  yasu
  Add INDENT-OFF/ON to control code beautifier

  Revision 1.15  09/28/2004 12:19:54  yada
  not call OS_InitException() because OS_Init() called

  Revision 1.14  06/08/2004 00:18:00  yada
  add '#pragma unused()' for FINALROM

  Revision 1.13  06/03/2004 07:21:33  yada
  sorry, mistake update. back to 1.11. ignore update 1.12.

  Revision 1.12  06/03/2004 06:59:41  yada
  fix indent processing

  Revision 1.11  04/15/2004 06:51:12  yada
  only arranged comment

  Revision 1.10  03/30/2004 08:38:22  yada
  Minor Revisions

  Revision 1.9  03/17/2004 08:18:38  yada
  #if -> #ifdef

  Revision 1.8  03/11/2004 23:48:42  yada
  Removed pseudo command test code

  Revision 1.7  03/11/2004 11:26:56  yada
  Avoid CW thumb compile bug

  Revision 1.6  03/09/2004 04:13:14  yada
  Cache considerations

  Revision 1.5  03/09/2004 01:20:22  yada
  fixed top comment


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

void    OS_InitException(void);
void    myExceptionCallback(u32 context, void *arg);

//================================================================================
/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
void NitroMain()
{
    OS_Init();

    //OS_InitException();   /*called in OS_Init()*/

    OS_SetUserExceptionHandler(myExceptionCallback, (void *)0);


    //---- Flushing cache here because
    //     The protection unit changes when an exception is generated.
    //     Therefore, want to reliably write addresses, etc related to the exception.
    DC_FlushAll();
    DC_WaitWriteBufferEmpty();


    //---- not occurred exception div0
    {
        int     a = 1;
        int     b = 0;
        volatile int c = a / b;
    }

    //---- force to occur exception
    OS_Printf("now force to occur exception...\n");
    asm
    {
        /* *INDENT-OFF**/
        ldr      r0, =123
        ldr      r1, =0
        ldr      r2, =256
        ldr      r3, =1000
        ldr      r4, =2000
        ldr      r5, =3000
        ldr      r6, =4000
        ldr      r7, =5000
        mov      r8, pc
        ldr      r9, =0xff
        ldr      r10, =0xee
        ldr      r12, =0x123

#ifdef SDK_ARM9
        ldr      r11, [r1,#0]
#else  //SDK_ARM9
        dcd      0x06000010

#endif //SDK_ARM9
        
        /* *INDENT-ON**/
    }
    OS_Printf("not occurred exception.\n");

    OS_Terminate();
}

/*---------------------------------------------------------------------------*
  Name:         myExceptionCallback

  Description:  user callback for exception

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void myExceptionCallback(u32 context, void *arg)
{
#ifdef SDK_FINALROM
#pragma unused( context, arg )
#endif

    OS_Printf("---------------- USER EXCEPTION CALLBACK\n");
    OS_Printf("context=%x  arg=%x\n", context, arg);

    OS_Printf("---------------- Thread LIST\n");
    OS_DumpThreadList();
}

#include <nitro/codereset.h>

/*====== End of main.c ======*/
