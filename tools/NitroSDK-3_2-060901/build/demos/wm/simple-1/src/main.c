/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - simple-1
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.15  01/20/2006 02:05:39  seiki_masashi
  Corrected the WMParentParam structure comment to match the current state.

  Revision 1.14  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.13  11/21/2005 11:07:10  kitase_hirotake
  Deleted OS_InitPrintServer, OS_PrintServer and PXI_Init
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.12  06/27/2005 11:10:13  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.11  02/28/2005 05:26:12  yosizaki
  do-indent.

  Revision 1.10  02/18/2005 12:09:33  seiki_masashi
  Revised Copyright notation

  Revision 1.9  02/18/2005 12:08:23  seiki_masashi
  warning measures

  Revision 1.8  10/22/2004 11:06:39  terui
  wc module cut to wireless shared region, supporting change in I/F.

  Revision 1.7  08/30/2004 04:06:37  seiki_masashi
  small fix.

  Revision 1.6  08/30/2004 02:41:19  seiki_masashi
  small fix.

  Revision 1.5  08/30/2004 01:56:53  seiki_masashi
  Changed so Port receive callback is used

  Revision 1.4  2004/08/23 07:27:27  yasu
  Workaround for pushing A Button in IPL

  Revision 1.3  08/17/2004 12:39:57  seiki_masashi
  Arranged send data in 32-byte boundary.

  Revision 1.2  08/02/2004 07:11:43  terui
  Changed transmission data editing buffer to array

  Revision 1.1  07/23/2004 15:28:14  terui
  Revisions and additions.

  Revision 1.4  07/23/2004 14:52:05  terui
  Only fix comment

  Revision 1.3  07/23/2004 01:04:02  terui
  Adjusted code

  Revision 1.2  07/20/2004 07:46:40  terui
  Uploaded tentative version for connection testing

  Revision 1.1  07/15/2004 12:48:42  terui
  initial upload

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    A simple functional sample of wireless communication using the WM library.
    Automatically connects to nearby simple demos and delivers a 4-byte frame
    count by wireless communication.

    HOWTO:
        1. To start communications as a parent, press the A button.
           When a nearby child with the same simple demo is found,
           automatically begins. Communication with up to 15 children is possible at the same time.
        2. To start communications as a child, press the B button.
           When a nearby parent with the same simple demo is found, communication with that parent
           automatically begins.
        3. If the START button is pressed during a busy screen while a connection is being established, or during a communication screen after a connection is established,
           the connection is reset, and the display returns to the initial screen.
 *---------------------------------------------------------------------------*/

#include    <nitro.h>
#include    <nitro/wm.h>

#include    "font.h"
#include    "wc.h"


/*---------------------------------------------------------------------------*
    constant definitions
 *---------------------------------------------------------------------------*/
#define     KEY_REPEAT_START    25     // number of frames until key repeat starts
#define     KEY_REPEAT_SPAN     10     // number of key repeat interval frames

#define     PICTURE_FRAME_PER_GAME_FRAME    1

#define     DEFAULT_GGID            0x003fff10
#define     NUM_MAX_CHILD           15
#define     DEFAULT_CHAN            1
#define     PARENT_DATA_SIZE        4
#define     CHILD_DATA_SIZE         4

static const WMParentParam defaultParameter = {
    NULL, 0, 0,
    DEFAULT_GGID,                      // Game group ID
    0x0000,                            // Temporary ID (incremented by 1 each time)
    1,                                 // Entry permit flag
    NUM_MAX_CHILD,                     // Maximum number of connected children
    0,                                 // Multiboot flag (normally 0)
    0,                                 // (Not used)
    0,                                 // (Not used)
    200,                               // Beacon interval
    {0, 0, 0, 0},                      // User name
    {0, 0, 0, 0, 0, 0, 0, 0},          // Game name
    DEFAULT_CHAN,                      // Connection channel
    PARENT_DATA_SIZE,                  // Size of parent send data
    CHILD_DATA_SIZE                    // Size of child send data
};

/*---------------------------------------------------------------------------*
    structure definitions
 *---------------------------------------------------------------------------*/
// key input data
typedef struct KeyInfo
{
    u16     cnt;                       // unprocessed input values
    u16     trg;                       // push trigger input
    u16     up;                        // release trigger input
    u16     rep;                       // push and hold repeat input

}
KeyInfo;


/*---------------------------------------------------------------------------*
    Internal function definitions
 *---------------------------------------------------------------------------*/
static void ModeSelect(void);          // Parent/child select screen
static void ModeError(void);           // Error display screen
static void ModeWorking(void);         // Busy screen
static void ModeParent(void);          // Parent communications screen
static void ModeChild(void);           // Child communications screen
static void VBlankIntr(void);          // V blank interrupt handler

// Functions called when receiving data
static void MpReceiveCallback(u16 aid, u16 *data, u16 length);

// General purpose subroutines
static void KeyRead(KeyInfo * pKey);
static void ClearString(void);
static void PrintString(s16 x, s16 y, u8 palette, char *text, ...);
static void ColorString(s16 x, s16 y, s16 length, u8 palette);
static void InitializeAllocateSystem(void);


/*---------------------------------------------------------------------------*
    internal variable definitions
 *---------------------------------------------------------------------------*/
static u16 gScreen[32 * 32];           // Virtual screen
static KeyInfo gKey;                   // key input
static u32 gFrame;                     // Frame counter

// Send/receive buffer for display
static u32 gSendBuf[1024] ATTRIBUTE_ALIGN(32);
static u32 gRecvBuf[1 + WM_NUM_MAX_CHILD][1024];
static BOOL gRecvFlag[1 + WM_NUM_MAX_CHILD];


/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  Initialization and main loop.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    // the different types of initialization
    OS_Init();
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
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
    GX_SetVisiblePlane(GX_PLANEMASK_BG0);
    GX_LoadBG0Char(d_CharData, 0, sizeof(d_CharData));
    GX_LoadBGPltt(d_PaletteData, 0, sizeof(d_PaletteData));
    MI_CpuFillFast((void *)gScreen, 0, sizeof(gScreen));
    DC_FlushRange(gScreen, sizeof(gScreen));
    /* IO register is accessed using DMA operation, so cache Wait is not needed*/
    // DC_WaitWriteBufferEmpty();
    GX_LoadBG0Scr(gScreen, 0, sizeof(gScreen));

    // Set interrupt
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)GX_VBlankIntr(TRUE);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();

    // Memory allocation
    InitializeAllocateSystem();

    //********************************
    // Initialize Wireless
    WcInit(&defaultParameter, FALSE, FALSE, PICTURE_FRAME_PER_GAME_FRAME);
    WcSetMpReceiveCallback(MpReceiveCallback);
    //********************************

    // LCD display start
    GX_DispOn();
    GXS_DispOn();

    // debug string output
    OS_Printf("ARM9: WM simple demo started.\n");

    // empty call for getting key input data (strategy for pressing A button in IPL)
    KeyRead(&gKey);

    // Main loop
    for (gFrame = 0; TRUE; gFrame++)
    {
        // get key input data
        KeyRead(&gKey);

        // Clears the screen
        ClearString();

        // Direct to process based on communication status
        switch (WcGetStatus())
        {
        case WC_STATUS_READY:
            ModeSelect();
            break;
        case WC_STATUS_ERROR:
            ModeError();
            break;
        case WC_STATUS_BUSY:
            ModeWorking();
            break;
        case WC_STATUS_PARENT:
            ModeParent();
            break;
        case WC_STATUS_CHILD:
            ModeChild();
            break;
        }

        // Waiting for the V-blank
        OS_WaitVBlankIntr();
    }
}

/*---------------------------------------------------------------------------*
  Name:         ModeSelect

  Description:  Process in parent/child selection screen

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ModeSelect(void)
{
    PrintString(3, 10, 0xf, "Push A to connect as PARENT");
    PrintString(3, 12, 0xf, "Push B to connect as CHILD");

    if (gKey.trg & PAD_BUTTON_A)
    {
        //********************************
        s32     i;

        for (i = 1; i < (WM_NUM_MAX_CHILD + 1); i++)
        {
            gRecvFlag[i] = FALSE;
        }

        WcStartParent();
        //********************************
    }
    else if (gKey.trg & PAD_BUTTON_B)
    {
        //********************************
        WcStartChild();
        //********************************
    }
}

/*---------------------------------------------------------------------------*
  Name:         ModeError

  Description:  Processing in error display screen

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ModeError(void)
{
    PrintString(5, 10, 0x1, "======= ERROR! =======");
    PrintString(5, 13, 0xf, " Fatal error occured.");
    PrintString(5, 14, 0xf, "Please reboot program.");
}

/*---------------------------------------------------------------------------*
  Name:         ModeWorking

  Description:  Processing in busy screen

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ModeWorking(void)
{
    PrintString(9, 11, 0xf, "Now working...");

    if (gKey.trg & PAD_BUTTON_START)
    {
        //********************************
        WcEnd();
        //********************************
    }
}

/*---------------------------------------------------------------------------*
  Name:         ModeParent

  Description:  Processing in parent communications screen.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ModeParent(void)
{
    PrintString(8, 1, 0x2, "Parent mode");
    PrintString(4, 3, 0x4, "Send:     %08X", gSendBuf[0]);
    PrintString(4, 5, 0x4, "Receive:");
    {
        s32     i;

        for (i = 1; i < (WM_NUM_MAX_CHILD + 1); i++)
        {
            if (gRecvFlag[i])
            {
                PrintString(5, (s16)(6 + i), 0x4, "Child%02d: %08X", i, gRecvBuf[i][0]);
            }
            else
            {
                PrintString(5, (s16)(6 + i), 0x7, "No child");
            }
        }
    }

    if (gKey.trg & PAD_BUTTON_START)
    {
        //********************************
        WcEnd();
        //********************************
    }
}

/*---------------------------------------------------------------------------*
  Name:         ModeChild

  Description:  Processing in child communications screen.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ModeChild(void)
{
    PrintString(8, 1, 0x2, "Child mode");
    PrintString(4, 3, 0x4, "Send:     %08X", gSendBuf[0]);
    PrintString(4, 5, 0x4, "Receive:");
    PrintString(5, 7, 0x4, "Parent:  %08X", gRecvBuf[0][0]);

    if (gKey.trg & PAD_BUTTON_START)
    {
        //********************************
        WcEnd();
        //********************************
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
    // Start sending new data
    switch (WcGetStatus())
    {
    case WC_STATUS_PARENT:
        gSendBuf[0] = gFrame;
        //********************************
        WcSetParentData(gSendBuf, PARENT_DATA_SIZE);
        //********************************
        break;
    case WC_STATUS_CHILD:
        gSendBuf[0] = gFrame;
        //********************************
        WcSetChildData(gSendBuf, CHILD_DATA_SIZE);
        //********************************
        break;
    }

    // Reflect virtual screen in VRAM
    DC_FlushRange(gScreen, sizeof(gScreen));
    /* IO register is accessed using DMA operation, so cache Wait is not needed*/
    // DC_WaitWriteBufferEmpty();
    GX_LoadBG0Scr(gScreen, 0, sizeof(gScreen));

    // sets the IRQ check flag
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         MpReceiveCallback

  Description:  Function called when MP data is received.

  Arguments:    aid     - aid of child originating transmission (If 0, then data is from parent)
                data    - Pointer to received data (NULL notifies disconnection)
                length  - Size of received data

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MpReceiveCallback(u16 aid, u16 *data, u16 length)
{
#pragma unused( length )
    SDK_MAX_ASSERT(aid, 15);

    if (data != NULL)
    {
        SDK_ASSERT(length >= 4);
        gRecvFlag[aid] = TRUE;
        // Copy source is aligned to 2 bytes (not 4 bytes)
        if (aid == 0)
        {
            // In the case of receiving from a parent
            MI_CpuCopy8(data, &(gRecvBuf[aid][0]), PARENT_DATA_SIZE);
        }
        else
        {
            MI_CpuCopy8(data, &(gRecvBuf[aid][0]), CHILD_DATA_SIZE);
        }
    }
    else
    {
        gRecvFlag[aid] = FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         KeyRead

  Description:  Edits key input data.
                Detects press trigger, release trigger and hold repeat trigger.

  Arguments:    pKey  - Structure that holds key input data to be edited.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void KeyRead(KeyInfo * pKey)
{
    static u16 repeat_count[12];
    int     i;
    u16     r;

    r = PAD_Read();
    pKey->trg = 0x0000;
    pKey->up = 0x0000;
    pKey->rep = 0x0000;

    for (i = 0; i < 12; i++)
    {
        if (r & (0x0001 << i))
        {
            if (!(pKey->cnt & (0x0001 << i)))
            {
                pKey->trg |= (0x0001 << i);     // push trigger
                repeat_count[i] = 1;
            }
            else
            {
                if (repeat_count[i] > KEY_REPEAT_START)
                {
                    pKey->rep |= (0x0001 << i); // push and hold repeat
                    repeat_count[i] = KEY_REPEAT_START - KEY_REPEAT_SPAN;
                }
                else
                {
                    repeat_count[i]++;
                }
            }
        }
        else
        {
            if (pKey->cnt & (0x0001 << i))
            {
                pKey->up |= (0x0001 << i);      // release trigger
            }
        }
    }
    pKey->cnt = r;                     // unprocessed key input
}

/*---------------------------------------------------------------------------*
  Name:         ClearString

  Description:  Clear the virtual screen.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ClearString(void)
{
    MI_CpuClearFast((void *)gScreen, sizeof(gScreen));
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
  Name:         ColorString

  Description:  Changes the color of character strings printed on the virtual screen.

  Arguments:    x       - x coordinate (X 8 dots ) from which to start color change.
                y       - y coordinate ( X 8 dots ) from which to start color change.
                length  - Number of characters to extend the color change.
                palette - specify Text color by palette number.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ColorString(s16 x, s16 y, s16 length, u8 palette)
{
    s32     i;
    u16     temp;
    s32     index;

    if (length < 0)
        return;

    for (i = 0; i < length; i++)
    {
        index = ((y * 32) + x + i) % (32 * 32);
        temp = gScreen[index];
        temp &= 0x0fff;
        temp |= (palette << 12);
        gScreen[index] = temp;
    }
}

/*---------------------------------------------------------------------------*
  Name:         InitializeAllocateSystem

  Description:  Initializes the memory allocation system within the main memory arena.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitializeAllocateSystem(void)
{
    void   *tempLo;
    OSHeapHandle hh;

    // Based on the premise that OS_Init is already called
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
  End of file
 *---------------------------------------------------------------------------*/
