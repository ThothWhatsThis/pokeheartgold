/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/VCount
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.7  01/18/2006 02:11:28  kitase_hirotake
  do-indent

  Revision 1.6  11/21/2005 10:45:49  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.5  02/28/2005 05:26:19  yosizaki
  do-indent.

  Revision 1.4  05/27/2004 09:28:02  takano_makoto
  Adjust indent.

  Revision 1.3  04/07/2004 01:23:27  yada
  fix header comment

  Revision 1.2  04/06/2004 12:48:08  yada
  fix header comment

  Revision 1.1  03/01/2004 10:44:45  terui
  initial upload

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
//---------------------------------------------------------------------------
//  A sample to adjust V-cycle.
//
//  Default V-cycle is 262 lines, and you can adjust this cycle from 252 to infinite.
//
//  HOWTO:
//      case 252 ~ 262
//          1.  Wait until V-Count has changed to 202 in VBlank interrupt.
//          2.  Skip V-Count along V-cycle parameter.
//          3.  End VBlank interrupt.
//      case 263 ~ infinite
//          1.  Wait until V-Count has changed to 212 in VBlank interrupt.
//          2.  Set V-Count to 211.
//          3.  Loop this operation along V-cycle parameter.
//          4.  When this loop finish, end VBlank interrupt.
//
//  OPERATION:
//  1.  Push UP or DOWN button to change V-cycle.
//
//---------------------------------------------------------------------------
#include <nitro.h>
#include "DEMO.h"

//---------------------------------------------------------------------------
//  V-cycle parameter
//---------------------------------------------------------------------------
static int s_VWaitCnt = 262;

//---------------------------------------------------------------------------
// V-Blank interrupt function:
//
// Interrupt handlers are registered on the interrupt table by OS_SetIRQFunction.
// OS_EnableIrqMask selects IRQ interrupts to enable, and
// OS_EnableIrq enables IRQ interrupts.
// Notice that you have to call 'OS_SetIrqCheckFlag' to check a V-Blank interrupt.
//---------------------------------------------------------------------------
void VBlankIntr(void)
{
    int     count;

    if (s_VWaitCnt <= 262)
    {
        count = 202 + 262 - s_VWaitCnt;
        if (count > 212)
            count = 212;
        while (1)
        {
            if (GX_GetVCount() == 202)
            {
                GX_SetVCount(count);
                break;
            }
        }
    }
    else
    {
        count = 0;
        while (1)
        {
            if (GX_GetVCount() == 212)
            {
                if (count < (s_VWaitCnt - 262))
                {
                    GX_SetVCount(211);
                    count++;
                }
                else
                {
                    count = 0;
                    break;
                }
            }
        }
    }

    // Set flag which checks V-Blank interrupt.
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

//---------------------------------------------------------------------------
//  main
//---------------------------------------------------------------------------
void NitroMain(void)
{
    unsigned int msg_cnt = 0;

    // Initialize
    DEMOInitCommon();
    DEMOInitVRAM();

    DEMOStartDisplay();

    // main loop
    while (1)
    {
        // Read input
        DEMOReadKey();
        // Change V-cycle
        if (DEMO_IS_PRESS(PAD_KEY_UP))
        {
            s_VWaitCnt++;
        }
        else if (DEMO_IS_PRESS(PAD_KEY_DOWN))
        {
            if (--s_VWaitCnt < 252)
            {
                s_VWaitCnt = 252;
            }
        }

        // Print information
        OS_Printf("mssg%d: %d frame V Wait count is =%d\n", msg_cnt++, msg_cnt, s_VWaitCnt);

        // Wait for V-Blank
        OS_WaitVBlankIntr();
    }
}
