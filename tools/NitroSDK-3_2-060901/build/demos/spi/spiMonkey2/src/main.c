/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SPI - demos
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.11  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.10  11/21/2005 11:04:41  kitase_hirotake
  Deleted OS_InitPrintServer, OS_PrintServer and PXI_Init
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.9  02/28/2005 05:26:23  yosizaki
  do-indent.

  Revision 1.8  12/03/2004 05:40:03  terui
  Complied with the added feature that explicitly indicates the data that was left out during the MIC auto sampling.

  Revision 1.7  11/04/2004 07:18:57  terui
  Small revisions in line with revising the specification of MIC_GetLastSamplingAddress.

  Revision 1.6  09/17/2004 09:36:03  terui
  Revised in connection with changes to MIC_StartAutoSampling(Async) parameters    

  Revision 1.5  2004/09/14 01:55:30  yasu
  Fixed backslashes at end of line.

  Revision 1.4  2004/09/10 14:13:34  yasu
  Changed the value of max sampling rate

  Revision 1.3  2004/08/23 07:27:27  yasu
  Workaround for pushing A Button in IPL

  Revision 1.2  08/20/2004 02:44:31  terui
  Revised so that PM_SetAmp is also done when TS_VERSION=0.

  Revision 1.1  08/10/2004 06:49:20  takano_makoto
  Initial Upload

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro.h>

#if     defined(SDK_TS) && ( SDK_TS_VERSION >= 100 )
#include    <nitro/spi/common/pm_common.h>
#include    <nitro/spi/ARM9/pm.h>
#endif

#include    "snd_data.h"


/*---------------------------------------------------------------------------*
    constant definitions
 *---------------------------------------------------------------------------*/
#define     KEY_REPEAT_START    25     // number of frames until key repeat starts
#define     KEY_REPEAT_SPAN     10     // number of key repeat interval frames
#define     TEST_BUFFER_SIZE    ( 1024 * 1024 ) // 1M

//
// Maximum sampling rate for the microphone
//  When SOUND or the Touch Screen is used at the same time as the microphone    
//  the maximum sampling rate is decreased, so we limit it here to 22K.    
//  If used for microphone sampling exclusively, can operate to LIMIT.    
//
#define     MY_SAMPLING_RATE_LIMIT  MIC_SAMPLING_RATE_22K


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
static void DrawLine(s16 sx, s16 sy, s16 ex, s16 ey, GXRgb color);
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
static TPData gTpData;
static u8 gDrawData[192];


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
    GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);

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
    gMicAutoParam.type = MIC_SAMPLING_TYPE_8BIT;
    gMicAutoParam.buffer = (void *)gMicData;
    gMicAutoParam.size = TEST_BUFFER_SIZE;
    gMicAutoParam.rate = MIC_SAMPLING_RATE_8K;
    gMicAutoParam.loop_enable = TRUE;
    gMicAutoParam.full_callback = NULL;
    (void)MIC_StartAutoSampling(&gMicAutoParam);
    //****************************************************************

    // Initialize the touch panel API.
    {
        TPCalibrateParam calibrate;

        TP_Init();
        if (TP_GetUserInfo(&calibrate))
        {
            TP_SetCalibrateParam(&calibrate);
        }
        else
        {
            OS_Panic("Can't find TP calibration data.");
        }
    }


    // Start thread for device sampling    
    {
        SND_Init();
        SND_AssignWaveArc((SNDBankData *)sound_bank_data, 0, (SNDWaveArc *)sound_wave_data);
        SND_StartSeq(0, sound_seq_data, 0, (SNDBankData *)sound_bank_data);
    }

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

    // debug string output
    OS_Printf("ARM9: MIC demo started.\n");
    OS_Printf("   up/down    -> change sampling span\n");
    OS_Printf("   left/right -> change bit range\n");
    OS_Printf("   select     -> change loop setting\n");
    OS_Printf("\n");
    PrintfVariableData();
    gTpData.touch = TP_TOUCH_OFF;

    // empty call for getting key input data (strategy for pressing A button in IPL)
    KeyRead(&gKey);

    // Main loop
    while (TRUE)
    {
        // get key input data
        KeyRead(&gKey);

        // TP sampling can be executed once during automatic MIC sampling.     
        // However, the TP sampling is executed after the next MIC sampling,
        // so there is a brief delay before the results of the request are returned.
        {
            TP_RequestSamplingAsync();
            // When <-MIC automatic sampling, it will take a bit longer for the TP results to return.

            if (TP_WaitCalibratedResult(&gTpData) != 0)
            {
                OS_Printf("TP_RequestSampling command fail\n");
            }
        }

        // Change variable parameters
        {
            // Change sampling type (bit width)
            if ((gKey.trg | gKey.rep) & (PAD_KEY_LEFT | PAD_KEY_RIGHT))
            {
                //****************************************************************
                (void)MIC_StopAutoSampling();
                gMicAutoParam.type =
                    (MICSamplingType)((gMicAutoParam.type + 1) % MIC_SAMPLING_TYPE_MAX);
                (void)MIC_StartAutoSampling(&gMicAutoParam);
                //****************************************************************
                PrintfVariableData();
            }
            // Change sampling rate
            if ((gKey.trg | gKey.rep) & PAD_KEY_UP)
            {
                //****************************************************************
                (void)MIC_StopAutoSampling();
                gMicAutoParam.rate = (u32)(gMicAutoParam.rate / 2);
                if (gMicAutoParam.rate < MY_SAMPLING_RATE_LIMIT)
                {
                    gMicAutoParam.rate = MY_SAMPLING_RATE_LIMIT;
                }

                (void)MIC_StartAutoSampling(&gMicAutoParam);
                //****************************************************************
                PrintfVariableData();
            }
            if ((gKey.trg | gKey.rep) & PAD_KEY_DOWN)
            {
                //****************************************************************
                (void)MIC_StopAutoSampling();
                gMicAutoParam.rate = (u32)(gMicAutoParam.rate * 2);
                if (gMicAutoParam.rate < MY_SAMPLING_RATE_LIMIT)
                {
                    gMicAutoParam.rate = MY_SAMPLING_RATE_LIMIT;
                }
                (void)MIC_StartAutoSampling(&gMicAutoParam);
                //****************************************************************
                PrintfVariableData();
            }
        }

        // Draw Waveform
        SetDrawData(MIC_GetLastSamplingAddress(), gMicAutoParam.type);

        // Render    
        Draw3D();

        // Sound
        while (SND_RecvCommandReply(SND_COMMAND_NOBLOCK) != NULL)
        {
        }
        (void)SND_FlushCommand(SND_COMMAND_NOBLOCK);

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

    {
        s32     i;

        if ((gMicAutoParam.type == MIC_SAMPLING_TYPE_SIGNED_8BIT) ||
            (gMicAutoParam.type == MIC_SAMPLING_TYPE_SIGNED_12BIT) ||
            (gMicAutoParam.type == MIC_SAMPLING_TYPE_SIGNED_12BIT_FILTER_OFF))
        {
            for (i = 0; i < 191; i++)
            {
                DrawLine((s16)((s8)gDrawData[i]),
                         (s16)i, (s16)((s8)gDrawData[i + 1]), (s16)(i + 1), GX_RGB(31, 31, 0));
            }
        }
        else
        {
            for (i = 0; i < 191; i++)
            {
                DrawLine((s16)(gDrawData[i]),
                         (s16)i, (s16)(gDrawData[i + 1]), (s16)(i + 1), GX_RGB(31, 31, 0));
            }
        }

        // Display touch panel contact in the shape of a plus    
        if (gTpData.touch == TP_TOUCH_ON)
        {
            if (!(gTpData.validity & TP_VALIDITY_INVALID_X))
            {
                // Display x coordinate by line    
                DrawLine((s16)(gTpData.x),
                         (s16)(0), (s16)(gTpData.x), (s16)(191), GX_RGB(31, 31, 31));
            }
            if (!(gTpData.validity & TP_VALIDITY_INVALID_Y))
            {
                // Display y coordinate by line    
                DrawLine((s16)(0),
                         (s16)(gTpData.y), (s16)(255), (s16)(gTpData.y), GX_RGB(31, 31, 31));
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
                color - Color of line drawn    

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DrawLine(s16 sx, s16 sy, s16 ex, s16 ey, GXRgb color)
{
    fx16    fsx;
    fx16    fsy;
    fx16    fex;
    fx16    fey;

    fsx = (fx16)(((sx - 128) * 0x1000) / 128);
    fsy = (fx16)(((96 - sy) * 0x1000) / 96);
    fex = (fx16)(((ex - 128) * 0x1000) / 128);
    fey = (fx16)(((96 - ey) * 0x1000) / 96);

    G3_Begin(GX_BEGIN_TRIANGLES);
    {
        G3_Color(color);
        G3_Vtx(fsx, fsy, 0);
        G3_Color(color);
        G3_Vtx(fex, fey, 0);
        G3_Color(color);
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

    // If sampling has never been performed, do nothing and stop.
    // (Because it would delete the memory cache unrelated to microphone)
    if ((address < gMicData) || (address >= (gMicData + TEST_BUFFER_SIZE)))
    {
        return;
    }

    // In the case of 8-bit sampling    
    switch (type)
    {
    case MIC_SAMPLING_TYPE_8BIT:
    case MIC_SAMPLING_TYPE_SIGNED_8BIT:
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
        break;
    case MIC_SAMPLING_TYPE_12BIT:
    case MIC_SAMPLING_TYPE_SIGNED_12BIT:
    case MIC_SAMPLING_TYPE_12BIT_FILTER_OFF:
    case MIC_SAMPLING_TYPE_SIGNED_12BIT_FILTER_OFF:
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
        break;
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
