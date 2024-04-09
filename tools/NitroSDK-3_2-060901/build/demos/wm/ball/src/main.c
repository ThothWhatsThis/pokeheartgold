/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - ball
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.8  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.7  11/21/2005 11:06:34  kitase_hirotake
  Deleted OS_InitPrintServer and OS_PrintServer.
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.6  06/27/2005 11:10:13  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.5  02/28/2005 05:26:01  yosizaki
  do-indent.

  Revision 1.4  2004/08/23 07:27:27  yasu
  Workaround for pushing A Button in IPL

  Revision 1.3  08/07/2004 05:12:55  seiki_masashi
  Implemented call to OS_PrintServer()

  Revision 1.2  2004/07/29 12:06:48  ikedae
  Added initialization

  Revision 1.1  07/23/2004 15:11:55  terui
  Revisions and additions.

  Revision 1.5  06/23/2004 10:01:31  Sato
  (none)

  Revision 1.4  06/22/2004 07:52:37  Sato
  Updated portions to support wmlib1.7

  Revision 1.3  06/14/2004 09:51:30  Sato
  (none)


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitro/wm.h>
#include "wm_lib.h"

#include "data.h"
#include "keysharing.h"
#include "ball.h"


OSHeapHandle heapHandle;               // Heap handle

void    VBlankIntr(void);
void    Select(void);

#define MAIN_HEAP_SIZE	(0x10000)

enum
{ MODE_PARENT, MODE_CHILD };

static u8 mode = MODE_PARENT;
u8      prog_state;


//======================================================================
//      Main
//
void NitroMain()
{
    void   *heapStart;
    void   *nstart;

    //================ Initialize
    //---- OS Initialization
    OS_Init();
    OS_InitThread();
    OS_InitTick();
    OS_InitAlarm();


    // Memory allocation =====================================================
    // initialize memory allocation system for MainRAM arena
    nstart = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 16);
    OS_SetMainArenaLo(nstart);
    // Allocate area for heap from arena ---------
    heapStart = OS_AllocFromMainArenaLo(MAIN_HEAP_SIZE, 32);
    //    OS_Printf( "heapStart %x\n", heapStart );
    // Create heap --------------------------
    heapHandle = OS_CreateHeap(OS_ARENA_MAIN, heapStart, (void *)((u32)heapStart + MAIN_HEAP_SIZE));

    //---- GX Initialization
    GX_Init();

    //================ Settings
    //---- All Power ON
    GX_SetPower(GX_POWER_ALL);

    //----  Enable Vblank interrupt
    (void)OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();

    //---- Occurrence of Vblank setting
    (void)GX_VBlankIntr(TRUE);

    //---- VRAM Clear
    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
    (void)GX_DisableBankForLCDC();

    //---- OAM and palette clear
    MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);
    MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);


    //---- Set bankA for OBJ
    GX_SetBankForOBJ(GX_VRAM_OBJ_128_A);        // Assign bank A onto OBJ

    //---- Set to graphics display mode
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);

    //---- Set only OBJ display ON
    GX_SetVisiblePlane(GX_PLANEMASK_OBJ);

    //---- Used with 32K byte OBJ in 2D mapping mode
    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);


    //---- Data load
    MI_DmaCopy32(3, sampleCharData, (void *)HW_OBJ_VRAM, sizeof(sampleCharData));
    MI_DmaCopy32(3, samplePlttData, (void *)HW_OBJ_PLTT, sizeof(samplePlttData));

    //---- Move hidden OBJ out of screen
    MI_DmaFill32(3, oamBak, 0xc0, sizeof(oamBak));

    //---- Started display
    OS_WaitVBlankIntr();
    GX_DispOn();

    prog_state = STATE_SELECT;

    //================ Main loop
    while (1)
    {
        //---- Wait for V-blank interrupt completion
        OS_WaitVBlankIntr();

        switch (prog_state)
        {
        case STATE_SELECT:
            Select();
            break;
        case STATE_KS_PARENTINIT:
            KsParentInit();
            break;
        case STATE_KS_PARENT:
            KsParentMain();
            break;
        case STATE_KS_CHILDINIT:
            KsChildInit();
            break;
        case STATE_KS_CHILDSCAN:
            KsChildScan();
            break;
        case STATE_KS_CHILD:
            KsChildMain();
            break;
        default:
            break;
        }
    }

}



//--------------------------------------------------------------------------------
//    V-blank interrupt process
//
void VBlankIntr(void)
{
    //---- OAM updating
    DC_FlushRange(oamBak, sizeof(oamBak));
    /* IO register is accessed using DMA operation, so cache Wait is not needed*/
    // DC_WaitWriteBufferEmpty();
    MI_DmaCopy32(3, oamBak, (void *)HW_OAM, sizeof(oamBak));

    //---- Interrupt check flag
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

//--------------------------------------------------------------------------------
//    Parent/child selection menu
//
void Select(void)
{
    u16     keyData;

    //---- Load pad data
    keyData = PAD_Read();

    if (keyData & PAD_KEY_RIGHT)
        mode = MODE_CHILD;
    else if (keyData & PAD_KEY_LEFT)
        mode = MODE_PARENT;
    else if (keyData & PAD_BUTTON_A)
    {
        prog_state = ((mode == MODE_PARENT) ? STATE_KS_PARENTINIT : STATE_KS_CHILDINIT);
    }

    ObjSetString(PARENT_STRING_OBJNO, 7 * 8, 3 * 8, "Parent", (mode == MODE_PARENT) ? 4 : 7);
    ObjSetString(CHILD_STRING_OBJNO, 19 * 8, 3 * 8, "Child", (mode == MODE_CHILD) ? 4 : 7);
    ObjClearRange(PLIST_STRING_OBJNO, PLIST_STRING_OBJNO + 6 - 1);
    ObjClearRange(CLIST_STRING_OBJNO, CLIST_STRING_OBJNO + 6 - 1);
    ObjClearRange(PARENT_NAME_OBJNO, PARENT_NAME_OBJNO + 32 - 1);
    ObjClearRange(MESSAGE_OBJNO, MESSAGE_OBJNO + 24 - 1);

    ObjSetMessage(" Select Parent or Child ");

    ObjSet(CURSOR_OBJNO, ((mode == MODE_PARENT) ? 6 : 18) * 8, 3 * 8, '@', 1);

}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
