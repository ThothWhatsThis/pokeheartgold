/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SND - demos - seq
  File:     channel.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: channel.c,v $
  Revision 1.2  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.1  2005/04/13 03:29:29  ida
  Moved SND to NITRO-SDK

  Revision 1.4  04/13/2005 01:15:26  kyuma_koichi
  Changed waveform file format

  Revision 1.3  03/08/2005 08:32:59  kyuma_koichi
  Support for warning

  Revision 1.2  03/08/2005 07:45:24  kyuma_koichi
  Initial release.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include "smfdefine.h"
#include "channel.h"
#include "piano.g5.pcm16.h"

#define MAX_CHANNEL 16
#define MAX_MIDI_CHANNEL 16

/* Hard channel structure in DS*/
typedef struct ChannelEx
{
    u8      playing;                   /* Playback flag*/
    u8      midi_ch;                   /* MIDI channel for sound being played back.*/
    u8      key;                       /* playback key*/
    u8      velocity;                  /* playback velocity*/
}
ChannelEx;

/* MIDI channel structure*/
typedef struct MidiChannel
{
    u8      pan;                       /* Pan*/
    u8      volume;                    /* Volume*/
    u8      expression;                /* Expression*/
}
MidiChannel;

ChannelEx channel[MAX_CHANNEL];
MidiChannel midi_ch[MAX_MIDI_CHANNEL];

static void NoteOn(const u8 *midi_data);
static void NoteOff(const u8 *midi_data);


/*---------------------------------------------------------------------------*
  Name:         ChInit

  Description:  Channel initialization

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void ChInit(void)
{
    int     ch_num;

    /* Locks all channels*/
    SND_LockChannel(0xffff, 0);

    for (ch_num = 0; ch_num < MAX_CHANNEL; ch_num++)
    {
        channel[ch_num].playing = FALSE;
    }

    for (ch_num = 0; ch_num < MAX_MIDI_CHANNEL; ch_num++)
    {
        midi_ch[ch_num].volume = 127;
        midi_ch[ch_num].pan = 64;
        midi_ch[ch_num].expression = 127;
    }
}

/*---------------------------------------------------------------------------*
  Name:         ChSetEvent

  Description:  Interprets and executes a MIDI event.

  Arguments:    midi_data - MIDI event data line

  Returns:      None
 *---------------------------------------------------------------------------*/
void ChSetEvent(const u8 *midi_data)
{
    switch (midi_data[0] & 0xf0)
    {
    case MIDI_NOTEOFF:                /* Note off*/
        NoteOff(midi_data);
        break;
    case MIDI_NOTEON:                 /* Note on*/
        NoteOn(midi_data);
        break;
    case MIDI_CONTROLCHANGE:          /* Control change*/
        switch (midi_data[1])
        {
        case MIDI_CC_VOLUME:          /* Channel volume*/
            midi_ch[midi_data[0] & 0x0f].volume = midi_data[2];
            break;
        case MIDI_CC_PAN:             /* Channel pan*/
            midi_ch[midi_data[0] & 0x0f].pan = midi_data[2];
            break;
        case MIDI_CC_EXPRESSION:      /* Expression*/
            midi_ch[midi_data[0] & 0x0f].expression = midi_data[2];
            break;
        default:
            break;
        }
    default:
        /* Other messages are not supported*/
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         ChAllNoteOff

  Description:  Stops all channels

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void ChAllNoteOff(void)
{
    int     ch_num;

    SND_StopTimer(0xffff, 0, 0, 0);

    for (ch_num = 0; ch_num < MAX_CHANNEL; ch_num++)
    {
        channel[ch_num].playing = FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NoteOn

  Description:  Executes the note-on operation based on the MIDI event.

  Arguments:    midi_data - MIDI event data line

  Returns:      None
 *---------------------------------------------------------------------------*/
static void NoteOn(const u8 *midi_data)
{
    int     ch_num;
    ChannelEx *ch;
    s16     db;
    u16     vol;

    for (ch_num = 0; ch_num < MAX_CHANNEL; ch_num++)
    {
        if (!channel[ch_num].playing)
            break;
    }
    if (ch_num == MAX_CHANNEL)
        return;                        /* An open channel was not found.*/

    ch = &channel[ch_num];
    ch->midi_ch = (u8)(midi_data[0] & 0x0f);
    ch->key = midi_data[1];
    ch->velocity = midi_data[2];

    /* Calculates volume.*/
    db = SND_CalcDecibel(ch->velocity);
    db += SND_CalcDecibel(midi_ch[ch->midi_ch].volume);
    db += SND_CalcDecibel(midi_ch[ch->midi_ch].expression);
    vol = SND_CalcChannelVolume(db);

    /* Plays sounds*/
    SND_SetupChannelPcm(ch_num,
                        PIANO_G5_PCM16_FORMAT,
                        piano_g5_pcm16,
                        PIANO_G5_PCM16_LOOPFLAG ? SND_CHANNEL_LOOP_REPEAT : SND_CHANNEL_LOOP_1SHOT,
                        PIANO_G5_PCM16_LOOPSTART,
                        PIANO_G5_PCM16_LOOPLEN,
                        vol & 0xff,
                        (SNDChannelDataShift)(vol >> 8),
                        SND_CalcTimer(PIANO_G5_PCM16_TIMER, (int)(ch->key - 67) * 64),
                        midi_ch[ch->midi_ch].pan);
    SND_StartTimer(1U << ch_num, 0, 0, 0);

    ch->playing = TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NoteOff

  Description:  Executes the note-off operation based on the MIDI event.

  Arguments:    midi_data - MIDI event data line

  Returns:      None
 *---------------------------------------------------------------------------*/
static void NoteOff(const u8 *midi_data)
{
    int     ch_num;
    ChannelEx *ch;

    for (ch_num = 0; ch_num < MAX_CHANNEL; ch_num++)
    {
        ch = &channel[ch_num];

        if (!ch->playing)
            continue;
        if ((ch->midi_ch == (u8)(midi_data[0] & 0x0f)) && (ch->key == midi_data[1]))
        {
            /* The MIDI channel and key number match and are considered to be the same sound.*/
            SND_StopTimer(1U << ch_num, 0, 0, 0);
            ch->playing = FALSE;
        }
    }

}
