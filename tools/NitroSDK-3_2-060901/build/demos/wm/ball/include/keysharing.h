/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - ball
  File:     keysharing.h

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef KEYSHARING_H_
#define KEYSHARING_H_

#ifdef	__cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include <nitro.h>

#include <nitro/wm.h>
#include "wm_lib.h"

extern void KsParentInit(void);
extern void KsParentMain(void);
extern void KsChildInit(void);
extern void KsChildScan(void);
extern void KsChildMain(void);


/*===========================================================================*/

#ifdef	__cplusplus
}          /* extern "C" */
#endif

#endif /* KEYSHARING_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
