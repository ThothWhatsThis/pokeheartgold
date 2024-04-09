/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - demos - entropy-1
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.7  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.6  11/21/2005 10:54:09  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.5  08/04/2005 01:41:24  seiki_masashi
  Added OS_InitAlarm()

  Revision 1.4  07/29/2005 00:48:50  seiki_masashi
  avoided certain Japanese character at line ends

  Revision 1.3  07/28/2005 01:27:47  seiki_masashi
  Fixed up comments

  Revision 1.2  07/28/2005 01:20:14  seiki_masashi
  Changed to initialize random number seed using mic sampling result

  Revision 1.1  07/27/2005 07:43:35  seiki_masashi
  new additions

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include "data.h"
#include "wmscan.h"

#define WX 32
#define WY 24
#define SCREEN_DATA_SIZE WX*WY*2;

static u16 screenData_Main[WY][WX];
static u16 screenData_Sub[WY][WX];
static u8 randomSeed[20];
static u8 micData[1024] ATTRIBUTE_ALIGN(32);
static MICAutoParam micAutoParam;
static volatile BOOL bufferFullFlag;
static u8 wmBuffer[WM_SYSTEM_BUF_SIZE] ATTRIBUTE_ALIGN(32);


void    VBlankIntr(void);
void    ObjSet(int x, int y, int charNo, int paletteNo);
void    PrintBinaryNumber(int x, int y, u32 num, int paletteNo);
void    PrintString(int x, int y, char *str, int paletteNo);
void    ChurnRandomSeed(void);
u32     GetRandomNumber(void);
void    MicFullCallback(MICResult result, void *arg);

//================================================================================
/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void NitroMain()
{

    // initialization
    OS_Init();
    // Increase the randomness of the data returned by OS_GetLowEntropyData function by starting Tick
    OS_InitTick();
    OS_InitAlarm();

    // Touch panel initialization
    {
        TPCalibrateParam calibrate;

        TP_Init();

        // get CalibrateParameter from FlashMemory
        if (!TP_GetUserInfo(&calibrate))
        {
            OS_Panic("FATAL ERROR: can't read UserOwnerInfo\n");
        }

        TP_SetCalibrateParam(&calibrate);
    }

    // Graphics initialization
    GX_Init();

    GX_DispOff();
    GXS_DispOff();

    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
    (void)GX_DisableBankForLCDC();

    MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);
    MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);
    MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);
    MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);

    // Main 2D
    GX_SetBankForBG(GX_VRAM_BG_128_A);
    GX_SetBankForOBJ(GX_VRAM_OBJ_128_B);
    GX_SetBankForBGExtPltt(GX_VRAM_BGEXTPLTT_0123_E);
    GX_SetBankForOBJExtPltt(GX_VRAM_OBJEXTPLTT_8_F);
    // Sub 2D
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);
    GX_SetBankForSubBGExtPltt(GX_VRAM_SUB_BGEXTPLTT_32_H);
    GX_SetBankForSubOBJExtPltt(GX_VRAM_SUB_OBJEXTPLTT_16_I);

    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
    GX_SetVisiblePlane(GX_PLANEMASK_BG1);
    G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256,
                     GX_BG_COLORMODE_16,
                     GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01);
    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);

    GXS_SetGraphicsMode(GX_BGMODE_0);
    GXS_SetVisiblePlane(GX_PLANEMASK_BG1);
    G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256,
                      GX_BG_COLORMODE_16,
                      GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x0c000, GX_BG_EXTPLTT_01);
    GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);

    GX_LoadOBJ(sampleCharData, 0, sizeof(sampleCharData));
    GX_LoadOBJPltt(samplePlttData, 0, sizeof(samplePlttData));
    GXS_LoadOBJ(sampleCharData, 0, sizeof(sampleCharData));
    GXS_LoadOBJPltt(samplePlttData, 0, sizeof(samplePlttData));

    GX_LoadBG1Char(sampleCharData, 0, sizeof(sampleCharData));
    GX_LoadBGPltt(samplePlttData, 0, sizeof(samplePlttData));
    GXS_LoadBG1Char(sampleCharData, 0, sizeof(sampleCharData));
    GXS_LoadBGPltt(samplePlttData, 0, sizeof(samplePlttData));

    MI_CpuClearFast((void *)screenData_Main, sizeof(screenData_Main));
    MI_CpuClearFast((void *)screenData_Sub, sizeof(screenData_Sub));

    GX_DispOn();
    GXS_DispOn();

    // Enable V blank interrupts
    (void)OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();

    // Initialize mic and start auto-sampling
    {
        MICResult result;
        MIC_Init();
        (void)PM_SetAmp(PM_AMP_ON);

        micAutoParam.type = MIC_SAMPLING_TYPE_8BIT;
        micAutoParam.buffer = (void *)micData;
        micAutoParam.size = sizeof(micData);
        micAutoParam.rate = MIC_SAMPLING_RATE_8K;
        micAutoParam.loop_enable = TRUE;
        micAutoParam.full_callback = MicFullCallback;

        bufferFullFlag = FALSE;
        result = MIC_StartAutoSampling(&micAutoParam);
        if (result != MIC_RESULT_SUCCESS)
        {
            bufferFullFlag = TRUE;
            OS_Panic("MIC_StartAutoSampling failed. result = %d\n", result);
        }
    }

    // Initialize wireless communication
    // Because the OS_GetLowEntropyData function also returns data based on the reception strength history
    // this demo starts wireless communication and searches for a parent.
    if (!WS_Initialize(wmBuffer, 3))
    {
        OS_TPanic("WS_Initialize failed.\n");
    }
    WS_TurnOnPictoCatch();
    {
        const u8 mac[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

        // Wait for initialization to complete
        while (WS_GetSystemState() != WS_SYSSTATE_IDLE)
        {
        }
        // Start search for parent
        if (!WS_StartScan(NULL, mac, TRUE))
        {
            OS_TPanic("WS_StartScan failed.\n");
        }
    }

    // Initialize random number seed using mic sampling result
    while (bufferFullFlag == FALSE)
    {
        // Wait until mic sampling clears the buffer
        ;
    }
    MATH_CalcSHA1(randomSeed, micData, sizeof(micData));

    // Use the OS_GetLowEntropyData function to randomize the random number seed
    ChurnRandomSeed();

    // V blank occurrence setting
    (void)GX_VBlankIntr(TRUE);

    // Main loop
    while (1)
    {
        int     i;
        int     j;
        int     y;
        TPData  raw_point;
        u32     data[OS_LOW_ENTROPY_DATA_SIZE / sizeof(u32)];

        DC_FlushRange((void *)screenData_Main, sizeof(screenData_Main));
        DC_FlushRange((void *)screenData_Sub, sizeof(screenData_Sub));
        GX_LoadBG1Scr(screenData_Main, 0, sizeof(screenData_Main));
        GXS_LoadBG1Scr(screenData_Sub, 0, sizeof(screenData_Sub));

        // Wait for V blank interrupt
        OS_WaitVBlankIntr();

        (void)TP_RequestRawSampling(&raw_point);

        // Display result of OS_GetLowEntropyData on screen every 2ms
        y = 0;
        for (j = 0; j < 6; j++)
        {
            if (j != 0)
            {
                OS_Sleep(2);
            }
            OS_GetLowEntropyData(data);
            for (i = 0; i < OS_LOW_ENTROPY_DATA_SIZE / sizeof(u32); i++)
            {
                PrintBinaryNumber(0, y, data[i], j + 1);
                y += 8;
            }
        }

        if ((OS_GetVBlankCount() % 16) == 0)
        {
            // Use OS_GetLowEntropyData function every 16 frames
            // Mix the random number seed
            ChurnRandomSeed();
        }

        // Display the random number
        OS_TPrintf("%08x\n", GetRandomNumber());
    }
}

//--------------------------------------------------------------------------------
// Example of changing the random number seed based on the OS_GetLowEntropyData function
//
void ChurnRandomSeed(void)
{
    u32     data[OS_LOW_ENTROPY_DATA_SIZE / sizeof(u32)];
    MATHSHA1Context context;

    OS_GetLowEntropyData(data);
    MATH_SHA1Init(&context);
    MATH_SHA1Update(&context, randomSeed, sizeof(randomSeed));
    MATH_SHA1Update(&context, data, sizeof(data));
    MATH_SHA1GetHash(&context, randomSeed);
}

//--------------------------------------------------------------------------------
// Example of getting random number from random number seed (Not thread-safe)
//
u32 GetRandomNumber(void)
{
    static u32 count = 0;
    u32     randomData[20 / sizeof(u32)];
    MATHSHA1Context context;

    MATH_SHA1Init(&context);
    MATH_SHA1Update(&context, randomSeed, sizeof(randomSeed));
    MATH_SHA1Update(&context, &count, sizeof(count));
    MATH_SHA1GetHash(&context, randomData);
    count++;

    // Although you can use the entire randomData as a random number,
    // you can also remove any part of randomData and use it as a random number.
    return randomData[0];
}

//--------------------------------------------------------------------------------
//  set OBJ
//
void ObjSet(int x, int y, int charNo, int paletteNo)
{
    if (0 <= x && x < WX * 8 && 0 <= y && y < WY * 8 * 2)
    {
        if (y < WY * 8)
        {
            screenData_Main[y / 8][x / 8] = (u16)(paletteNo << 12 | charNo);
        }
        else
        {
            screenData_Sub[y / 8 - WY][x / 8] = (u16)(paletteNo << 12 | charNo);
        }
    }
}


//--------------------------------------------------------------------------------
//  Binary output
//
void PrintBinaryNumber(int x, int y, u32 num, int paletteNo)
{
    u32     n = 1U << 31;

    while (n > 0)
    {
        if (num >= n)
        {
            ObjSet(x, y, '1', paletteNo);
            num -= n;
        }
        else
        {
            ObjSet(x, y, '0', paletteNo);
        }
        x += 8;
        n >>= 1;
    }
}


//--------------------------------------------------------------------------------
//  String output
//
void PrintString(int x, int y, char *str, int paletteNo)
{
    while (*str != 0)
    {
        ObjSet(x, y, *str, paletteNo);
        x += 8;
        if (x >= 256)
        {
            y += 8;
            x = 0;
        }
    }
}

//--------------------------------------------------------------------------------
//    V-blank interrupt process
//
void VBlankIntr(void)
{
    //---- Interrupt check flag
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

//--------------------------------------------------------------------------------
//    Auto-sampling buffer callback
//
void MicFullCallback(MICResult result, void *arg)
{
#pragma unused(result)
#pragma unused(arg)
    bufferFullFlag = TRUE;
}

/*====== End of main.c ======*/
