/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WBT - demos - wbt-fs
  File:     util.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: util.c,v $
  Revision 1.6  01/18/2006 02:12:38  kitase_hirotake
  do-indent

  Revision 1.5  06/27/2005 11:10:11  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.4  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.3  10/25/2004 10:20:01  yosizaki
  add some functions.

  Revision 1.2  09/29/2004 07:01:48  yosizaki
  change PrintString to break at '\r', '\n'.

  Revision 1.1  09/03/2004 01:11:58  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include    "util.h"


/*---------------------------------------------------------------------------*
    internal variable definitions
 *---------------------------------------------------------------------------*/

static u16 gScreen[32 * 32];           /* Virtual screen*/
static u16 repeat_count[12];           /* key repeat*/
static char recent_output[24][32 + 1];


/*---------------------------------------------------------------------------*
  Name:         InitializeAllocateSystem

  Description:  Initializes memory allocation system in the main memory arena.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitializeAllocateSystem(void)
{
    void   *tempLo;
    OSHeapHandle hh;

    tempLo = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
    OS_SetArenaLo(OS_ARENA_MAIN, tempLo);
    hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
    if (hh < 0)
    {
        OS_Panic("ARM9: Fail to create heap...\n");
    }
    hh = OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
}


/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  V blank interrupt vector.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    /* Reflect virtual screen in VRAM*/
    DC_FlushRange(gScreen, sizeof(gScreen));
    /* IO register is accessed using DMA operation, so cache Wait is not needed*/
    // DC_WaitWriteBufferEmpty();
    GX_LoadBG0Scr(gScreen, 0, sizeof(gScreen));

    /* sets the IRQ check flag*/
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}


/*---------------------------------------------------------------------------*
  Name:         UTIL_Init

  Description:  Initialize sample framework

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void UTIL_Init(void)
{
    /* initializes display settings*/
    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
    (void)GX_DisableBankForLCDC();
    MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);
    MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);
    MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);
    MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);

    /* 2D display settings for text string display*/
    GX_SetBankForBG(GX_VRAM_BG_128_A);
    G2_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256,
                     GX_BG_COLORMODE_16,
                     GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
    G2_SetBG0Priority(0);
    G2_BG0Mosaic(FALSE);
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
    GX_SetVisiblePlane(GX_PLANEMASK_BG0);
    GX_LoadBG0Char(d_CharData, 0, sizeof(d_CharData));
    GX_LoadBGPltt(d_PaletteData, 0, sizeof(d_PaletteData));

    MI_CpuFillFast((void *)gScreen, 0, sizeof(gScreen));
    DC_FlushRange(gScreen, sizeof(gScreen));
    /* IO register is accessed using DMA operation, so cache Wait is not needed*/
    // DC_WaitWriteBufferEmpty();
    GX_LoadBG0Scr(gScreen, 0, sizeof(gScreen));

    /* Set interrupt*/
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)GX_VBlankIntr(TRUE);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();

    /* Memory allocation*/
    InitializeAllocateSystem();
}


/*---------------------------------------------------------------------------*
  Name:         KeyRead

  Description:  Acquire key data

  Arguments:    pKey : Address at which key information is stored

  Returns:      None.
 *---------------------------------------------------------------------------*/
void KeyRead(KeyInfo * pKey)
{
    enum
    {
        KEY_BIT_MAX = 12,
        KEY_REPEAT_START = 25,         /* number of frames until key repeat starts*/
        KEY_REPEAT_SPAN = 10           /* number of key repeat interval frames*/
    };

    int     i;
    const u16 r = PAD_Read();
    pKey->trg = 0x0000;
    pKey->up = 0x0000;
    pKey->rep = 0x0000;

    for (i = 0; i < KEY_BIT_MAX; ++i)
    {
        const u16 bit = (u16)(1 << i);
        if ((r & bit) != 0)
        {
            if (!(pKey->cnt & bit))
            {
                pKey->trg |= bit;
                repeat_count[i] = 1;
            }
            else if (repeat_count[i] > KEY_REPEAT_START)
            {
                pKey->rep |= bit;
                repeat_count[i] = KEY_REPEAT_START - KEY_REPEAT_SPAN;
            }
            else
            {
                ++repeat_count[i];
            }
        }
        else if ((pKey->cnt & bit) != 0)
        {
            pKey->up |= bit;
        }
    }
    pKey->cnt = r;
}

/*---------------------------------------------------------------------------*
  Name:         ClearString

  Description:  Initializes the virtual screen for display.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ClearString(void)
{
    MI_CpuClearFast(gScreen, sizeof(gScreen));
}

/*---------------------------------------------------------------------------*
  Name:         PrintString

  Description:  Displays a character string of up to 32 characters on the virtual screen

  Arguments:    x       - The x coordinate that positions the start of the character string / 8 dot
                y       - The y coordinate that positions the start of the character string / 8 dot
                palette - The palette number that designates the character color
                text    - The displayed character string
                - Subsequent variable arguments

  Returns:      None.
 *---------------------------------------------------------------------------*/
void PrintString(int x, int y, int palette, const char *text, ...)
{
    va_list vlist;
    static char temp[64 + 2];
    u8      d;
    int     i;

    va_start(vlist, text);
    (void)OS_VSNPrintf(temp, sizeof(temp) - 1, text, vlist);
    va_end(vlist);
    *(u16 *)&temp[32] = 0x0000;
    for (i = 0; (d = MI_ReadByte(temp + i)) != '\0'; ++i)
    {
        if ((d == '\r') || (d == '\n'))
            break;
        gScreen[((y * 32) + x + i) % (32 * 32)] = (u16)((palette << 12) | d);
    }
}

/*---------------------------------------------------------------------------*
  Name:         ColorString

  Description:  Changes the color of the character string shown on the virtual screen

  Arguments:    x       - x coordinate for the beginning of the color change / 8 dot
                y       - y coordinate for the beginning of the color change / 8 dot
                length  - Number of characters to change the color of
                palette - The palette number that designates the character color

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ColorString(int x, int y, int length, int palette)
{
    int     i;
    for (i = 0; i < length; ++i)
    {
        const int index = ((y * 32) + x + i) % (32 * 32);
        gScreen[index] = (u16)((gScreen[index] & 0x0FFF) | (palette << 12));
    }
}

/*---------------------------------------------------------------------------*
  Name:         ClearLine

  Description:  Initialize drawing character string

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ClearLine(void)
{
    MI_CpuClear8(recent_output, sizeof(recent_output));
}

/*---------------------------------------------------------------------------*
  Name:         PrintLine

  Description:  In a specified line, this draws a nonvolatile character string that will remain for at least one frame.

  Arguments:    x          Display position x grid. (8 pixel units)
                y          Display position y grid. (8 pixel units)
                text        Formatted character string that receives the variable argument that follows.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void PrintLine(int x, int y, const char *text, ...)
{
    va_list va;
    va_start(va, text);
    MI_CpuFill8(recent_output[y], ' ', sizeof(recent_output[y]));
    (void)OS_VSNPrintf(&recent_output[y][x], (size_t) (32 - x), text, va);
    va_end(va);
}

/*---------------------------------------------------------------------------*
  Name:         FlushLine

  Description:  Overwrites the nonvolatile character string from PrintLine() to the normal character string draw.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FlushLine(void)
{
    int     y;
    for (y = 0; y < 24; ++y)
    {
        PrintString(0, y, COLOR_WHITE, recent_output[y]);
    }
}


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
