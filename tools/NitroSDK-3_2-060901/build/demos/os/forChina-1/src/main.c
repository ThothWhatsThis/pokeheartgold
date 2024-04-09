/*---------------------------------------------------------------------------*
  Project:  NitroSDK - demos - OS - forChina-1
  File:     main.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.4  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.3  11/21/2005 10:54:30  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.2  06/27/2005 11:10:12  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.1  05/14/2005 01:07:02  terui
  new additions

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro.h>
#include    <nitro/rtc.h>

#include    "font.h"


/*---------------------------------------------------------------------------*
    constant definitions
 *---------------------------------------------------------------------------*/
// ISBN char strings for Chinese language support
// (Example) { "7-900381-62-7", "025-2004-017", "2005", " 065" }
static const char *isbn[4] = { "0-000000-00-0", "000-0000-000", "0000", " 000" };


/*---------------------------------------------------------------------------*
    Internal function definitions
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void);
static void ClearString(void);
static void PrintString(s16 x, s16 y, u8 palette, char *text, ...);


/*---------------------------------------------------------------------------*
    internal variable definitions
 *---------------------------------------------------------------------------*/
static OSOwnerInfo gOwnerInfo;
static u16 gScreen[32 * 32];           // Virtual screen


/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  Initialization and main loop.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    // the different types of initialization
#if 1
    //****************************************
    OS_InitChina(isbn);
    //****************************************
#else
    OS_Init();
#endif

    FX_Init();
    GX_Init();
    GX_DispOff();
    GXS_DispOff();

    // initializes display settings
    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
    (void)GX_DisableBankForLCDC();
    MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);
    MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);
    MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);
    MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);

    // 2D display settings for text string display
    GX_SetBankForBG(GX_VRAM_BG_128_A);
    G2_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xf800,      // SCR base block 31
                     GX_BG_CHARBASE_0x00000,    // CHR base block 0
                     GX_BG_EXTPLTT_01);
    G2_SetBG0Priority(0);
    G2_BG0Mosaic(FALSE);
    G2_SetBG0Offset(0, 0);
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
    GX_SetVisiblePlane(GX_PLANEMASK_BG0);
    GX_LoadBG0Char(d_CharData, 0, sizeof(d_CharData));
    GX_LoadBGPltt(d_PaletteData, 0, sizeof(d_PaletteData));
    MI_CpuClearFast(gScreen, sizeof(gScreen));
    DC_StoreRange(gScreen, sizeof(gScreen));
    /* IO register is accessed using DMA operation, so cache Wait is not needed*/
    // DC_WaitWriteBufferEmpty();
    GX_LoadBG0Scr(gScreen, 0, sizeof(gScreen));

    // Set interrupt
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)GX_VBlankIntr(TRUE);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();

    // LCD display start
    GX_DispOn();

    // debug string output
    OS_Printf("forChina-1 test started.\n");

    //****************************************
    // Obtain owner information
    OS_GetOwnerInfo(&gOwnerInfo);
    //****************************************

    // Main loop
    {
        s32     frame;

        for (frame = 0; TRUE; frame++)
        {
            // Clears the screen
            ClearString();

            // Character display
            PrintString(4, 4, 0x0, "Frame : %d", frame);
            PrintString(4, 8, 0x3, "LanguageCode : %d", gOwnerInfo.language);

            // Waiting for the V-blank
            OS_WaitVBlankIntr();
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  V blank interrupt vector.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    // Reflect virtual screen in VRAM
    DC_StoreRange(gScreen, sizeof(gScreen));
    /* IO register is accessed using DMA operation, so cache Wait is not needed*/
    // DC_WaitWriteBufferEmpty();
    GX_LoadBG0Scr(gScreen, 0, sizeof(gScreen));

    // sets the IRQ check flag
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         ClearString

  Description:  Clear the virtual screen.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ClearString(void)
{
    MI_CpuClearFast(gScreen, sizeof(gScreen));
}

/*---------------------------------------------------------------------------*
  Name:         PrintString

  Description:  Positions the character string on the virtual screen. The string can be up to 32 chars.

  Arguments:    x       -  x coordinate where character string starts (x 8 dots).
                y       -  y coordinate where character string starts (x 8 dots).
                palette - specify Text color by palette number.
                text    - Character string to position. NULL terminated.
                ...     -   Virtual argument.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void PrintString(s16 x, s16 y, u8 palette, char *text, ...)
{
    va_list vlist;
    char    temp[32 + 2];
    s32     i;

    va_start(vlist, text);
    (void)vsnprintf(temp, 33, text, vlist);
    va_end(vlist);

    *(u16 *)(&temp[32]) = 0x0000;
    for (i = 0;; i++)
    {
        if (temp[i] == 0x00)
        {
            break;
        }
        gScreen[((y * 32) + x + i) % (32 * 32)] = (u16)((palette << 12) | temp[i]);
    }
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
