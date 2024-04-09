/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - demos - thread-10
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.4  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.3  11/21/2005 10:55:51  kitase_hirotake
  Deleted PXI_Init
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.2  08/26/2005 04:02:23  kitase_hirotake
  Corrected "Project" notation

  Revision 1.1  07/13/2005 06:24:49  kitase_hirotake
  Initial Release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include "font.h"

#define STACK_SIZE 1024
#define MAIN_HEAP_SIZE    0x100000
#define MESSAGE_QUEUE_SIZE    64

const int g_nThreadMax = (
#ifdef SDK_THREAD_INFINITY
                             1024
#else
                             OS_THREAD_MAX_NUM
#endif
    );

OSThreadQueue g_threadQueue;
OSMessage g_msgBuffer[MESSAGE_QUEUE_SIZE];
OSMessageQueue g_msgQueue;
void    proc(void *);
void    AllocThreadStorage(OSThread **ppThread, void **ppStackTop);
OSThread *SpawnThread(void (*func) (void *), u32 prio);
void    PrepareAlloc();

static void Init3D(void);
static void VBlankIntr(void);

static void ClearString(void);
static void PrintString(s16 x, s16 y, u8 palette, char *text, ...);
static void ColorString(s16 x, s16 y, s16 length, u8 palette);

int     g_nTotalThreadCount = 0;

MATHRandContext16 rnd;
static u16 gScreen[32 * 32];

void NitroMain(void)
{
    u32     nPadResult = 0, nPadResultPrev = 0;

    OS_Init();
    FX_Init();
    GX_Init();
    OS_InitThread();
    OS_InitThreadQueue(&g_threadQueue);
    OS_InitMessageQueue(&g_msgQueue, &g_msgBuffer[0], MESSAGE_QUEUE_SIZE);

    GX_DispOff();
    GXS_DispOff();

    PrepareAlloc();

    OS_Printf("==== sample start\n");
    OS_Printf("infinity thread support: ");
#ifdef SDK_THREAD_INFINITY
    OS_Printf("yes\n");
#else
    OS_Printf("no\n");
#endif

    // initializes display settings
    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
    (void)GX_DisableBankForLCDC();
    MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);
    MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);
    MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);
    MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);

    // 3D related initialization
    Init3D();

    // Initialize 2D character display screen
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_32_H);
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);  // Screen base a little
    G2S_SetBG0Priority(0);
    G2S_BG0Mosaic(FALSE);
    GXS_SetGraphicsMode(GX_BGMODE_0);
    GXS_SetVisiblePlane(GX_PLANEMASK_BG0);

    GXS_LoadBG0Char(d_CharData, 0, sizeof(d_CharData));
    GXS_LoadBGPltt(d_PaletteData, 0, sizeof(d_PaletteData));

    MI_CpuFillFast((void *)gScreen, 0, sizeof(gScreen));
    DC_FlushRange(gScreen, sizeof(gScreen));
    GXS_LoadBG0Scr(gScreen, 0, sizeof(gScreen));

    // Set interrupt
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
    (void)GX_VBlankIntr(TRUE);

    // LCD display start
    GX_DispOn();
    GXS_DispOn();

    MATH_InitRand16(&rnd, OS_GetVBlankCount());

    // Create first thread
    (void)SpawnThread(proc, 16);

    while (1)
    {
        G3X_Reset();
        G3_Identity();
        G3_PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 0, 31, 0);

        nPadResultPrev = nPadResult;
        nPadResult = PAD_Read();
        if ((nPadResult ^ nPadResultPrev) & nPadResult & PAD_BUTTON_A)
        {
            (void)SpawnThread(proc, 16);
        }
        if ((nPadResult ^ nPadResultPrev) & nPadResult & PAD_BUTTON_B)
        {
            OS_DumpThreadList();
        }

        PrintString(1, 1, 15, "TotalThread = %d", OS_GetNumberOfThread());
        PrintString(1, 2, 15, "ObjThread   = %d", g_nTotalThreadCount);

        G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);

        // Waiting for the V-blank
        OS_WaitVBlankIntr();

        OS_WakeupThread(&g_threadQueue);
    }

    OS_Printf("==== Finish sample.\n");
    OS_Terminate();
}

/* Allocate memory required for the thread*/
void AllocThreadStorage(OSThread **ppThread, void **ppStackTop)
{
    /* Check free memory so that warning will not occur*/
    if (OS_GetMaxFreeSize(OS_ARENA_MAIN, OS_CURRENT_HEAP_HANDLE) < STACK_SIZE
        || OS_GetTotalFreeSize(OS_ARENA_MAIN,
                               OS_CURRENT_HEAP_HANDLE) < STACK_SIZE + sizeof(OSThread))
    {
        *ppThread = NULL;
        *ppStackTop = NULL;
        return;
    }
    *ppThread = OS_Alloc(sizeof(OSThread));
    *ppStackTop = OS_Alloc(STACK_SIZE);
}

/* Allocate the memory required by the thread and create the thread*/
OSThread *SpawnThread(void (*func) (void *), u32 prio)
{
    OSThread *pThread = NULL;
    void   *pStackTop = NULL;
    AllocThreadStorage(&pThread, &pStackTop);
    if (!pThread || !pStackTop || OS_GetNumberOfThread() >= g_nThreadMax)
    {
        return NULL;
    }

    //Obtain the initial position with a random number and pass it
    OS_CreateThread(pThread, func, NULL, (void *)((u8 *)pStackTop + STACK_SIZE), STACK_SIZE, prio);
    OS_WakeupThreadDirect(pThread);
    ++g_nTotalThreadCount;
    OS_Printf("spawn     : id=%08x, number=%d(total %d)\n", pThread->id, OS_GetNumberOfThread(),
              g_nTotalThreadCount);
    return pThread;
}

/* Prepare the heap*/
void PrepareAlloc()
{
    void   *heapStart = NULL;
    void   *nstart = NULL;
    OSHeapHandle handle;
    //---- Initialize memory allocation system for MainRAM arena
    nstart = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 2);
    OS_SetMainArenaLo(nstart);

    //---- Allocate region for heap from arena
    heapStart = OS_AllocFromMainArenaLo(MAIN_HEAP_SIZE, 32);
    OS_Printf("heapStart %x\n", heapStart);

    //---- Create heap
    handle = OS_CreateHeap(OS_ARENA_MAIN, heapStart, (void *)((u32)heapStart + MAIN_HEAP_SIZE));
    OS_Printf("heap handle %d\n", handle);

    //---- Set current heap
    (void)OS_SetCurrentHeap(OS_ARENA_MAIN, handle);
}


//--------------------------------------------------------------------------------
//    proc
//
void proc(void *)
{
    u16     x, y;
    fx16    fx, fy;

    //Initial coordinates
    x = (u16)MATH_Rand16(&rnd, 256);
    y = (u16)MATH_Rand16(&rnd, 192);

    OS_Printf("x=%d, y=%d\n", x, y);

    while (1)
    {
        fx = (fx16)(((x - 128) * 0x1000) / 128);
        fy = (fx16)(((96 - y) * 0x1000) / 96);

        G3_Begin(GX_BEGIN_TRIANGLES);
        {
            G3_Color(GX_RGB(31, 31, 31));
            G3_Vtx(fx, fy, 0);
            G3_Color(GX_RGB(31, 31, 31));
            G3_Vtx((fx16)(fx + (FX16_ONE >> 5)), (fx16)(fy + (FX16_ONE >> 5)), 0);
            G3_Color(GX_RGB(31, 31, 31));
            G3_Vtx((fx16)(fx - (FX16_ONE >> 5)), (fx16)(fy - (FX16_ONE >> 5)), 0);
        }
        G3_End();

        OS_SleepThread(&g_threadQueue);

        if ((u16)MATH_Rand16(&rnd, 2) == 0)
        {
            x++;
            if (x > 256)
            {
                x = 256;
            }
        }
        else
        {
            x--;
            if (x < 0)
            {
                x = 0;
            }
        }
        if ((u16)MATH_Rand16(&rnd, 2) == 0)
        {
            y++;
            if (y > 192)
            {
                y = 192;
            }
        }
        else
        {
            y--;
            if (y < 0)
            {
                y = 0;
            }
        }

    }
}

/*---------------------------------------------------------------------------*
  Name:         Init3D

  Description:  Initialization process to display in 3D

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void Init3D(void)
{
    G3X_Init();
    G3X_InitMtxStack();
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
    GX_SetVisiblePlane(GX_PLANEMASK_BG0);
    G2_SetBG0Priority(0);
    G3X_AlphaTest(FALSE, 0);
    G3X_AntiAlias(TRUE);
    G3X_EdgeMarking(FALSE);
    G3X_SetFog(FALSE, (GXFogBlend)0, (GXFogSlope)0, 0);
    G3X_SetClearColor(0, 0, 0x7fff, 63, FALSE);
    G3_ViewPort(0, 0, 255, 191);
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
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
    DC_FlushRange(gScreen, sizeof(gScreen));
    GXS_LoadBG0Scr(gScreen, 0, sizeof(gScreen));

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

/*====== End of main.c ======*/
