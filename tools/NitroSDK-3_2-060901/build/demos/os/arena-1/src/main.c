/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - demos - arena-1
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.11  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.10  02/28/2005 05:26:19  yosizaki
  do-indent.

  Revision 1.9  04/15/2004 06:51:12  yada
  only arranged comment

  Revision 1.8  03/30/2004 08:38:22  yada
  Minor Revisions

  Revision 1.7  03/08/2004 08:31:58  yada
  fixed top comment

  Revision 1.6  02/13/2004 10:40:36  yada
  Changed from 6 to 9 arenas.

  Revision 1.5  02/12/2004 11:41:07  yada
  Worked on distribution of processing for ARM9/ARM7.

  Revision 1.4  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.3  01/19/2004 02:30:03  yada
  In case that arena of DTCM and MainEX region are none or small, did not alloc.    

  Revision 1.2  01/16/2004 04:13:48  yada
  added support for Extended Main Memory

  Revision 1.1  01/07/2004 06:18:27  yada
  Initial release.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

//----------------------------------------------------------------

#define getArenaSize( id )  (u32)OS_GetArenaHi( (id) ) - (u32)OS_GetArenaLo( (id) )

//================================================================================
/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    void   *nstart;

    OS_Init();
    OS_Printf("================ SAMPLE START ================\n");

    //---- print each arena boundary
    OS_Printf("MainRAM-main arena lo = %x\n", OS_GetMainArenaLo());
    OS_Printf("             arena hi = %x\n", OS_GetMainArenaHi());
    OS_Printf("MainRAM-sub  arena lo = %x\n", OS_GetSubPrivArenaLo());
    OS_Printf("             arena hi = %x\n", OS_GetSubPrivArenaHi());
    OS_Printf("MainEX       arena lo = %x\n", OS_GetMainExArenaLo());
    OS_Printf("             arena hi = %x\n", OS_GetMainExArenaHi());
    OS_Printf("ITCM         arena lo = %x\n", OS_GetITCMArenaLo());
    OS_Printf("             arena hi = %x\n", OS_GetITCMArenaHi());
    OS_Printf("DTCM         arena lo = %x\n", OS_GetDTCMArenaLo());
    OS_Printf("             arena hi = %x\n", OS_GetDTCMArenaHi());
    OS_Printf("SharedM      arena lo = %x\n", OS_GetSharedArenaLo());
    OS_Printf("             arena hi = %x\n", OS_GetSharedArenaHi());
    OS_Printf("WorkRAM-main arena lo = %x\n", OS_GetWramMainArenaLo());
    OS_Printf("             arena hi = %x\n", OS_GetWramMainArenaHi());
    OS_Printf("WorkRAM-sub  arena lo = %x\n", OS_GetWramSubArenaLo());
    OS_Printf("             arena hi = %x\n", OS_GetWramSubArenaHi());
    OS_Printf("Sub PrivWork arena lo = %x\n", OS_GetWramSubPrivArenaLo());
    OS_Printf("             arena hi = %x\n", OS_GetWramSubPrivArenaHi());

    //---- Allocate from arena of main memory    
    OS_Printf("---- now allocate from arena of MainRAM.\n");
    nstart = OS_AllocFromArenaLo(OS_ARENA_MAIN, 0x200, 8);
    OS_Printf("nstart %x\n", nstart);
    nstart = OS_AllocFromArenaHi(OS_ARENA_MAIN, 0x300, 8);
    OS_Printf("nstart %x\n", nstart);
    OS_Printf("MainRAM arena lo = %x\n", OS_GetMainArenaLo());
    OS_Printf("        arena hi = %x\n", OS_GetMainArenaHi());
    OS_Printf("        size     = %x\n", getArenaSize(OS_ARENA_MAIN));

    //---- Allocate from arena of  ITCM    
    OS_Printf("---- now allocate from arena of ITCM.\n");
    nstart = OS_AllocFromArenaLo(OS_ARENA_ITCM, 0x200, 8);
    OS_Printf("nstart %x\n", nstart);
    nstart = OS_AllocFromArenaHi(OS_ARENA_ITCM, 0x300, 8);
    OS_Printf("nstart %x\n", nstart);
    OS_Printf("ITCM    arena lo = %x\n", OS_GetArenaLo(OS_ARENA_ITCM));
    OS_Printf("        arena hi = %x\n", OS_GetArenaHi(OS_ARENA_ITCM));
    OS_Printf("        size     = %x\n", getArenaSize(OS_ARENA_ITCM));

    //---- Allocate from arena of DTCM    
    if ((u32)OS_GetArenaHi(OS_ARENA_DTCM) - (u32)OS_GetArenaLo(OS_ARENA_DTCM) > 0x500)
    {
        OS_Printf("---- now allocate from arena of DTCM.\n");
        nstart = OS_AllocFromArenaLo(OS_ARENA_DTCM, 0x130, 8);
        OS_Printf("nstart %x\n", nstart);
        nstart = OS_AllocFromArenaHi(OS_ARENA_DTCM, 0x330, 8);
        OS_Printf("nstart %x\n", nstart);
        OS_Printf("DTCM    arena lo = %x\n", OS_GetArenaLo(OS_ARENA_DTCM));
        OS_Printf("        arena hi = %x\n", OS_GetArenaHi(OS_ARENA_DTCM));
        OS_Printf("        size     = %x\n", getArenaSize(OS_ARENA_DTCM));
    }

    //---- Allocate from shared memory   
    OS_Printf("---- now allocate from arena of Shared Memory.\n");
    nstart = OS_AllocFromArenaLo(OS_ARENA_SHARED, 0x130, 8);
    OS_Printf("nstart %x\n", nstart);
    nstart = OS_AllocFromArenaHi(OS_ARENA_SHARED, 0x330, 8);
    OS_Printf("nstart %x\n", nstart);
    OS_Printf("SharedM arena lo = %x\n", OS_GetArenaLo(OS_ARENA_SHARED));
    OS_Printf("        arena hi = %x\n", OS_GetArenaHi(OS_ARENA_SHARED));
    OS_Printf("        size     = %x\n", getArenaSize(OS_ARENA_SHARED));

    //---- Allocate from WRAM    
    OS_Printf("---- now allocate from arena of WorkRAM.\n");
    nstart = OS_AllocFromArenaLo(OS_ARENA_WRAM_MAIN, 0x130, 8);
    OS_Printf("nstart %x\n", nstart);
    nstart = OS_AllocFromArenaHi(OS_ARENA_WRAM_MAIN, 0x330, 8);
    OS_Printf("nstart %x\n", nstart);
    OS_Printf("WRAM    arena lo = %x\n", OS_GetArenaLo(OS_ARENA_WRAM_MAIN));
    OS_Printf("        arena hi = %x\n", OS_GetArenaHi(OS_ARENA_WRAM_MAIN));
    OS_Printf("        size     = %x\n", getArenaSize(OS_ARENA_WRAM_MAIN));

    //---- Allocate from expanded main memory    
    if ((u32)OS_GetArenaHi(OS_ARENA_MAINEX) - (u32)OS_GetArenaLo(OS_ARENA_MAINEX) > 0x500)
    {
        OS_Printf("---- now allocate from arena of Extended MainRAM.\n");
        nstart = OS_AllocFromArenaLo(OS_ARENA_MAINEX, 0x130, 8);
        OS_Printf("nstart %x\n", nstart);
        nstart = OS_AllocFromArenaHi(OS_ARENA_MAINEX, 0x330, 8);
        OS_Printf("nstart %x\n", nstart);
        OS_Printf("MainEX  arena lo = %x\n", OS_GetArenaLo(OS_ARENA_MAINEX));
        OS_Printf("        arena hi = %x\n", OS_GetArenaHi(OS_ARENA_MAINEX));
        OS_Printf("        size     = %x\n", getArenaSize(OS_ARENA_MAINEX));
    }

    OS_Printf("==== Finish sample.\n");
    OS_Terminate();
}

/*====== End of main.c ======*/
