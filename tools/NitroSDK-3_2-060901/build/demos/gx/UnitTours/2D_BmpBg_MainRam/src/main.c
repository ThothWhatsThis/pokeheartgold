/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/2D_BmpBg_MainRam
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.18  01/18/2006 02:11:21  kitase_hirotake
  do-indent

  Revision 1.17  11/21/2005 10:26:04  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.16  02/28/2005 05:26:13  yosizaki
  do-indent.

  Revision 1.15  11/02/2004 09:33:56  takano_makoto
  fix comment.

  Revision 1.14  2004/07/20 08:39:21  yasu
  Add comment about ARM9 DMA

  Revision 1.13  07/20/2004 07:43:37  yada
  add MI_SetMainMemoryPriority( MI_PROCESSOR_ARM9 )

  Revision 1.12  05/24/2004 10:28:25  takano_makoto
  Fixed autotest code

  Revision 1.11  04/07/2004 01:23:26  yada
  fix header comment

  Revision 1.10  04/06/2004 12:48:07  yada
  fix header comment

  Revision 1.9  2004/03/30 08:27:15  yasu
  fixed check sum code

  Revision 1.8  2004/02/24 05:25:21  yasu
  used OS_EnableIrqMask instead of OS_SetIrqMask

  Revision 1.7  02/09/2004 01:44:47  kitani_toshikazu
  Add default setting codes for auto testing.

  Revision 1.6  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.5  02/05/2004 01:15:24  kitani_toshikazu
  added codes for autoTesing.

  Revision 1.4  01/19/2004 02:15:56  nishida_kenji
  fix comments

  Revision 1.3  01/18/2004 09:20:26  nishida_kenji
  adds DEMOStartDisplay

  Revision 1.2  01/18/2004 07:49:38  nishida_kenji
  change the place of SVC_WaitVBlankIntr

  Revision 1.1  01/10/2004 02:18:32  nishida_kenji
  directory renamed

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// A sample that displays the image on the main memory by main memory display mode:
//
// If display mode is main memory display mode, a bitmap image on
// the main memory is displayed through the specialized FIFO.
// This sample loads different four images to the main memory,
// and displays them by turns.
//
// HOWTO:
// Switch to main memory display mode by GX_SetGraphicsMode(GX_DISPMODE_MMEM, ...).
// Transfer images on the main memory by MI_DispMemDmaCopy() every V-Blank.
//---------------------------------------------------------------------------

#include <nitro.h>
#include "DEMO.h"
#include "image.h"

void NitroMain(void)
{
    int     count = 0;
    int     slot = 0;

    //---------------------------------------------------------------------------
    // Initialize:
    // They enable IRQ interrupts, and initialize VRAM.
    //---------------------------------------------------------------------------
    DEMOInitCommon();
    DEMOInitVRAM();

    GX_SetGraphicsMode(GX_DISPMODE_MMEM,        // display main memory
                       (GXBGMode)0,    // dummy
                       (GXBG0As)0);    // dummy

    DEMOStartDisplay();

#ifdef  SDK_AUTOTEST
    GX_SetBankForLCDC(GX_VRAM_LCDC_C);
    EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
#endif // SDK_AUTOTEST

    //-------------------------------------------------------------------------
    // Give the main memory access priority to ARM9.
    // In MI_DispMemDmaCopy, ARM9 should send a lot of pixel to VRAM via DMA
    // at highest priority (higher than ARM7 sound DMA).
    // This may be a risk that sound noise occurs.
    //---------------------------------------------------------------------------
    MI_SetMainMemoryPriority(MI_PROCESSOR_ARM9);

    while (1)
    {
        if (count++ > 90)
        {
            slot++;
            slot &= 0x03;
            count = 0;
        }

#ifdef SDK_AUTOTEST
        //        EXT_TestScreenShot( 100, 0xF1338040 );
        EXT_TestScreenShot(100, 0xEFA8B182);
        EXT_TestTickCounter();
#endif //SDK_AUTOTEST

        OS_WaitVBlankIntr();           // Waiting the end of V-Blank interrupt

        //---------------------------------------------------------------------------
        // Transfer images from main memory to VRAM through the specialized FIFO by DMA.
        // It must be completed within a period of V-Blank.
        //---------------------------------------------------------------------------
        MI_DispMemDmaCopy(3, IMAGE_MRAM256x192[slot]);
    }
}

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
    OS_SetIrqCheckFlag(OS_IE_V_BLANK); // checking V-Blank interrupt
}
