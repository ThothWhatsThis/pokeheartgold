/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SND - demos - synth
  File:     main.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.3  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.2  11/21/2005 11:03:15  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.1  2005/04/13 03:29:29  ida
  Moved SND to NITRO-SDK

  Revision 1.4  04/13/2005 01:11:48  kyuma_koichi
  Added a message

  Revision 1.3  03/08/2005 08:32:59  kyuma_koichi
  Support for warning

  Revision 1.2  03/08/2005 07:45:24  kyuma_koichi
  Initial release.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// USAGE:
//  Touch panel: play
//---------------------------------------------------------------------------

#include <nitro.h>

#define CHANNEL_NUM 4
#define ALARM_NUM 0
#define STREAM_THREAD_PRIO 12
#define THREAD_STACK_SIZE 1024
#define STRM_BUF_PAGESIZE 64*32
#define STRM_BUF_SIZE STRM_BUF_PAGESIZE*2
#define STRM_SAMPLE_RATE 44100
#define OSC_MAX_VOLUME 18767

// Requests the cycle count from the key number.
#define GetFreq(pitch) (SND_TIMER_CLOCK / SND_CalcTimer((SND_TIMER_CLOCK / 440), (pitch - 69 * 64)))

// Stream object
typedef struct StreamInfo
{
    u32     bufPage;
}
StreamInfo;

// Touch panel
typedef struct TpData
{
    int     touch:1;
    int     trg:1;
    int     rls:1;
    u16     x;
    u16     y;
}
TpData;

// Oscillator
typedef struct Oscillator
{
    fx16    index;
    fx16    step;
    fx32    rate;                      // Output sampling rate
    u16     gain;
    u16     dummy;
}
Oscillator;


static void SoundAlarmHandler(void *arg);
static void StrmThread(void *arg);
static void Play(StreamInfo * strm);
static void Stop();
static void MakeStreamData(StreamInfo * strm);

static void VBlankIntr(void);
static void TpRead(void);

static u16 Cont;
static u16 Trg;
static TpData tp;
static u64 strmThreadStack[THREAD_STACK_SIZE / sizeof(u64)];
static OSThread strmThread;
static OSMessageQueue msgQ;
static OSMessage msgBuf[1];

static u8 strmBuf[STRM_BUF_SIZE] ATTRIBUTE_ALIGN(32);

static Oscillator osc;
static TPCalibrateParam calibrate;

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void NitroMain()
{
    StreamInfo strm;

    // initialization
    OS_Init();
    OS_InitThread();
    GX_Init();
    FX_Init();
    SND_Init();
    TP_Init();

    // Initializes touch panel read
    (void)TP_GetUserInfo(&calibrate);
    TP_SetCalibrateParam(&calibrate);
    tp.touch = 0;
    tp.trg = 0;
    tp.rls = 0;

    // Oscillator settings
    osc.rate = STRM_SAMPLE_RATE << FX32_SHIFT;

    // VBlank interrupt settings
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)GX_VBlankIntr(TRUE);

    // print usage
    OS_Printf("=================================\n");
    OS_Printf("USAGE:\n");
    OS_Printf(" Touch panel  : play\n");
    OS_Printf(" Release panel: stop\n");
    OS_Printf("=================================\n");

    // Lock the channel
    SND_LockChannel(1 << CHANNEL_NUM, 0);

    /* Startup stream thread*/
    OS_CreateThread(&strmThread,
                    StrmThread,
                    NULL,
                    strmThreadStack + THREAD_STACK_SIZE / sizeof(u64),
                    THREAD_STACK_SIZE, STREAM_THREAD_PRIO);
    OS_WakeupThreadDirect(&strmThread);

    //================ Main loop
    while (1)
    {
        u16     ReadData;

        OS_WaitVBlankIntr();

        // Receive ARM7 command reply
        while (SND_RecvCommandReply(SND_COMMAND_NOBLOCK) != NULL)
        {
        }

        ReadData = PAD_Read();
        TpRead();
        Trg = (u16)(ReadData & (ReadData ^ Cont));
        Cont = ReadData;

        if (tp.touch)
        {
            osc.step = (fx16)FX_Div(GetFreq(tp.x * 4 + 60 * 64) << FX32_SHIFT, osc.rate);
            osc.gain = (u16)(tp.y * OSC_MAX_VOLUME / 192);
        }

        // Plays the PSG rectangular wave
        if (tp.trg)
        {
            Play(&strm);
        }

        if (tp.rls)
        {
            Stop();
        }

        // Command flush
        (void)SND_FlushCommand(SND_COMMAND_NOBLOCK);
    }
}

/*---------------------------------------------------------------------------*
  Name:         Play

  Description:  Plays streaming playback

  Arguments:    strm - Stream object
                filename - Name of the streaming playback file

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void Play(StreamInfo * strm)
{
    s32     timerValue;
    u32     alarmPeriod;

    osc.index = 0;

    /* Set parameters*/
    timerValue = SND_TIMER_CLOCK / STRM_SAMPLE_RATE;
    alarmPeriod = timerValue * (STRM_BUF_PAGESIZE / sizeof(s16)) / 32;

    // Reads initial stream data
    strm->bufPage = 0;
    MakeStreamData(strm);
    MakeStreamData(strm);

    // Sets up the channel and alarm
    SND_SetupChannelPcm(CHANNEL_NUM,
                        SND_WAVE_FORMAT_PCM16,
                        strmBuf,
                        SND_CHANNEL_LOOP_REPEAT,
                        0,
                        STRM_BUF_SIZE / sizeof(u32),
                        127, SND_CHANNEL_DATASHIFT_NONE, timerValue, 0);
    SND_SetupAlarm(ALARM_NUM, alarmPeriod, alarmPeriod, SoundAlarmHandler, strm);
    SND_StartTimer(1 << CHANNEL_NUM, 0, 1 << ALARM_NUM, 0);
}

/*---------------------------------------------------------------------------*
  Name:         StopStream

  Description:  Stops streaming playback

  Arguments:    strm - Stream object

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void Stop()
{
    SND_StopTimer(1 << CHANNEL_NUM, 0, 1 << ALARM_NUM, 0);
}

/*---------------------------------------------------------------------------*
  Name:         StrmThread

  Description:  Stream thread

  Arguments:    arg - User data (unused)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void StrmThread(void * /*arg*/ )
{
    OSMessage message;

    OS_InitMessageQueue(&msgQ, msgBuf, 1);

    while (1)
    {
        (void)OS_ReceiveMessage(&msgQ, &message, OS_MESSAGE_BLOCK);
        (void)MakeStreamData((StreamInfo *) message);
    }
}

/*---------------------------------------------------------------------------*
  Name:         SoundAlarmHandler

  Description:  Alarm callback function

  Arguments:    arg - Stream object

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SoundAlarmHandler(void *arg)
{
    (void)OS_SendMessage(&msgQ, (OSMessage)arg, OS_MESSAGE_NOBLOCK);
}

/*---------------------------------------------------------------------------*
  Name:         MakeStreamData

  Description:  Create stream data

  Arguments:    strm - Stream object

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MakeStreamData(StreamInfo * strm)
{
    u8     *buf;
    int     i;

    // Buffer page settings
    if (strm->bufPage == 0)
    {
        buf = strmBuf;
        strm->bufPage = 1;
    }
    else
    {
        buf = strmBuf + STRM_BUF_PAGESIZE;
        strm->bufPage = 0;
    }

    // Generates data
    for (i = 0; i < STRM_BUF_PAGESIZE / sizeof(s16); i++)
    {
        ((s16 *)buf)[i] = (s16)FX_Whole(FX_Mul32x64c(osc.gain << FX32_SHIFT,
                                                     FX_SinFx64c(FX_Mul32x64c(osc.index,
                                                                              FX64C_TWOPI))));
        osc.index += osc.step;
        osc.index &= FX32_DEC_MASK;
    }
}

//--------------------------------------------------------------------------------
//    V-blank interrupt process
//
static void VBlankIntr(void)
{
    // Interrupt check flag
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         TpRead

  Description:  Reads the touch panel

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void TpRead(void)
{
    TPData  tp_data;
    TPData  tp_raw;
    int     old;

    old = tp.touch;
    while (TP_RequestRawSampling(&tp_raw) != 0)
    {
    };
    TP_GetCalibratedPoint(&tp_data, &tp_raw);

    tp.touch = tp_data.touch;
    tp.trg = tp.touch & (tp.touch ^ old);
    tp.rls = old & (old ^ tp.touch);

    switch (tp_data.validity)
    {
    case TP_VALIDITY_VALID:
        tp.x = tp_data.x;
        tp.y = tp_data.y;
        break;
    case TP_VALIDITY_INVALID_X:
        tp.y = tp_data.y;
        break;
    case TP_VALIDITY_INVALID_Y:
        tp.x = tp_data.x;
        break;
    case TP_VALIDITY_INVALID_XY:
        break;
    default:
        break;
    }
}
