/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - ball
  File:     data.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: data.h,v $
  Revision 1.5  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.4  02/28/2005 05:26:01  yosizaki
  do-indent.

  Revision 1.3  01/18/2005 09:50:40  yosizaki
  Revised copyright year notation

  Revision 1.2  01/17/2005 06:38:36  yosizaki
  Unified font data.

  Revision 1.1  07/23/2004 15:11:39  terui
  Revisions and additions.

  Revision 1.3  06/22/2004 07:52:36  Sato
  Updated portions to support wmlib1.7

  Revision 1.2  06/14/2004 09:53:30  Sato
  (none)

  Revision 1.1  06/14/2004 08:27:16  Miya
  (none)

  Revision 1.3  03/08/2004 09:19:04  yada
  fixed top comment

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef DATA_H_
#define DATA_H_

#ifdef	__cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include <nitro.h>

extern u8 prog_state;
enum
{ STATE_NONE, STATE_SELECT, STATE_KS_PARENTINIT, STATE_KS_PARENT, STATE_KS_CHILDINIT,
    STATE_KS_CHILDSCAN, STATE_KS_CHILD
};
extern OSHeapHandle heapHandle;        // Heap handle


extern const u32 sampleCharData[8 * 0x100];
extern const u16 samplePlttData[16][16];


/*===========================================================================*/

#ifdef	__cplusplus
}          /* extern "C"*/
#endif

#endif /* DATA_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
