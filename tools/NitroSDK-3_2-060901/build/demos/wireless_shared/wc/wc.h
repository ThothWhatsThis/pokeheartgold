/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - wireless_shared
  File:     wc.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wc.h,v $
  Revision 1.11  03/13/2006 06:04:46  okubata_ryoma
  Minor revisions to macro

  Revision 1.10  2006/01/18 02:12:39  kitase_hirotake
  do-indent

  Revision 1.9  04/26/2005 02:35:02  terui
  Fix comment

  Revision 1.8  02/28/2005 05:26:34  yosizaki
  do-indent.

  Revision 1.7  02/02/2005 04:21:20  terui
  Corrected a spelling mistake for TurnOff

  Revision 1.6  01/11/2005 07:51:38  terui
  Update copyright.

  Revision 1.5  01/11/2005 04:59:09  terui
  Added WcFinish function.

  Revision 1.4  12/22/2004 02:46:48  terui
  Support for Pictochat Search

  Revision 1.3  10/29/2004 01:54:47  terui
  Made changes so that the channel used for scanning children uses the channel passed to WcInt.

  Revision 1.2  10/28/2004 11:20:16  yosizaki
  merge with wbt-1.

  Revision 1.1  10/22/2004 11:04:14  terui
  Moved the wc module in the shared region

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef DEMOS_WIRELESS_SHARED_WC_H_
#define DEMOS_WIRELESS_SHARED_WC_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include    <nitro/types.h>
#include    <nitro/wm.h>


/*---------------------------------------------------------------------------*
    constant definitions
 *---------------------------------------------------------------------------*/
#define     WC_DMA_NO                   2       // Make sure that there is no overlap with the DMA numbers for GX, etc.
#define     WC_DEFAULT_PORT             4       // The port number  (4 - 7)
#define     WC_DEFAULT_PORT_PRIO        2       // The port priority ( 0 - 3 )
#define     WC_DS_PORT_NUMBER           12      // Port number for data sharing ( 12 - 15 )


/*---------------------------------------------------------------------------*
    macro definitions
 *---------------------------------------------------------------------------*/
// For 32-byte alignment
#define     WC_ROUNDUP_32B( x ) ( ( (x) + 31 ) & 0xffffffe0 )


/*---------------------------------------------------------------------------*
    structure definitions
 *---------------------------------------------------------------------------*/
typedef enum WCStatus
{
    WC_STATUS_READY = 0,               // Initialization state
    WC_STATUS_ERROR,                   // Unrecoverable error state
    WC_STATUS_BUSY,                    // Busy state when in the middle of a state transition  
    WC_STATUS_PARENT,                  // Connection completed state as parent device
    WC_STATUS_CHILD,                   // Connection completed state as child device
    WC_STATUS_MAX
}
WCStatus;

// Definition of the callback function type for single device MP mode
typedef void (*WCCallbackFunc) (u16 aid, u16 *data, u16 length);

// Definition of the callback function type for block transmission mode.  
// The WMCallback pointer is currently passed at the following points in time. 
// - At WM_StartMP completion
// - At WcSetParentData and WcSetChildData completion
// - At WM_Reset completion
typedef void (*WCBlockTransferCallbackFunc) (WMCallback *arg);

// Callback function type definition for pictochat search
typedef void (*WCPictoCatchCallbackFunc) (WMBssDesc *pBssDesc);


/*---------------------------------------------------------------------------*
    function definitions
 *---------------------------------------------------------------------------*/
void    WcInit(const WMParentParam *pp, BOOL dsFlag, BOOL btFlag, u16 framePeriod);     // Various initialization. Mainly to secure memory
void    WcFinish(void);                // Restore the state before the initialization. Mainly to release memory.
void    WcStartParent(void);           // Starts the connection as the parent device
void    WcStartChild(void);            // Starts the connection as the child device
void    WcEnd(void);                   // Starts the transition to the initialized state
WCStatus WcGetStatus(void);            // Gets the internal state of the WCStatus type
u16     WcGetAid(void);                // Gets the AID

// Configure the callback called when MP communications data is received.
void    WcSetMpReceiveCallback(WCCallbackFunc cb);
// Configure the callback called when configuring block transmission mode  
void    WcSetBlockTransferCallback(WCBlockTransferCallbackFunc cb);
// Set send data for the parent device
void    WcSetParentData(const void *buf, u16 size);
// Set send data as the child device
void    WcSetChildData(const void *buf, u16 size);

// After the connection is complete, advance data-sharing communications.  Call once per game frame.
BOOL    WcStepDataSharing(const void *send, u16 *bitmap);
// Gets the pointer to the terminal data from the reception data. 
u8     *WcGetSharedDataAddress(u16 aid);

// Enables the Pictochat Search feature.
void    WcTurnOnPictoCatch(WCPictoCatchCallbackFunc func);
// Disables the Pictochat Search feature.
void    WcTurnOffPictoCatch(void);


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C"*/
#endif

#endif /* DEMOS_WIRELESS_SHARED_WC_H_*/

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
