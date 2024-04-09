/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SND - demos - seq
  File:     seq.h

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: seq.h,v $
  Revision 1.2  01/18/2006 02:12:27  kitase_hirotake
  do-indent

  Revision 1.1  2005/04/13 03:29:19  ida
  Moved SND to NITRO-SDK

  Revision 1.2  03/08/2005 07:45:24  kyuma_koichi
  Initial release.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef SEQ_H_
#define SEQ_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>

/* Clock interval constant (microseconds)*/
#define SEQ_CLOCK_INTERVAL_NITRO_VBLANK 16715   /* NITRO VBlank interval*/

/* error code*/
#define SEQ_ERROR_WRONG_HEADER         1        /* Illegal header chunk.*/
#define SEQ_ERROR_NOT_FORMAT0          2        /* SMF that is not format 0*/
#define SEQ_ERROR_DIVISION_TIMECODE    3        /* SMF division is in time code format.*/
#define SEQ_ERROR_WRONG_TRACK          4        /* Illegal track chunk.*/
#define SEQ_ERROR_WRONG_DELTA_TIME     5        /* Length value of an illegal delta time variable.*/

/* Sound handle*/
typedef struct tSeqHandle
{
    u8      state;                     /* State*/
    u8      running_status;            /* Running status buffer*/
    u8      next_delta_bytesize;       /* Bite size of the delta time of the next event.*/
    u8      padding1;
    u16     division;                  /* SMF resolution*/
    u16     padding2;
    u32     tempo;                     /* Tempo*/
    u32     time_control;              /* Time manager*/
    u32     time_per_callback;         /* Time passed in one callback (tick*microseconds/quarter notes)*/
    u32     chunk_size;                /* Size of data chunk*/
    u32     total_tick;                /* Total number of ticks at the current position counting from the beginning.*/
    u32     delta_time;                /* Delta time until the next event.*/
    const u8 *current_ptr;             /* Pointer to the current position.*/
    const u8 *track_begin;             /* Pointer to the location of the start of a track.*/
    const u8 *loop_begin;              /* Pointer to the location of the start of a loop.*/
}
SeqHandle;

void    SeqInit(u32 clock_interval, void (*callback) (const u8 *));
BOOL    SeqMain(SeqHandle * block);
BOOL    SeqPlay(SeqHandle * block, const u8 *smfdata);
void    SeqStop(SeqHandle * block);
void    SeqPause(SeqHandle * block);
u8      SeqGetErrorCode(void);

#ifdef __cplusplus
}
#endif

#endif // SEQ_H_
