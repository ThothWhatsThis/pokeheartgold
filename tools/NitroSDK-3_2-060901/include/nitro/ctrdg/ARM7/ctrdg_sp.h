/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG - include
  File:     ctrdg_sp.h

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg_sp.h,v $
  Revision 1.2  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.1  2004/09/14 07:25:46  yada
  first release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_CTRDG_SP_H_
#define NITRO_CTRDG_SP_H_


#include <nitro/types.h>
#include <nitro/memorymap.h>
#include <nitro/mi.h>
#include <nitro/os.h>
#include <nitro/pxi.h>


#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
  Name:         CTRDG_CheckPullOut_Polling()

  Description:  polling that cartridge is pulled out

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void CTRDG_CheckPullOut_Polling(void);


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_CTRDG_SP_H_ */
#endif
