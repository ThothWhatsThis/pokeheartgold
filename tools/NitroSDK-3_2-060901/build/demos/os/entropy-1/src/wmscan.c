/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CHT - demos - wmscan
  File:     chtmin.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wmscan.c,v $
  Revision 1.2  01/18/2006 02:12:40  kitase_hirotake
  do-indent

  Revision 1.1  07/27/2005 07:43:35  seiki_masashi
  new additions

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include    <nitro.h>
#include    <nitro/cht.h>
#include    "wmscan.h"


static int sSysState = WS_SYSSTATE_STOP;        // wmscan internal state
static WSStartScanCallbackFunc sScanCallback = NULL;    // Callback pointer when parent is discovered
static WMBssDesc sBssDesc ATTRIBUTE_ALIGN(32);  // Buffer to obtain parent data
static WMScanParam sScanParam ATTRIBUTE_ALIGN(32);      // Scan parameter structure
static BOOL sContinuousScanFlag;       // Continuous scan flag
static BOOL sPictoCatchFlag = FALSE;   // PictoCatch flag
static u32 sGgid = 0;                  // GGID

#define WS_ASSERT(exp) \
    (void) ((exp) || (OSi_Panic(__FILE__, __LINE__, "Failed assertion " #exp), 0))

static void WS_ChangeSysState(int state);
static void WS_StateOutInitialize(void *arg);
static BOOL WS_StateInStartScan(void);
static void WS_StateOutStartScan(void *arg);
static BOOL WS_StateInEndScan(void);
static void WS_StateOutEndScan(void *arg);
static void WS_StateOutEnd(void *arg);


/*---------------------------------------------------------------------------*
  Name:         WS_ChangeSysState
  Description:  Changes the WS status
  Arguments:    None.
  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void WS_ChangeSysState(int state)
{
    sSysState = state;
}

/*---------------------------------------------------------------------------*
  Name:         WS_GetSystemState

  Description:  Obtains the WS state.

  Arguments:    None.

  Returns:      The WS state.
 *---------------------------------------------------------------------------*/
int WS_GetSystemState(void)
{
    return sSysState;
}

/*---------------------------------------------------------------------------*
  Name:         WS_Initialize

  Description:  Initializes wireless.

  Arguments:    buf   Buffer size passed to WM. Size of WM_SYSTEM_BUF_SIZE required.
                dmaNo The DMA number used by wireless

  Returns:      If the process starts successfully, TRUE
                If it does not start, FALSE
 *---------------------------------------------------------------------------*/
BOOL WS_Initialize(void *buf, u16 dmaNo)
{
    WMErrCode result;

    SDK_NULL_ASSERT(buf);

    WS_ChangeSysState(WS_SYSSTATE_BUSY);
    result = WM_Initialize(buf, WS_StateOutInitialize, dmaNo);
    if (result != WM_ERRCODE_OPERATING)
    {
        WS_ChangeSysState(WS_SYSSTATE_FATAL);
        return FALSE;
    }
    sScanParam.channel = 0;
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         WS_StateOutInitialize
  Description:  Initializes wireless.
  Arguments:    None.
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WS_StateOutInitialize(void *arg)
{
    // State after power-on.
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WS_ChangeSysState(WS_SYSSTATE_FATAL);
        return;
    }

    // Changes the system status to idling (waiting).
    WS_ChangeSysState(WS_SYSSTATE_IDLE);

    // Does not set the next status, so the sequence ends here.
}

/*---------------------------------------------------------------------------*
  Name:         WS_TurnOnPictoCatch

  Description:  Enables the PictoCatch feature.
                With WS_StartScan, even if Pictochat is found during a scan,
                the callback function is called.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WS_TurnOnPictoCatch(void)
{
    sPictoCatchFlag = TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         WS_TurnOffPictoCatch

  Description:  Disables the PictoCatch feature.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WS_TurnOffPictoCatch(void)
{
    sPictoCatchFlag = FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         WS_SetGgid

  Description:  Configures the game group ID.
                Call before making a connection to the parent device.

  Arguments:    ggid    The game group ID to configure.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WS_SetGgid(u32 ggid)
{
    sGgid = ggid;
}


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
BOOL WS_StartScan(WSStartScanCallbackFunc callback, const u8 *macAddr, BOOL continuous)
{
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();

    sScanCallback = callback;
    sContinuousScanFlag = continuous;

    // Sets the conditions for the MAC address to be searched for
    *(u16 *)(&sScanParam.bssid[4]) = *(u16 *)(macAddr + 4);
    *(u16 *)(&sScanParam.bssid[2]) = *(u16 *)(macAddr + 2);
    *(u16 *)(&sScanParam.bssid[0]) = *(u16 *)(macAddr);

    (void)OS_RestoreInterrupts(enabled);

    if (sSysState == WS_SYSSTATE_SCANNING)
    {
        return TRUE;
    }

    WS_ChangeSysState(WS_SYSSTATE_SCANNING);

    if (!WS_StateInStartScan())
    {
        WS_ChangeSysState(WS_SYSSTATE_ERROR);
        return FALSE;
    }

    return TRUE;
}

/* ----------------------------------------------------------------------
  state : StartScan
  ---------------------------------------------------------------------- */
static BOOL WS_StateInStartScan(void)
{
    // When in this state, looks for a parent.
    WMErrCode result;
    u16     chanpat;

    WS_ASSERT(sSysState == WS_SYSSTATE_SCANNING);

    chanpat = WM_GetAllowedChannel();

    // Checks whether wireless can be used
    if (chanpat == 0x8000)
    {
        // If 0x8000 is returned, wireless is not initialized, or there is some other
        // wireless library exception. Sets to error.
        return FALSE;
    }
    if (chanpat == 0)
    {
        // In this state, wireless cannot be used.
        return FALSE;
    }

    /* Search possible channels in ascending order from the current designation*/
    while (TRUE)
    {
        sScanParam.channel++;
        if (sScanParam.channel > 16)
        {
            sScanParam.channel = 1;
        }

        if (chanpat & (0x0001 << (sScanParam.channel - 1)))
        {
            break;
        }
    }

    sScanParam.maxChannelTime = WM_GetDispersionScanPeriod();
    sScanParam.scanBuf = &sBssDesc;
    result = WM_StartScan(WS_StateOutStartScan, &sScanParam);

    if (result != WM_ERRCODE_OPERATING)
    {
        return FALSE;
    }
    return TRUE;
}

static void WS_StateOutStartScan(void *arg)
{
    WMstartScanCallback *cb = (WMstartScanCallback *)arg;

    // If the scan command fails
    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WS_ChangeSysState(WS_SYSSTATE_ERROR);
        return;
    }

    if (sSysState != WS_SYSSTATE_SCANNING)
    {
        // End scan if the state has been changed
        if (!WS_StateInEndScan())
        {
            WS_ChangeSysState(WS_SYSSTATE_ERROR);
        }
        return;
    }

    switch (cb->state)
    {
    case WM_STATECODE_SCAN_START:
        return;

    case WM_STATECODE_PARENT_NOT_FOUND:
        break;

    case WM_STATECODE_PARENT_FOUND:
        // If a parent is found
        // compares ggid and fails if different.
        // First, check WMBssDesc.gameInfoLength,
        // and compare if ggid is a valid value.

        // Because the BssDesc information is written from ARM7
        // discards the BssDesc cache that is set in the buffer
        DC_InvalidateRange(&sBssDesc, sizeof(WMbssDesc));

        // Determines if the parent is using PictoChat
        if (sPictoCatchFlag)
        {
            if (CHT_IsPictochatParent(&sBssDesc))
            {
                // If the parent is using PictoChat
                if (sScanCallback != NULL)
                {
                    sScanCallback(&sBssDesc);
                }
                break;
            }
        }

        if (cb->gameInfoLength < 8 || cb->gameInfo.ggid != sGgid)
        {
            // If GGID is different, this is ignored
            break;
        }

        // If the entry flag is not up, the child is not receiving, so this is ignored
        if (!(cb->gameInfo.gameNameCount_attribute & WM_ATTR_FLAG_ENTRY))
        {
            break;
        }

        // call if callback is necessary
        if (sScanCallback != NULL)
        {
            sScanCallback(&sBssDesc);
        }

        break;
    }

    if (!sContinuousScanFlag)
    {
        WS_ChangeSysState(WS_SYSSTATE_SCANIDLE);
        return;
    }

    // Changes the channel and starts another scan.
    if (!WS_StateInStartScan())
    {
        WS_ChangeSysState(WS_SYSSTATE_ERROR);
    }
}


/*---------------------------------------------------------------------------*
  Name:         WS_EndScan

  Description:  Function that terminates scanning

  Arguments:    None.

  Returns:      If the process starts successfully, TRUE
                If it does not start, FALSE
 *---------------------------------------------------------------------------*/
BOOL WS_EndScan(void)
{
    if (sSysState == WS_SYSSTATE_SCANIDLE)
    {
        return WS_StateInEndScan();
    }

    if (sSysState != WS_SYSSTATE_SCANNING)
    {
        return FALSE;
    }

    {
        OSIntrMode enabled = OS_DisableInterrupts();
        sScanCallback = NULL;
        (void)OS_RestoreInterrupts(enabled);
    }

    WS_ChangeSysState(WS_SYSSTATE_BUSY);
    return TRUE;
}


static BOOL WS_StateInEndScan(void)
{
    WMErrCode result;

    // In this state, scan end processing is carried out.
    result = WM_EndScan(WS_StateOutEndScan);
    if (result != WM_ERRCODE_OPERATING)
    {
        return FALSE;
    }

    return TRUE;
}

static void WS_StateOutEndScan(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        return;
    }

    WS_ChangeSysState(WS_SYSSTATE_IDLE);
}


/*---------------------------------------------------------------------------*
  Name:         WS_End

  Description:  Ends wireless communications.

  Arguments:    None.

  Returns:      If it succeeds returns TRUE.
 *---------------------------------------------------------------------------*/
BOOL WS_End(void)
{
    WS_ASSERT(sSysState == WS_SYSSTATE_IDLE);

    WS_ChangeSysState(WS_SYSSTATE_BUSY);
    if (WM_End(WS_StateOutEnd) != WM_ERRCODE_OPERATING)
    {
        WS_ChangeSysState(WS_SYSSTATE_ERROR);

        return FALSE;
    }
    return TRUE;
}

/* ----------------------------------------------------------------------
  state : WS_StateOutEnd
  ---------------------------------------------------------------------- */
static void WS_StateOutEnd(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;
    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        WS_ChangeSysState(WS_SYSSTATE_FATAL);
        return;
    }
    WS_ChangeSysState(WS_SYSSTATE_STOP);
}
