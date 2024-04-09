/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SND - demos - seq
  File:     main.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.3  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.2  11/21/2005 11:02:56  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.1  2005/04/13 03:29:29  ida
  Moved SND to NITRO-SDK

  Revision 1.3  03/08/2005 08:32:59  kyuma_koichi
  Support for warning

  Revision 1.2  03/08/2005 07:45:24  kyuma_koichi
  Initial release.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// USAGE:
//  A: start sequence
//  B: stop sequence
//---------------------------------------------------------------------------

#include <nitro.h>
#include "seq.h"
#include "channel.h"

#define MIDI_BUF_SIZE 8*1024
#define CHANNEL_NUM 4
#define ORIGINAL_KEY 67

u16     Cont;
u16     Trg;

u8      midifile_buf[MIDI_BUF_SIZE];

static SeqHandle handle;

void    VBlankIntr(void);
void    FileLoad(void *buf, const char *filename);
void    MidiCallback(const u8 *midi_event);


/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  Main function

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void NitroMain()
{
    // initialization
    OS_Init();
    GX_Init();
    FS_Init(MI_DMA_MAX_NUM);
    SND_Init();

    SeqInit(SEQ_CLOCK_INTERVAL_NITRO_VBLANK, MidiCallback);
    ChInit();

    // VBlank interrupt settings
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)GX_VBlankIntr(TRUE);

    // print usage
    OS_Printf("=================================\n");
    OS_Printf("USAGE:\n");
    OS_Printf(" A : start sequence\n");
    OS_Printf(" B : stop sequence\n");
    OS_Printf("=================================\n");

    // Loads a MIDI file
    FileLoad(midifile_buf, "/elise.mid");

    while (1)
    {
        u16     ReadData;

        OS_WaitVBlankIntr();

        // Receive ARM7 command reply
        while (SND_RecvCommandReply(SND_COMMAND_NOBLOCK) != NULL)
        {
        }

        // Read key input
        ReadData = PAD_Read();
        Trg = (u16)(ReadData & (ReadData ^ Cont));
        Cont = ReadData;

        if (Trg & PAD_BUTTON_A)
        {
            ChAllNoteOff();
            (void)SeqPlay(&handle, midifile_buf);
        }

        if (Trg & PAD_BUTTON_B)
        {
            (void)SeqStop(&handle);
            ChAllNoteOff();
        }

        (void)SeqMain(&handle);

        // Command Flash (flashes and requests immediate execution)
        (void)SND_FlushCommand(SND_COMMAND_NOBLOCK | SND_COMMAND_IMMEDIATE);
    }
}

void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK); // checking V-Blank interrupt
}

void FileLoad(void *buf, const char *filename)
{
    FSFile  file;
    u32     file_size;

    FS_InitFile(&file);
    (void)FS_OpenFile(&file, filename);
    file_size = FS_GetLength(&file);
    (void)FS_ReadFile(&file, buf, (s32)file_size);
    (void)FS_CloseFile(&file);
}

/*---------------------------------------------------------------------------*
  Name:         MidiCallback

  Description:  Function called when a MIDI event occurs
                during the SeqMain() sequence

  Arguments:    midi_event - generated MIDI event line

  Returns:      None
 *---------------------------------------------------------------------------*/
void MidiCallback(const u8 *midi_event)
{
    static u8 running_status;
    u8      midi_buf[4];
    int     i;

    // Byte Size by MIDI event
    const u8 midi_byte_size[8] = {
        /*  8x,9x,Ax,Bx,Cx,Dx,Ex,Fx*/
        2, 2, 2, 2, 1, 1, 2, 0
    };

    // Check the running status
    if (midi_event[0] < 0x80)
    {
        midi_buf[0] = running_status;
        for (i = 0; i < midi_byte_size[(midi_buf[0] >> 4) - 8]; i++)
        {
            midi_buf[i + 1] = midi_event[i];
        }
    }
    else
    {
        running_status = midi_event[0];
        midi_buf[0] = midi_event[0];
        for (i = 0; i < midi_byte_size[(midi_buf[0] >> 4) - 8]; i++)
        {
            midi_buf[i + 1] = midi_event[i + 1];
        }
    }

    ChSetEvent(midi_buf);
}
