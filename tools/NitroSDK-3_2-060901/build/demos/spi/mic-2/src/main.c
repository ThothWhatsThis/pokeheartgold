/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SPI - demos - mic-2
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.13  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.12  11/21/2005 11:03:53  kitase_hirotake
  Deleted OS_InitPrintServer, OS_PrintServer and PXI_Init
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.11  04/08/2005 01:49:02  yosizaki
  add DC_InvalidateRange before reading the capture-buffer.

  Revision 1.10  02/28/2005 05:26:23  yosizaki
  do-indent.

  Revision 1.9  11/04/2004 07:18:57  terui
  Small revisions in line with revising the specification of MIC_GetLastSamplingAddress.

  Revision 1.8  09/17/2004 09:36:03  terui
  Revised in connection with changes to MIC_StartAutoSampling(Async) parameters    

  Revision 1.7  2004/08/23 07:27:27  yasu
  Workaround for pushing A Button in IPL

  Revision 1.6  08/20/2004 02:44:31  terui
  Revised so that PM_SetAmp is also done when TS_VERSION=0.

  Revision 1.5  08/05/2004 07:38:59  takano_makoto
  Revised due to change of MICSamplingType to enum type    

  Revision 1.4  08/04/2004 23:44:28  terui
  Changed LCD backlight operation functions

  Revision 1.3  08/02/2004 00:47:45  yada
  PM_GAINAMP_80 -> PM_AMPGAIN_80

  Revision 1.2  07/31/2004 12:46:19  terui
  Added code to initialize PMIC for mic

  Revision 1.1  06/17/2004 04:42:46  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    A sample that controls mic sampling status.

    USAGE:
        UP, DOWN   : Control sampling span.
        LEFT, RIGHT: Control sampling bit range. ( 8bit or 12 bit )
        A: start / stop sampling. (toggle)
        B: force-stop. (stop and ignore rest data)

    HOWTO:
        Initialize memory allocate system to get 32byte aligned big buffer.
        Initialize MIC library.
        When sampling is stopped, you can change status
           and start auto sampling.
           Debug-output is sampling data for tool "mic2wav.exe".
        Debug-output log can make its waveform files by tool "mic2wav.exe".
           > $(NITROSDK_ROOT)/tools/bin/mic2wav [logfile] [,directory]
           each sampling data (separated by A-Button) creates a waveform file
           in "[directory]/%08X.wav".

    NOTE:
        The speed of debug-output is later than sampling.
           When you stop sampling, then please wait all data is printed.

 *---------------------------------------------------------------------------*/

#include    <nitro.h>

#ifdef  SDK_TS
#if     ( SDK_TS_VERSION >= 100 )
#include    <nitro/spi/common/pm_common.h>
#include    <nitro/spi/ARM9/pm.h>
#endif
#endif


/*---------------------------------------------------------------------------*
    constant definitions
 *---------------------------------------------------------------------------*/
#define     KEY_REPEAT_START    25     // number of frames until key repeat starts
#define     KEY_REPEAT_SPAN     10     // number of key repeat interval frames
#define     TEST_BUFFER_SIZE    ( 1024 * 1024 ) // 1M


/*---------------------------------------------------------------------------*
    structure definitions
 *---------------------------------------------------------------------------*/
// key input data
typedef struct KeyInformation
{
    u16     cnt;                       // unprocessed input values
    u16     trg;                       // push trigger input
    u16     up;                        // release trigger input
    u16     rep;                       // push and hold repeat input

}
KeyInformation;

/*---------------------------------------------------------------------------*
    Internal function definitions
 *---------------------------------------------------------------------------*/
static void InitializeAllocateSystem(void);
static void Init3D(void);
static void Draw3D(void);
static void DrawLine(s16 sx, s16 sy, s16 ex, s16 ey);
static void VBlankIntr(void);
static void KeyRead(KeyInformation * pKey);
static void SetDrawData(void *address, MICSamplingType type);
static void PrintfVariableData(void);

/*---------------------------------------------------------------------------*
    internal variable definitions
 *---------------------------------------------------------------------------*/
static KeyInformation gKey;
static MICAutoParam gMicAutoParam;
static u8 *gMicData;
static u8 gDrawData[192];


/* this differs from mic-1 in that there is a debug output of data
   and the fact that recording is always performed just once     */

/* variables for waveform output*/
static volatile BOOL g_sample_busy = FALSE;
static const void *g_record_smps = NULL;

static void StartSampling(void);
static void OnSampleDone(MICResult result, void *arg);
static void OutputSampleWave(void *dat, MICSamplingType type);
static void StopSamplingOutput(void);

/* begin sampling*/
static void StartSampling(void)
{
    gMicAutoParam.buffer = (void *)gMicData;
    gMicAutoParam.size = TEST_BUFFER_SIZE;
    gMicAutoParam.loop_enable = FALSE;
    gMicAutoParam.full_callback = OnSampleDone;
    g_sample_busy = TRUE;
    g_record_smps = gMicData;
    PrintfVariableData();
    (void)MIC_StartAutoSampling(&gMicAutoParam);
}

/* end sampling output*/
static void StopSamplingOutput(void)
{
    OS_PutString("$end\n");
    OS_PutString("\n");
    g_record_smps = NULL;
}

/* sampling completion notification, or waveform output process at stop*/
static void OnSampleDone(MICResult result, void *arg)
{
    (void)result;
    (void)arg;
    if (g_sample_busy)
    {
        g_sample_busy = FALSE;
    }
}

/* log output the waveform during sampling*/
static void OutputSampleWave(void *dat, MICSamplingType type)
{
    /* Up to 2 lines of 16 samples per line;    
       Output beyond that may not be logged.    */
    enum
    { smps_per_line = 16, max_line_per_frame = 2 };
    if (!g_record_smps || !dat)
        return;

    DC_InvalidateRange((void *)g_record_smps, (u32)((u8 *)dat - (u8 *)g_record_smps));
    switch (type)
    {
    case MIC_SAMPLING_TYPE_8BIT:
        {
            typedef u8 SMP;
            /* Below is identical to MIC_SAMPLING_TYPE_12BIT*/
            char    buf[1 + (sizeof(SMP) * 2 + 1) * smps_per_line + 1 + 1], *s;
            const SMP *from = (const SMP *)g_record_smps;
            const SMP *to = (const SMP *)dat;
            int     lines = 0;
            while ((lines < max_line_per_frame) && (from + smps_per_line <= to))
            {
                int     i, j;
                s = buf;
                *s++ = '|';
                for (i = 0; i < smps_per_line; ++i)
                {
                    u32     unit = from[i];
                    for (j = sizeof(SMP) * 8; (j -= 4) >= 0;)
                    {
                        u32     c = (u32)((unit >> j) & 0x0F);
                        c += (u32)((c < 10) ? ('0' - 0) : ('A' - 10));
                        MI_WriteByte(s++, (u8)c);
                    }
                    MI_WriteByte(s++, (u8)',');
                }
                MI_WriteByte(s++, (u8)'\n');
                MI_WriteByte(s++, (u8)'\0');
                OS_PutString(buf);
                from += smps_per_line;
                ++lines;
            }
            g_record_smps = from;

        }
        break;
    case MIC_SAMPLING_TYPE_12BIT:
        {
            typedef u16 SMP;
            /* Below is identical to MIC_SAMPLING_TYPE_8BIT*/
            char    buf[1 + (sizeof(SMP) * 2 + 1) * smps_per_line + 1 + 1], *s;
            const SMP *from = (const SMP *)g_record_smps;
            const SMP *to = (const SMP *)dat;
            int     lines = 0;
            while ((lines < max_line_per_frame) && (from + smps_per_line <= to))
            {
                int     i, j;
                s = buf;
                *s++ = '|';
                for (i = 0; i < smps_per_line; ++i)
                {
                    u32     unit = from[i];
                    for (j = sizeof(SMP) * 8; (j -= 4) >= 0;)
                    {
                        u32     c = (u32)((unit >> j) & 0x0F);
                        c += (u32)((c < 10) ? ('0' - 0) : ('A' - 10));
                        MI_WriteByte(s++, (u8)c);
                    }
                    MI_WriteByte(s++, (u8)',');
                }
                MI_WriteByte(s++, (u8)'\n');
                MI_WriteByte(s++, (u8)'\0');
                OS_PutString(buf);
                from += smps_per_line;
                ++lines;
            }
            g_record_smps = from;

        }
        break;
    }

    /* Also output remaining data after stopping sampling*/
    if (!g_sample_busy && g_record_smps)
    {
        if ((u8 *)g_record_smps + smps_per_line * 2 >= (u8 *)dat)
        {
            StopSamplingOutput();
        }
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

    // 3D related initialization
    Init3D();

    // Set interrupt
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
    (void)GX_VBlankIntr(TRUE);

    //****************************************************************
    // Initialize MIC.
    InitializeAllocateSystem();
    // Because the memory allocated by OS_Alloc is 32-byte aligned
    // other memory is not destroyed even if the cache is manipulated.
    gMicData = (u8 *)OS_Alloc(TEST_BUFFER_SIZE);
    gMicAutoParam.type = MIC_SAMPLING_TYPE_12BIT;
    gMicAutoParam.rate = MIC_SAMPLING_RATE_11K;
    MIC_Init();

#ifdef  SDK_TS
    // Initialize PMIC
    PM_Init();
    // AMP on
    (void)PM_SetAmp(PM_AMP_ON);
#if ( SDK_TS_VERSION >= 100 )
    // Adjust AMP gain
    (void)PM_SetAmpGain(PM_AMPGAIN_80);
#endif
#if ( SDK_TS_VERSION == 100 )
    // Turn off LCD backlight to deal with noise.
    (void)PM_SetBackLight(PM_LCD_ALL, PM_BACKLIGHT_OFF);
#endif
#endif

    //****************************************************************

    // Initialize internal variable
    {
        s32     i;
        for (i = 0; i < 192; i++)
        {
            gDrawData[i] = 0x80;
        }
    }

    // LCD display start
    GX_DispOn();
    GXS_DispOn();

    // Output of start messages
    OS_Printf("#ARM9: MIC demo started.\n");
    OS_Printf("#   up/down    -> change sampling span\n");
    OS_Printf("#   left/right -> change bit range\n");
    OS_Printf("#   A          -> start / stop\n");
    OS_Printf("#   B          -> force-stop\n");

    // empty call for getting key input data (strategy for pressing A button in IPL)
    KeyRead(&gKey);

    // main loop
    while (TRUE)
    {
        // get key input data
        KeyRead(&gKey);

        // Start if button A pressed; stop if button A pressed again
        if ((gKey.trg & PAD_BUTTON_A) != 0)
        {
            if (!g_sample_busy)
            {
                StartSampling();
            }
            else
            {
                (void)MIC_StopAutoSampling();
                OnSampleDone(MIC_RESULT_SUCCESS, &gMicAutoParam);
            }
        }
        // If button B pressed, stop and ignore remaining data.
        if ((gKey.trg & PAD_BUTTON_B) != 0)
        {
            if (g_sample_busy)
            {
                (void)MIC_StopAutoSampling();
                OnSampleDone(MIC_RESULT_SUCCESS, &gMicAutoParam);
            }
            if (g_record_smps)
            {
                StopSamplingOutput();
            }
        }

        // Change variable parameters if sampling output under way    
        if (!g_record_smps)
        {
            // Change sampling type (bit width)
            if ((gKey.trg | gKey.rep) & (PAD_KEY_LEFT | PAD_KEY_RIGHT))
            {
                //****************************************************************
                if (gMicAutoParam.type == MIC_SAMPLING_TYPE_8BIT)
                {
                    gMicAutoParam.type = MIC_SAMPLING_TYPE_12BIT;
                }
                else
                {
                    gMicAutoParam.type = MIC_SAMPLING_TYPE_8BIT;
                }
                //****************************************************************
                if (!g_record_smps)
                    PrintfVariableData();
            }
            // Change sampling rate
            if ((gKey.trg | gKey.rep) & PAD_KEY_UP)
            {
                //****************************************************************
                switch (gMicAutoParam.rate)
                {
                case MIC_SAMPLING_RATE_8K:
                    gMicAutoParam.rate = MIC_SAMPLING_RATE_11K;
                    break;
                case MIC_SAMPLING_RATE_11K:
                    gMicAutoParam.rate = MIC_SAMPLING_RATE_16K;
                    break;
                case MIC_SAMPLING_RATE_16K:
                    gMicAutoParam.rate = MIC_SAMPLING_RATE_22K;
                    break;
                case MIC_SAMPLING_RATE_22K:
                    gMicAutoParam.rate = MIC_SAMPLING_RATE_32K;
                    break;
                case MIC_SAMPLING_RATE_32K:
                    gMicAutoParam.rate = MIC_SAMPLING_RATE_8K;
                    break;
                }
                //****************************************************************
                if (!g_record_smps)
                    PrintfVariableData();
            }
            if ((gKey.trg | gKey.rep) & PAD_KEY_DOWN)
            {
                //****************************************************************
                switch (gMicAutoParam.rate)
                {
                case MIC_SAMPLING_RATE_8K:
                    gMicAutoParam.rate = MIC_SAMPLING_RATE_32K;
                    break;
                case MIC_SAMPLING_RATE_11K:
                    gMicAutoParam.rate = MIC_SAMPLING_RATE_8K;
                    break;
                case MIC_SAMPLING_RATE_16K:
                    gMicAutoParam.rate = MIC_SAMPLING_RATE_11K;
                    break;
                case MIC_SAMPLING_RATE_22K:
                    gMicAutoParam.rate = MIC_SAMPLING_RATE_16K;
                    break;
                case MIC_SAMPLING_RATE_32K:
                    gMicAutoParam.rate = MIC_SAMPLING_RATE_22K;
                    break;
                }
                //****************************************************************
                if (!g_record_smps)
                    PrintfVariableData();
            }
        }

        // Log output of waveform
        OutputSampleWave(MIC_GetLastSamplingAddress(), gMicAutoParam.type);

        // Draw Waveform
        if (g_sample_busy)
        {
            SetDrawData(MIC_GetLastSamplingAddress(), gMicAutoParam.type);
            Draw3D();
        }

        // Waiting for the V-blank
        OS_WaitVBlankIntr();
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
  Name:         Init3D

  Description:  Initialization process to display in 3D

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void Init3D(void)
{
    G3X_Init();
    G3X_InitMtxStack();
    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
    GX_SetVisiblePlane(GX_PLANEMASK_BG0);
    G2_SetBG0Priority(0);
    G3X_SetShading(GX_SHADING_TOON);
    G3X_AlphaTest(FALSE, 0);
    G3X_AlphaBlend(TRUE);
    G3X_AntiAlias(TRUE);
    G3X_EdgeMarking(FALSE);
    G3X_SetFog(FALSE, (GXFogBlend)0, (GXFogSlope)0, 0);
    G3X_SetClearColor(0, 0, 0x7fff, 63, FALSE);
    G3_ViewPort(0, 0, 255, 191);
}

/*---------------------------------------------------------------------------*
  Name:         Draw3D

  Description:  Displays waveforms in 3D

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void Draw3D(void)
{
    G3X_Reset();

    G3_MtxMode(GX_MTXMODE_PROJECTION);
    G3_Identity();
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    G3_Identity();

    G3_PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 0, 31, 0);

    if (g_sample_busy)
    {
        s32     i;

        for (i = 0; i < 191; i++)
        {
            DrawLine((s16)(gDrawData[i]), (s16)i, (s16)(gDrawData[i + 1]), (s16)(i + 1));
        }
    }

    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
}

/*---------------------------------------------------------------------------*
  Name:         DrawLine

  Description:  Draw lines in triangular polygon

  Arguments:    sx  - x coordinate of line's starting point
                sy  - y coordinate of line's starting point
                ex  - x coordinate of line's ending point
                ey  - y coordinate of line's ending point

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DrawLine(s16 sx, s16 sy, s16 ex, s16 ey)
{
    fx16    fsx, fsy, fex, fey;

    fsx = (fx16)(((sx - 128) * 0x1000) / 128);
    fsy = (fx16)(((96 - sy) * 0x1000) / 96);
    fex = (fx16)(((ex - 128) * 0x1000) / 128);
    fey = (fx16)(((96 - ey) * 0x1000) / 96);

    G3_Begin(GX_BEGIN_TRIANGLES);
    {
        G3_Color(GX_RGB(31, 31, 31));
        G3_Vtx(fsx, fsy, 0);
        G3_Color(GX_RGB(31, 31, 31));
        G3_Vtx(fex, fey, 0);
        G3_Color(GX_RGB(31, 31, 31));
        G3_Vtx(fsx, fsy, 1);
    }
    G3_End();
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  V blank interrupt vector.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    // sets the IRQ check flag
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         KeyRead

  Description:  Edits key input data.
                Detects press trigger, release trigger and hold repeat trigger.

  Arguments:    pKey  - Structure that holds key input data to be edited.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void KeyRead(KeyInformation * pKey)
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
  Name:         SetDrawData

  Description:  Stores the current newest sampled data in the
                buffer that puts it on the display.

  Arguments:    address - Location in main memory where the most recent sampling
                          data are stored by the component
                type    - Sampling type (bit width)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SetDrawData(void *address, MICSamplingType type)
{
    s32     i;

    if (!address)
        return;

    if (type == MIC_SAMPLING_TYPE_8BIT)
    {
        u8     *p;

        p = (u8 *)((u32)address - 191);
        if (p < gMicData)
        {
            p = (u8 *)((u32)p + TEST_BUFFER_SIZE);
        }
        DC_InvalidateRange((void *)((u32)p & 0xffffffe0), 32);
        for (i = 0; i < 192; i++)
        {
            gDrawData[i] = *p;
            p++;
            if ((u32)p >= (u32)(gMicData + TEST_BUFFER_SIZE))
            {
                p -= TEST_BUFFER_SIZE;
            }
            if (((u32)p % 32) == 0)
            {
                DC_InvalidateRange(p, 32);
            }
        }
    }

    if (type == MIC_SAMPLING_TYPE_12BIT)
    {
        u16    *p;

        p = (u16 *)((u32)address - 382);
        if ((u32)p < (u32)gMicData)
        {
            p = (u16 *)((u32)p + TEST_BUFFER_SIZE);
        }
        DC_InvalidateRange((void *)((u32)p & 0xffffffe0), 32);
        for (i = 0; i < 192; i++)
        {
            gDrawData[i] = (u8)((*p >> 8) & 0x00ff);
            p++;
            if ((u32)p >= (u32)(gMicData + TEST_BUFFER_SIZE))
            {
                p = (u16 *)((u32)p - TEST_BUFFER_SIZE);
            }
            if (((u32)p % 32) == 0)
            {
                DC_InvalidateRange(p, 32);
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         PrintfVariableData

  Description:  Print output variable sampling setting

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void PrintfVariableData(void)
{
    s32     range = 0;
    s32     rate = 0;

    switch (gMicAutoParam.type)
    {
    case MIC_SAMPLING_TYPE_8BIT:
        range = 8;
        break;
    case MIC_SAMPLING_TYPE_12BIT:
        range = 16;
        break;
    }

    switch (gMicAutoParam.rate)
    {
    case MIC_SAMPLING_RATE_8K:
        rate = 8000;
        break;
    case MIC_SAMPLING_RATE_11K:
        rate = 11025;
        break;
    case MIC_SAMPLING_RATE_16K:
        rate = 16000;
        break;
    case MIC_SAMPLING_RATE_22K:
        rate = 22050;
        break;
    case MIC_SAMPLING_RATE_32K:
        rate = 32000;
        break;
    }

    OS_Printf("$rate=%d\n$bits=%d\n", rate, range);
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
