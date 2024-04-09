/*---------------------------------------------------------------------------*
  Project:  NitroSDK - libraries - ctrdg
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

  Revision 1.1  2004/09/13 10:45:14  yada
  initial release.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef LIBRARIES_CTRDG_SP_H_
#define LIBRARIES_CTRDG_SP_H_

#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/
typedef enum CTRDGStatus
{
    CTRDG_STATUS_READY = 0,
    CTRDG_STATUS_INIT_MODULE_INFO
}
CTRDGStatus;

typedef struct CTRDGWork
{
    union
    {
        vu32 cmd32[CTRDG_PXI_PACKET_MAX];
        vu16 cmd16[CTRDG_PXI_PACKET_MAX * 2];
    };
    volatile CTRDGStatus status;
    vu16 count;
    vu16 pad[1];
}
CTRDGWorkSp;


/*===========================================================================*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LIBRARIES_CTRDG_SP_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
