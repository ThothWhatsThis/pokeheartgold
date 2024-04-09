/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - wireless_shared
  File:     wc.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wc.c,v $
  Revision 1.27  06/14/2006 01:42:12  okubata_ryoma
  Added a process for calling the WM_EndDataSharing function when resetting

  Revision 1.26  04/10/2006 13:29:43  yosizaki
  fix specific to WcSetBlockTransferCallback.

  Revision 1.25  02/20/2006 08:04:30  seiki_masashi
  Changed code to use WM_IsValidGameBeacon function

  Revision 1.24  01/18/2006 02:12:39  kitase_hirotake
  do-indent

  Revision 1.23  01/10/2006 02:51:04  adachi_hiroaki
  All hexadecimal notation output was expressly labeled with 0x. /([^xX])(%\d*[xX][^hH])/ -> \10x\2

  Revision 1.22  01/06/2006 08:20:25  kitase_hirotake
  Debug output changed to expressly hexadecimal notation.

  Revision 1.21  10/28/2005 11:24:57  seiki_masashi
  Supported addition of WM_STATECODE_DISCONNECTED_FROM_MYSELF.

  Revision 1.20  06/17/2005 06:34:45  miya
  Added WBT callback for disconnection of child

  Revision 1.19  04/26/2005 02:35:02  terui
  Fix comment

  Revision 1.18  02/28/2005 05:26:34  yosizaki
  do-indent.

  Revision 1.17  02/18/2005 12:06:07  seiki_masashi
  revised the debug display

  Revision 1.16  02/18/2005 09:26:50  yosizaki
  fix around hidden warnings.

  Revision 1.15  02/08/2005 07:03:16  terui
  Made revisions so that the clearing of the callback function is performed first in the WcFinish function

  Revision 1.14  02/02/2005 04:21:20  terui
  Corrected a spelling mistake for TurnOff

  Revision 1.13  01/11/2005 07:48:40  takano_makoto
  fix copyright header.

  Revision 1.12  01/11/2005 04:59:46  terui
  Added WcFinish function.

  Revision 1.11  01/07/2005 12:03:56  terui
  Added the process for receiving unrecoverable error notification.

  Revision 1.10  12/27/2004 05:29:46  terui
  Added the condition to call back Wc_Reset in StartConnect callback

  Revision 1.9  12/22/2004 02:47:29  terui
  Support for Pictochat Search

  Revision 1.8  11/19/2004 08:22:37  terui
  Changed the GGID check method.

  Revision 1.7  11/10/2004 06:24:45  terui
  Omitted the branch process when a child was disconnected from a parent.

  Revision 1.6  10/29/2004 04:31:43  terui
  Fixed a bug in the state management during reset

  Revision 1.5  10/29/2004 03:37:21  seiki_masashi
  Corrected comment

  Revision 1.4  10/29/2004 01:55:33  terui
  Made changes so that the channel used for scanning children uses the channel passed to WcInt.

  Revision 1.3  10/28/2004 14:09:10  yosizaki
  change to use wireless_shared.

  Revision 1.2  10/28/2004 11:20:16  yosizaki
  merge with wbt-1.

  Revision 1.1  10/22/2004 11:04:14  terui
  Moved the wc module in the shared region

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro.h>
#include    <nitro/cht.h>
#include    "wc.h"


/*---------------------------------------------------------------------------*
    Compile switch for debugging
 *---------------------------------------------------------------------------*/
#define     WC_DEBUG_PRINT             // Debug output switch


/*---------------------------------------------------------------------------*
    Internal function definitions
 *---------------------------------------------------------------------------*/
static void WcCreateParentParameter(void);      // Edit parent information
static void WcCreateScanParameter(void);        // Edit scan settings
static void WcSetNextScanCannel(void);

// Callback function from WM
static void WcCb_Enable(void *arg);
static void WcCb_Disable(void *arg);
static void WcCb_PowerOn(void *arg);
static void WcCb_PowerOff(void *arg);
static void WcCb_Indicate(void *arg);
static void WcCb_Reset(void *arg);
static void WcCb_SetParentParameter(void *arg);
static void WcCb_StartParent(void *arg);
static void WcCb_StartMP(void *arg);
static void WcCb_StartScan(void *arg);
static void WcCb_EndScan(void *arg);
static void WcCb_StartConnect(void *arg);
static void WcCb_SetMPData(void *arg);
static void WcCb_ReceiveData(void *arg);

// WM call single-feature functions
static BOOL Wc_Enable(void);
static BOOL Wc_Disable(void);
static BOOL Wc_PowerOn(void);
static BOOL Wc_PowerOff(void);
static BOOL Wc_SetIndCallback(void);
static BOOL Wc_Reset(void);
static BOOL Wc_SetParentParameter(void);
static BOOL Wc_StartParent(void);
static BOOL Wc_StartScan(void);
static BOOL Wc_EndScan(void);
static BOOL Wc_StartConnect(void);
static BOOL Wc_StartMP(void);
static BOOL Wc_StartDataSharing(void);
static BOOL Wc_SetMPData(const void *buf, u16 size);

// Print output suppression dummy functions
#ifdef  WC_DEBUG_PRINT
#define     WcDebugPrintf       OS_TPrintf
#else
#define     WcDebugPrintf(...)  ((void) 0)
#endif


/*---------------------------------------------------------------------------*
    internal variable definitions
 *---------------------------------------------------------------------------*/
static WCStatus wcStatus;              // Manage the current state
static WCStatus wcTarget;              // Manage the target of the state change
static u16 wcAid;                      // AID (when child device)
static u16 wcFramePeriod;              // Game frame interval
static BOOL wcDataShareFlag;           // Data share / normal MP switch flag
static BOOL wcStartDataShareFlag;      // Data sharing active flag
static BOOL wcBlockTransFlag;          // Block transfer / normal MP switch flag
static BOOL wcSendFlag;                // Send completion flag (when normal MP mode)

static WMParentParam *wcDefaultParameter;
static WCCallbackFunc wcMpRecvCallback;
static WCBlockTransferCallbackFunc wcBtCallback;

static void *wcWmBuffer;               // The entity is a cache-safe buffer
static WMParentParam *wcParentParameter;        // The entity is a cache-safe buffer
static WMScanParam *wcScanParameter;   // The entity is a cache-safe buffer
static WMBssDesc *wcWmBssDesc;         // The entity is a cache-safe buffer
static s32 wcSendBufferSize;           // A multiple of 32
static void *wcSendBuffer;             // The entity is a cache-safe buffer
static s32 wcRecvBufferSize;           // A multiple of 32
static void *wcRecvBuffer;             // The entity is a cache-safe buffer
static WMDataSharingInfo *wcDataSharingInfo;
static WMDataSet *wcDataSet;

static BOOL wcPictoCatchStatus;
static WCPictoCatchCallbackFunc wcPictoCatchCallback;


/*===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         WcInit

  Description:  Initializes wireless communications.

  Arguments:    pp          -   Designates parameter for the parent in wireless
                                communications with a pointer. Do not alter the contents
                                while WC module is running.
                dsFlag      -   The data sharing flag.
                                If TRUE is designated, goes into data sharing mode.
                                If FALSE is designated, the MP goes into direct manipulation mode.
                btFlag      -   The block transfer flag.
                                If TRUE is designated, goes into block transfer mode.
                framePeriod -   The number of picture frames for each game frame.
                                Applicable only when in data sharing mode.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WcInit(const WMParentParam *pp, BOOL dsFlag, BOOL btFlag, u16 framePeriod)
{
    // State initialization
    wcStatus = WC_STATUS_READY;
    wcAid = 0;
    wcFramePeriod = framePeriod;
    wcDataShareFlag = dsFlag;
    wcStartDataShareFlag = FALSE;
    wcBlockTransFlag = btFlag;
    wcDefaultParameter = (WMParentParam *)pp;
    // Initialize the send flag
    {
        wcSendFlag = FALSE;
    }
    // Initialize the user callback
    wcMpRecvCallback = NULL;
    wcBtCallback = NULL;

    // Allocate memory for WM
    wcWmBuffer = OS_Alloc(WM_SYSTEM_BUF_SIZE);
    wcParentParameter = (WMParentParam *)OS_Alloc(sizeof(WMParentParam));
    wcScanParameter = (WMScanParam *)OS_Alloc(sizeof(WMScanParam));
    wcWmBssDesc = (WMBssDesc *)OS_Alloc(sizeof(WMBssDesc));
    if (wcDataShareFlag)
    {
        wcDataSharingInfo = (WMDataSharingInfo *)OS_Alloc(sizeof(WMDataSharingInfo));
        wcDataSet = (WMDataSet *)OS_Alloc(sizeof(WMDataSet));
    }
    wcPictoCatchStatus = 0;

    // Allocate memory for the send buffer
    {
        s32     parentSize;
        s32     childSize;

        parentSize = WM_SIZE_MP_PARENT_SEND_BUFFER(pp->parentMaxSize, FALSE);
        childSize = WM_SIZE_MP_CHILD_SEND_BUFFER(pp->childMaxSize, FALSE);
        wcSendBufferSize = (parentSize > childSize) ? parentSize : childSize;
    }
    wcSendBuffer = OS_Alloc((u32)wcSendBufferSize);

    // Allocate memory for the receive buffer
    {
        s32     parentSize;
        s32     childSize;

        parentSize = (s32)WM_SIZE_MP_PARENT_RECEIVE_BUFFER(pp->childMaxSize, pp->maxEntry, FALSE);
        childSize = (s32)WM_SIZE_MP_CHILD_RECEIVE_BUFFER(pp->parentMaxSize, FALSE);
        wcRecvBufferSize = (parentSize > childSize) ? parentSize : childSize;
    }
    wcRecvBuffer = OS_Alloc((u32)wcRecvBufferSize);

    // initializes the WM library
    (void)WM_Init(wcWmBuffer, WC_DMA_NO);
}

/*---------------------------------------------------------------------------*
  Name:         WcFinish

  Description:  Returns to the wireless communication to the state before the initialization.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WcFinish(void)
{
    WMErrCode wmResult;

    // Port receiving callback clear
    {
        WcDebugPrintf("> WM_SetPortCallback start.\n");
        wmResult = WM_SetPortCallback(WC_DEFAULT_PORT, NULL, NULL);
        if (wmResult == WM_ERRCODE_SUCCESS)
        {
            WcDebugPrintf("< WM_SetPortCallback success.\n");
        }
        else
        {
            WcDebugPrintf("< WM_SetPortCallback failed. ERRCODE: 0x%02x\n", wmResult);
        }
    }

    switch (wcStatus)
    {
    case WC_STATUS_READY:
    case WC_STATUS_ERROR:
        // Stop WM library
        WcDebugPrintf("> WM_Finish start.\n");
        wmResult = WM_Finish();
        if (wmResult == WM_ERRCODE_SUCCESS)
        {
            WcDebugPrintf("< WM_Finish success.\n");
            break;
        }
        WcDebugPrintf("< WM_Finish failed. ERRCODE: 0x%02x\n", wmResult);
    default:
        WcDebugPrintf("* Could not finish Wireless library\n");
        return;
    }

    // Release memory
    OS_Free(wcRecvBuffer);
    OS_Free(wcSendBuffer);
    if (wcDataShareFlag)
    {
        OS_Free(wcDataSet);
        OS_Free(wcDataSharingInfo);
    }
    OS_Free(wcWmBssDesc);
    OS_Free(wcScanParameter);
    OS_Free(wcParentParameter);
    OS_Free(wcWmBuffer);
}

/*---------------------------------------------------------------------------*
  Name:         WcStartParent

  Description:  Starts wireless communications as a parent.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WcStartParent(void)
{
    switch (wcStatus)
    {
    case WC_STATUS_READY:
        // Start initialization as a parent
        wcStatus = WC_STATUS_BUSY;
        wcTarget = WC_STATUS_PARENT;   // Set the target state to "parent connection state"
        (void)Wc_Enable();
        break;
    case WC_STATUS_BUSY:
        // Update the target state
        wcTarget = WC_STATUS_PARENT;   // Set the target state to "parent connection state"
        break;
    case WC_STATUS_PARENT:
        // No processing, since already parent
        break;
    case WC_STATUS_CHILD:
        // Reset once.
        wcStatus = WC_STATUS_BUSY;
        wcTarget = WC_STATUS_PARENT;   // Set the target state to "parent connection state"
        (void)Wc_Reset();
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcStartChild

  Description:  Start wireless communications as the child device.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WcStartChild(void)
{
    switch (wcStatus)
    {
    case WC_STATUS_READY:
        // Start initialization as the child device
        wcStatus = WC_STATUS_BUSY;
        wcTarget = WC_STATUS_CHILD;    // Set the target state to "child device connection state"
        (void)Wc_Enable();
        break;
    case WC_STATUS_BUSY:
        // Update the target state
        wcTarget = WC_STATUS_CHILD;    // Set the target state to "child device connection state"
        break;
    case WC_STATUS_PARENT:
        // Reset once.
        wcStatus = WC_STATUS_BUSY;
        wcTarget = WC_STATUS_CHILD;    // Set the target state to "child device connection state"
        (void)Wc_Reset();
        break;
    case WC_STATUS_CHILD:
        // Since already child device, no processing
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcEnd

  Description:  Returns the pre-communications state.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WcEnd(void)
{
    switch (wcStatus)
    {
    case WC_STATUS_READY:
        // No processing, since already in the READY state
        break;
    case WC_STATUS_BUSY:
        // Update the target state
        wcTarget = WC_STATUS_READY;    // Set the target state to "READY"
        break;
    case WC_STATUS_PARENT:
    case WC_STATUS_CHILD:
        // Disconnect by resetting
        wcStatus = WC_STATUS_BUSY;
        wcTarget = WC_STATUS_READY;    // Set the target state to "READY"
        (void)Wc_Reset();
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcGetStatus

  Description:  Gets the current wireless communications state.

  Arguments:    None.

  Returns:      Returns the current state.
 *---------------------------------------------------------------------------*/
WCStatus WcGetStatus(void)
{
    return wcStatus;
}

/*---------------------------------------------------------------------------*
  Name:         WcGetAid

  Description:  Gets the AID allocated in the wireless communications group.

  Arguments:    None.

  Returns:      Returns the allocated AID.
 *---------------------------------------------------------------------------*/
u16 WcGetAid(void)
{
    return wcAid;
}

/*---------------------------------------------------------------------------*
  Name:         WcStepDataSharing

  Description:  Proceed to the next step in the synchronized data sharing process.

  Arguments:    send   - The pointer to the data to share. Data length is
                         fixed at WC_SHARED_DATA_SIZE. Must be 32-byte aligned.
                bitmap - The pointer to the variables that get the bitmap that indicates
                         the terminal included in the communication group.

  Returns:      BOOL   - If the process succeeds and the shared data was able to be obtained,
                         returns TRUE.
                         The obtained data gets the address to the data from each terminal
                         with the WcGetSharedDataAddress function.
                         Returns FALSE if there is a failure for any reason.
 *---------------------------------------------------------------------------*/
BOOL WcStepDataSharing(const void *send, u16 *bitmap)
{
    WMErrCode wmResult;

    // Not possible to execute if not in data sharing mode.
    if (!wcDataShareFlag)
    {
        return FALSE;
    }

    // Proceed to the next step in the synchronized data sharing process.
    WcDebugPrintf("> WM_StepDataSharing start.\n");
    wmResult = WM_StepDataSharing(wcDataSharingInfo, send, wcDataSet);

    // Determine processing results
    if (wmResult != WM_ERRCODE_SUCCESS)
    {
        WcDebugPrintf("< WM_StepDataSharing failed. ERRCODE: 0x%02x\n", wmResult);
        return FALSE;
    }

    // Store the bitmap of the connected terminal
    *bitmap = wcDataSet->receivedBitmap;

    WcDebugPrintf("< WM_StepDataSharing success.\n");
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         WcGetSharedDataAddress

  Description:  Gets the address to the data from the designated AID terminal from the
                data obtained in data sharing.

  Arguments:    aid - Designates the terminal AID that gets the data. The parent device is "0".

  Returns:      u8* - Returns the address to the data from the designated terminal. Data length is
                      is fixed at WC_SHARED_DATA_SIZE.
                      If the data from the designated terminal does not exist, etc.,
                      and the process fails, a NULL is returned.
 *---------------------------------------------------------------------------*/
u8     *WcGetSharedDataAddress(u16 aid)
{
    // Not possible to execute if not in data sharing mode.
    if (!wcDataShareFlag)
    {
        return NULL;
    }

    return (u8 *)WM_GetSharedDataAddress(wcDataSharingInfo, wcDataSet, aid);
}

/*---------------------------------------------------------------------------*
  Name:         WcSetParentData

  Description:  Configures the data to send as the parent device.

  Arguments:    buf  - The pointer to the data to send.
                       (Must be 32-byte aligned)
                size - The size of the data to send

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WcSetParentData(const void *buf, u16 size)
{
    // Confirm the send data size
    if (size > wcParentParameter->parentMaxSize)
    {
        return;
    }
    // Check that the previous send has completed
    if (wcSendFlag)
    {
        wcSendFlag = FALSE;

        (void)Wc_SetMPData(buf, size);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcSetChildData

  Description:  Configures the data to send as the child device.

  Arguments:    buf  - The pointer to the data to send.
                       (Must be 32-byte aligned)
                size - The size of the data to send

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WcSetChildData(const void *buf, u16 size)
{
    // Confirm the send data size
    if (size > wcDefaultParameter->childMaxSize)
    {
        return;
    }
    // Check that the previous send has completed
    if (wcSendFlag)
    {
        wcSendFlag = FALSE;

        (void)Wc_SetMPData(buf, size);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcSetMpReceiveCallback

  Description:  Configures the callback function when data is received on MP communications.

  Arguments:    cb  -   Configures the callback function

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WcSetMpReceiveCallback(WCCallbackFunc cb)
{
    wcMpRecvCallback = cb;
}

/*---------------------------------------------------------------------------*
  Name:         WcSetBlockTransferCallback

  Description:  Configures the callback function when data is received on MP communications.

  Arguments:    cb  -   Configures the callback function

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WcSetBlockTransferCallback(WCBlockTransferCallbackFunc cb)
{
    wcBtCallback = cb;
}

/*---------------------------------------------------------------------------*
  Name:         WcTurnOnPictoCatch

  Description:  Enables the Pictochat Search feature.

  Arguments:    func    -   This specifies the callback function to be called
                            when the beacon information of pictochat was found.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WcTurnOnPictoCatch(WCPictoCatchCallbackFunc func)
{
    wcPictoCatchStatus = 1;
    wcPictoCatchCallback = func;
}

/*---------------------------------------------------------------------------*
  Name:         WcTurnOffPictoCatch

  Description:  Disables the Pictochat Search feature.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WcTurnOffPictoCatch(void)
{
    wcPictoCatchStatus = 0;
    wcPictoCatchCallback = NULL;
}

/*===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         WcCreateParentParameter

  Description:  Edits the parent information.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcCreateParentParameter(void)
{
    static u16 tempID;

    // Copies to the buffer that will not have any problems if the parent information was cache stored
    MI_CpuCopy32(wcDefaultParameter, wcParentParameter, sizeof(WMParentParam));
    // Set the beacon interval to an appropriate value
    wcParentParameter->beaconPeriod = WM_GetDispersionBeaconPeriod();
    // Increment tempID
    wcParentParameter->tgid = ++tempID;
}

/*---------------------------------------------------------------------------*
  Name:         WcCreateScanParameter

  Description:  Edits the scan configuration information.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcCreateScanParameter(void)
{
    wcScanParameter->scanBuf = wcWmBssDesc;
    wcScanParameter->channel = wcDefaultParameter->channel;
    // Set the search limit time to an appropriate value
    wcScanParameter->maxChannelTime = WM_GetDispersionScanPeriod();
    // Designate a broadcast address and set so that it looks for all parent devices
    wcScanParameter->bssid[0] = 0xff;
    wcScanParameter->bssid[1] = 0xff;
    wcScanParameter->bssid[2] = 0xff;
    wcScanParameter->bssid[3] = 0xff;
    wcScanParameter->bssid[4] = 0xff;
    wcScanParameter->bssid[5] = 0xff;
}

/*---------------------------------------------------------------------------*
  Name:         WcSetNextScanCannel

  Description:  Updates to the channel specification in the scan setting to the next channel.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcSetNextScanCannel(void)
{
    u16     allowedChannel;
    s32     i;

    allowedChannel = WM_GetAllowedChannel();
    if (allowedChannel == 0)
        return;
    for (i = 0; i < 16; i++)
    {
        if (allowedChannel & (0x0001 << ((wcScanParameter->channel + i) % 16)))
        {
            wcScanParameter->channel = (u16)(((wcScanParameter->channel + i) % 16) + 1);
            break;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcCb_Enable

  Description:  The callback function to WM_Enable.

  Arguments:    arg - The pointer to the callback structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcCb_Enable(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS)
    {
        WcDebugPrintf("< WM_Enable success.\n");
        switch (wcTarget)
        {
        case WC_STATUS_READY:
            (void)Wc_Disable();
            break;
        case WC_STATUS_PARENT:
        case WC_STATUS_CHILD:
            (void)Wc_PowerOn();
            break;
        }
    }
    else
    {
        WcDebugPrintf("< WM_Enable failed. ERRCODE: 0x%02x 0x%02x 0x%02x\n", cb->errcode,
                      cb->wlCmdID, cb->wlResult);
        wcStatus = WC_STATUS_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcCb_Disable

  Description:  The callback function to WM_Disable.

  Arguments:    arg - The pointer to the callback structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcCb_Disable(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS)
    {
        WcDebugPrintf("< WM_Disable success.\n");
        switch (wcTarget)
        {
        case WC_STATUS_READY:
            wcStatus = WC_STATUS_READY; // Complete the transition to the READY state as normal
            break;
        case WC_STATUS_PARENT:
            wcStatus = WC_STATUS_READY;
            WcStartParent();
            break;
        case WC_STATUS_CHILD:
            wcStatus = WC_STATUS_READY;
            WcStartChild();
            break;
        }
    }
    else
    {
        WcDebugPrintf("< WM_Disable failed. ERRCODE: 0x%02x 0x%02x 0x%02x\n", cb->errcode,
                      cb->wlCmdID, cb->wlResult);
        wcStatus = WC_STATUS_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcCb_PowerOn

  Description:  The callback function to WM_PowerOn.

  Arguments:    arg - The pointer to the callback structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcCb_PowerOn(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS)
    {
        WcDebugPrintf("< WM_PowerOn success.\n");
        switch (wcTarget)
        {
        case WC_STATUS_READY:
            (void)Wc_PowerOff();
            break;
        case WC_STATUS_PARENT:
            if (Wc_SetIndCallback())
            {
                WcCreateParentParameter();
                if (!Wc_SetParentParameter())
                {
                    wcTarget = WC_STATUS_READY; // Set the target state to "READY"
                    (void)Wc_Reset();
                }
            }
            break;
        case WC_STATUS_CHILD:
            if (TRUE == Wc_SetIndCallback())
            {
                WcCreateScanParameter();
                if (!Wc_StartScan())
                {
                    wcTarget = WC_STATUS_READY; // Set the target state to "READY"
                    (void)Wc_Reset();
                }
            }
            break;
        }
    }
    else
    {
        WcDebugPrintf("< WM_PowerOn failed. ERRCODE: 0x%02x 0x%02x 0x%02x\n", cb->errcode,
                      cb->wlCmdID, cb->wlResult);
        wcStatus = WC_STATUS_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcCb_PowerOff

  Description:  The callback function to WM_PowerOff.

  Arguments:    arg - The pointer to the callback structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcCb_PowerOff(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS)
    {
        WcDebugPrintf("< WM_PowerOff success.\n");
        switch (wcTarget)
        {
        case WC_STATUS_READY:
            (void)Wc_Disable();
            break;
        case WC_STATUS_PARENT:
        case WC_STATUS_CHILD:
            (void)Wc_PowerOn();
            break;
        }
    }
    else
    {
        WcDebugPrintf("< WM_PowerOff failed. ERRCODE: 0x%02x 0x%02x 0x%02x\n", cb->errcode,
                      cb->wlCmdID, cb->wlResult);
        wcStatus = WC_STATUS_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcCb_Indicate

  Description:  The callback function called when Indicate occurs.

  Arguments:    arg - The pointer to the callback structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcCb_Indicate(void *arg)
{
    WMindCallback *cb = (WMindCallback *)arg;

    WcDebugPrintf("- Indication. STATECODE: 0x%02x\n", cb->state);
    if (cb->errcode == WM_ERRCODE_FIFO_ERROR)
    {
        // Unrecoverable error
        wcTarget = WC_STATUS_ERROR;
        wcStatus = WC_STATUS_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcCb_Reset

  Description:  The callback function to WM_Reset.

  Arguments:    arg - The pointer to the callback structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcCb_Reset(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS)
    {
        WcDebugPrintf("< WM_Reset success.\n");
        wcAid = 0;
        if (wcBtCallback != NULL)
            (*wcBtCallback) (arg);
        switch (wcTarget)
        {
        case WC_STATUS_READY:
            (void)Wc_PowerOff();
            break;
        case WC_STATUS_PARENT:
            WcCreateParentParameter();
            if (!Wc_SetParentParameter())
            {
                wcTarget = WC_STATUS_READY;
                (void)Wc_Reset();
            }
            break;
        case WC_STATUS_CHILD:
            WcCreateScanParameter();
            if (!Wc_StartScan())
            {
                wcTarget = WC_STATUS_READY;
                (void)Wc_Reset();
            }
            break;
        }
    }
    else
    {
        WcDebugPrintf("< WM_Reset failed. ERRCODE: 0x%02x 0x%02x 0x%02x\n", cb->errcode,
                      cb->wlCmdID, cb->wlResult);
        wcStatus = WC_STATUS_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcCb_SetParentParameter

  Description:  The callback function to WM_SetParentParameter.

  Arguments:    arg - The pointer to the callback structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcCb_SetParentParameter(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS)
    {
        WcDebugPrintf("< WM_SetParentParameter success.\n");
        switch (wcTarget)
        {
        case WC_STATUS_READY:
            (void)Wc_Reset();
            break;
        case WC_STATUS_PARENT:
            if (!Wc_StartParent())
            {
                wcTarget = WC_STATUS_READY;
                (void)Wc_Reset();
            }
            break;
        case WC_STATUS_CHILD:
            wcTarget = WC_STATUS_CHILD;
            // Reset once to go into child mode
            (void)Wc_Reset();
            break;
        }
    }
    else
    {
        WcDebugPrintf("< WM_SetParentParameter failed. ERRCODE: 0x%02x 0x%02x 0x%02x\n",
                      cb->errcode, cb->wlCmdID, cb->wlResult);
        wcTarget = WC_STATUS_READY;
        (void)Wc_Reset();
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcCb_StartParent

  Description:  The callback function to WM_StartParent.

  Arguments:    arg - The pointer to the callback structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcCb_StartParent(void *arg)
{
    WMstartParentCallback *cb = (WMstartParentCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS)
    {
        switch (cb->state)
        {
        case WM_STATECODE_PARENT_START:
            WcDebugPrintf("< WM_StartParent success.\n");
            wcAid = 0;
            switch (wcTarget)
            {
            case WC_STATUS_READY:
                (void)Wc_Reset();
                break;
            case WC_STATUS_PARENT:
                if (!Wc_StartMP())
                {
                    wcTarget = WC_STATUS_READY;
                    (void)Wc_Reset();
                }
                break;
            case WC_STATUS_CHILD:
                wcTarget = WC_STATUS_CHILD;
                // Reset once to go into child mode
                (void)Wc_Reset();
                break;
            }
            break;
        case WM_STATECODE_CONNECTED:
            WcDebugPrintf("- Child Connected.\n");
            if (wcBtCallback != NULL)
                (*wcBtCallback) (arg);
            break;
        case WM_STATECODE_BEACON_SENT:
            WcDebugPrintf("- Beacon sent.\n");
            break;
        case WM_STATECODE_DISCONNECTED:
            WcDebugPrintf("- Child disconnected.\n");
            if (wcBtCallback != NULL)
                (*wcBtCallback) (arg);
            break;
        case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
            // Does nothing if the unit disconnects from itself
            break;
        }
    }
    else
    {
        WcDebugPrintf("< WM_StartParent failed. ERRCODE: 0x%02x 0x%02x 0x%02x\n", cb->errcode,
                      cb->wlCmdID, cb->wlResult);
        wcTarget = WC_STATUS_READY;
        (void)Wc_Reset();
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcCb_StartScan

  Description:  The callback function to WM_StartScan.

  Arguments:    arg - The pointer to the callback structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcCb_StartScan(void *arg)
{
    WMstartScanCallback *cb = (WMstartScanCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS)
    {
        WcDebugPrintf("< WM_StartScan success.\n");
        switch (wcTarget)
        {
        case WC_STATUS_READY:
            (void)Wc_Reset();
            break;
        case WC_STATUS_PARENT:
            // Return to IDLE once in order to transition to parent mode
            if (!Wc_EndScan())
            {
                wcTarget = WC_STATUS_READY;
                (void)Wc_Reset();
            }
            break;
        case WC_STATUS_CHILD:
            switch (cb->state)
            {
            case WM_STATECODE_PARENT_FOUND:
                // Delete cache in the parent information storage buffer
                DC_InvalidateRange(wcWmBssDesc, sizeof(WMbssDesc));
                // Checks if it is the beacon of pictochat
                if (wcPictoCatchStatus)
                {
                    if ((CHT_IsPictochatParent(wcWmBssDesc)) && (wcPictoCatchCallback != NULL))
                    {
                        wcPictoCatchCallback(wcWmBssDesc);
                    }
                }
                else
                {
                    // Check whether the GameGroupID matches
                    if (WM_IsValidGameBeacon(wcWmBssDesc) &&
                        (wcWmBssDesc->gameInfo.ggid == wcDefaultParameter->ggid))
                    {
                        // Determine that it is parent of same application
                        if (!Wc_EndScan())
                        {
                            wcTarget = WC_STATUS_READY;
                            (void)Wc_Reset();
                        }
                        break;
                    }
                }
                // Don't break here.
                // Execute a scan once more without a break
            case WM_STATECODE_PARENT_NOT_FOUND:
                // Execute a scan once more
                WcSetNextScanCannel();
                if (!Wc_StartScan())
                {
                    wcTarget = WC_STATUS_READY;
                    (void)Wc_Reset();
                }
                break;
            default:
                WcDebugPrintf("- Invalid state code. STATECODE: 0x%02x\n", cb->state);
                wcStatus = WC_STATUS_BUSY;
                wcTarget = WC_STATUS_READY;
                (void)Wc_Reset();
            }
            break;
        }
    }
    else
    {
        WcDebugPrintf("< WM_StartScan failed. ERRCODE: 0x%02x 0x%02x 0x%02x\n", cb->errcode,
                      cb->wlCmdID, cb->wlResult);
        wcTarget = WC_STATUS_READY;
        (void)Wc_Reset();
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcCb_EndScan

  Description:  The callback function to WM_EndScan.

  Arguments:    arg - The pointer to the callback structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcCb_EndScan(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS)
    {
        WcDebugPrintf("< WM_EndScan success.\n");
        switch (wcTarget)
        {
        case WC_STATUS_READY:
            (void)Wc_PowerOff();
            break;
        case WC_STATUS_PARENT:
            WcCreateParentParameter();
            if (!Wc_SetParentParameter())
            {
                wcTarget = WC_STATUS_READY;
                (void)Wc_Reset();
            }
            break;
        case WC_STATUS_CHILD:
            // Start connection as child
            if (!Wc_StartConnect())
            {
                wcTarget = WC_STATUS_READY;
                (void)Wc_Reset();
            }
            break;
        }
    }
    else
    {
        WcDebugPrintf("< WM_EndScan failed. ERRCODE: 0x%02x 0x%02x 0x%02x\n", cb->errcode,
                      cb->wlCmdID, cb->wlResult);
        wcTarget = WC_STATUS_READY;
        (void)Wc_Reset();
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcCb_StartConnect

  Description:  The callback function to WM_StartConnect.

  Arguments:    arg - The pointer to the callback structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcCb_StartConnect(void *arg)
{
    WMstartConnectCallback *cb = (WMstartConnectCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS)
    {
        switch (wcTarget)
        {
        case WC_STATUS_READY:
        case WC_STATUS_PARENT:
            WcDebugPrintf("< WM_StartConnect success.\n");
            // Reset once, since the target state isn't "child"
            // Do not call Wc_Reset if in the middle of the callback chain
            if (wcStatus != WC_STATUS_BUSY)
            {
                (void)Wc_Reset();
            }
            break;
        case WC_STATUS_CHILD:
            switch (cb->state)
            {
            case WM_STATECODE_CONNECT_START:
                // No processing, since it's waiting to become CONNECTED
                break;
            case WM_STATECODE_CONNECTED:
                // Save the allocated AID
                wcAid = cb->aid;
                // Do nothing if already child
                if (wcStatus == WC_STATUS_CHILD)
                {
                    WcDebugPrintf("- Connected , but already child mode.\n");
                }
                else
                {
                    // Start MP
                    WcDebugPrintf("< WM_StartConnect success.\n");
                    if (!Wc_StartMP())
                    {
                        wcTarget = WC_STATUS_READY;
                        (void)Wc_Reset();
                    }
                }
                break;
            case WM_STATECODE_BEACON_LOST:
                WcDebugPrintf("- Beacon from parent lost.\n");
                // Do not reset when there is only beacon loss
                break;
            case WM_STATECODE_DISCONNECTED:
                WcDebugPrintf("- DisConnected from parent.\n");
                wcTarget = WC_STATUS_READY;
                // Do not call Wc_Reset if in the middle of the callback chain
                if (wcStatus != WC_STATUS_BUSY)
                {
                    (void)Wc_Reset();
                }
                break;
            case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
                // Does nothing if the unit disconnects from itself
                break;
            default:
                WcDebugPrintf("< WM_StartConnect failed. Invalid state code. STATECODE: 0x%02x\n",
                              cb->state);
                wcStatus = WC_STATUS_BUSY;
                wcTarget = WC_STATUS_READY;
                (void)Wc_Reset();
            }
            break;
        }
    }
    else
    {
        WcDebugPrintf("< WM_StartConnect failed. ERRCODE: 0x%02x 0x%02x 0x%02x\n", cb->errcode,
                      cb->wlCmdID, cb->wlResult);
        if (cb->errcode == WM_ERRCODE_NO_ENTRY)
        {
            WcDebugPrintf(" ! Parent doesn't accept entry now.\n");
        }
        if (cb->errcode == WM_ERRCODE_OVER_MAX_ENTRY)
        {
            WcDebugPrintf(" ! Over max entry of BSS group.\n");
        }
        wcTarget = WC_STATUS_READY;
        (void)Wc_Reset();
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcCb_StartMP

  Description:  The callback function to WM_StartMP.

  Arguments:    arg - The pointer to the callback structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcCb_StartMP(void *arg)
{
    WMstartMPCallback *cb = (WMstartMPCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS)
    {
        BOOL mp_ready = FALSE;
        switch (cb->state)
        {
        case WM_STATECODE_MP_START:
            WcDebugPrintf("< WM_StartMP success.\n");
            switch (wcTarget)
            {
            case WC_STATUS_READY:
                (void)Wc_Reset();
                break;
            case WC_STATUS_PARENT:
            case WC_STATUS_CHILD:
                if (wcDataShareFlag)
                {
                    if (!Wc_StartDataSharing())
                    {
                        wcTarget = WC_STATUS_READY;
                        (void)Wc_Reset();
                    }
                    else
                    {
                        mp_ready = TRUE;
                    }
                }
                else
                {
                    mp_ready = TRUE;
                }
                if (mp_ready)
                {
                    wcSendFlag = TRUE;
                    wcStatus = wcTarget;        // The connection as the parent (child) has completed normally
                    if (wcBtCallback != NULL)
                        (*wcBtCallback) (arg);
                }
                break;
            }
            break;
        case WM_STATECODE_MPEND_IND:
            // Indication when sending/receiving as the parent device has completed
            break;
        case WM_STATECODE_MP_IND:
            // Indication when reception as the child device has completed
            break;
        }
    }
    else
    {
        switch (cb->errcode)
        {
        case WM_ERRCODE_SEND_FAILED:
        case WM_ERRCODE_TIMEOUT:
            // Indication when sending/receiving has not completed
            // Does nothing and waits for the resend
            break;
        case WM_ERRCODE_INVALID_POLLBITMAP:
            // Indication when MP communications not addressed itself were received
            break;
        default:
            WcDebugPrintf("< WM_StartMP failed. ERRCODE: 0x%02x 0x%02x\n", cb->errcode, cb->state);
            wcStatus = WC_STATUS_BUSY;
            wcTarget = WC_STATUS_READY;
            (void)Wc_Reset();
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcCb_SetMPData

  Description:  The callback function to WM_SetMPData.

  Arguments:    arg - The pointer to the callback structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcCb_SetMPData(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    wcSendFlag = TRUE;

    if (cb->errcode == WM_ERRCODE_SUCCESS)
    {
        WcDebugPrintf("< WM_SetMPData success.\n");
    }
    else
    {
        WMPortSendCallback *cb_PortSend = (WMPortSendCallback *)cb;
        WcDebugPrintf("< WM_SetMPData failed. ERRCODE: 0x%02x 0x%02x 0x%02x restBitmap: 0x%02x\n",
                      cb->errcode, cb->wlCmdID, cb->wlResult, cb_PortSend->restBitmap);
    }
    if (wcBtCallback != NULL)
        (*wcBtCallback) (arg);
}

/*---------------------------------------------------------------------------*
  Name:         WcCb_ReceiveData

  Description:  The reception notification to the port

  Arguments:    arg - The pointer to the callback structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcCb_ReceiveData(void *arg)
{
    WMPortRecvCallback *cb = (WMPortRecvCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS)
    {
        WcDebugPrintf("< Receive Data success.\n");
        switch (cb->state)
        {
        case WM_STATECODE_PORT_RECV:
            // Data reception
            switch (wcStatus)
            {
            case WC_STATUS_PARENT:
                // Notify of reception to main
                if (wcMpRecvCallback != NULL)
                {
                    wcMpRecvCallback(cb->aid, cb->data, cb->length);
                }
                break;
            case WC_STATUS_CHILD:
                // Notify of reception to main
                if (wcMpRecvCallback != NULL)
                {
                    wcMpRecvCallback(0, cb->data, cb->length);
                }
                break;
            }
            break;
        case WM_STATECODE_CONNECTED:
            // Connection notification
            break;
        case WM_STATECODE_DISCONNECTED:
            // Disconnect notification
            switch (wcStatus)
            {
            case WC_STATUS_PARENT:
                // Notify of disconnect to main
                if (wcMpRecvCallback != NULL)
                {
                    wcMpRecvCallback(cb->aid, NULL, 0);
                }
                break;
            case WC_STATUS_CHILD:
                // Notify of disconnect to main
                if (wcMpRecvCallback != NULL)
                {
                    wcMpRecvCallback(0, NULL, 0);
                }
                break;
            }
            break;
        case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
            // Does nothing if the unit disconnects from itself
            break;
        }
    }
    else
    {
        WcDebugPrintf("< Receive Data failed. ERRCODE: 0x%02x\n", cb->errcode);
    }
}

/*---------------------------------------------------------------------------*
  Name:         Wc_Enable

  Description:  Calls the WM_Enable function.

  Arguments:    None.

  Returns:      BOOL - When asynchronous processing was able to be started normally by WM_Enable,
                       returns TRUE. Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static BOOL Wc_Enable(void)
{
    WMErrCode wmResult;

    WcDebugPrintf("> WM_Enable start.\n");
    wmResult = WM_Enable(WcCb_Enable);
    if (wmResult != WM_ERRCODE_OPERATING)
    {
        WcDebugPrintf("< WM_Enable failed. ERRCODE: 0x%02x\n", wmResult);
        wcStatus = WC_STATUS_ERROR;
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         Wc_Disable

  Description:  Calls the WM_Disable function.

  Arguments:    None.

  Returns:      BOOL - When asynchronous processing was able to be started normally by WM_Disable,
                       returns TRUE. Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static BOOL Wc_Disable(void)
{
    WMErrCode wmResult;

    WcDebugPrintf("> WM_Disable start.\n");
    wmResult = WM_Disable(WcCb_Disable);
    if (wmResult != WM_ERRCODE_OPERATING)
    {
        WcDebugPrintf("< WM_Disable failed. ERRCODE: 0x%02x\n", wmResult);
        wcStatus = WC_STATUS_ERROR;
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         Wc_PowerOn

  Description:  Calls the WM_PowerOn function.

  Arguments:    None.

  Returns:      BOOL - When asynchronous processing was able to be started normally by WM_PowerOn,
                       returns TRUE. Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static BOOL Wc_PowerOn(void)
{
    WMErrCode wmResult;

    WcDebugPrintf("> WM_PowerOn start.\n");
    wmResult = WM_PowerOn(WcCb_PowerOn);
    if (wmResult != WM_ERRCODE_OPERATING)
    {
        WcDebugPrintf("< WM_PowerOn failed. ERRCODE: 0x%02x\n", wmResult);
        wcStatus = WC_STATUS_ERROR;
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         Wc_PowerOff

  Description:  Calls the WM_PowerOff function.

  Arguments:    None.

  Returns:      BOOL - When asynchronous processing was able to be started normally by WM_PowerOff,
                       returns TRUE. Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static BOOL Wc_PowerOff(void)
{
    WMErrCode wmResult;

    WcDebugPrintf("> WM_PowerOff start.\n");
    wmResult = WM_PowerOff(WcCb_PowerOff);
    if (wmResult != WM_ERRCODE_OPERATING)
    {
        WcDebugPrintf("< WM_PowerOff failed. ERRCODE: 0x%02x\n", wmResult);
        wcStatus = WC_STATUS_ERROR;
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         Wc_SetIndCallback

  Description:  Calls the WM_SetIndCallback function.

  Arguments:    None.

  Returns:      BOOL - When the WM_SetIndCallback function succeeds, returns TRUE.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static BOOL Wc_SetIndCallback(void)
{
    WMErrCode wmResult;

    WcDebugPrintf("> WM_SetIndCallback start.\n");
    wmResult = WM_SetIndCallback(WcCb_Indicate);
    if (wmResult != WM_ERRCODE_SUCCESS)
    {
        WcDebugPrintf("< WM_SetIndCallback failed. ERRCODE: 0x%02x\n", wmResult);
        wcStatus = WC_STATUS_ERROR;
        return FALSE;
    }
    WcDebugPrintf("< WM_SetIndCallback success.\n");
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         Wc_Reset

  Description:  Calls the WM_Reset function.

  Arguments:    None.

  Returns:      BOOL - When asynchronous processing was able to be started normally by WM_Reset,
                       returns TRUE. Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static BOOL Wc_Reset(void)
{
    WMErrCode wmResult;
    
    if (wcStartDataShareFlag)
    {
        wmResult = WM_EndDataSharing(wcDataSharingInfo);
        if (wmResult != WM_ERRCODE_SUCCESS)
        {
            WcDebugPrintf("< WM_EndDataSharing failed. ERRCODE: 0x%02x\n", wmResult);
            return FALSE;
        }
        wcStartDataShareFlag = FALSE;
    }
    
    if ((wcStatus != WC_STATUS_BUSY) && (wcStatus != WC_STATUS_ERROR))
    {
        wcStatus = WC_STATUS_BUSY;
    }
    WcDebugPrintf("> WM_Reset start.\n");
    wmResult = WM_Reset(WcCb_Reset);
    if (wmResult != WM_ERRCODE_OPERATING)
    {
        WcDebugPrintf("< WM_Reset failed. ERRCODE: 0x%02x\n", wmResult);
        wcStatus = WC_STATUS_ERROR;
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         Wc_SetParentParameter

  Description:  Calls the WM_SetParentParameter function.

  Arguments:    None.

  Returns:      BOOL - When asynchronous processing was able to be started normally by WM_SetParentParameter,
                       returns TRUE. Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static BOOL Wc_SetParentParameter(void)
{
    WMErrCode wmResult;

    WcDebugPrintf("> WM_SetParentParameter start.\n");
    wmResult = WM_SetParentParameter(WcCb_SetParentParameter, wcParentParameter);
    if (wmResult != WM_ERRCODE_OPERATING)
    {
        WcDebugPrintf("< WM_SetParentParameter failed. ERRCODE: 0x%02x\n", wmResult);
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         Wc_StartParent

  Description:  Calls the WM_StartParent function.

  Arguments:    None.

  Returns:      BOOL - When asynchronous processing was able to be started normally by WM_StartParent,
                       returns TRUE. Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static BOOL Wc_StartParent(void)
{
    WMErrCode wmResult;

    WcDebugPrintf("> WM_StartParent start.\n");
    wmResult = WM_StartParent(WcCb_StartParent);
    if (wmResult != WM_ERRCODE_OPERATING)
    {
        WcDebugPrintf("< WM_StartParent failed. ERRCODE: 0x%02x\n", wmResult);
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         Wc_StartScan

  Description:  Calls the WM_StartScan function.

  Arguments:    None.

  Returns:      BOOL - When asynchronous processing was able to be started normally by WM_StartScan,
                       returns TRUE. Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static BOOL Wc_StartScan(void)
{
    WMErrCode wmResult;

    WcDebugPrintf("> WM_StartScan start.\n");
    wmResult = WM_StartScan(WcCb_StartScan, wcScanParameter);
    if (wmResult != WM_ERRCODE_OPERATING)
    {
        WcDebugPrintf("< WM_StartScan failed. ERRCODE: 0x%02x\n", wmResult);
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         Wc_EndScan

  Description:  Calls the WM_EndScan function.

  Arguments:    None.

  Returns:      BOOL - When asynchronous processing was able to be started normally by WM_EndScan,
                       returns TRUE. Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static BOOL Wc_EndScan(void)
{
    WMErrCode wmResult;

    WcDebugPrintf("> WM_EndScan start.\n");
    wmResult = WM_EndScan(WcCb_EndScan);
    if (wmResult != WM_ERRCODE_OPERATING)
    {
        WcDebugPrintf("< WM_EndScan failed. ERRCODE: 0x%02x\n", wmResult);
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         Wc_StartConnect

  Description:  Calls the WM_StartConnect function.

  Arguments:    None.

  Returns:      BOOL - When asynchronous processing was able to be started normally by WM_StartConnect,
                       returns TRUE. Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static BOOL Wc_StartConnect(void)
{
    WMErrCode wmResult;

    WcDebugPrintf("> WM_StartConnect start.\n");
    wmResult = WM_StartConnect(WcCb_StartConnect, wcWmBssDesc, NULL);
    if (wmResult != WM_ERRCODE_OPERATING)
    {
        WcDebugPrintf("< WM_StartConnect failed. ERRCODE: 0x%02x\n", wmResult);
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         Wc_StartMP

  Description:  Calls the WM_StartMP function.

  Arguments:    None.

  Returns:      BOOL - When asynchronous processing was able to be started normally by WM_StartMP,
                       returns TRUE. Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static BOOL Wc_StartMP(void)
{
    WMErrCode wmResult;

    if (!wcDataShareFlag)
    {
        WcDebugPrintf("> WM_SetPortCallback start.\n");
        wmResult = WM_SetPortCallback(WC_DEFAULT_PORT, WcCb_ReceiveData, NULL);
        if (wmResult == WM_ERRCODE_SUCCESS)
        {
            WcDebugPrintf("< WM_SetPortCallback success.\n");
        }
        else
        {
            WcDebugPrintf("< WM_SetPortCallback failed. ERRCODE: 0x%02x\n", wmResult);
            return FALSE;
        }
    }

    WcDebugPrintf("> WM_StartMP start.\n");
    wmResult = WM_StartMP(WcCb_StartMP,
                          wcRecvBuffer,
                          (u16)wcRecvBufferSize,
                          wcSendBuffer,
                          (u16)wcSendBufferSize, (u16)(wcParentParameter->CS_Flag ? 0 : 1));
    if (wmResult != WM_ERRCODE_OPERATING)
    {
        WcDebugPrintf("< WM_StartMP failed. ERRCODE: 0x%02x\n", wmResult);
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         Wc_StartDataSharing

  Description:  Calls the WM_StartDataSharing function.

  Arguments:    None.

  Returns:      BOOL - If WM_StartDataSharing completed normally, returns TRUE.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static BOOL Wc_StartDataSharing(void)
{
    WMErrCode wmResult;
    u16     aidBitmap;

    aidBitmap = (u16)((1 << (wcDefaultParameter->maxEntry + 1)) - 1);   // Bitmap with the lower WC_NUM_MAX_CHILD+1 bit set to 1
    WcDebugPrintf("> WM_StartDataSharing start.\n");
    wmResult = WM_StartDataSharing(wcDataSharingInfo, WC_DS_PORT_NUMBER,        // Port
                                   aidBitmap,   // Bitmap of the terminal to connect
                                   wcDefaultParameter->childMaxSize,    // Size of the shared data for one terminal
                                   (wcFramePeriod > 1 ? FALSE : TRUE));
    if (wmResult != WM_ERRCODE_SUCCESS)
    {
        WcDebugPrintf("< WM_StartDataSharing failed. ERRCODE: 0x%02x\n", wmResult);
        return FALSE;
    }
    wcStartDataShareFlag = TRUE;
    WcDebugPrintf("< WM_StartDataSharing success.\n");
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         Wc_SetMPData

  Description:  Calls the WM_SetMPData function.

  Arguments:    None.

  Returns:      BOOL - When asynchronous processing was able to be started normally by WM_SetMPData,
                       returns TRUE. Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static BOOL Wc_SetMPData(const void *buf, u16 size)
{
    WMErrCode wmResult;

    WcDebugPrintf("> WM_SetMPData start. data:0x%08x\n", *(u32 *)buf);
    wmResult = WM_SetMPDataToPort(WcCb_SetMPData,
                                  buf, size, 0xffff, WC_DEFAULT_PORT, WC_DEFAULT_PORT_PRIO);
    if (wmResult != WM_ERRCODE_OPERATING)
    {
        wcSendFlag = TRUE;
        WcDebugPrintf("< WM_SetMPData failed. ERRCODE: 0x%02x\n", wmResult);
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
