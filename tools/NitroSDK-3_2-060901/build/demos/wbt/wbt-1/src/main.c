/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WBT - demos - wbt-1
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.13  08/01/2006 02:38:45  yosizaki
  fix reinitialization.

  Revision 1.12  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.11  11/21/2005 11:05:59  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.10  06/27/2005 11:10:12  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.9  06/17/2005 06:37:11  miya
  bug fix

  Revision 1.8  02/28/2005 05:26:01  yosizaki
  do-indent.

  Revision 1.7  11/04/2004 04:00:27  yosizaki
  fix comment.

  Revision 1.6  10/31/2004 23:53:18  yosizaki
  delete WC_DEFAULT_CHANNEL (abandoned symbol)

  Revision 1.5  10/28/2004 11:05:36  yosizaki
  change to use block-transfer callback of wc.
  fix around sub-BG VRAM mappings.

  Revision 1.4  10/19/2004 10:27:02  miya
  Made changes so that the Port reception callback usage and the print statement are displayed on the NITRO screen

  Revision 1.3  10/04/2004 08:40:31  miya
  add WBT_CancelCurrentCommand

  Revision 1.2  10/04/2004 05:51:10  miya
  change transfer size according to the programming guideline

  Revision 1.1  08/23/2004 09:47:33  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    A simple functional sample of wireless communication using the WM library.
    It automatically connects to the wbt-demo in the area.

    HOWTO:
        1. To start communications as a parent, press the A button.
           As soon as a nearby child that has the same wbt-1 demo is found, it automatically
           automatically begins. Communication with up to 15 children is possible at the same time.
        2. To start communications as a child, press the B button.
           As soon as a nearby parent that has the same wbt-1 demo is found, it automatically
           automatically begins.
        3. If the START button is pressed during a busy screen while a connection is being established, or during a communication screen after a connection is established,
           the connection is reset, and the display returns to the initial screen.
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitro/wm.h>
#include <nitro/wbt.h>

#include    "font.h"
#include    "text.h"
#include    "wc.h"
#include    "bt.h"


#define MIYA_PRINT 1


/*---------------------------------------------------------------------------*
    constant definitions
 *---------------------------------------------------------------------------*/
#define     KEY_REPEAT_START    25     // number of frames until key repeat starts
#define     KEY_REPEAT_SPAN     10     // number of key repeat interval frames

// Game information
static const WMpparam wcDefaultParameter = {
    NULL, 0, 0,
    GGID_WBT_1,                        // Game group ID
    0x0000,                            // Temporary ID (incremented by 1 each time)
    1,                                 // Entry permit flag
    15,                                // Maximum number of connected children
    0,                                 // Multiboot flag
    0,                                 // Key sharing
    1,                                 // Continuous send flag
    200,                               // Beacon interval
    {0, 0, 0, 0},                      // User name
    {0, 0, 0, 0, 0, 0, 0, 0},          // Game name
    1,                                 // Connect channel (Does MeasureChannel at start time)
    WC_PARENT_DATA_SIZE_MAX,           // Size of parent send data
    WC_CHILD_DATA_SIZE_MAX             // Size of child send data
        // Reserves
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
static void ParentReceiveCallback(u16 aid, u16 *data, u16 length);
static void ChildReceiveCallback(u16 aid, u16 *data, u16 length);

// Function called when data is sent
void    ParentSendCallback(void);
void    ChildSendCallback(void);

// Block transfer status notification function
static void BlockTransferCallback(WMCallback *arg);

// General purpose subroutines
static void KeyRead(KeyInfo * pKey);
static void ClearString(int vram_num);
static void PrintString(s16 x, s16 y, u8 palette, char *text, ...);
static void InitializeAllocateSystem(void);


/*---------------------------------------------------------------------------*
    internal variable definitions
 *---------------------------------------------------------------------------*/
static KeyInfo gKey;                   // key input
static s32 gFrame;                     // Frame counter

// Send/receive buffer for display
static u8 gSendBuf[256] ATTRIBUTE_ALIGN(32);
static BOOL gRecvFlag[1 + WM_NUM_MAX_CHILD];

static int send_counter[16];
static int recv_counter[16];


#define PRINTF mprintf

#define VRAM_SIZE 2*32*32
static u8 g_screen[NUM_OF_SCREEN][VRAM_SIZE] ATTRIBUTE_ALIGN(32);


static TEXT_CTRL textctrl[NUM_OF_SCREEN];
TEXT_CTRL *tc[NUM_OF_SCREEN];

static int vram_num[2];
static int screen_toggle = 0;

static u32 v_blank_intr_counter = 0;
#define TEXT_HEAPBUF_SIZE 0x8000
static u8 text_heap_buffer[TEXT_HEAPBUF_SIZE];

static BOOL wbt_available = FALSE;
static u16 connected_bitmap = 0;


/*---------------------------------------------------------------------------*
  Name:         InitWireless

  Description:  Wireless initialization / Re-initialization process

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitWireless(void)
{
    //********************************
    // Initialize Wireless
    connected_bitmap = 0;
    WcInit(&wcDefaultParameter, FALSE, TRUE, 1);
    WcSetBlockTransferCallback(BlockTransferCallback);
    //********************************
    if (wbt_available)
    {
        bt_stop();
        WBT_End();
        wbt_available = FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  Initialization and main loop.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    int     i;

    // the different types of initialization
    OS_Init();
    OS_InitTick();

    // Memory allocation
    (void)init_text_buf_sys((void *)&(text_heap_buffer[0]),
                            (void *)&(text_heap_buffer[TEXT_HEAPBUF_SIZE]));

    for (i = 0; i < NUM_OF_SCREEN; i++)
    {
        tc[i] = &(textctrl[i]);
        init_text(tc[i], (u16 *)&(g_screen[i]), 4 /* pal no.*/ );
    }

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


    /* BG0 display configuration*/
    GX_SetBankForBG(GX_VRAM_BG_128_A);
    G2_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xf000,      // SCR base block 31
                     GX_BG_CHARBASE_0x00000,    // CHR base block 0
                     GX_BG_EXTPLTT_01);
    G2_SetBG0Priority(0);
    G2_BG0Mosaic(FALSE);
    // 2D display settings
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
    GX_LoadBG0Char(d_CharData, 0, sizeof(d_CharData));
    GX_LoadBGPltt(d_PaletteData, 0, sizeof(d_PaletteData));

    // OBJ display settings
    GX_SetBankForOBJ(GX_VRAM_OBJ_128_B);
    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);

    GX_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ);

    /* sub BG0 display configuration*/
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_48_HI);
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xb800,     // SCR base block 23
                      GX_BG_CHARBASE_0x00000,   // CHR base block 0
                      GX_BG_EXTPLTT_01);
    G2S_SetBG0Priority(0);
    G2S_BG0Mosaic(FALSE);
    GXS_SetGraphicsMode(GX_BGMODE_0);
    GXS_LoadBG0Char(d_CharData, 0, sizeof(d_CharData));
    GXS_LoadBGPltt(d_PaletteData, 0, sizeof(d_PaletteData));

    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);
    GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);

    GXS_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ);

    G2_SetBG0Offset(0, 0);
    G2S_SetBG0Offset(0, 0);


    InitializeAllocateSystem();

    // LCD display start
    GX_DispOn();
    GXS_DispOn();

    // for V-Blank
    (void)OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
    (void)GX_VBlankIntr(TRUE);


    // debug string output
    PRINTF("ARM9: WBT-1 demo started.\n");
    vram_num[0] = 1;
    vram_num[1] = 0;


    // Main loop

    for (gFrame = 0; TRUE; gFrame++)
    {
        text_buf_to_vram(tc[vram_num[0]]);
        text_buf_to_vram(tc[vram_num[1]]);

        if (gKey.trg & PAD_BUTTON_SELECT)
        {
            screen_toggle ^= 1;
        }
        if (gKey.trg & PAD_BUTTON_L)
        {
            vram_num[screen_toggle]--;
            if (vram_num[screen_toggle] < 0)
            {
                vram_num[screen_toggle] = (NUM_OF_SCREEN - 1);
            }
        }
        else if (gKey.trg & PAD_BUTTON_R)
        {
            vram_num[screen_toggle]++;
            if (vram_num[screen_toggle] > (NUM_OF_SCREEN - 1))
            {
                vram_num[screen_toggle] = 0;
            }
        }

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
        InitWireless();
        WBT_InitParent(BT_PARENT_PACKET_SIZE, BT_CHILD_PACKET_SIZE, bt_callback);
        WcSetMpReceiveCallback(ParentReceiveCallback);
        bt_register_blocks();
        WcStartParent();
        wbt_available = TRUE;
        //********************************
    }
    else if (gKey.trg & PAD_BUTTON_B)
    {
        //********************************
        InitWireless();
        WBT_InitChild(bt_callback);
        WcSetMpReceiveCallback(ChildReceiveCallback);
        WcStartChild();
        wbt_available = TRUE;
        //********************************
        vram_num[1] = 2;
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
    PrintString(4, 3, 0x4, "Send:     %08X", send_counter[0]);

    PrintString(4, 5, 0x4, "Receive:");
    {
        s32     i;

        for (i = 1; i < (WM_NUM_MAX_CHILD + 1); i++)
        {
            if (gRecvFlag[i])
            {
                PrintString(5, (s16)(6 + i), 0x4, "Child%02d: %08X", i, recv_counter[i]);
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
    PrintString(4, 3, 0x4, "Send:     %08X", send_counter[0]);
    PrintString(4, 5, 0x4, "Receive:");
    PrintString(5, 7, 0x4, "Parent:  %08X", recv_counter[0]);

    if (gKey.trg & PAD_BUTTON_START)
    {
        //********************************
        WcEnd();
        //********************************
    }
    else if (gKey.trg & PAD_BUTTON_Y)
    {
        bt_start();
    }
    else if (gKey.trg & PAD_BUTTON_X)
    {
        bt_stop();
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
    vu16   *ptr;
    int     i;

    DC_FlushRange((void *)&(g_screen[vram_num[0]]), VRAM_SIZE);
    /* IO register is accessed using DMA operation, so cache Wait is not needed*/
    // DC_WaitWriteBufferEmpty();
    GX_LoadBG0Scr(&(g_screen[vram_num[0]]), 0, VRAM_SIZE);

    DC_FlushRange((void *)&(g_screen[vram_num[1]]), VRAM_SIZE);
    /* IO register is accessed using DMA operation, so cache Wait is not needed*/
    // DC_WaitWriteBufferEmpty();
    GXS_LoadBG0Scr(&(g_screen[vram_num[1]]), 0, VRAM_SIZE);

    if (screen_toggle)
        ptr = (u16 *)G2S_GetBG0ScrPtr();
    else
        ptr = (u16 *)G2_GetBG0ScrPtr();

    for (i = 0; i < 32; i++)
    {
        *ptr = (u16)((2 << 12) | (0x00ff & '='));
        ptr++;
    }


    v_blank_intr_counter++;
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         ParentReceiveCallback

  Description:  Function that is called when a parent receives data from a child.

  Arguments:    aid     - aid of send origin child
                data    - Pointer to received data (NULL notifies disconnection)
                length  - Size of received data

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ParentReceiveCallback(u16 aid, u16 *data, u16 length)
{
    if (data != NULL)
    {
        gRecvFlag[aid] = TRUE;
        // Copy source is aligned to 2 bytes (not 4 bytes)
        recv_counter[aid]++;
        WBT_MpParentRecvHook((u8 *)data, length, aid);
    }
    else
    {
        gRecvFlag[aid] = FALSE;
    }
}


/*---------------------------------------------------------------------------*
  Name:         ChildReceiveCallback

  Description:  Function that is called when a child receives data from a parent.

  Arguments:    aid     - aid of sending source parent (always 0)
                data    - Pointer to received data (NULL notifies disconnection)
                length  - Size of received data

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ChildReceiveCallback(u16 aid, u16 *data, u16 length)
{
    (void)aid;
    recv_counter[0]++;
    if (data != NULL)
    {
        gRecvFlag[0] = TRUE;
        // Copy source is aligned to 2 bytes (not 4 bytes)
        WBT_MpChildRecvHook((u8 *)data, length);
    }
    else
    {
        gRecvFlag[0] = FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         ChildReceiveCallback

  Description:  Block transfer status notification function

  Arguments:    arg     - Callback pointer for the notification source WM function.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void BlockTransferCallback(WMCallback *arg)
{
    WCStatus wcStatus = WcGetStatus();
    switch (arg->apiid)
    {
    case WM_APIID_START_MP:
        {                              /* begin MP state*/
            WMStartMPCallback *cb = (WMStartMPCallback *)arg;
            switch (cb->state)
            {
            case WM_STATECODE_MP_START:
                if (wcStatus == WC_STATUS_PARENT)
                {
                    ParentSendCallback();
                }
                else if (wcStatus == WC_STATUS_CHILD)
                {
                    WBT_SetOwnAid(WcGetAid());
                    mfprintf(tc[2], "aid = %d\n", WcGetAid());
                    bt_start();
                    ChildSendCallback();
                }
                break;
            }
        }
        break;
    case WM_APIID_SET_MP_DATA:
        {                              /* complete single MP communication*/
            if (wcStatus == WC_STATUS_PARENT)
            {
                if (connected_bitmap != 0)
                {
                    ParentSendCallback();
                }
            }
            else if (wcStatus == WC_STATUS_CHILD)
            {
                ChildSendCallback();
            }
        }
        break;
    case WM_APIID_START_PARENT:
        {                              /* connect to a new child*/
            WMStartParentCallback *cb = (WMStartParentCallback *)arg;
            if (wcStatus == WC_STATUS_PARENT)
            {
                switch (cb->state)
                {
                case WM_STATECODE_CONNECTED:
                    if (connected_bitmap == 0)
                    {
                        ParentSendCallback();
                    }
                    connected_bitmap |= (1 << cb->aid);
                    break;
                case WM_STATECODE_DISCONNECTED:
                    connected_bitmap &= ~(1 << cb->aid);
                    break;
                }
            }
        }
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         ParentSendCallback

  Description:  Function that is called when a parent sends data to a child.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ParentSendCallback(void)
{
    const u16 size = (u16)WBT_MpParentSendHook(gSendBuf, WC_PARENT_DATA_SIZE_MAX);
    send_counter[0]++;
    WcSetParentData(gSendBuf, size);
}


/*---------------------------------------------------------------------------*
  Name:         ChildSendCallback

  Description:  Function that is called when a child receives data from a parent.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ChildSendCallback(void)
{
    const u16 size = (u16)WBT_MpChildSendHook(gSendBuf, WC_CHILD_DATA_SIZE_MAX);
    send_counter[0]++;
    WcSetChildData(gSendBuf, size);
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
static void ClearString(int vram_num)
{
    MI_DmaClear32(0, (void *)&(g_screen[vram_num]), VRAM_SIZE);
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
    (void)OS_VSNPrintf(temp, 32, text, vlist);
    va_end(vlist);
    *(u16 *)(&temp[31]) = 0x0000;
    for (i = 0;; i++)
    {
        if (temp[i] == 0x00)
        {
            break;
        }
        tc[1]->screen[(y * 32) + x + i] = (u16)((palette << 12) | temp[i]);
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

    mprintf(" arena lo = %08x\n", OS_GetMainArenaLo());
    mprintf(" arena hi = %08x\n", OS_GetMainArenaHi());

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
