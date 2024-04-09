/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - demos - overlay
  File:     func.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: func.h,v $
  Revision 1.5  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.4  02/28/2005 05:26:03  yosizaki
  do-indent.

  Revision 1.3  09/29/2004 07:01:29  yosizaki
  add comments.
  add extern "C".
  change overlay functions' interface.

  Revision 1.2  09/09/2004 05:04:45  yosizaki
  add macro OVERLAY_CODE_BY_XXX

  Revision 1.1  09/06/2004 04:36:21  yosizaki
  (none)

  Revision 1.1  04/02/2004 12:21:22  yosizaki
  (none)


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef __NITROSDK_DEMO_FS_OVERLAY_FUNC_H
#define __NITROSDK_DEMO_FS_OVERLAY_FUNC_H

#include <nitro.h>


/******************************************************************************/
/* macro*/

/*
 * This macro is to allow redundancy in the overlay module size
 * volatile int v; OVAERLAY_CODE_BY_10000(++v);
 * should be used as shown above.
 * Be careful as the build time has become extremely long (particularly during optimization).
 */
#define	OVAERLAY_CODE_BY_10(expr)		(expr), (expr), (expr), (expr), (expr), (expr), (expr), (expr), (expr), (expr)
#define	OVAERLAY_CODE_BY_100(expr)		OVAERLAY_CODE_BY_10(OVAERLAY_CODE_BY_10(expr))
#define	OVAERLAY_CODE_BY_1000(expr)		OVAERLAY_CODE_BY_10(OVAERLAY_CODE_BY_100(expr))
#define	OVAERLAY_CODE_BY_10000(expr)	OVAERLAY_CODE_BY_10(OVAERLAY_CODE_BY_1000(expr))
#define	OVAERLAY_CODE_BY_100000(expr)	OVAERLAY_CODE_BY_10(OVAERLAY_CODE_BY_10000(expr))
#define	OVAERLAY_CODE_BY_1000000(expr)	OVAERLAY_CODE_BY_10(OVAERLAY_CODE_BY_100000(expr))
#define	OVAERLAY_CODE_BY_10000000(expr)	OVAERLAY_CODE_BY_10(OVAERLAY_CODE_BY_1000000(expr))


/******************************************************************************/
/* function*/

#if	defined(__cplusplus)
extern  "C"
{
#endif


/* Obtain the character string used to confirm that the overlay is working*/
    void    func_1(char *dst);
    void    func_2(char *dst);
    void    func_3(char *dst);


#if	defined(__cplusplus)
}                                      /* extern "C"*/
#endif


/******************************************************************************/


#endif                                 /* __NITROSDK_DEMO_FS_OVERLAY_FUNC_H*/
