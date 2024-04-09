/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WVR - demos - switchover
  File:     font.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: font.h,v $
  Revision 1.2  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.1  2005/02/16 12:47:20  yosizaki
  initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef	FONT_H_
#define	FONT_H_

#ifdef	__cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include	<nitro/types.h>

extern const u32 d_CharData[8 * 256];
extern const u32 d_PaletteData[8 * 16];

/*===========================================================================*/

#ifdef	__cplusplus
}          /* extern "C" */
#endif

#endif /* FONT_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
