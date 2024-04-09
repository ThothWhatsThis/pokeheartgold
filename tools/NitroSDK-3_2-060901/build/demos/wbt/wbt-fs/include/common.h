/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - demos - overlay
  File:     common.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: common.h,v $
  Revision 1.6  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.5  10/20/2005 07:49:17  yosizaki
  fix port number.

  Revision 1.4  10/03/2005 09:54:32  yosizaki
  fix port number.

  Revision 1.3  02/28/2005 05:26:03  yosizaki
  do-indent.

  Revision 1.2  10/28/2004 13:05:01  yosizaki
  move some parameters.

  Revision 1.1  10/25/2004 10:18:47  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef __NITROSDK_DEMO_WBT_WBT_FS_COMMON_H
#define __NITROSDK_DEMO_WBT_WBT_FS_COMMON_H

#include <nitro.h>


/******************************************************************************/
/* constant*/

/* wireless communication parameters*/

#define		WC_PARENT_DATA_SIZE_MAX		128
#define		WC_CHILD_DATA_SIZE_MAX		256

#if	!defined(SDK_MAKEGGID_SYSTEM)
#define SDK_MAKEGGID_SYSTEM(num)    (0x003FFF00 | (num))
#endif
#define GGID_WBT_FS                 SDK_MAKEGGID_SYSTEM(0x31)

/* port number for WFS*/
#define	port_wbt            5

/* maximum parent device send value for WFS*/
#define	parent_packet_max   WC_PARENT_DATA_SIZE_MAX


/******************************************************************************/
/* function*/

#if	defined(__cplusplus)
extern  "C"
{
#endif

/* functions for each sample demo screen transition*/
    void    ModeSelect(const KeyInfo * p_key);  /* Parent/child select screen*/
    void    ModeError(const KeyInfo * p_key);   /* Error display screen*/
    void    ModeWorking(const KeyInfo * p_key); /* Busy screen*/
    void    ModeParent(const KeyInfo * p_key);  /* Parent communications screen*/
    void    ModeChild(const KeyInfo * p_key);   /* Child communications screen*/

#if	defined(__cplusplus)
}                                      /* extern "C"*/
#endif


/******************************************************************************/


#endif                                 /* __NITROSDK_DEMO_WBT_WBT_FS_COMMON_H*/
