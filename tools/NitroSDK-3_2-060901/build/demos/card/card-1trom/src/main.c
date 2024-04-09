/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - demos - card-1trom
  File:     main.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.2  01/18/2006 02:11:21  kitase_hirotake
  do-indent

  Revision 1.1  04/05/2005 11:45:00  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro.h>
#include <nitro/card.h>


/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main entry point

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    OS_Init();
    OS_InitTick();
    CARD_Init();
    (void)OS_EnableIrq();

    /*
     * Repeats a large read access 10 times and measures the transfer speed.
     * Using DMA takes up the overhead of the interrupt handler,
     * so CPU transfer is used.
     */
    {
        enum
        { BUFFER_SIZE = 1 * 1024 * 1024 };
        static u8 buf[BUFFER_SIZE];
        int     i;
        OSTick  t;
        const u16 id = (u16)OS_GetLockID();

        CARD_LockRom(id);
        for (i = 0; i < 10; ++i)
        {
            t = OS_GetTick();
            CARD_ReadRom(4, (const void *)0, buf, BUFFER_SIZE);
            t = OS_GetTick() - t;
            /* n[B/s] = 1[MB] / t[us] */
            OS_Printf("[%2d] ... %10.6f[B/s]\n", i + 1,
                      1.0 * BUFFER_SIZE / OS_TicksToMicroSeconds(t));
        }
        CARD_UnlockRom(id);

        OS_Printf("[end]\n");
    }

    OS_Terminate();
}
