/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wm_lib.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm_lib.c,v $
  Revision 1.73  03/07/2006 02:02:20  ooe
  Revised processing inside StartScanEx callback

  Revision 1.72  2006/02/20 09:03:48  seiki_masashi
  Changed code to use WM_IsValidGameInfo function

  Revision 1.71  2006/02/20 03:39:35  seiki_masashi
  Removed WMGameInfo.version and added magicNumber and ver 
  Removed support for WM_GAMEINFO_TYPE_OLD 

  Revision 1.70  2006/01/18 02:12:39  kitase_hirotake
  do-indent

  Revision 1.69  11/01/2005 01:34:38  seiki_masashi
  Supported addition of WM_STATECODE_DISCONNECTED_FROM_MYSELF.

  Revision 1.68  06/07/2005 06:45:56  seiki_masashi
  Revised to reduce special processing related to key sharing

  Revision 1.67  06/02/2005 10:05:28  terui
  Deleted section that used WM_SetBeaconPeriod

  Revision 1.66  03/17/2005 08:16:52  ooe
  Revisions of ScanEx were applied to DCF mode.

  Revision 1.65  03/16/2005 09:46:27  ooe
  Dealt with the correction that changed APIID notified with WM_StartScanEx to SCAN_EX.

  Revision 1.64  03/02/2005 09:28:42  ooe
  addition of debug output, etc.

  Revision 1.63  03/01/2005 11:37:36  terui
  fixed comments

  Revision 1.62  02/28/2005 06:26:27  yosizaki
  do-indent.

  Revision 1.61  02/15/2005 01:09:42  ooe
  WMi_StartScanEx -> WM_StartScanEx
  Made revisions so that the ignoreFatalError flag works at the same time as the contSend flag.

  Revision 1.60  02/14/2005 00:54:05  terui
  WMi_StartScanEx -> WM_StartScanEx

  Revision 1.59  02/08/2005 07:53:38  ooe
  Large-scale revisions along with the change to the WM_StartScanEx specification

  Revision 1.58  01/18/2005 10:54:31  yosizaki
  Revised copyright year notation

  Revision 1.57  01/17/2005 02:09:32  ooe
  Added support for WM_StartConnectEx

  Revision 1.56  11/09/2004 03:51:00  seiki_masashi
  Implemented timeout on NULL response

  Revision 1.55  10/29/2004 05:38:02  terui
  Deal with WM_StartScanEx functions being temporarily non-public.

  Revision 1.54  10/26/2004 09:27:08  ooe
  Support WM_StartScanEx

  Revision 1.53  10/18/2004 05:17:46  terui
  Added a specification to randomize the beacon interval and the scan interval.

  Revision 1.52  10/05/2004 10:45:28  terui
  Standardized MAC addresses as type u8 arrays.

  Revision 1.51  10/04/2004 14:40:25  terui
  Standardized game group IDs as type u32.

  Revision 1.50  10/03/2004 09:15:26  ooe
  small fix

  Revision 1.49  10/03/2004 09:11:50  ooe
  Support for changed specifications of StartMPEx.

  Revision 1.48  09/20/2004 11:20:41  ooe
  Revised endReq determination for WM_lib_End
  Supported addition of WM_SetBeaconPeriod

  Revision 1.47  09/18/2004 11:05:52  sato_masaki
  Added measures for when the WM_lib_End() process is called twice

  Revision 1.46  09/15/2004 06:42:48  ooe
  Revised the tgid determination section in accordance with changes in beaconRecv.Ind callback arguments

  Revision 1.45  09/14/2004 10:51:15  ooe
  Added mpBusy flag

  Revision 1.44  09/13/2004 10:25:06  ooe
  Put back LIFE_TIME

  Revision 1.43  09/03/2004 11:13:15  ooe
  Changed the seek channels to 1, 7, & 13 during Scan

  Revision 1.42  09/01/2004 10:15:42  ooe
  Added processing for WM_ERRCODE_INVALID_POLLBITMAP

  Revision 1.41  08/31/2004 10:13:18  ooe
  Changed WM_StartMP to Ex, supported a no-resend mode

  Revision 1.40  08/30/2004 02:57:30  seiki_masashi
  Changed WM_STATECODE_CHILD_CONNECTED to WM_STATECODE_CONNECTED

  Revision 1.39  08/26/2004 10:49:06  ooe
  Added a state check when BeaconRecv.

  Revision 1.38  08/23/2004 05:40:35  ooe
  Supported changes to WM_StartConnect() specifications.

  Revision 1.37  08/19/2004 16:27:01  miya
  modified SetGameInfo function.

  Revision 1.36  08/19/2004 12:17:41  miya
  add GAMEINFO_VERSION

  Revision 1.35  08/19/2004 03:02:38  miya
  modified WMGameInfo format

  Revision 1.34  08/18/2004 05:22:25  ooe
  Changed CHK_ERRCODE to check_WMAPI_errcode().

  Revision 1.33  08/18/2004 01:26:25  seiki_masashi
  small fix

  Revision 1.32  08/17/2004 14:13:27  seiki_masashi
  Tentative support for changes to WM_StartMP specifications.

  Revision 1.31  08/16/2004 11:10:46  ooe
  Added WM_lib_CalcRSSI.

  Revision 1.30  08/16/2004 10:39:56  ooe
  Added TGID check process during BeaconRecvInd.

  Revision 1.29  08/12/2004 05:02:28  ooe
  Commented out scan time random process.

  Revision 1.28  08/11/2004 09:35:19  ooe
  Incorporated randomization test during scan

  Revision 1.27  08/10/2004 06:47:08  ooe
  Added WM_lib_set_channel() (for scan test)

  Revision 1.26  08/07/2004 10:52:16  ooe
  Accommodated major MP-related changes

  Revision 1.25  08/05/2004 05:05:36  miya
  add functions

  Revision 1.24  08/05/2004 02:20:56  ikedae
  Made changes near mpSending

  Revision 1.23  08/04/2004 10:30:37  ikedae
  Changed so that SEND_ENABLE is not issued when the child received bitmap 0

  Revision 1.22  08/02/2004 06:37:24  ooe
  Support for MPACK.Ind

  Revision 1.21  07/31/2004 09:46:17  yosiokat
  Fixed problem that caused return to default settings with WM_APIID_SET_LIFETIME callback when WM_lib_set_max_scan_time was called before wm_lib_start

  Revision 1.20  07/31/2004 05:40:09  yosiokat
  Added WM_lib_get_max_scan_time

  Revision 1.19  07/30/2004 10:22:42  miya
  Changed scan time

  Revision 1.18  07/30/2004 04:59:14  miya
  For DCF parallel

  Revision 1.17  07/29/2004 13:27:11  miya
  Specified lifetime

  Revision 1.16  07/29/2004 08:41:02  miya
  modified callback table size

  Revision 1.15  07/29/2004 02:49:49  miya
  Stopped automatic beacon scan control

  Revision 1.14  07/29/2004 01:45:12  miya
  Changed to allow channel changes to be stopped during scan

  Revision 1.13  07/29/2004 01:15:13  miya
  Support for beacon scan specifying bssid (MAC address)

  Revision 1.12  07/28/2004 13:03:54  ikedae
  Made changes near mpSending

  Revision 1.11  07/28/2004 12:00:30  ooe
  Fixed DCF mode scan

  Revision 1.10  07/28/2004 10:03:48  miya
  small fix

  Revision 1.9  07/28/2004 05:28:53  miya
  add functions about ggid & tgid

  Revision 1.8  07/28/2004 04:53:28  ikedae
  Moved WM_DMA_NO to header

  Revision 1.7  07/27/2004 12:54:22  miya
  Added bssid Settings

  Revision 1.6  07/27/2004 12:31:37  miya
  Added event

  Revision 1.5  07/27/2004 10:00:59  ooe
  Added WM_APIID_INDICATION processing (do nothing)

  Revision 1.4  07/27/2004 07:41:51  miya
  hook IndicationCallback and error event

  Revision 1.3  07/27/2004 03:52:02  ooe
  Added ScanChannel change function

  Revision 1.2  07/27/2004 03:39:56  ikedae
  Added WM_lib_SetMPDataEX supporting pollbitmap

  Revision 1.1  07/23/2004 16:29:17  terui
  Revisions and additions.

  Revision 1.4  07/23/2004 15:41:47  terui
  DMA number 3 conflicted with GX, so changed to 2
  Made changes to include headers

  Revision 1.3  07/22/2004 09:30:48  ikedae
  Changed scanParam to 32-byte alignment

  Revision 1.2  07/20/2004 13:46:15  terui
  Added include header

  Revision 1.1  07/20/2004 12:26:09  terui
  Restructured due to removal of wm_lib

  Revision 1.10  07/20/2004 02:16:49  terui
  Supported WM_SetGameInfo specification changes

  Revision 1.9  07/18/2004 12:29:44  seiki_masashi
  add WM_Initialize wait loop

  Revision 1.8  07/15/2004 02:09:33  ooe
  Revised StartKeySharing() callback specification of child

  Revision 1.7  07/15/2004 01:16:20  ooe
  Created continuous scan flag

  Revision 1.6  07/14/2004 10:41:47  ooe
  Tentative revision for StartScan argument change

  Revision 1.5  2004/07/11 04:03:21  ooe
  Additions and other changes related WM_SetBeaconIndication

  Revision 1.4  07/08/2004 11:07:46  terui
  Changes due to start of WM7 restructuring

  Revision 1.18  07/05/2004 07:03:27  ikedae
  Added WM_TYPE_PARENT_STARTED

  Revision 1.17  07/03/2004 08:31:17  Ooe
  Added WM_lib_GetKeySet()

  Revision 1.16  07/02/2004 05:51:37  Miya
  Revised cache control size

  Revision 1.15  07/01/2004 04:38:28  Miya
  Integrated wm_tool.h wm_lib.h

  Revision 1.14  2004/06/29 07:45:55  Miya
  Made the hard coding of data size to macros as much as possible

  Revision 1.13  06/29/2004 07:20:48  Ooe
  Added send enable callback (WM_TYPE_MP_SEND_ENABLE)

  Revision 1.12  2004/06/29 07:08:37  Miya
  Deleted all unnecessary parts of WMCallback *buf

  Revision 1.11  06/25/2004 07:18:10  Ooe
  Added argument (pollbmp) to WM_SetMPData()

  Revision 1.10  06/24/2004 12:21:59  Ooe
  Added WM_SetGameInfo() error handling

  Revision 1.9  06/24/2004 11:04:10  Ooe
  Added WM_SetGameInfo()

  Revision 1.8  06/23/2004 07:34:31  ikedae
  Changed so that the function WM_lib returns WM_ERRCODE_SUCCESS when all processing successful

  Revision 1.7  06/22/2004 07:22:15  Ooe
  Added the child information update process by disconnect processing from parent.

  Revision 1.6  06/22/2004 06:19:17  ikedae
  Changes related to WM_lib_Main deletion

  Revision 1.5  2004/06/21 07:40:40  Ooe
  (none)

  Revision 1.4  06/19/2004 06:56:29  Miya
  modified header file reference

  Revision 1.3  06/19/2004 05:05:10  miya
  ver. up Marionea-1.48.00

  Revision 1.2  06/02/2004 03:29:40  miya
  ver. up

  Revision 1.1  05/26/2004 09:55:07  terui
  Additions accompanying incorporation of wireless communication

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

// 2005/05/26 update by terui@nintendo
#include <nitro/wm.h>
#include "wm_lib.h"
#include <stdlib.h>
#include <string.h>

void    mprintf(const char *fmt, ...);

#undef WMLIB_DEBUG                     // debug output

#define CAM_LIFE_TIME       40
#define FRAME_LIFE_TIME     5
#define TABLE_NO          0xffff

#undef  NO_LIFETIME

#define STARTCONNECT_EX_TEST	1      // StartConnectEx test

static BOOL WM_lib_auto_flag = FALSE;
static BOOL WM_lib_mp_dcf_parallel = FALSE;


static WM_lib_param *param = NULL;     // Pointer to parameter structure

static void WM_lib_mp_parent_callback(void *arg);       // MP parent mode callback
static void WM_lib_mp_child_callback(void *arg);        // MP child mode callback
static void WM_lib_dcf_child_callback(void *arg);       // DCF child mode callback
static void WM_lib_dummy(void *arg);   // Dummy callback
static void changeScanChannel(WMscanParam *param);      // scan channel change function

static void (*callback_ptr) (void *arg);

static const u8 test_ssid[WM_SIZE_CHILD_SSID] = "TEST SSID\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";      // 24Byte

#if 0
#define CHK_ERRCODE(x,y)    if ( x != WM_ERRCODE_OPERATING &&           \
                                    x != WM_ERRCODE_SUCCESS)            \
                            {                                           \
                                param->callback( WM_TYPE_ERROR, y); \
                            }
#else
static void check_WMAPI_errcode(u16 apiid, int errcode);
#endif

static inline int conv_errcode(int errcode)
{
    return (errcode == WM_ERRCODE_OPERATING) ? WM_ERRCODE_SUCCESS : errcode;
}

static WMscanParam scanParam ATTRIBUTE_ALIGN(32);
static WMscanExParam scanExParam ATTRIBUTE_ALIGN(32);
static u16 scanning_flag = 0;

static BOOL WM_lib_change_scan_channel_flag = TRUE;

static u16 maxChannelTime_org;

//--------------------------------------------------------
// WM automatic parent child registration feature flag ON
void WM_lib_set_auto(BOOL flag)
{
    WM_lib_auto_flag = flag;
}
void WM_lib_set_mp_dcf_parallel(BOOL flag)
{
    WM_lib_mp_dcf_parallel = flag;
}

// Configure MaxScanTime
void WM_lib_set_max_scan_time(u16 time)
{
    scanParam.maxChannelTime = time;
    maxChannelTime_org = time;
}

// Get MaxScanTime
u16 WM_lib_get_max_scan_time(void)
{
    return scanParam.maxChannelTime;
}

// Configure ScanChannel
void WM_lib_set_channel(u16 channel)
{
    if (channel == 0)
    {
        WM_lib_set_scan_channel_seek(TRUE);
        scanParam.channel = 1;
    }
    else
    {
        WM_lib_set_scan_channel_seek(FALSE);
        scanParam.channel = channel;
    }
}

// MacAddress configuration used in MacAddress (BSSID) filtering during Scan
void WM_lib_set_scanBssid(u8 *bssid)
{
    MI_CpuCopy8(bssid, scanParam.bssid, WM_SIZE_MACADDR);
}

//-------------------------------------------------------------
// parameter configuration function for StartScanEx

// configure maxChannelTime for ScanEx
void WM_lib_set_scanEx_maxChannelTime(u16 time)
{
    scanExParam.maxChannelTime = time;
    maxChannelTime_org = time;
}

// Get maxChannelTime for ScanEx
u16 WM_lib_get_scanEx_maxChannelTime(void)
{
    return scanExParam.maxChannelTime;
}

// Channel configuration for ScanEx
void WM_lib_set_scanEx_channelList(u16 channelList)
{
    // Be aware that channel is a bitmap
    // If 0x1041, 1,7,13ch
    WM_lib_set_scan_channel_seek(FALSE);
    scanExParam.channelList = channelList;
}

// Get channel for ScanEx
u16 WM_lib_get_scanEx_channelList(void)
{
    return scanExParam.channelList;
}

// ScanType(passive or active) configuration for ScanEx
void WM_lib_set_scanEx_scanType(u16 scanType)
{
    scanExParam.scanType = scanType;
}

// Get ScanType(passive or active) for ScanEx
// WM_SCANTYPE_ACTIVE(0), WM_SCANTYPE_PASSIVE(1)
u16 WM_lib_get_scanEx_scanType(void)
{
    return scanExParam.scanType;
}

// SSID filtering (SSID length) configuration for ScanEx
void WM_lib_set_scanEx_ssidLength(u16 ssidLength)
{
    scanExParam.ssidLength = ssidLength;
}

// Configuration of SSID used in SSID filtering for ScanEx
void WM_lib_set_scanEx_ssid(u8 *ssid)
{
    if (ssid == NULL)
    {
        MI_CpuClear8(scanExParam.ssid, WM_SIZE_SSID);
    }
    else
    {
        MI_CpuCopy8(ssid, scanExParam.ssid, WM_SIZE_SSID);
    }
}

// Configuration of MacAddress used in MacAddress(BSSID) filtering for ScanEx
void WM_lib_set_scanEx_bssid(u8 *bssid)
{
    MI_CpuCopy8(bssid, scanExParam.bssid, WM_SIZE_MACADDR);
}


//--------------------------------------------------------
// WM initialization
int WM_lib_Init(WM_lib_param * para)
{
    int     errcode;
    param = para;

    param->mpStarted = 0;
    param->mpBusy = 0;
    param->child_bitmap = 0;
    param->currentTgid = 0;

    param->endReq = 0;
    WM_lib_mp_dcf_parallel = FALSE;

    switch (param->mode)
    {
    case WM_MODE_MP_PARENT:
        callback_ptr = WM_lib_mp_parent_callback;
        break;
    case WM_MODE_MP_CHILD:
        callback_ptr = WM_lib_mp_child_callback;
        break;
    case WM_MODE_INFRASTRUCTURE:
        callback_ptr = WM_lib_dcf_child_callback;
        break;
    default:
        callback_ptr = WM_lib_dummy;
        return WM_ERRCODE_FAILED;
    }

    do
    {
        errcode = WM_Initialize(param->wmBuf, callback_ptr, WM_DMA_NO);
    }
    while (errcode == WM_ERRCODE_WM_DISABLE);
    if (errcode != WM_ERRCODE_OPERATING)
    {
        return WM_ERRCODE_FAILED;
    }
    else
    {
        return WM_SetIndCallback(callback_ptr);
    }
}

//=================================================================
// Dummy callback
static void WM_lib_dummy(void *arg)
{
    WMCallback *buf = (WMCallback *)arg;
}

//=================================================================
// MP parent callback
void WM_lib_mp_parent_callback(void *arg)
{
    int     errcode;
    WMCallback *buf = (WMCallback *)arg;

    switch (buf->apiid)
    {
    case WM_APIID_START_PARENT:
        {
            WMstartParentCallback *callback = (WMstartParentCallback *)arg;

#ifdef	WMLIB_DEBUG
            OS_Printf("StartParent callback\n");
            OS_Printf("      state:%d\n", callback->state);
#endif

            if (WM_ERRCODE_SUCCESS == callback->errcode)
            {
                switch (callback->state)
                {
                case WM_STATECODE_PARENT_START:
                    param->callback(WM_TYPE_PARENT_STARTED, arg);
                    break;

                case WM_STATECODE_CONNECTED:
                    errcode = WM_ReadStatus(param->statusBuf);
                    check_WMAPI_errcode(WM_APIID_READ_STATUS, errcode);

                    // Load status
                    param->child_bitmap = param->statusBuf->child_bitmap;

                    if (WM_lib_auto_flag == TRUE)
                    {
                        wm_lib_add_child_list(callback);
                    }

                    param->callback(WM_TYPE_CHILD_CONNECTED, arg);

                    if (param->statusBuf->mp_flag == 0) // If before start of MP, then start MP
                    {
                        errcode = WM_StartMPEx(WM_lib_mp_parent_callback, param->recvBuf, param->recvBufSize, param->sendBuf, param->sendBufSize, (u16)(param->contSend ? 0 : 1), 0, FALSE, FALSE, param->no_retry,     // Default is FALSE
                                               param->contSend ? TRUE : FALSE   // ignoreFatalError
                            );

                        check_WMAPI_errcode(WM_APIID_START_MP, errcode);
                    }
                    // MP send-enabled callback
                    if (TRUE == WM_lib_CheckMPSend())
                    {
                        param->callback(WM_TYPE_MP_SEND_ENABLE, NULL);
                    }
                    break;

                case WM_STATECODE_DISCONNECTED:
                    errcode = WM_ReadStatus(param->statusBuf);  // Load status
                    check_WMAPI_errcode(WM_APIID_READ_STATUS, errcode);

                    param->child_bitmap = param->statusBuf->child_bitmap;

                    if (WM_lib_auto_flag == TRUE)
                    {
                        wm_lib_delete_child_list(callback);
                    }
                    param->callback(WM_TYPE_CHILD_DISCONNECTED, arg);
                    break;

                case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
                    // Does nothing if the unit disconnects from itself
                    break;

                case WM_STATECODE_BEACON_SENT: // Notify end of beacon transmission
                    param->callback(WM_TYPE_BEACON_SENT, arg);
                    break;

                default:
                    param->callback(WM_TYPE_ERROR, arg);
                    break;
                }
            }
            else
            {
                param->callback(WM_TYPE_ERROR, arg);
            }
        }
        break;

    case WM_APIID_SET_LIFETIME:
        {
            if (WM_ERRCODE_SUCCESS == buf->errcode)
            {
                param->callback(WM_TYPE_SET_LIFETIME, arg);

                // Issue API
                errcode = WM_SetParentParameter(WM_lib_mp_parent_callback, param->parentParam);
                check_WMAPI_errcode(WM_APIID_SET_P_PARAM, errcode);
            }
            else
            {
                param->callback(WM_TYPE_ERROR, arg);
            }
        }
        break;

    case WM_APIID_START_MP:
        {
            // Error code does not get returned here
            WMstartMPCallback *callback = (WMstartMPCallback *)arg;

#ifdef	WMLIB_DEBUG
            OS_Printf("StartMP callback\n");
            OS_Printf("     state:%d\n", callback->state);
#endif
            switch (callback->state)
            {
            case WM_STATECODE_MP_START:
                param->mpStarted = 1;  // Set flag indicating that MP has started MP
                param->callback(WM_TYPE_MP_STARTED, arg);

                if (param->keySharing == 1)
                {
                    // Start KeySharing
                    errcode = WM_StartKeySharing(param->ksBuf, WM_PORT_KEYSHARING);
                    check_WMAPI_errcode(WM_APIID_START_KS, errcode);
                    if (TRUE == WM_lib_CheckMPSend())
                        param->callback(WM_TYPE_MP_SEND_ENABLE, NULL);
                }
                else
                {
                    // MP send-enabled callback
                    if (TRUE == WM_lib_CheckMPSend())
                    {
                        param->callback(WM_TYPE_MP_SEND_ENABLE, NULL);
                        if (WM_lib_mp_dcf_parallel == TRUE)
                        {
                            errcode =
                                WM_StartDCF(WM_lib_mp_parent_callback, param->dcfBuf,
                                            param->dcfBufSize);
                            check_WMAPI_errcode(WM_APIID_START_DCF, errcode);
                        }
                    }
                }
                break;

            case WM_STATECODE_MPEND_IND:
                // MP reception callback
                param->callback(WM_TYPE_MP_PARENT_RECV, (void *)(callback->recvBuf));
                break;

            default:
                param->callback(WM_TYPE_ERROR, arg);
                break;
            }
            break;
        }
        break;

    case WM_APIID_START_DCF:
        if (WM_ERRCODE_SUCCESS == buf->errcode)
        {
#if 0
#define WM_TYPE_DCF_STARTED
#define WM_TYPE_DCF_SENT
#define WM_TYPE_DCF_SENT_ERR
#define WM_TYPE_DCF_RECV
#define WM_TYPE_DCF_END
#endif
            param->callback(WM_TYPE_DCF_STARTED, arg);
        }
        else
        {
            param->callback(WM_TYPE_ERROR, arg);
        }
        break;

    case WM_APIID_SET_MP_DATA:
        {
            param->mpBusy = 0;

            if (buf->errcode == WM_ERRCODE_SUCCESS)
            {
                param->callback(WM_TYPE_MP_PARENT_SENT, arg);
                param->callback(WM_TYPE_MP_SEND_ENABLE, NULL);  // Next transmission is allowed
            }
            else if (buf->errcode == WM_ERRCODE_SEND_QUEUE_FULL)
            {
                param->callback(WM_TYPE_SEND_QUEUE_FULL_ERR, arg);
            }
            else
            {
                param->callback(WM_TYPE_MP_PARENT_SENT_ERR, arg);
                param->callback(WM_TYPE_MP_SEND_ENABLE, NULL);  // Next transmission is allowed
            }
        }
        break;

    case WM_APIID_INITIALIZE:
        {
            if (WM_ERRCODE_SUCCESS == buf->errcode)
            {
                errcode = WM_ReadStatus(param->statusBuf);
                check_WMAPI_errcode(WM_APIID_READ_STATUS, errcode);
                // Load status
                // Issue API
                errcode = WM_SetIndCallback(WM_lib_mp_parent_callback);
                check_WMAPI_errcode(WM_APIID_INDICATION, errcode);
#ifdef NO_LIFETIME
                errcode =
                    WM_SetLifeTime(WM_lib_mp_parent_callback, TABLE_NO, 0xffff, 0xffff, 0xffff);
#else
                errcode =
                    WM_SetLifeTime(WM_lib_mp_parent_callback, TABLE_NO, CAM_LIFE_TIME,
                                   FRAME_LIFE_TIME, CAM_LIFE_TIME);
#endif
                check_WMAPI_errcode(WM_APIID_SET_LIFETIME, errcode);
            }
            else
            {
                param->callback(WM_TYPE_ERROR, arg);
            }
        }
        break;

#if 1
    case WM_APIID_SET_P_PARAM:
        {
            param->callback(WM_TYPE_INIT_COMPLETE, arg);
            // Error code does not get returned here
            errcode = WM_SetBeaconIndication(WM_lib_mp_parent_callback, 1 /* 1:ON, 0:OFF*/ );
            check_WMAPI_errcode(WM_APIID_SET_BEACON_IND, errcode);
        }
        break;
        // Check whether Beacon Send/Recv Indication has occurred
    case WM_APIID_SET_BEACON_IND:
        {
            if (WM_ERRCODE_SUCCESS == buf->errcode)
            {
                errcode = WM_StartParent(WM_lib_mp_parent_callback);
                check_WMAPI_errcode(WM_APIID_START_PARENT, errcode);
            }
            else
            {
                param->callback(WM_TYPE_ERROR, arg);
            }
        }
        break;
#else
    case WM_APIID_SET_P_PARAM:
        {
            param->callback(WM_TYPE_INIT_COMPLETE, arg);
            // Error code does not get returned here
            errcode = WM_StartParent(WM_lib_mp_parent_callback);
            check_WMAPI_errcode(WM_APIID_START_PARENT, errcode);
        }
        break;
#endif

    case WM_APIID_RESET:
        {
#ifdef	WMLIB_DEBUG
            OS_Printf("Reset callback\n");
            OS_Printf("      errcode:%d\n", buf->errcode);
#endif
            if (buf->errcode == WM_ERRCODE_SUCCESS)
            {
                errcode = WM_End(WM_lib_mp_parent_callback);
                check_WMAPI_errcode(WM_APIID_END, errcode);
            }
            else
            {
                param->endReq = 0;
                param->callback(WM_TYPE_ERROR, arg);
            }
        }
        break;

    case WM_APIID_END:
        {
            param->endReq = 0;
            if (buf->errcode == WM_ERRCODE_SUCCESS)
            {
                param->callback(WM_TYPE_END_COMPLETE, arg);
            }
            else
            {
                param->callback(WM_TYPE_ERROR, arg);
            }
        }
        break;

    case WM_APIID_DISCONNECT:
        {
            WMDisconnectCallback *cb = (WMDisconnectCallback *)buf;
#ifdef WMLIB_DEBUG
            OS_Printf("Disconnect callback\n");
            OS_Printf("    cb->errcode            = %d\n", cb->errcode);
            OS_Printf("    cb->wlCmdID            = %d\n", cb->wlCmdID);
            OS_Printf("    cb->wlResult           = %d\n", cb->wlResult);
            OS_Printf("    cb->tryBitmap          = %d\n", cb->tryBitmap);
            OS_Printf("    cb->disconnectedBitmap = %d\n", cb->disconnectedBitmap);
#endif
            if (buf->errcode == WM_ERRCODE_SUCCESS)
            {
                // Update the child info
                errcode = WM_ReadStatus(param->statusBuf);
                check_WMAPI_errcode(WM_APIID_READ_STATUS, errcode);
                param->child_bitmap = param->statusBuf->child_bitmap;

                if (WM_lib_auto_flag == TRUE)
                {
                    wm_lib_delete_child_list((WMstartParentCallback *)arg);
                }
                param->callback(WM_TYPE_DISCONNECT_COMPLETE, arg);
            }
            else
            {
                param->callback(WM_TYPE_DISCONNECT_FAILED, arg);
            }
        }
        break;

    case WM_APIID_SET_GAMEINFO:
        {
            if (buf->errcode == WM_ERRCODE_SUCCESS)
            {
                param->callback(WM_TYPE_SET_GAMEINFO_COMPLETE, arg);
            }
            else
            {
                param->callback(WM_TYPE_ERROR, arg);
            }
        }
        break;

    case WM_APIID_END_MP:
        {
            param->callback(WM_TYPE_END_MP_COMPLETE, arg);
        }
        break;

    case WM_APIID_INDICATION:
        {
            WMindCallback *cb = (WMindCallback *)arg;
            switch (cb->state)
            {
            case WM_STATECODE_BEACON_RECV:     // Indicate beacon received
                param->callback(WM_TYPE_BEACON_RECV, arg);
                break;
            case WM_STATECODE_DISASSOCIATE:    // Indicate disconnection
                param->callback(WM_TYPE_DISASSOCIATE, arg);
                break;
            case WM_STATECODE_REASSOCIATE:     // Indicate reconnection
                param->callback(WM_TYPE_REASSOCIATE, arg);
                break;
            case WM_STATECODE_AUTHENTICATE:    // Indicate confirmed authentication
                param->callback(WM_TYPE_AUTHENTICATE, arg);
                break;
            }
        }
        break;

    default:
        param->callback(WM_TYPE_ERROR, arg);
        break;

    }
}

//=================================================================
// MP child callback
void WM_lib_mp_child_callback(void *arg)
{
    WMCallback *buf = (WMCallback *)arg;
    int     errcode;

    switch (buf->apiid)
    {
        //---------------------------------------------------------------
    case WM_APIID_INITIALIZE:
        {
            if (WM_ERRCODE_SUCCESS == buf->errcode)
            {
#ifdef WMLIB_DEBUG
                OS_TPrintf("mp C: WM_Initialize callback\n");
#endif
                param->callback(WM_TYPE_INIT_COMPLETE, arg);
                {
                    errcode = WM_ReadStatus(param->statusBuf);  // Load status
                    check_WMAPI_errcode(WM_APIID_READ_STATUS, errcode);

                    // Issue API
                    errcode = WM_SetIndCallback(WM_lib_mp_child_callback);
                    check_WMAPI_errcode(WM_APIID_INDICATION, errcode);
#ifdef NO_LIFETIME
                    errcode =
                        WM_SetLifeTime(WM_lib_mp_child_callback, TABLE_NO, 0xffff, 0xffff, 0xffff);
#else
                    errcode =
                        WM_SetLifeTime(WM_lib_mp_child_callback, TABLE_NO, CAM_LIFE_TIME,
                                       FRAME_LIFE_TIME, CAM_LIFE_TIME);
#endif
                    check_WMAPI_errcode(WM_APIID_SET_LIFETIME, errcode);
                }
            }
            else
            {
                param->callback(WM_TYPE_ERROR, arg);
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_SET_LIFETIME:
        {
#ifdef WMLIB_DEBUG
            OS_TPrintf("mp C: WM_SetLifeTime callback\n");
#endif
            if (WM_ERRCODE_SUCCESS == buf->errcode)
            {
                param->callback(WM_TYPE_SET_LIFETIME, arg);

                // Issue WM_StartScan
                if (param->scanExFlag == TRUE)
                {
                    // When using WM_StartScanEx
                    scanExParam.scanBuf = param->parentInfoBuf;
                    scanExParam.scanBufSize = param->parentInfoBufSize;

                    if (scanExParam.maxChannelTime == 0)
                        WM_lib_set_scanEx_maxChannelTime(200);

                    scanExParam.bssid[0] = 0xff;
                    scanExParam.bssid[1] = 0xff;
                    scanExParam.bssid[2] = 0xff;
                    scanExParam.bssid[3] = 0xff;
                    scanExParam.bssid[4] = 0xff;
                    scanExParam.bssid[5] = 0xff;

                    scanning_flag = 1;
                    errcode = WM_StartScanEx(WM_lib_mp_child_callback, &scanExParam);
                    check_WMAPI_errcode(WM_APIID_START_SCAN_EX, errcode);
                }
                else
                {
                    // When using WM_StartScan
                    scanParam.scanBuf = param->parentInfoBuf;

                    if (scanParam.channel == 0)
                        scanParam.channel = 1;
                    if (scanParam.maxChannelTime == 0)
                        WM_lib_set_max_scan_time(200);

                    scanParam.bssid[0] = 0xff;
                    scanParam.bssid[1] = 0xff;
                    scanParam.bssid[2] = 0xff;
                    scanParam.bssid[3] = 0xff;
                    scanParam.bssid[4] = 0xff;
                    scanParam.bssid[5] = 0xff;

                    scanning_flag = 1;
                    errcode = WM_StartScan(WM_lib_mp_child_callback, &scanParam);
                    check_WMAPI_errcode(WM_APIID_START_SCAN, errcode);
                }
            }
            else
            {
                param->callback(WM_TYPE_ERROR, arg);
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_START_SCAN:
        {
            WMstartScanCallback *callback = (WMstartScanCallback *)arg;

#ifdef WMLIB_DEBUG
            OS_TPrintf("mp C: WM_StartScan callback\n");
#endif
            if (WM_ERRCODE_SUCCESS == callback->errcode)
            {
                switch (callback->state)
                {
                case WM_STATECODE_SCAN_START:
                    break;

                case WM_STATECODE_PARENT_FOUND:
                    if (WM_lib_auto_flag)
                    {                  // When using wm_tool.c automation
                        if (WM_IsValidGameInfo(&((WMstartScanCallback *)arg)->gameInfo, ((WMstartScanCallback *)arg)->gameInfoLength))
                        {
                            wm_lib_add_parent_list((WMstartScanCallback *)arg);
                        }
                    }

                    param->callback(WM_TYPE_PARENT_FOUND, arg);
                    if (scanning_flag == 1)
                    {
                        if (WM_lib_change_scan_channel_flag == TRUE)
                            changeScanChannel(&scanParam);

                        errcode = WM_StartScan(WM_lib_mp_child_callback, &scanParam);
                        check_WMAPI_errcode(WM_APIID_START_SCAN, errcode);
                    }
                    break;

                case WM_STATECODE_PARENT_NOT_FOUND:
                    if (WM_lib_auto_flag)
                    {                  // When using wm_tool.c automation
                        wm_lib_delete_parent_list((WMstartScanCallback *)arg);
                    }
                    param->callback(WM_TYPE_PARENT_NOT_FOUND, arg);
                    if (scanning_flag == 1)
                    {
                        if (WM_lib_change_scan_channel_flag == TRUE)
                            changeScanChannel(&scanParam);

                        errcode = WM_StartScan(WM_lib_mp_child_callback, &scanParam);
                        check_WMAPI_errcode(WM_APIID_START_SCAN, errcode);
                    }
                    break;

                default:
                    param->callback(WM_TYPE_ERROR, arg);
                    break;
                }
            }
            else
            {
                param->callback(WM_TYPE_ERROR, arg);
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_START_SCAN_EX:
        {
            WMstartScanExCallback *callback = (WMstartScanExCallback *)arg;

#ifdef WMLIB_DEBUG
            OS_TPrintf("mp C: WM_StartScanEx callback\n");
#endif
            if (WM_ERRCODE_SUCCESS == callback->errcode)
            {
                switch (callback->state)
                {
                case WM_STATECODE_SCAN_START:
                    break;

                case WM_STATECODE_PARENT_FOUND:
                    if (WM_lib_auto_flag)
                    {                  // When using wm_tool.c automation
                        WMBssDesc  *p_bssDesc;
                        u16        bssDescCount;
                        int        i;

                        bssDescCount = ((WMStartScanExCallback*)arg)->bssDescCount;
                        for( i = 0; i < bssDescCount; ++i )
                        {
                        	p_bssDesc = ((WMStartScanExCallback*)arg)->bssDesc[i];

                            if(WM_IsValidGameInfo( &p_bssDesc->gameInfo, p_bssDesc->gameInfoLength))
                            {
        	                    wm_lib_add_parent_listEx((WMstartScanExCallback *)arg);
                            }
                        }
                    }

                    param->callback(WM_TYPE_PARENT_FOUND, arg);
                    if (scanning_flag == 1)
                    {
                        errcode = WM_StartScanEx(WM_lib_mp_child_callback, &scanExParam);
                        check_WMAPI_errcode(WM_APIID_START_SCAN_EX, errcode);
                    }
                    break;

                case WM_STATECODE_PARENT_NOT_FOUND:
                    param->callback(WM_TYPE_PARENT_NOT_FOUND, arg);
                    if (scanning_flag == 1)
                    {
                        errcode = WM_StartScanEx(WM_lib_mp_child_callback, &scanExParam);
                        check_WMAPI_errcode(WM_APIID_START_SCAN_EX, errcode);
                    }
                    break;

                default:
                    param->callback(WM_TYPE_ERROR, arg);
                    break;
                }
            }
            else
            {
                param->callback(WM_TYPE_ERROR, arg);
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_END_SCAN:
        {
            if (WM_ERRCODE_SUCCESS == buf->errcode)
            {
                errcode = WM_StartConnect(WM_lib_mp_child_callback, param->pInfo, test_ssid);
                //errcode = WM_StartConnect( WM_lib_mp_child_callback, param->pInfo, NULL );
                check_WMAPI_errcode(WM_APIID_START_CONNECT, errcode);
            }
            else
            {
                param->callback(WM_TYPE_ERROR, arg);
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_START_CONNECT:
        {
            WMstartConnectCallback *callback = (WMstartConnectCallback *)arg;

            if (callback->errcode == WM_ERRCODE_SUCCESS)
            {
                switch (callback->state)
                {
                case WM_STATECODE_CONNECT_START:
                    break;

                case WM_STATECODE_CONNECTED:
                    if (WM_lib_auto_flag == TRUE)
                    {
                        wm_lib_set_my_aid(callback->aid);
                    }
                    param->callback(WM_TYPE_CONNECTED_TO_PARENT, arg);
                    param->child_bitmap = 1;
#if 0
                    OS_Printf("WM_GetMPReceiveBufferSize:%d\n", WM_GetMPReceiveBufferSize());

                    OS_Printf("WM_GetMPSendBufferSize   :%d\n", WM_GetMPSendBufferSize());
#endif
                    errcode = WM_StartMPEx(WM_lib_mp_child_callback, param->recvBuf, param->recvBufSize, param->sendBuf, param->sendBufSize, (u16)(param->contSend ? 0 : 1), 0, FALSE, FALSE, param->no_retry,  // Default is FALSE
                                           param->contSend ? TRUE : FALSE       // ignoreFatalError
                        );


                    check_WMAPI_errcode(WM_APIID_START_MP, errcode);
                    break;

                case WM_STATECODE_DISCONNECTED:
                    param->callback(WM_TYPE_DISCONNECTED_FROM_PARENT, arg);
                    param->child_bitmap = 0;
                    break;

                case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
                    // Does nothing if the unit disconnects from itself
                    break;

                case WM_STATECODE_BEACON_LOST:
                    param->callback(WM_TYPE_BEACON_LOST, arg);
                    break;

                default:
                    param->callback(WM_TYPE_ERROR, arg);
                    break;
                }
            }
            else
            {
                // Notify of callback error
                if (WM_lib_auto_flag == TRUE)
                {
                    wm_lib_parent_found_count_reset();
                }
                param->callback(WM_TYPE_CONNECT_FAILED, arg);
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_START_MP:
        {
            WMstartMPCallback *callback = (WMstartMPCallback *)arg;

            switch (callback->state)
            {
            case WM_STATECODE_MP_START:
                param->mpStarted = 1;  // Set flag indicating that MP has started MP
                param->callback(WM_TYPE_MP_STARTED, arg);

                if (param->keySharing == 1)
                {
                    // Start KeySharing
                    errcode = WM_StartKeySharing(param->ksBuf, WM_PORT_KEYSHARING);
                    check_WMAPI_errcode(WM_APIID_START_KS, errcode);
                    if (TRUE == WM_lib_CheckMPSend())
                        param->callback(WM_TYPE_MP_SEND_ENABLE, NULL);
                }
                else
                {
                    // MP send-enabled callback
                    if (TRUE == WM_lib_CheckMPSend())
                    {
                        param->callback(WM_TYPE_MP_SEND_ENABLE, NULL);
                        if (WM_lib_mp_dcf_parallel == TRUE)
                        {
                            errcode =
                                WM_StartDCF(WM_lib_mp_child_callback, param->dcfBuf,
                                            param->dcfBufSize);
                            check_WMAPI_errcode(WM_APIID_START_DCF, errcode);
                        }
                    }
                }
                break;

            case WM_STATECODE_MP_IND:
                if (callback->errcode == WM_ERRCODE_INVALID_POLLBITMAP)
                {
                    // Although itself has not been polled, notify as is because it has received MP frame.
                    param->callback(WM_TYPE_MP_CHILD_RECV, (void *)(callback->recvBuf));
                }
                else
                {
                    param->callback(WM_TYPE_MP_CHILD_RECV, (void *)(callback->recvBuf));
                }
                break;

            case WM_STATECODE_MPACK_IND:
                break;

            default:
                param->callback(WM_TYPE_ERROR, arg);
                break;
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_START_DCF:
        if (WM_ERRCODE_SUCCESS == buf->errcode)
        {
            param->callback(WM_TYPE_DCF_STARTED, arg);
        }
        else
        {
            param->callback(WM_TYPE_ERROR, arg);
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_SET_MP_DATA:
        {
            param->mpBusy = 0;

            if (buf->errcode == WM_ERRCODE_SUCCESS)
            {
                param->callback(WM_TYPE_MP_CHILD_SENT, arg);
            }
            else if (buf->errcode == WM_ERRCODE_TIMEOUT)
            {
                param->callback(WM_TYPE_MP_CHILD_SENT_TIMEOUT, arg);
            }
            else
            {
                param->callback(WM_TYPE_MP_CHILD_SENT_ERR, arg);
            }

            // Next transmission is allowed
            param->callback(WM_TYPE_MP_SEND_ENABLE, NULL);
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_RESET:
        {
            if (buf->errcode == WM_ERRCODE_SUCCESS)
            {
                errcode = WM_End(WM_lib_mp_child_callback);
                check_WMAPI_errcode(WM_APIID_END, errcode);
            }
            else
            {
                param->endReq = 0;
                param->callback(WM_TYPE_ERROR, arg);
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_END:
        {
            param->endReq = 0;
            if (buf->errcode == WM_ERRCODE_SUCCESS)
            {
                param->callback(WM_TYPE_END_COMPLETE, arg);
            }
            else
            {
                param->callback(WM_TYPE_ERROR, arg);
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_INDICATION:
        {
            WMindCallback *cb = (WMindCallback *)arg;
            switch (cb->state)
            {
            case WM_STATECODE_BEACON_RECV:     // Indicate beacon received
                {
                    static u16 flag = 0;        // Prevents WM_lib_End2 from being issued twice, if this point is reached twice in a row
                    WMBeaconRecvIndCallback *callback = (WMBeaconRecvIndCallback *)arg;
                    if ((param->currentTgid != 0) && (callback->gameInfo.tgid != param->currentTgid)
                        && (callback->wmstate == WM_STATE_MP_CHILD))
                    {
                        if (flag == 0)
                        {
                            int     errcode;
                            flag = 1;
                            errcode = WM_lib_End();
                            check_WMAPI_errcode(WM_APIID_END, errcode);
                        }
                    }
                    else
                    {
                        flag = 0;
                    }
                    param->callback(WM_TYPE_BEACON_RECV, arg);
                }
                break;
            case WM_STATECODE_DISASSOCIATE:    // Indicate disconnection
                param->callback(WM_TYPE_DISASSOCIATE, arg);
                break;
            case WM_STATECODE_REASSOCIATE:     // Indicate reconnection
                param->callback(WM_TYPE_REASSOCIATE, arg);
                break;
            case WM_STATECODE_AUTHENTICATE:    // Indicate confirmed authentication
                param->callback(WM_TYPE_AUTHENTICATE, arg);
                break;
            }
        }
        break;

    default:
        param->callback(WM_TYPE_ERROR, arg);
        break;
    }
}

//========================================================================================

//=================================================================
// DCF child callback
void WM_lib_dcf_child_callback(void *arg)
{
    WMCallback *buf = (WMCallback *)arg;
    int     errcode;

    switch (buf->apiid)
    {
        //---------------------------------------------------------------
    case WM_APIID_INITIALIZE:
        {
            if (WM_ERRCODE_SUCCESS == buf->errcode)
            {
#ifdef WMLIB_DEBUG
                OS_TPrintf("dcf: WM_Initialize callback\n");
#endif
                param->callback(WM_TYPE_INIT_COMPLETE, arg);

                errcode =
                    WM_SetLifeTime(WM_lib_dcf_child_callback, TABLE_NO, 0xffff, 0xffff, 0xffff);
                check_WMAPI_errcode(WM_APIID_SET_LIFETIME, errcode);
            }
            else
            {
                param->callback(WM_TYPE_ERROR, arg);
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_SET_LIFETIME:
        {
            if (WM_ERRCODE_SUCCESS == buf->errcode)
            {
#ifdef WMLIB_DEBUG
                OS_TPrintf("dcf: WM_SetLifeTime callback\n");
#endif

                errcode = WM_SetWEPKeyEx(WM_lib_dcf_child_callback, param->wepMode,
                                         param->wepKeyId, param->wepKey);
                check_WMAPI_errcode(WM_APIID_SET_WEPKEY_EX, errcode);
            }
            else
            {
                param->callback(WM_TYPE_ERROR, arg);
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_SET_WEPKEY:
    case WM_APIID_SET_WEPKEY_EX:
        {
#ifdef WMLIB_DEBUG
            OS_TPrintf("dcf: WM_SetWEPKey(Ex) callback\n");
#endif
            if (WM_ERRCODE_SUCCESS == buf->errcode)
            {
                // Load status
                errcode = WM_ReadStatus(param->statusBuf);
                check_WMAPI_errcode(WM_APIID_READ_STATUS, errcode);

                // Issue WM_SetIncCallback
                errcode = WM_SetIndCallback(WM_lib_dcf_child_callback);
                check_WMAPI_errcode(WM_APIID_INDICATION, errcode);

                // Scan start
                if (param->scanExFlag == TRUE)
                {
                    // When using WM_StartScanEx
                    scanExParam.scanBuf = param->parentInfoBuf;
                    scanExParam.scanBufSize = param->parentInfoBufSize;

                    if (scanExParam.maxChannelTime == 0)
                        WM_lib_set_scanEx_maxChannelTime(200);

                    scanExParam.bssid[0] = 0xff;
                    scanExParam.bssid[1] = 0xff;
                    scanExParam.bssid[2] = 0xff;
                    scanExParam.bssid[3] = 0xff;
                    scanExParam.bssid[4] = 0xff;
                    scanExParam.bssid[5] = 0xff;

                    scanning_flag = 1;

#ifdef WMLIB_DEBUG
                    OS_TPrintf("dcf: WM_StartScanEx\n");
#endif
                    // Issue WM_StartScanEx
                    errcode = WM_StartScanEx(WM_lib_dcf_child_callback, &scanExParam);
                    check_WMAPI_errcode(WM_APIID_START_SCAN_EX, errcode);
                }
                else
                {
                    // When using WM_StartScan
                    scanParam.scanBuf = param->parentInfoBuf;

                    if (scanParam.channel == 0)
                        scanParam.channel = 1;
                    if (scanParam.maxChannelTime == 0)
                        WM_lib_set_max_scan_time(200);

                    scanExParam.bssid[0] = 0xff;
                    scanExParam.bssid[1] = 0xff;
                    scanExParam.bssid[2] = 0xff;
                    scanExParam.bssid[3] = 0xff;
                    scanExParam.bssid[4] = 0xff;
                    scanExParam.bssid[5] = 0xff;

                    scanning_flag = 1;

#ifdef WMLIB_DEBUG
                    OS_TPrintf("dcf: WMi_StartScan\n");
#endif
                    // Issue WM_StartScan
                    errcode = WM_StartScan(WM_lib_dcf_child_callback, &scanParam);
                    check_WMAPI_errcode(WM_APIID_START_SCAN, errcode);
                }
            }
        }
        break;


        //---------------------------------------------------------------
    case WM_APIID_START_SCAN:
        {
            WMstartScanCallback *callback = (WMstartScanCallback *)arg;

#ifdef WMLIB_DEBUG
            OS_TPrintf("dcf: WMi_StartScan callback ");
#endif
            if (WM_ERRCODE_SUCCESS == callback->errcode)
            {
                switch (callback->state)
                {
                case WM_STATECODE_SCAN_START:
#ifdef WMLIB_DEBUG
                    OS_TPrintf("start\n");
#endif
                    break;

                case WM_STATECODE_PARENT_FOUND:
#ifdef WMLIB_DEBUG
                    OS_TPrintf("found\n");
#endif
                    param->callback(WM_TYPE_PARENT_FOUND, arg);
                    if (scanning_flag == 1)
                    {
                        if (WM_lib_change_scan_channel_flag == TRUE)
                            changeScanChannel(&scanParam);

                        errcode = WM_StartScan(WM_lib_dcf_child_callback, &scanParam);

                        check_WMAPI_errcode(WM_APIID_START_SCAN, errcode);
                    }
                    break;

                case WM_STATECODE_PARENT_NOT_FOUND:
#ifdef WMLIB_DEBUG
                    OS_TPrintf("not found\n");
#endif
                    param->callback(WM_TYPE_PARENT_NOT_FOUND, arg);
                    if (scanning_flag == 1)
                    {
                        if (WM_lib_change_scan_channel_flag == TRUE)
                            changeScanChannel(&scanParam);

                        errcode = WM_StartScan(WM_lib_dcf_child_callback, &scanParam);

                        check_WMAPI_errcode(WM_APIID_START_SCAN, errcode);
                    }
                    break;

                default:
#ifdef WMLIB_DEBUG
                    OS_TPrintf("error\n");
#endif
                    param->callback(WM_TYPE_ERROR, arg);
                    break;
                }
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_START_SCAN_EX:
        {
            WMstartScanExCallback *callback = (WMstartScanExCallback *)arg;

#ifdef WMLIB_DEBUG
            OS_TPrintf("dcf: WMi_StartScanEx callback ");
#endif
            if (WM_ERRCODE_SUCCESS == callback->errcode)
            {
                switch (callback->state)
                {
                case WM_STATECODE_SCAN_START:
#ifdef WMLIB_DEBUG
                    OS_TPrintf("start\n");
#endif
                    break;

                case WM_STATECODE_PARENT_FOUND:
#ifdef WMLIB_DEBUG
                    OS_TPrintf("found\n");
#endif
                    param->callback(WM_TYPE_PARENT_FOUND, arg);
                    if (scanning_flag == 1)
                    {
                        errcode = WM_StartScanEx(WM_lib_dcf_child_callback, &scanExParam);
                        check_WMAPI_errcode(WM_APIID_START_SCAN_EX, errcode);
                    }
                    break;

                case WM_STATECODE_PARENT_NOT_FOUND:
#ifdef WMLIB_DEBUG
                    OS_TPrintf("not found\n");
#endif
                    param->callback(WM_TYPE_PARENT_NOT_FOUND, arg);
                    if (scanning_flag == 1)
                    {
                        errcode = WM_StartScanEx(WM_lib_dcf_child_callback, &scanExParam);
                        check_WMAPI_errcode(WM_APIID_START_SCAN_EX, errcode);
                    }
                    break;

                default:
#ifdef WMLIB_DEBUG
                    OS_TPrintf("error\n");
#endif
                    param->callback(WM_TYPE_ERROR, arg);
                    break;
                }
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_END_SCAN:
        {
            switch (buf->errcode)
            {
            case WM_ERRCODE_SUCCESS:
#ifdef STARTCONNECT_EX_TEST
                errcode = WM_StartConnectEx(WM_lib_dcf_child_callback, param->pInfo, NULL,
                                            FALSE, param->authMode);
#else
                errcode = WM_StartConnect(WM_lib_dcf_child_callback, param->pInfo, NULL);
#endif
                check_WMAPI_errcode(WM_APIID_START_CONNECT, errcode);
                break;

            default:
                param->callback(WM_TYPE_ERROR, arg);
                break;
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_START_CONNECT:
        {
            WMstartConnectCallback *callback = (WMstartConnectCallback *)arg;

            if (callback->errcode == WM_ERRCODE_SUCCESS)
            {
                switch (callback->state)
                {
                case WM_STATECODE_CONNECT_START:
                    break;

                case WM_STATECODE_CONNECTED:
                    param->callback(WM_TYPE_CONNECTED_TO_PARENT, arg);
                    errcode =
                        WM_StartDCF(WM_lib_dcf_child_callback, param->dcfBuf, param->dcfBufSize);
                    check_WMAPI_errcode(WM_APIID_START_DCF, errcode);
                    break;

                case WM_STATECODE_DISCONNECTED:
                    param->callback(WM_TYPE_DISCONNECTED_FROM_PARENT, arg);
                    break;

                case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
                    // Does nothing if the unit disconnects from itself
                    break;

                default:
                    param->callback(WM_TYPE_ERROR, arg);
                    break;
                }
            }
            else
            {
                // Notify of callback error
                param->callback(WM_TYPE_CONNECT_FAILED, arg);
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_START_DCF:
        {
            WMstartDCFCallback *callback = (WMstartDCFCallback *)arg;

            if (callback->errcode == WM_ERRCODE_SUCCESS)
            {
                switch (callback->state)
                {
                case WM_STATECODE_DCF_START:
                    break;

                case WM_STATECODE_DCF_IND:
                    param->callback(WM_TYPE_DCF_CHILD_RECV, callback->recvBuf);
                    break;

                default:
                    param->callback(WM_TYPE_ERROR, arg);
                    break;
                }
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_RESET:
        {
            if (buf->errcode == WM_ERRCODE_SUCCESS)
            {
                errcode = WM_End(WM_lib_dcf_child_callback);
                check_WMAPI_errcode(WM_APIID_END, errcode);
            }
            else
            {
                param->endReq = 0;
                param->callback(WM_TYPE_ERROR, arg);
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_END:
        {
            param->endReq = 0;
            if (buf->errcode == WM_ERRCODE_SUCCESS)
            {
                param->callback(WM_TYPE_END_COMPLETE, arg);
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_SET_DCF_DATA:
        {
            if (buf->errcode == WM_ERRCODE_SUCCESS)
            {
                param->callback(WM_TYPE_DCF_CHILD_SENT, arg);
            }
            else
            {
                param->callback(WM_TYPE_DCF_CHILD_SENT_ERR, arg);
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_INDICATION:
        {
        }
        break;

    default:
        param->callback(WM_TYPE_ERROR, arg);
        break;
    }
}




//========================================================================================

//--------------------------------------------------------
// WM_lib termination request
int WM_lib_End(void)
{
    int     errcode;

    if (param == NULL)
        return WM_ERRCODE_FAILED;

    /* do nothing if the End request has already been put out*/
    if (param->endReq == 0)
    {
        scanning_flag = 0;
        param->endReq = 1;
        errcode = WM_Reset(callback_ptr);
        return conv_errcode(errcode);
    }

    return WM_ERRCODE_FAILED;

}

//--------------------------------------------------------
// WM_lib MP data send request
int WM_lib_SetMPDataEX(u16 *data, u16 size, u16 pollbitmap)
{
    int     errcode;

    if (param == NULL)
        return WM_ERRCODE_FAILED;

    // If MP is not started, is busy or endReq is complete, MP data cannot be sent.
    if ((param->mpStarted == 0) || (param->mpBusy == 1) || (param->endReq == 1))
        return WM_ERRCODE_FAILED;

    // Enable the MP data transmit request flag
    param->mpBusy = 1;

    switch (param->mode)
    {
    case WM_MODE_MP_PARENT:
        if (0 == param->contSend)
        {
            errcode = WM_SetMPData(WM_lib_mp_parent_callback, data, size, 1000, pollbitmap);
        }
        else
        {
            errcode = WM_SetMPData(WM_lib_mp_parent_callback, data, size, 0, pollbitmap);
        }
        if (errcode == WM_ERRCODE_OPERATING)
        {
        }

        if (errcode != WM_ERRCODE_SUCCESS)
            param->mpBusy = 0;
        return conv_errcode(errcode);

    case WM_MODE_MP_CHILD:
        errcode = WM_SetMPData(WM_lib_mp_child_callback, data, size, 0, pollbitmap);
        if (errcode == WM_ERRCODE_OPERATING)
        {
        }
        if (errcode != WM_ERRCODE_SUCCESS)
            param->mpBusy = 0;
        return conv_errcode(errcode);

    default:
        return WM_ERRCODE_FAILED;
    }
}

int WM_lib_SetMPData(u16 *data, u16 size)
{
    return WM_lib_SetMPDataEX(data, size, 0xffff);
}


//--------------------------------------------------------
// WM_lib DCF data transmit request
int WM_lib_SetDCFData(const u8 *destAdr, u16 *data, u16 size)
{
    int     errcode;
    errcode = WM_SetDCFData(WM_lib_dcf_child_callback, destAdr, data, size);
    return conv_errcode(errcode);
}



//--------------------------------------------------------
// WM_lib connection request
int WM_lib_ConnectToParent(WMbssDesc *pInfo)
{
    int     errcode;
    WMGameInfo *pGameInfo = (WMGameInfo *)&(pInfo->gameInfo);

    if (param == NULL)
        return WM_ERRCODE_FAILED;

    param->pInfo = pInfo;
    param->currentTgid = pGameInfo->tgid;

    scanning_flag = 0;

    errcode = WM_EndScan(callback_ptr);
    return conv_errcode(errcode);
}

//--------------------------------------------------------
// WM_lib disconnect request
int WM_lib_Disconnect(u16 aid)
{
    int     errcode;

    if (param == NULL)
        return WM_ERRCODE_FAILED;

    errcode = WM_Disconnect(callback_ptr, aid);
    if (errcode != WM_ERRCODE_OPERATING)
        param->callback(WM_TYPE_DISCONNECT_FAILED, 0);
    return conv_errcode(errcode);
}

//--------------------------------------------------------
// WM_lib GameInfo set request
int WM_lib_SetGameInfo(u16 *userGameInfo, u16 size, u32 ggid, u16 tgid)
{
    int     errcode;
    u8      attribute;
    if (param == NULL)
        return WM_ERRCODE_FAILED;

    param->parentParam->userGameInfo = userGameInfo;
    param->parentParam->userGameInfoLength = size;
    param->parentParam->ggid = ggid;
    param->parentParam->tgid = tgid;

    attribute = (u8)(((param->parentParam->entryFlag) ? WM_ATTR_FLAG_ENTRY : 0) |       // entryFlag lowest bit
                     ((param->parentParam->multiBootFlag) ? WM_ATTR_FLAG_MB : 0) |      // multiBootFlag second bit
                     ((param->parentParam->KS_Flag) ? WM_ATTR_FLAG_KS : 0) |    // KS_Flag third bit
                     ((param->parentParam->CS_Flag) ? WM_ATTR_FLAG_CS : 0)      // CS_Flag fourth bit
        );


    // Actually, not just userGameInfo, but parentParam itself is updated.
//    errcode = WM_SetGameInfo( callback_ptr, param->parentParam );
    errcode = WM_SetGameInfo(callback_ptr,
                             param->parentParam->userGameInfo,
                             param->parentParam->userGameInfoLength,
                             param->parentParam->ggid, param->parentParam->tgid, attribute);

    if (errcode != WM_ERRCODE_OPERATING)
        param->callback(WM_TYPE_SET_GAMEINFO_FAILED, 0);
    return conv_errcode(errcode);
}


//--------------------------------------------------------
// WM_lib MP send enable check
BOOL WM_lib_CheckMPSend(void)
{
    if (param == NULL)
        return WM_ERRCODE_FAILED;

    if ((param->mpStarted == 1) && (param->mpBusy == 0) &&
        (param->endReq == 0) && (param->child_bitmap != 0))
        return TRUE;
    else
        return FALSE;
}

//--------------------------------------------------------
// WM_lib keyset obtain function
int WM_lib_GetKeySet(WMkeySet *keySet)
{
    if (TRUE == WM_lib_CheckMPSend())
        return WM_GetKeySet(param->ksBuf, keySet);
    else
        return 0xffff;
}

//--------------------------------------------------------
u16 WM_lib_CurrentScanChannel(void)
{
    return scanParam.channel;
}


//--------------------------------------------------------
// ScanChannel change function
static void changeScanChannel(WMscanParam *param)
{
    switch (param->channel)
    {
    case 1:
        param->channel = 7;
        break;

    case 7:
        param->channel = 13;
        break;

    case 13:
        param->channel = 1;
        break;

    default:
        param->channel = 1;
        break;
    }
}


void WM_lib_set_scan_channel_seek(BOOL flag)
{
    WM_lib_change_scan_channel_flag = flag;
}


void WM_lib_set_bssid(u16 bssid0, u16 bssid1, u16 bssid2)
{
    /* Strictly prohibited to change the order*/
    scanParam.bssid[0] = (u8)(bssid0 & 0x00ff);
    scanParam.bssid[1] = (u8)(bssid0 >> 8);
    scanParam.bssid[2] = (u8)(bssid1 & 0x00ff);
    scanParam.bssid[3] = (u8)(bssid1 >> 8);
    scanParam.bssid[4] = (u8)(bssid2 & 0x00ff);
    scanParam.bssid[5] = (u8)(bssid2 >> 8);
}

void   *WM_lib_get_mp_parent_callback_ptr(void)
{
    return &WM_lib_mp_parent_callback;
}

void   *WM_lib_get_mp_child_callback_ptr(void)
{
    return &WM_lib_mp_child_callback;
}

//-----------------------------------------------------------------------------------
// Convert rate_rssi to a four-tier RSSI value
u16 WM_lib_CalcRSSI(u16 rate_rssi, u16 aid)
{
    u16     rssi;
    u16     i;
    static u16 bufCounter[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    static u16 rssiBuf[16][16] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    rssi = (u16)(rate_rssi >> 8);
//„¡„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„¦„Ÿ„Ÿ„¦„Ÿ„Ÿ„¢
// | RSSI (Upper 6 bits)                           | LNA  | rsv   |
//„¤„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„Ÿ„¨„Ÿ„Ÿ„¨„Ÿ„Ÿ„£
    rssi &= ~0x0001;                   // Clear rsv just in case
    if (rssi & 0x0002)
    {
        rssi >>= 2;
    }
    else
    {
        rssi >>= 2;
        rssi += 25;
    }

    // Calculate the average for the last 16 times
    rssiBuf[aid][bufCounter[aid]] = rssi;
    if (++bufCounter[aid] == 16)
        bufCounter[aid] = 0;

    rssi = 0;
    for (i = 0; i < 16; ++i)
        rssi += rssiBuf[aid][i];

    rssi >>= 4;

    // Return as four-tier RSSI value
    if (rssi < 8)
        return 0;
    else if (rssi < 20)
        return 1;
    else if (rssi < 36)
        return 2;
    else
        return 3;
}


// Check WM_API's returned value
static void check_WMAPI_errcode(u16 apiid, int errcode)
{
    u16     arg[2];

    if (errcode != WM_ERRCODE_OPERATING && errcode != WM_ERRCODE_SUCCESS)
    {
        arg[0] = apiid;
        arg[1] = (u16)errcode;
        param->callback(WM_TYPE_API_ERROR, arg);
    }
}


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
