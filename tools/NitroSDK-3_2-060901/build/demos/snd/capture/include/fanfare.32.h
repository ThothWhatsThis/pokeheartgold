/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SND - demos - capture
  File:     fanfare.32.h

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fanfare.32.h,v $
  Revision 1.2  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.1  2005/04/13 03:29:35  ida
  Moved SND to NITRO-SDK

  Revision 1.3  04/13/2005 01:15:26  kyuma_koichi
  Changed waveform file format

  Revision 1.2  03/08/2005 07:45:24  kyuma_koichi
  Initial release.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef FANFARE_32_H_
#define FANFARE_32_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>

#define FANFARE_32_FORMAT    SND_WAVE_FORMAT_PCM16
#define FANFARE_32_LOOPFLAG  0
#define FANFARE_32_RATE      32000
#define FANFARE_32_TIMER     523
#define FANFARE_32_LOOPSTART 0
#define FANFARE_32_LOOPLEN   56926

const s16 fanfare_32[];

#ifdef __cplusplus
}
#endif

#endif // FANFARE_32_H_
