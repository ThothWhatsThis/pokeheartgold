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
  Revision 1.10  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.9  11/21/2005 11:04:30  kitase_hirotake
  Deleted OS_InitPrintServer, OS_PrintServer and PXI_Init
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.8  02/28/2005 05:26:23  yosizaki
  do-indent.

  Revision 1.7  09/06/2004 13:02:48  terui
  Only fixed indent.

  Revision 1.6  2004/08/23 07:27:27  yasu
  Workaround for pushing A Button in IPL

  Revision 1.5  08/02/2004 08:23:35  ida
  update NitroSystem modules

  Revision 1.4  2004/07/28 10:38:29  yasu
  Switch playing screen to the below.

  Revision 1.3  06/30/2004 06:55:48  ida
  update NitroSystem modules

  Revision 1.2  06/10/2004 09:43:53  terui
  Change debug print string.

  Revision 1.1  06/02/2004 09:53:25  terui
  Initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro.h>
#include    "monkey.h"
#include    "snd_data.h"


/*---------------------------------------------------------------------------*
    constant definitions
 *---------------------------------------------------------------------------*/
#define     KEY_REPEAT_START    25     // number of frames until key repeat starts
#define     KEY_REPEAT_SPAN     10     // number of key repeat interval frames


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
static void Init3D(void);
static void Draw3D(void);
static void DrawLine(s16 sx, s16 sy, s16 ex, s16 ey, GXRgb color);
static void VBlankIntr(void);
static void KeyRead(KeyInformation * pKey);


/*---------------------------------------------------------------------------*
    internal variable definitions
 *---------------------------------------------------------------------------*/
static KeyInformation gKey;
static MICAutoParam gMicAutoParam;
static u16 gMicData[192];
static TPData gTpData[4];


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
    OS_InitThread();
    OS_InitTick();
    OS_InitAlarm();
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

    // Start thread for device sampling
    MonkeyInit();

    // Start thread for device sampling    
    {
        SND_Init();
        SND_AssignWaveArc((SNDBankData *)sound_bank_data, 0, (SNDWaveArc *)sound_wave_data);
        SND_StartSeq(0, sound_seq_data, 0, (SNDBankData *)sound_bank_data);
    }

    // LCD display start
    GX_DispOn();
    GXS_DispOn();

    // debug string output
    OS_Printf("ARM9: SPI parallel sampling demo \"spiMonkey\" started.\n");

    // empty call for getting key input data (strategy for pressing A button in IPL)
    KeyRead(&gKey);

    // Main loop
    while (TRUE)
    {
        // get key input data
        KeyRead(&gKey);

        // Get mike and touch panel input information    
        MonkeyGetNewMicData(192, gMicData);
        MonkeyGetNewTpData(4, gTpData);

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

        // Display mike data in waveform    
        for (i = 0; i < 191; i++)
        {
            DrawLine((s16)(gMicData[i] >> 8),
                     (s16)(i), (s16)(gMicData[i + 1] >> 8), (s16)(i + 1), GX_RGB(31, 31, 0));
        }

        // Display touch panel contact in the shape of a plus    
        for (i = 0; i < 4; i++)
        {
            if (gTpData[i].touch == TP_TOUCH_ON)
            {
                if (!(gTpData[i].validity & TP_VALIDITY_INVALID_X))
                {
                    // Display x coordinate by line    
                    DrawLine((s16)(gTpData[i].x),
                             (s16)(0),
                             (s16)(gTpData[i].x),
                             (s16)(191), GX_RGB(31 - (i * 6), 31 - (i * 6), 31 - (i * 6)));
                }
                if (!(gTpData[i].validity & TP_VALIDITY_INVALID_Y))
                {
                    // Display y coordinate by line    
                    DrawLine((s16)(0),
                             (s16)(gTpData[i].y),
                             (s16)(255),
                             (s16)(gTpData[i].y), GX_RGB(31 - (i * 6), 31 - (i * 6), 31 - (i * 6)));
                }
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
  End of file
 *---------------------------------------------------------------------------*/
