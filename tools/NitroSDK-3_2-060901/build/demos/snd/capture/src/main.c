/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SND - demos - capture
  File:     main.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.4  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.3  11/21/2005 11:02:29  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.2  06/27/2005 11:10:12  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.1  2005/04/13 03:29:28  ida
  Moved SND to NITRO-SDK

  Revision 1.4  04/13/2005 01:15:26  kyuma_koichi
  Changed waveform file format

  Revision 1.3  03/08/2005 08:32:59  kyuma_koichi
  Support for warning

  Revision 1.2  03/08/2005 07:45:24  kyuma_koichi
  Initial release.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// USAGE:
//  A: Start Bgm
//  B: Stop Bgm
//  x : Start Capture
//  y : Stop Capture
//---------------------------------------------------------------------------

#include <nitro.h>
#include "fanfare.32.h"

#define CAPTURE_BUF_PAGESIZE 64*32
#define CAPTURE_BUF_SIZE CAPTURE_BUF_PAGESIZE*2
#define CAPTURE_SAMPLE_RATE 44100
#define CAPTURE_FIFO_SIZE 32
#define THREAD_STACK_SIZE 1024

#define CHANNEL_NUM 4
#define ALARM_NUM 0
#define CAPTURE_THREAD_PRIO 12
#define MAX_VOLUME 127
#define CENTER_PAN 64
#define FILTER_SIZE 12

typedef s16 smp_t;

typedef struct EffectCallbackInfo
{
    smp_t   sample[FILTER_SIZE - 1][2];
}
EffectCallbackInfo;

typedef struct CaptureInfo
{
    u32     bufPage;
    EffectCallbackInfo callbackInfo;
}
CaptureInfo;

static void SoundAlarmHandler(void *arg);
static void CaptureThread(void *arg);
static void CaptureFunc(CaptureInfo * cap);
static void VBlankIntr(void);
void    EffectCallback(void *bufferL_p, void *bufferR_p, u32 len, SNDCaptureFormat format,
                       void *arg);

u16     Cont;
u16     Trg;

static u64 captureThreadStack[THREAD_STACK_SIZE / sizeof(u64)];
static OSThread captureThread;
static OSMessageQueue msgQ;
static OSMessage msgBuf[1];

static u8 captureBufL[CAPTURE_BUF_SIZE] ATTRIBUTE_ALIGN(32);
static u8 captureBufR[CAPTURE_BUF_SIZE] ATTRIBUTE_ALIGN(32);

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void NitroMain()
{
    CaptureInfo cap;

    // initialization
    OS_Init();
    GX_Init();
    SND_Init();

    // VBlank interrupt settings
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)GX_VBlankIntr(TRUE);

    // print usage
    OS_Printf("=================================\n");
    OS_Printf("USAGE:\n");
    OS_Printf(" A : start sound\n");
    OS_Printf(" B : stop sound\n");
    OS_Printf(" X : start capture(effect)\n");
    OS_Printf(" Y : stop capture(effect)\n");
    OS_Printf("=================================\n");

    // Lock the channel
    SND_LockChannel((1 << CHANNEL_NUM) | (1 << 1) | (1 << 3), 0);

    /* Startup stream thread*/
    OS_CreateThread(&captureThread,
                    CaptureThread,
                    NULL,
                    captureThreadStack + THREAD_STACK_SIZE / sizeof(u64),
                    THREAD_STACK_SIZE, CAPTURE_THREAD_PRIO);
    OS_WakeupThreadDirect(&captureThread);

    while (1)
    {
        u16     ReadData;

        OS_WaitVBlankIntr();

        // Receive ARM7 command reply
        while (SND_RecvCommandReply(SND_COMMAND_NOBLOCK) != NULL)
        {
        }

        ReadData = PAD_Read();
        Trg = (u16)(ReadData & (ReadData ^ Cont));
        Cont = ReadData;

        // Play PCM
        if (Trg & PAD_BUTTON_A)
        {
            SND_SetupChannelPcm(CHANNEL_NUM,
                                FANFARE_32_FORMAT,
                                fanfare_32,
                                FANFARE_32_LOOPFLAG ? SND_CHANNEL_LOOP_REPEAT :
                                SND_CHANNEL_LOOP_1SHOT, FANFARE_32_LOOPSTART, FANFARE_32_LOOPLEN,
                                MAX_VOLUME, SND_CHANNEL_DATASHIFT_NONE, FANFARE_32_TIMER,
                                CENTER_PAN);
            SND_StartTimer(1 << CHANNEL_NUM, 0, 0, 0);
        }

        // Stop PCM
        if (Trg & PAD_BUTTON_B)
        {
            SND_StopTimer(1 << CHANNEL_NUM, 0, 0, 0);
        }

        // Start capture
        if (Trg & PAD_BUTTON_X)
        {
            int     timerValue;
            u32     alarmPeriod;
            u32     alarmFirst;

            timerValue = SND_TIMER_CLOCK / CAPTURE_SAMPLE_RATE;
            alarmPeriod = timerValue * (CAPTURE_BUF_PAGESIZE / sizeof(s16)) / 32;
            alarmFirst =
                timerValue * ((CAPTURE_BUF_PAGESIZE + CAPTURE_FIFO_SIZE) / sizeof(s16)) / 32;
            cap.bufPage = 0;

            // Selector change: Output only channels 1 and 3
            SND_SetOutputSelector(SND_OUTPUT_CHANNEL1,
                                  SND_OUTPUT_CHANNEL3,
                                  SND_CHANNEL_OUT_BYPASS, SND_CHANNEL_OUT_BYPASS);
            // Play sound for capture 0 buffer
            SND_SetupChannelPcm(1,
                                SND_WAVE_FORMAT_PCM16,
                                captureBufL,
                                SND_CHANNEL_LOOP_REPEAT,
                                0,
                                CAPTURE_BUF_SIZE / sizeof(u32),
                                MAX_VOLUME, SND_CHANNEL_DATASHIFT_NONE, timerValue, 0);
            // Play sound for capture 1 buffer
            SND_SetupChannelPcm(3,
                                SND_WAVE_FORMAT_PCM16,
                                captureBufR,
                                SND_CHANNEL_LOOP_REPEAT,
                                0,
                                CAPTURE_BUF_SIZE / sizeof(u32),
                                MAX_VOLUME, SND_CHANNEL_DATASHIFT_NONE, timerValue, 127);
            // Capture 0 settings
            SND_SetupCapture(SND_CAPTURE_0,
                             SND_CAPTURE_FORMAT_PCM16,
                             captureBufL,
                             CAPTURE_BUF_SIZE / sizeof(u32),
                             TRUE, SND_CAPTURE_IN_MIXER, SND_CAPTURE_OUT_NORMAL);
            // Capture 1 settings
            SND_SetupCapture(SND_CAPTURE_1,
                             SND_CAPTURE_FORMAT_PCM16,
                             captureBufR,
                             CAPTURE_BUF_SIZE / sizeof(u32),
                             TRUE, SND_CAPTURE_IN_MIXER, SND_CAPTURE_OUT_NORMAL);
            // Alarm settings
            SND_SetupAlarm(ALARM_NUM, alarmFirst,       // Consider size of FIFO buffer the first time
                           alarmPeriod, SoundAlarmHandler, &cap);
            SND_StartTimer((1 << 1) | (1 << 3),
                           (1 << SND_CAPTURE_0) | (1 << SND_CAPTURE_1), 1 << ALARM_NUM, 0);
            OS_Printf("capture start\n");
        }

        // Stop capture
        if (Trg & PAD_BUTTON_Y)
        {
            SND_StopTimer((1 << 1) | (1 << 3),
                          (1 << SND_CAPTURE_0) | (1 << SND_CAPTURE_1), 1 << ALARM_NUM, 0);
            // Selector change: Output from mixer
            SND_SetOutputSelector(SND_OUTPUT_MIXER,
                                  SND_OUTPUT_MIXER, SND_CHANNEL_OUT_MIXER, SND_CHANNEL_OUT_MIXER);

            OS_Printf("capture stop\n");
        }

        // Command flush
        (void)SND_FlushCommand(SND_COMMAND_NOBLOCK);
    }
}

//--------------------------------------------------------------------------------
//    V-blank interrupt process
//
void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK); // checking V-Blank interrupt
}


/*---------------------------------------------------------------------------*
  Name:         CaptureThread

  Description:  Stream thread

  Arguments:    arg - User data (unused)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CaptureThread(void * /*arg*/ )
{
    OSMessage message;

    OS_InitMessageQueue(&msgQ, msgBuf, 1);

    while (1)
    {
        (void)OS_ReceiveMessage(&msgQ, &message, OS_MESSAGE_BLOCK);
        (void)CaptureFunc((CaptureInfo *) message);
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
    if (!OS_SendMessage(&msgQ, (OSMessage)arg, OS_MESSAGE_NOBLOCK))
        OS_Printf("faild to send message\n");
}

/*---------------------------------------------------------------------------*
  Name:         CaptureFunc

  Description:  Create stream data

  Arguments:    strm - Stream object

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CaptureFunc(CaptureInfo * cap)
{
    u8     *bufL, *bufR;

    // Buffer page settings
    if (cap->bufPage == 0)
    {
        bufL = captureBufL;
        bufR = captureBufR;
        cap->bufPage = 1;
    }
    else
    {
        bufL = captureBufL + CAPTURE_BUF_PAGESIZE;
        bufR = captureBufR + CAPTURE_BUF_PAGESIZE;
        cap->bufPage = 0;
    }

    EffectCallback(bufL, bufR, CAPTURE_BUF_PAGESIZE, SND_CAPTURE_FORMAT_PCM16, &cap->callbackInfo);
}


static inline smp_t GetSample(smp_t * p, int x, int n, const EffectCallbackInfo * info)
{
    if (x >= 0)
        return p[x];
    x += FILTER_SIZE - 1;
    return info->sample[x][n];
}

void EffectCallback(void *bufferL_p, void *bufferR_p, u32 len, SNDCaptureFormat format, void *arg)
{
    smp_t  *lp = (smp_t *) bufferL_p;
    smp_t  *rp = (smp_t *) bufferR_p;
    EffectCallbackInfo *info = (EffectCallbackInfo *) arg;
    smp_t   org[FILTER_SIZE - 1][2];
    u32     samples;
    int     x;
    long    i, j;

    samples = (format == SND_CAPTURE_FORMAT_PCM8) ? len : (len >> 1);

    // store original sample data
    for (i = 0; i < FILTER_SIZE - 1; i++)
    {
        org[i][0] = lp[i + samples - FILTER_SIZE + 1];
        org[i][1] = rp[i + samples - FILTER_SIZE + 1];
    }

    // filtering
    for (i = (long)(samples - 1); i >= FILTER_SIZE - 1; i--)
    {
        x = 0;
        for (j = 0; j < FILTER_SIZE; j++)
        {
            x += lp[i - j];
        }
        x /= FILTER_SIZE;
        lp[i] = (smp_t) x;

        x = 0;
        for (j = 0; j < FILTER_SIZE; j++)
        {
            x += rp[i - j];
        }
        x /= FILTER_SIZE;
        rp[i] = (smp_t) x;
    }

#if FILTER_SIZE >= 2
    for (i = FILTER_SIZE - 2; i >= 0; i--)
    {
        x = lp[i];
        for (j = 1; j < FILTER_SIZE; j++)
        {
            x += GetSample(lp, i - j, 0, info);
        }
        x /= FILTER_SIZE;
        lp[i] = (smp_t) x;

        x = rp[i];
        for (j = 1; j < FILTER_SIZE; j++)
        {
            x += GetSample(rp, i - j, 1, info);
        }
        x /= FILTER_SIZE;
        rp[i] = (smp_t) x;
    }
#endif

    // store last samples for next callback
    for (i = 0; i < FILTER_SIZE - 1; i++)
    {
        info->sample[i][0] = org[i][0];
        info->sample[i][1] = org[i][1];
    }

    DC_FlushRange(bufferL_p, len);
    DC_FlushRange(bufferR_p, len);
    DC_WaitWriteBufferEmpty();

}
