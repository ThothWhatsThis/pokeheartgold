/*
  Project:  NitroSDK - CHT - demos - catch-min
  File:     wmscan.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wmscan.h,v $
  Revision 1.2  01/18/2006 02:12:40  kitase_hirotake
  do-indent

  Revision 1.1  07/14/2005 01:34:40  kitase_hirotake
  Initial Release

  $NoKeywords$
 */
#include <nitro/types.h>
#include <nitro/wm.h>

typedef void (*WSStartScanCallbackFunc) (WMBssDesc *bssDesc);

enum
{
    WS_SYSSTATE_STOP,                  // Wireless is stopped
    WS_SYSSTATE_IDLE,                  // IDLE state
    WS_SYSSTATE_SCANNING,              // Scanning
    WS_SYSSTATE_SCANIDLE,              // Scanning is stopped
    WS_SYSSTATE_BUSY,                  // Busy
    WS_SYSSTATE_ERROR,                 // Error occurred
    WS_SYSSTATE_FATAL                  // A FATAL error has occurred
};


/*---------------------------------------------------------------------------*
  Name:         WS_Initialize
  Description:  Initializes wireless.
  Arguments:    buf   Buffer size passed to WM. Size of WM_SYSTEM_BUF_SIZE required.
                dmaNo The DMA number used by wireless
  Returns:      If the process starts successfully, TRUE
                If it does not start, FALSE
 *---------------------------------------------------------------------------*/
BOOL    WS_Initialize(void *buf, u16 dmaNo);

/*---------------------------------------------------------------------------*
  Name:         WS_StartScan
  Description:  Starts scanning for a parent.
  Arguments:    callback  Callback when parent is found
                macAddr  Specifies the parent's MAC address
                         When searching for all parents, specify FF:FF:FF:FF:FF:FF
                If the 'continuous' flag is set to TRUE, until WS_EndScan is called,
                           all valid channels will continue to be scanned.
                           If FALSE, while cycling between valid channels,
                           it will stop scanning after each scan and transition to the WS_SYSSTATE_SCANIDLE state.
  Returns:      If the process starts successfully, TRUE
                If it does not start, FALSE
 *---------------------------------------------------------------------------*/
BOOL    WS_StartScan(WSStartScanCallbackFunc callback, const u8 *macAddr, BOOL continuous);

/*---------------------------------------------------------------------------*
  Name:         WS_EndScan
  Description:  Function that terminates scanning
  Arguments:    None.
  Returns:      If the process starts successfully, TRUE
                If it does not start, FALSE
 *---------------------------------------------------------------------------*/
BOOL    WS_EndScan(void);

/*---------------------------------------------------------------------------*
  Name:         WS_End
  Description:  Ends wireless communications.
  Arguments:    None.
  Returns:      If it succeeds returns TRUE.
 *---------------------------------------------------------------------------*/
BOOL    WS_End(void);

/*---------------------------------------------------------------------------*
  Name:         WS_SetGgid
  Description:  Configures the game group ID.
                Call before making a connection to the parent device.
  Arguments:    ggid    The game group ID to configure.
  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WS_SetGgid(u32 ggid);

/*---------------------------------------------------------------------------*
  Name:         WS_GetSystemState
  Description:  Obtains the WS state.
  Arguments:    None.
  Returns:      The WS state.
 *---------------------------------------------------------------------------*/
int     WS_GetSystemState(void);

/*---------------------------------------------------------------------------*
  Name:         WS_TurnOnPictoCatch
  Description:  Enables the PictoCatch feature.
                With WS_StartScan, even if Pictochat is found during a scan,
                the callback function is called.
  Arguments:    None.
  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WS_TurnOnPictoCatch(void);

/*---------------------------------------------------------------------------*
  Name:         WS_TurnOffPictoCatch
  Description:  Disables the PictoCatch feature.
  Arguments:    None.
  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WS_TurnOffPictoCatch(void);