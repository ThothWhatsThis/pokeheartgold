/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MATH - demos
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.5  01/18/2006 02:11:28  kitase_hirotake
  do-indent

  Revision 1.4  11/21/2005 10:47:29  kitase_hirotake
  Deleted OS_InitPrintServer, OS_PrintServer and PXI_Init
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.3  07/22/2005 08:18:58  kitase_hirotake
  Corrected contents of debug output

  Revision 1.2  07/05/2005 03:00:38  kitase_hirotake
  (none)

  
  $NoKeywords: $

 *---------------------------------------------------------------------------*/

#include    <nitro.h>
#include <nitro/fx/fx_trig.h>

//mic
#ifdef  SDK_TS
#if     ( SDK_TS_VERSION >= 100 )
#include    <nitro/spi/common/pm_common.h>
#include    <nitro/spi/ARM9/pm.h>
#endif
#endif


/*---------------------------------------------------------------------------*
    constant definitions
 *---------------------------------------------------------------------------*/
//mic
#define     KEY_REPEAT_START    25     // number of frames until key repeat starts
#define     KEY_REPEAT_SPAN     10     // number of key repeat interval frames
#define     TEST_BUFFER_SIZE    ( 1024 * 1024 ) // 1M


//fft
#define FFT_NSHIFT         9
#define FFT_N              (1 << FFT_NSHIFT)

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
//mic
static void InitializeAllocateSystem(void);
static void Init3D(void);
static void Draw3D(void);
static void DrawLine(s16 sx, s16 sy, s16 ex, s16 ey);
static void VBlankIntr(void);
static void KeyRead(KeyInformation * pKey);
static void SetDrawData(void *address);
static void PrintfVariableData(void);

//original
static void Draw3D2(void);
static void DrawBar(s16 sx, s16 sy, s16 ex, s16 ey);

/*---------------------------------------------------------------------------*
    internal variable definitions
 *---------------------------------------------------------------------------*/
//mic
static KeyInformation gKey;
static MICAutoParam gMicAutoParam;
static u8 *gMicData;
static u8 gDrawData[FFT_N];

//fft
static fx32 data[FFT_N * 2];
static fx32 orig[FFT_N * 2];
static fx16 sinTable[FFT_N - FFT_N / 4];
static fx16 sinTable2[(FFT_N - FFT_N / 4) / 2];

//original
static BOOL drawMode;
static fx32 totalData[FFT_N * 2];
static OSTick start, end;
static u8 blockCount;

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
    OS_InitTick();                     //fft
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

    gMicAutoParam.type = MIC_SAMPLING_TYPE_12BIT;       //Forcibly use 12bit for Fourier transform.
    gMicAutoParam.buffer = (void *)gMicData;
    gMicAutoParam.size = TEST_BUFFER_SIZE;
    gMicAutoParam.rate = MIC_SAMPLING_RATE_8K;
    gMicAutoParam.loop_enable = TRUE;
    gMicAutoParam.full_callback = NULL;
    (void)MIC_StartAutoSampling(&gMicAutoParam);
    //****************************************************************

    // Initialize internal variable
    {
        s32     i;

        for (i = 0; i < 256; i++)
        {
            gDrawData[i] = 0x80;
        }
    }

    // LCD display start
    GX_DispOn();
    GXS_DispOn();

    // debug string output
    OS_Printf("ARM9: MIC demo started.\n");
    OS_Printf("   up/down    -> change sampling span\n");
    OS_Printf("   left/right -> change mode\n");
    OS_Printf("   select     -> change loop setting\n");
    OS_Printf("\n");
    PrintfVariableData();

    // empty call for getting key input data (strategy for pressing A button in IPL)
    KeyRead(&gKey);


    //Display mode initially displays raw sampled value.
    drawMode = 0;
    blockCount = 4;
    start = OS_GetTick();

    // main loop
    while (TRUE)
    {
        // get key input data
        KeyRead(&gKey);

        // Change variable parameters
        {

            // Change display mode
            if ((gKey.trg | gKey.rep) & (PAD_KEY_LEFT | PAD_KEY_RIGHT))
            {
                //To change data display to bar-like object, initialize total data
                if (drawMode == 0)
                {
                    int     i;
                    for (i = 0; i < FFT_N * 2; i++)
                    {
                        totalData[i] = 0;
                    }
                }
                drawMode = !drawMode;
            }

            // Update block count
            if ((gKey.trg | gKey.rep) & PAD_KEY_UP)
            {
                blockCount += 1;
                if (blockCount >= 16)
                {
                    blockCount = 16;
                }
                OS_Printf("block = %d", blockCount);
            }
            if ((gKey.trg | gKey.rep) & PAD_KEY_DOWN)
            {
                blockCount -= 1;
                if (blockCount <= 2)
                {
                    blockCount = 2;
                }
                OS_Printf("block = %d", blockCount);
            }
            // Change loop availability when buffer is full
            if ((gKey.trg | gKey.rep) & (PAD_BUTTON_SELECT | PAD_BUTTON_START))
            {
                //****************************************************************
                (void)MIC_StopAutoSampling();
                gMicAutoParam.loop_enable = (gMicAutoParam.loop_enable + 1) % 2;
                (void)MIC_StartAutoSampling(&gMicAutoParam);
                //****************************************************************
                PrintfVariableData();
            }
        }

        // Draw Waveform
        SetDrawData(MIC_GetLastSamplingAddress());
        //Switch for each drawMode
        if (drawMode == 0)
        {
            Draw3D();
        }
        else
        {
            Draw3D2();
            end = OS_GetTick();
            if (OS_TicksToMilliSeconds(end - start) >= 100)
            {
                int     i;

                //Modify component data for display data use
                //i=0 data is DC component, so display is omitted
                for (i = 1; i < FFT_N; i += blockCount)
                {
                    u8      j;
                    fx64    tmpDrawData = 0;

                    //gDrawData[i-1] = (u8)(( (totalData[i*2]*totalData[i*2])+(totalData[i*2+1]*totalData[i*2+1]) >> 24) & 0x00ff);
                    //Display expands in vertical direction, so only up to 192 can be displayed
                    //gDrawData[i-1] = (u8)(( (totalData[i*2]*totalData[i*2])+(totalData[i*2+1]*totalData[i*2+1]) >> 24) & 0x00ff);

                    for (j = 0; j < blockCount; j++)
                    {
                        tmpDrawData +=
                            (totalData[(i + j) * 2] * totalData[(i + j) * 2]) +
                            (totalData[(i + j) * 2 + 1] * totalData[(i + j) * 2 + 1]);
                    }
                    tmpDrawData = tmpDrawData / blockCount;
                    gDrawData[i - 1] = (u8)((tmpDrawData >> 24) & 0x00bf);
                    for (j = 1; j < blockCount; j++)
                    {
                        gDrawData[i - 1 + j] = gDrawData[i - 1];
                    }
                }

                for (i = 0; i < FFT_N * 2; i++)
                {
                    totalData[i] = 0;
                }
                start = OS_GetTick();
            }
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
  Name:         Draw3D

  Description:  Displays waveforms in 3D

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void Draw3D(void)
{
    G3X_Reset();
    G3_Identity();
    G3_PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 0, 31, 0);

    {
        s32     i;

        //Modify component data for display data use
        //i=0 data is DC component, so display is omitted
        for (i = 1; i < FFT_N; i++)
        {
            //gDrawData[i-1] = (u8)(( (data[i*2]*data[i*2])+(data[i*2+1]*data[i*2+1]) >> 24) & 0x00ff);
            //Display expands in vertical direction, so only up to 192 can be displayed
            gDrawData[i - 1] =
                (u8)(((data[i * 2] * data[i * 2]) +
                      (data[i * 2 + 1] * data[i * 2 + 1]) >> 24) & 0x00bf);
        }

        //When drawing unsigned or drawing at the left end,
        //the x coordinate to draw the line is cast to u8 to forcibly move it?
        if ((gMicAutoParam.type == MIC_SAMPLING_TYPE_SIGNED_8BIT) ||
            (gMicAutoParam.type == MIC_SAMPLING_TYPE_SIGNED_12BIT) ||
            (gMicAutoParam.type == MIC_SAMPLING_TYPE_SIGNED_12BIT_FILTER_OFF))
        {
            //This is actually 255, but only up to 254 is displayed because display of DC component is omitted.
            for (i = 0; i < 254; i++)
            {
                DrawLine((s16)i, (s16)((s8)192 - (s8)gDrawData[i]),
                         (s16)(i + 1), (s16)((s8)192 - (s8)gDrawData[i + 1]));
            }
        }
        else
        {
            for (i = 0; i < 254; i++)
            {
                DrawLine((s16)i, (s16)(192 - gDrawData[i]), (s16)(i + 1),
                         (s16)(192 - gDrawData[i + 1]));
            }
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
    fx16    fsx = (fx16)(((sx - 128) * 0x1000) / 128);
    fx16    fsy = (fx16)(((96 - sy) * 0x1000) / 96);
    fx16    fex = (fx16)(((ex - 128) * 0x1000) / 128);
    fx16    fey = (fx16)(((96 - ey) * 0x1000) / 96);

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
  Name:         Draw3D2

  Description:  In 3D display, waveform is displayed in total amount

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void Draw3D2(void)
{
    G3X_Reset();
    G3_Identity();
    G3_PolygonAttr(GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 0, 31, 0);

    {
        s32     i;

        for (i = 0; i < 254; i += blockCount)
        {
            if (gDrawData[i] > gDrawData[i + blockCount])
            {
                DrawBar((s16)i, (s16)(192 - gDrawData[i]), (s16)(i + blockCount), (s16)192);
            }
            else
            {
                DrawBar((s16)i, (s16)192, (s16)(i + blockCount),
                        (s16)(192 - gDrawData[i + blockCount]));
            }
        }
    }

    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
}

/*---------------------------------------------------------------------------*
  Name:         DrawBar

  Description:  Draw a bar with square (rectangular) polygons

  Arguments:    sx  - Upper-left x coordinate of square to draw
                sy  - Upper-left y coordinate of square to draw
                sx  - Lower-right x coordinate of square to draw
                ey  - Lower-right y coordinate of square to draw

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DrawBar(s16 sx, s16 sy, s16 ex, s16 ey)
{
    fx16    fsx = (fx16)(((sx - 128) * 0x1000) / 128);
    fx16    fsy = (fx16)(((96 - sy) * 0x1000) / 96);
    fx16    fex = (fx16)(((ex - 128) * 0x1000) / 128);
    fx16    fey = (fx16)(((96 - ey) * 0x1000) / 96);

    G3_Begin(GX_BEGIN_QUADS);
    {
        G3_Color(GX_RGB(31, 31, 31));
        G3_Vtx(fsx, fsy, 0);
        G3_Color(GX_RGB(31, 31, 31));
        G3_Vtx(fsx, fey, 0);
        G3_Color(GX_RGB(31, 31, 31));
        G3_Vtx(fex, fey, 0);
        G3_Color(GX_RGB(31, 31, 31));
        G3_Vtx(fex, fsy, 0);
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
                          
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SetDrawData(void *address)
{
    s32     i;

    u16    *p;


    // If sampling has never been performed, do nothing and stop.
    // (Because it would delete the memory cache unrelated to microphone)
    if ((address < gMicData) || (address >= (gMicData + TEST_BUFFER_SIZE)))
    {
        return;
    }


    MATH_MakeFFTSinTable(sinTable, FFT_NSHIFT);
    MATH_MakeFFTSinTable(sinTable2, FFT_NSHIFT - 1);


    //This one is 12bit sampling, but the data uses 16bit,
    //therefore, there are 511 sets of 2 bytes, or 1,022.
    p = (u16 *)((u32)address - 1022);
    if ((u32)p < (u32)gMicData)
    {
        p = (u16 *)((u32)p + TEST_BUFFER_SIZE);
    }
    DC_InvalidateRange((void *)((u32)p & 0xffffffe0), 32);
    for (i = 0; i < FFT_N; i++)
    {
        orig[i * 2] = data[i * 2] = ((*p) << 6);
        orig[i * 2 + 1] = data[i * 2 + 1] = 0;
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

    MATH_FFT(data, FFT_NSHIFT, sinTable);

    //If the display is a bar, save the total of the data.
    if (drawMode == 1)
    {
        for (i = 0; i < FFT_N * 2; i++)
        {
            totalData[i] += data[i];
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

    OS_Printf(" sampling-span: %d , bit-range: ", gMicAutoParam.rate);
    switch (gMicAutoParam.type)
    {
    case MIC_SAMPLING_TYPE_8BIT:
        OS_Printf("8");
        break;
    case MIC_SAMPLING_TYPE_12BIT:
        OS_Printf("12");
        break;
    case MIC_SAMPLING_TYPE_SIGNED_8BIT:
        OS_Printf("signed 8");
        break;
    case MIC_SAMPLING_TYPE_SIGNED_12BIT:
        OS_Printf("signed 12");
        break;
    case MIC_SAMPLING_TYPE_12BIT_FILTER_OFF:
        OS_Printf("12(filter off)");
        break;
    case MIC_SAMPLING_TYPE_SIGNED_12BIT_FILTER_OFF:
        OS_Printf("signed 12(filter off)");
        break;
    }
    if (gMicAutoParam.loop_enable)
    {
        OS_Printf(" , loop: on\n");
    }
    else
    {
        OS_Printf(" , loop: off\n");
    }
}


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
