/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - dataShare-Model
  File:     unicode.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: unicode.h,v $
  Revision 1.4  01/18/2006 02:12:28  kitase_hirotake
  do-indent

  Revision 1.3  11/25/2005 02:13:27  adachi_hiroaki
  Corrected copyright notices

  Revision 1.1  11/21/2005 00:41:10  adachi_hiroaki
  Made the parent device selection screen display the number of players joining and their nicknames

  $NoKeywords$
 *---------------------------------------------------------------------------*/

#ifndef	__UNICODE_H_
#define	__UNICODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>

// define data----------------------------------

// function's prototype declaration-------------
void    ExSJIS_BEtoUTF16_LE(u8 *sjisp, u16 *unip, u16 length);
void    ExUTF16_LEtoSJIS_BE(u8 *sjisp, u16 *unip, u16 length);
void    CheckSJIS_BEtoUTF16_LE(void);


#ifdef __cplusplus
}
#endif

#endif // __UNICODE_H_
