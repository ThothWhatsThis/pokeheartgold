/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - demos - multiboot-wfs
  File:     common.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: common.h,v $
  Revision 1.4  10/20/2005 07:49:56  yosizaki
  fix port number.

  Revision 1.3  10/03/2005 09:54:20  yosizaki
  fix port number.

  Revision 1.2  08/09/2005 13:55:24  yosizaki
  fix GGID.

  Revision 1.1  06/23/2005 09:05:57  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef __NITROSDK_DEMO_MB_MULTIBOOT_WFS_COMMON_H
#define __NITROSDK_DEMO_MB_MULTIBOOT_WFS_COMMON_H


#include <nitro.h>
#include "util.h"


/******************************************************************************/
/* constant*/

/* wireless communication parameters*/
#define		WC_PARENT_DATA_SIZE_MAX		128
#define		WC_CHILD_DATA_SIZE_MAX		256

#if	!defined(SDK_MAKEGGID_SYSTEM)
#define SDK_MAKEGGID_SYSTEM(num)    (0x003FFF00 | (num))
#endif
#define GGID_WBT_FS                 SDK_MAKEGGID_SYSTEM(0x23)

extern const WMParentParam def_parent_param[1];


/* Communications settings for WFS*/
#define	port_wbt            5
#define	parent_packet_max   WC_PARENT_DATA_SIZE_MAX


/* screen transition mode*/
enum
{
    MODE_SELECT,                       /* Select start options*/
    MODE_ERROR,                        /* Stop due to error*/
    MODE_BUSY,                         /* Startup processing under way*/
    MODE_PARENT,                       /* Parent processing under way*/
    MODE_CHILD,                        /* Child processing under way*/
    MODE_MAX
};


/******************************************************************************/
/* function*/

#if	defined(__cplusplus)
extern  "C"
{
#endif

/*---------------------------------------------------------------------------*
  Name:         StateCallbackForWFS

  Description:  WFS callback function.
                This is called when the state became WFS_STATE_READY.
                WFS_GetStatus() can be used at an arbitrary position to make a determination
                without receiving notification with this callback.

  Arguments:    arg       User-defined argument specified in the callback.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    StateCallbackForWFS(void *arg);

/*---------------------------------------------------------------------------*
  Name:         AllocatorForWFS

  Description:  Dynamic allocation function for memory specified in WFS.

  Arguments:    arg       User-defined argument specified in the allocator.
                size      Required size if requesting memory allocation.
                ptr       If NULL, allocates memory. Otherwise, requests release.

  Returns:      If ptr is NULL, the amount of memory in size is allocated and its pointer returned.
                If not, the ptr memory is released.
                If released, the return value is simply ignored.
 *---------------------------------------------------------------------------*/
    void   *AllocatorForWFS(void *arg, u32 size, void *ptr);

/*---------------------------------------------------------------------------*
  Name:         WCCallbackForWFS

  Description:  Callback that receives status notification from the WC's wireless automatic drive.

  Arguments:    arg     - Callback pointer for the notification source WM function.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    WCCallbackForWFS(WMCallback *arg);

/*---------------------------------------------------------------------------*
  Name:         InitFrameLoop

  Description:  Initialization for game frame loop.

  Arguments:    p_key           Key information structure to initialize.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    InitFrameLoop(KeyInfo * p_key);

/*---------------------------------------------------------------------------*
  Name:         WaitForNextFrame

  Description:  Wait until next drawing frame.

  Arguments:    p_key           Key information structure to update.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    WaitForNextFrame(KeyInfo * p_key);

/*---------------------------------------------------------------------------*
  Name:         ModeError

  Description:  Processing in error display screen

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    ModeError(void);

/*---------------------------------------------------------------------------*
  Name:         ModeWorking

  Description:  Processing in busy screen

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    ModeWorking(void);

/*---------------------------------------------------------------------------*
  Name:         ModeSelect

  Description:  Process in parent/child selection screen

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    ModeSelect(void);

/*---------------------------------------------------------------------------*
  Name:         ModeMultiboot

  Description:  DS Single-Card play parent processing.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    ModeMultiboot(void);

/*---------------------------------------------------------------------------*
  Name:         ModeParent

  Description:  Processing in parent communications screen.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    ModeParent(void);

/*---------------------------------------------------------------------------*
  Name:         ModeChild

  Description:  child communications screen

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    ModeChild(void);


#if	defined(__cplusplus)
}                                      /* extern "C" */
#endif


/******************************************************************************/


#endif                                 /* __NITROSDK_DEMO_WBT_WBT_FS_COMMON_H*/
