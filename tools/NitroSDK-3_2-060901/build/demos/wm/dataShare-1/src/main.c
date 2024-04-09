/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - dataShare-1
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.13  01/20/2006 02:05:39  seiki_masashi
  Corrected the WMParentParam structure comment to match the current state.

  Revision 1.12  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.11  11/21/2005 11:06:47  kitase_hirotake
  Deleted OS_InitPrintServer, OS_PrintServer and PXI_Init
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.10  06/27/2005 11:10:13  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.9  02/28/2005 05:26:12  yosizaki
  do-indent.

  Revision 1.8  10/22/2004 11:06:44  terui
  wc module cut to wireless shared region, supporting change in I/F.

  Revision 1.7  2004/08/23 07:27:27  yasu
  Workaround for pushing A Button in IPL

  Revision 1.6  08/17/2004 12:25:28  seiki_masashi
  Revised in accordance with removal of WM_ERRCODE_FRAME_GAP.

  Revision 1.5  08/12/2004 14:57:11  seiki_masashi
  Supported WM_StepDataSharing return value: WM_ERRCODE_FRAME_GAP.

  Revision 1.4  08/11/2004 10:04:16  seiki_masashi
  Set to 1 the number of frames to wait when StepDataSharing fails.

  Revision 1.3  08/11/2004 08:02:06  terui
  Modified so that the number of game frame V-blanks is specified for WcInit.

  Revision 1.2  08/10/2004 06:38:40  terui
  Fix comment.

  Revision 1.1  08/10/2004 00:41:54  terui
  Initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    A sample that performs wireless data sharing communication using the WM library.
    Automatically connects to nearby dataShare-1 demos and shares a 4-byte game frame
    A total of 8 bytes of the 4-byte picture frame count is shared
    inside the wireless communications group.

    HOWTO:
        1. To start communications as a parent, press the A button.
           When a nearby child with the same dataShare-1 demo is found,
           data-sharing communication with the child  automatically begins. Communication with up to 15 children is possible at the same time.
        2. To start communications as a child, press the B button.
           When a nearby parent with the dataShare-1 demo is found, communication with that parent
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

#define     PICTURE_FRAME_PER_GAME_FRAME    2

#define     DEFAULT_GGID            0x003fff11
#define     NUM_MAX_CHILD           15
#define     DEFAULT_CHAN            1
#define     SHARED_DATA_SIZE        8
#define     PARENT_DATA_SIZE_MAX    ( SHARED_DATA_SIZE * ( 1 + NUM_MAX_CHILD ) + 4 )
#define     CHILD_DATA_SIZE_MAX     SHARED_DATA_SIZE

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
    PARENT_DATA_SIZE_MAX,              // Size of parent send data
    CHILD_DATA_SIZE_MAX                // Size of child send data
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
static void GameFrame(void);           // Main loop for game frame
static void ModeSelect(void);          // Parent/child select screen
static void ModeError(void);           // Error display screen
static void ModeWorking(void);         // Busy screen
static void ModeParent(void);          // Parent communications screen
static void ModeChild(void);           // Child communications screen
static void VBlankIntr(void);          // V blank interrupt handler
static void PrintSharedData(u16 bitmap);        // Displays shared data

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
static vs32 gPictureFrame;             // Picture frame counter
static vs32 gGameFrame;                // Game frame counter

// send data buffer (must be 32-Byte aligned)
static u16 gSendBuf[SHARED_DATA_SIZE / sizeof(u16)] ATTRIBUTE_ALIGN(32);


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

    // initializes the variables
    gPictureFrame = 0;

    // Set interrupt
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)GX_VBlankIntr(TRUE);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();

    // Initialize memory allocation system
    InitializeAllocateSystem();

    //********************************
    // Initialize Wireless
    WcInit(&defaultParameter, TRUE, FALSE, PICTURE_FRAME_PER_GAME_FRAME);
    //********************************

    // LCD display start
    GX_DispOn();
    GXS_DispOn();

    // debug string output
    OS_Printf("ARM9: WM data sharing demo started.\n");

    // empty call for getting key input data (strategy for pressing A button in IPL)
    KeyRead(&gKey);

    // Main loop
    GameFrame();                       // never return
}

/*---------------------------------------------------------------------------*
  Name:         GameFrame

  Description:  Main loop for game frame

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void GameFrame(void)
{
    for (gGameFrame = 0; TRUE; gGameFrame++)
    {
        // get key input data
        KeyRead(&gKey);

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

        // Wait for completion of multiple picture frames
        while (TRUE)
        {
            // Wait for V-blank
            OS_WaitVBlankIntr();
            if (!(gPictureFrame % PICTURE_FRAME_PER_GAME_FRAME))
            {
                break;
            }
        }
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
    // Clears the screen
    ClearString();
    // Character display
    PrintString(3, 10, 0xf, "Push A to connect as PARENT");
    PrintString(3, 12, 0xf, "Push B to connect as CHILD");

    if (gKey.trg & PAD_BUTTON_A)
    {
        //********************************
        // Start parent communication
        WcStartParent();
        //********************************
    }
    else if (gKey.trg & PAD_BUTTON_B)
    {
        //********************************
        // Start child communication
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
    // Clears the screen
    ClearString();
    // Character display
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
    // Clears the screen
    ClearString();
    // Character display
    PrintString(9, 11, 0xf, "Now working...");

    if (gKey.trg & PAD_BUTTON_START)
    {
        //********************************
        // End communication
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
    BOOL    result;
    u16     bitmap;

    // Edit send data
    gSendBuf[0] = (u16)(gPictureFrame >> 16);
    gSendBuf[1] = (u16)(gPictureFrame & 0xffff);
    gSendBuf[2] = (u16)(gGameFrame >> 16);
    gSendBuf[3] = (u16)(gGameFrame & 0xffff);

    //********************************
    // Synchronize data sharing
    result = WcStepDataSharing((void *)gSendBuf, &bitmap);
    //********************************

    // Clears the screen
    ClearString();
    // Character display
    PrintString(8, 1, 0x2, "Parent mode");
    // Display shared data
    if (result)
    {
        PrintSharedData(bitmap);
    }
    else
    {
        // Try StepDataSharing also in the next frame.
        gPictureFrame--;
    }

    if (gKey.trg & PAD_BUTTON_START)
    {
        //********************************
        // End communication
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
    BOOL    result;
    u16     bitmap;

    // Edit send data
    gSendBuf[0] = (u16)(gPictureFrame >> 16);
    gSendBuf[1] = (u16)(gPictureFrame & 0xffff);
    gSendBuf[2] = (u16)(gGameFrame >> 16);
    gSendBuf[3] = (u16)(gGameFrame & 0xffff);

    //********************************
    // Synchronize data sharing
    result = WcStepDataSharing((void *)gSendBuf, &bitmap);
    //********************************

    // Clears the screen
    ClearString();
    // Character display
    PrintString(8, 1, 0x2, "Child mode");
    // Display shared data
    if (result)
    {
        PrintSharedData(bitmap);
    }
    else
    {
        // Try StepDataSharing also in the next frame.
        gPictureFrame--;
    }

    if (gKey.trg & PAD_BUTTON_START)
    {
        //********************************
        // End communication
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
    // Increment picture frame counter
    gPictureFrame++;

    // Repeat rendering to match game frame
    if (!(gPictureFrame % PICTURE_FRAME_PER_GAME_FRAME))
    {
        // Reflect virtual screen in VRAM
        DC_FlushRange(gScreen, sizeof(gScreen));
        /* IO register is accessed using DMA operation, so cache Wait is not needed*/
        // DC_WaitWriteBufferEmpty();
        GX_LoadBG0Scr(gScreen, 0, sizeof(gScreen));
    }

    // sets the IRQ check flag
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         PrintSharedData

  Description:  Displays shared data from each terminal

  Arguments:    bitmap - Specifies ID bitmap indicating terminal receiving data

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void PrintSharedData(u16 bitmap)
{
    u16     i;
    u8     *p;
    u16     temp[SHARED_DATA_SIZE / sizeof(u16)];

    // Loop for maximum number of parent device + child devices
    for (i = 0; i < (1 + WM_NUM_MAX_CHILD); i++)
    {
        if (bitmap & (0x0001 << i))
        {
            //********************************
            // Get data from terminal with ID of 'i'
            p = WcGetSharedDataAddress(i);
            //********************************

            if (p != NULL)
            {
                // Copy once to ensure alignment
                MI_CpuCopy8(p, temp, SHARED_DATA_SIZE);
                PrintString(2, (s16)(3 + i), 0x4,
                            "%04x%04x-%04x%04x", temp[0], temp[1], temp[2], temp[3]);
            }
            else
            {
                PrintString(2, (s16)(3 + i), 0x1, "xxxxxxxx-xxxxxxxx");
            }
        }
    }
    // Change color only for data for this device
    ColorString(2, (s16)(3 + WcGetAid()), 17, 0xf);
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
