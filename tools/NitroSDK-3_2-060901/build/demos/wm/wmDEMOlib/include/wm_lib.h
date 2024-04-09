/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - wmDEMOlib
  File:     wm_lib.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm_lib.h,v $
  Revision 1.48  03/07/2006 02:01:04  ooe
  Added wm_lib_add_parent_listEx().

  Revision 1.47  2006/01/18 03:12:40  kitase_hirotake
  do-indent

  Revision 1.46  06/07/2005 06:45:56  seiki_masashi
  Revised to reduce special processing related to key sharing

  Revision 1.45  03/03/2005 11:20:13  ooe
  small fix

  Revision 1.44  02/28/2005 06:26:27  yosizaki
  do-indent.

  Revision 1.43  02/17/2005 09:08:54  ooe
  Added wm_lib_set_ggidFilter

  Revision 1.42  02/17/2005 01:16:06  ooe
  Added ggidFilter

  Revision 1.41  02/08/2005 10:36:57  ooe
  Revised copyright year notation

  Revision 1.40  02/08/2005 07:57:39  ooe
  Added support for StartScanEx, SetWEPKey, etc.

  Revision 1.39  10/26/2004 09:25:32  ooe
  Support WM_StartScanEx
  Added scanExFlag and save_parentInfoEx()

  Revision 1.38  10/05/2004 10:45:28  terui
  Standardized MAC addresses as type u8 arrays.

  Revision 1.37  10/04/2004 14:40:25  terui
  Standardized game group IDs as type u32.

  Revision 1.36  09/14/2004 10:51:34  ooe
  Added mpBusy flag

  Revision 1.35  09/10/2004 13:08:10  seiki_masashi
  Changed it so the number of bytes sent for key sharing is concealed in the library.

  Revision 1.34  08/31/2004 10:14:16  ooe
  Added no_retry flag to WM_lib_param. Used when MP is not resent.

  Revision 1.33  08/30/2004 06:36:16  sato_masaki
  Added wm_lib_set_gameinfo function

  Revision 1.32  08/19/2004 00:52:49  ikedae
  Removed mpSending

  Revision 1.31  08/18/2004 05:22:49  ooe
  Added WM_TYPE_API_ERROR

  Revision 1.30  08/18/2004 01:40:28  seiki_masashi
  small fix

  Revision 1.29  08/18/2004 01:26:07  seiki_masashi
  Changed recvBuf to u16*

  Revision 1.28  08/17/2004 14:13:54  seiki_masashi
  Tentative support for changes to WM_StartMP specifications.

  Revision 1.27  08/16/2004 11:11:02  ooe
  Added WM_lib_CalcRSSI.

  Revision 1.26  08/16/2004 10:40:45  ooe
  Added currentTgrig to WM_lib_param.

  Revision 1.25  08/10/2004 06:45:39  ooe
  Added find_counter (for scan test)

  Revision 1.24  08/07/2004 10:52:23  ooe
  Accommodated major MP-related changes

  Revision 1.23  08/05/2004 05:05:41  miya
  add functions

  Revision 1.22  08/03/2004 13:00:12  sato_masaki
  Function that performs connection processing via the WMbssDesc structure. Added wm_lib_connect_parent_via_bssdesc.

  Revision 1.21  08/02/2004 06:37:47  ooe
  Added WM_TYPE_MPACK_IND

  Revision 1.20  07/31/2004 05:40:00  yosiokat
  Added WM_lib_get_max_scan_time

  Revision 1.19  07/30/2004 10:22:33  miya
  Changed scan time

  Revision 1.18  07/30/2004 04:59:07  miya
  For DCF parallel

  Revision 1.17  07/29/2004 08:40:48  miya
  modified callback table size

  Revision 1.16  07/29/2004 02:28:44  yosiokat
  Added function prototype for void WM_lib_set_scan_channel_seek(BOOL flag)

  Revision 1.15  07/29/2004 01:15:06  miya
  Support for beacon scan specifying bssid (MAC address)

  Revision 1.14  07/28/2004 10:03:43  miya
  small fix

  Revision 1.13  07/28/2004 09:53:02  miya
  small fix

  Revision 1.12  07/28/2004 09:34:36  miya
  added comment

  Revision 1.11  07/28/2004 05:25:53  miya
  add functions about ggid & tgid

  Revision 1.9  07/28/2004 04:53:28  ikedae
  Moved WM_DMA_NO to header

  Revision 1.8  07/28/2004 03:46:47  terui
  Fix padding of structure

  Revision 1.7  07/28/2004 03:23:59  miya
  add functions about UserGameInfo

  Revision 1.6  07/27/2004 12:54:33  miya
  Added bssid Settings

  Revision 1.5  07/27/2004 12:31:23  miya
  Added event

  Revision 1.4  07/27/2004 10:23:10  ooe
  Added WM_TYPE_MP_SEND_DISABLE

  Revision 1.3  07/27/2004 09:07:25  yasu
  Add #pragma  warn_padding off

  Revision 1.2  07/27/2004 03:39:56  ikedae
  Added WM_lib_SetMPDataEX supporting pollbitmap

  Revision 1.1  07/23/2004 16:13:50  terui
  Revisions and additions.

  Revision 1.5  2004/07/11 04:02:33  ooe
  Additions and other changes related WM_SetBeaconIndication

  Revision 1.4  07/08/2004 11:04:22  terui
  Changes due to start of WM7 restructuring

  Revision 1.19  07/06/2004 02:50:57  Ooe
  Added and revised macros for calculating send/receive buffer sizes

  Revision 1.18  07/05/2004 14:00:25  Miya
  Added settings for keysharing and contSend

  Revision 1.17  07/05/2004 06:03:15 AM  ikedae
  Added WM_TYPE_PARENT_STARTED

  Revision 1.16  07/05/2004 01:12:57 AM  Sato
  add functions wm_lib_get_lib_param and wm_get_parent_bssdesc

  Revision 1.15  07/03/2004 08:29:47  Ooe
  Added WM_lib_GetKeySet()

  Revision 1.14  07/03/2004 07:55:04  Miya
  small change

  Revision 1.13  07/03/2004 02:48:56  Miya
  change checkMacAddress function to internal

  Revision 1.12  07/02/2004 05:51:46  Miya
  Revised cache control size

  Revision 1.11  07/01/2004 04:38:11  Miya
  Integrated wm_tool.h wm_lib.h

  Revision 1.10  2004/06/29 07:58:44  Miya
  Revised WM_CalcChildRecvBufSize

  Revision 1.9  06/29/2004 07:43:54  Ooe
  Revised macro for calculating buffer size

  Revision 1.8  06/29/2004 07:10:46  Ooe
  Added TYPE(WM_TYPE_MP_SEND_ENABLE) for send enable callback

  Revision 1.7  06/24/2004 12:21:42  Ooe
  Added WM_SetGameInfo() error handling

  Revision 1.6  06/24/2004 11:02:47  Ooe
  Added WM_SetGameInfo()

  Revision 1.5  06/22/2004 06:19:15  ikedae
  Changes related to WM_lib_Main deletion

  Revision 1.4  2004/06/21 07:39:48  Ooe
  (none)

  Revision 1.3  06/19/2004 06:56:58  Miya
  modified header file reference

  Revision 1.3  06/19/2004 05:03:14  miya
  ver. up Marionea-1.48.00

  Revision 1.2  06/02/2004 05:19:04  miya
  ver. up

  Revision 1.1  05/26/2004 09:47:30  terui
  Additions accompanying incorporation of wireless communication

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef WMDEMOLIB_WM_LIB_H_
#define WMDEMOLIB_WM_LIB_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

/* wm_lib.h*/
#include <nitro.h>
#include <nitro/wm/common/wm.h>

#define WM_MODE_MP_PARENT       1
#define WM_MODE_MP_CHILD        2
#define WM_MODE_INFRASTRUCTURE  3

#define WM_DMA_NO 2

#define WM_PORT_KEYSHARING      15

#define WM_TYPE_CHILD_CONNECTED             0
#define WM_TYPE_CHILD_DISCONNECTED          1
#define WM_TYPE_MP_PARENT_SENT              2
#define WM_TYPE_MP_PARENT_RECV              3
#define WM_TYPE_PARENT_FOUND                4
#define WM_TYPE_PARENT_NOT_FOUND            5
#define WM_TYPE_CONNECTED_TO_PARENT         6
#define WM_TYPE_DISCONNECTED                7
#define WM_TYPE_MP_CHILD_SENT               8
#define WM_TYPE_MP_CHILD_RECV               9
#define WM_TYPE_DISCONNECTED_FROM_PARENT    10
#define WM_TYPE_CONNECT_FAILED              11
#define WM_TYPE_DCF_CHILD_SENT              12
#define WM_TYPE_DCF_CHILD_SENT_ERR          13
#define WM_TYPE_DCF_CHILD_RECV              14
#define WM_TYPE_DISCONNECT_COMPLETE         15
#define WM_TYPE_DISCONNECT_FAILED           16
#define WM_TYPE_END_COMPLETE                17
#define WM_TYPE_MP_CHILD_SENT_ERR           18
#define WM_TYPE_MP_PARENT_SENT_ERR          19
#define WM_TYPE_MP_STARTED                  20
#define WM_TYPE_INIT_COMPLETE               21
#define WM_TYPE_END_MP_COMPLETE             22
#define WM_TYPE_SET_GAMEINFO_COMPLETE       23
#define WM_TYPE_SET_GAMEINFO_FAILED         24
#define WM_TYPE_MP_SEND_ENABLE              25
#define WM_TYPE_PARENT_STARTED              26
#define WM_TYPE_BEACON_LOST                 27
#define WM_TYPE_BEACON_SENT                 28
#define WM_TYPE_BEACON_RECV                 29
#define WM_TYPE_MP_SEND_DISABLE             30
#define WM_TYPE_DISASSOCIATE                31
#define WM_TYPE_REASSOCIATE                 32
#define WM_TYPE_AUTHENTICATE                33
#define WM_TYPE_SET_LIFETIME                34
#define WM_TYPE_DCF_STARTED                 35
#define WM_TYPE_DCF_SENT                    36
#define WM_TYPE_DCF_SENT_ERR                37
#define WM_TYPE_DCF_RECV                    38
#define WM_TYPE_DCF_END                     39
#define WM_TYPE_MPACK_IND                   40
#define WM_TYPE_MP_CHILD_SENT_TIMEOUT       41
#define WM_TYPE_SEND_QUEUE_FULL_ERR         42

#define WM_TYPE_API_ERROR                   255 // Error on retuned value of API call
#define WM_TYPE_ERROR                       256 // Error on callback


#define WL_HEADER_LENGTH_P  48         // WL header length when parent receives
#define WM_HEADER_LENGTH    (2+4)      // WM Header length
#define WL_HEADER_LENGTH_T  8          // Total WL header length
#define WL_HEADER_LENGTH_C  8          // WL header length of each child's data

// Macro for computing size of child Reception buffer
#define WM_CalcChildSendBufSize(_pInfo_)        (u16)(WM_SIZE_MP_CHILD_SEND_BUFFER(((WMgameInfo *)(&(((WMbssDesc*)(_pInfo_))->gameInfo)))->childMaxSize, TRUE))
#define WM_CalcChildRecvBufSize(_pInfo_)        (u16)(WM_SIZE_MP_CHILD_RECEIVE_BUFFER(((WMgameInfo *)(&(((WMbssDesc*)(_pInfo_))->gameInfo)))->parentMaxSize, TRUE))

// Macro for computing size of parent send/receive buffers
#define WM_CalcParentSendBufSize(_libParam_)    (u16)(WM_SIZE_MP_PARENT_SEND_BUFFER(((WM_lib_param*)(&(_libParam_)))->parentParam->parentMaxSize, TRUE))
#define WM_CalcParentRecvBufSize(_libParam_)    (u16)(WM_SIZE_MP_PARENT_RECEIVE_BUFFER(((WM_lib_param*)(&(_libParam_)))->parentParam->childMaxSize, WM_NUM_MAX_CHILD, TRUE))


typedef void (*WMcallbackFunc2) (u16 type, void *arg);  // WM callback type (2)

// wm_lib parameter structure
typedef struct
{
    void   *wmBuf;                     // WM system buffer
    WMpparam *parentParam;             // Parent information setting (Used by parent)
    WMbssDesc *parentInfoBuf;          // Buffer that holds parent info (Used by child)
    u16    *sendBuf;                   // Send buffer
    u16    *recvBuf;                   // MP receive buffer
    WMdcfRecvBuf *dcfBuf;              // DCF receive buffer
    WMstatus *statusBuf;               // Buffer for obtaining status (Delete if possible)
    WMcallbackFunc2 callback;          // Callback for WM_lib
    WMbssDesc *pInfo;                  // Pointer to information of connected parent (Used by child)
    u16     sendBufSize;               // Size of Transmission buffer
    u16     recvBufSize;               // Size of MP Reception buffer
    u16     dcfBufSize;                // DCF receive buffer size
    u16     mode;                      // WM_MODE_***
    u16     endReq;
    u16     mpStarted;                 // Flag indicating MP has started
    u16     mpBusy;                    // MP transmit request flag
    u16     child_bitmap;              // Child connection state
    u16     parentInfoBufSize;         // size of the buffer that obtains the parent device information (used by StartScanEx)

    // KeySharing function
    u16     keySharing;                // KeySharing execution flag (1: run KS, 0: Do not run KS)
    WMkeySetBuf *ksBuf;                // KeySharing buffer

    // Continuous send related
    u16     contSend;                  // Flag indicating execution of continuous transmission (1: Continuous transmission, 0: Sending 1 frame at a time) 

    // wep related
    u16     wepMode;                   // WEPmode used continuously
    u16     wepKeyId;                  // ID of WEPKey used in connection
    u8      wepKey[80];                // WEPkey used continuously

    // connection authorization-related
    u16     authMode;                  // WM_AUTHMODE_OPEN_SYSTEM or WM_AUTHMODE_SHARED_KEY

    // This is related to gameinfo
    u8     *uname;                     // UserName
    u8     *gname;                     // GameName
    u16     currentTgid;               // TGID of parent being connected (Check with BeaconRecv.Ind)

    // user gameinfo related
    u16     userGameInfoLength;        // Length of user area
    u16    *userGameInfo;              // Pointer to user area buffer

    // MP resend related
    BOOL    no_retry;                  // Resend processing execution flag (FALSE: Resend, TRUE: No resend)

    // Related to ScanEx
    BOOL    scanExFlag;                // Set to TRUE when using ScanEx

    u32     ggidFilter;                // GGID filtering (pass through everything if 0xffffffff)

//    u16             rsv1;               // for 4byte padding
}
WM_lib_param;

//-----------------------------------
// WM_lib_Init()
int     WM_lib_Init(WM_lib_param * para);       // WM_lib initialization

//-----------------------------------
// WM_lib_SetMPData()
int     WM_lib_SetMPData(u16 *data, u16 size);  // MP data transmission

//-----------------------------------
// WM_lib_SetMPDataEX()
int     WM_lib_SetMPDataEX(u16 *data, u16 size, u16 pollbitmap);
                                                // Send MP data (support for pollbitmap)

//-----------------------------------
// WM_lib_End()
int     WM_lib_End(void);              // Request terminate parent mode

//-----------------------------------
// WM_lib_ConnectToParent()
int     WM_lib_ConnectToParent(WMbssDesc *pInfo);       // Request connect to parent

//-----------------------------------
// WM_lib_SetDCFData()
int     WM_lib_SetDCFData(const u8 *destAdr, u16 *data, u16 size);      // DCF data transmission

//-----------------------------------
// WM_lib_SetGameInfo()
int     WM_lib_SetGameInfo(u16 *userGameInfo, u16 size, u32 ggid, u16 tgid);

//-----------------------------------
// WM_lib_Disconnect()
int     WM_lib_Disconnect(u16 aid);    // Disconnect child (aid) from parent


//-----------------------------------
// WM_lib_CheckMPSend()
BOOL    WM_lib_CheckMPSend(void);      // Check MP send permit


//-----------------------------------
// WM_lib_GetKeySet()
int     WM_lib_GetKeySet(WMkeySet *keySet);

//-----------------------------------
// WM_lib_set_auto()
void    WM_lib_set_auto(BOOL flag);    // Automatically register both parent and child

//-----------------------------------
// WM_lib_set_bssid()
void    WM_lib_set_bssid(u16 bssid0, u16 bssid1, u16 bssid2);   // BssId setting

//-----------------------------------
// WM_lib_CurrentScanChannel()

void    WM_lib_set_mp_dcf_parallel(BOOL flag);
void    WM_lib_set_no_retry_flag(BOOL flag);

//-----------------------------------
// functions for configuring and obtaining parameters for Scan
void    WM_lib_set_max_scan_time(u16 time);     // set continuous scan time
u16     WM_lib_get_max_scan_time(void); // get continuous Scan time
void    WM_lib_set_scan_channel_seek(BOOL flag);        // channel seek configuration
void    WM_lib_set_channel(u16 channel);        // channel configuration
u16     WM_lib_CurrentScanChannel(void);        // Returns channel now scanning
void    WM_lib_set_scanBssid(u8 *bssid);        // BSSID filtering

//-----------------------------------
// functions for configuring and obtaining parameters for ScanEx
void    WM_lib_set_scanEx_maxChannelTime(u16 time);     // configure maxChannelTime
u16     WM_lib_get_scanEx_maxChannelTime(void); // Get maxChannelTime
void    WM_lib_set_scanEx_channelList(u16 channel);     // channelList(bitmap) configuration
u16     WM_lib_get_scanEx_channelList(void);    // get channelList(bitmap)
void    WM_lib_set_scanEx_scanType(u16 scanType);       // ScanType configuration WM_SCANTYPE_ACTIVE(0), WM_SCANTYPE_PASSIVE(1)
u16     WM_lib_get_scanEx_scanType(void);       // Get ScanType WM_SCANTYPE_ACTIVE(0), WM_SCANTYPE_PASSIVE(1)
void    WM_lib_set_scanEx_ssidLength(u16 ssidLength);   // Configure SSID length for SSID filtering
void    WM_lib_set_scanEx_ssid(u8 *ssid);       // SSID configuration for SSID filtering
void    WM_lib_set_scanEx_bssid(u8 *bssid);     // BSSID configuration for BSSID filtering

//-----------------------------------
// WM_lib_CalcRSSI()
u16     WM_lib_CalcRSSI(u16 rate_rssi, u16 aid);        // Convert rate_rssi to RSSI value


/************ start where wm_tool.h was **********************/


#define WM_NUM_PARENT_INFORMATIONS 16
#define WM_NUM_CHILD_INFORMATIONS 15


typedef struct
{
    u16     found_parent_count;        // Number of parents found
    u16     my_aid;                    // AID assigned to self;
    WMmpRecvBuf *recvBuf;              // MP buffer passed with WM_StartMP()
    u16    *sendBuf;                   // MP send data buffer passed with WM_SetMPData()
    u8      MacAdrList[WM_NUM_PARENT_INFORMATIONS][6];  // For saving Parent MacAddresses (for 4 machines) 
    WMbssDesc *parentInfo[WM_NUM_PARENT_INFORMATIONS];  // For storing Parent information (for 4 machines), BssDesc used during connection 
    u16     channelList[WM_NUM_PARENT_INFORMATIONS];    // For saving Parent channels (for 4 machines) 
    u16     ssidLength[WM_NUM_PARENT_INFORMATIONS];     // Parent SSID lengths
    u8      ssidList[WM_NUM_PARENT_INFORMATIONS][32];   // For saving Parent SSIDs (for 4 machines) 
    u16     gameInfoLength[WM_NUM_PARENT_INFORMATIONS]; // Length of parent GameInfo

    u16     find_counter[WM_NUM_PARENT_INFORMATIONS];   // Number of times a parent was found

    u16     rsv[10];
    WMgameInfo gameInfoList[WM_NUM_PARENT_INFORMATIONS] ATTRIBUTE_ALIGN(32);    // For saving parent GameInfo (for 4 machines)
}
childInfo;


typedef struct
{
    u8      addr[6];
    u8      reserved[2];               // for 4 byte padding
}
MACADDRESS;

typedef struct
{
    int     valid;
    MACADDRESS macaddr;
}
CHILD_INFO;

typedef void (*WM_LIB_CALLBACK) (u16 type, void *arg);


int     wm_lib_get_last_found_parent_no(void);
void    save_parentInfo(WMstartScanCallback *buf, WM_lib_param * param, childInfo * info);
void    save_parentInfoEx(WMstartScanExCallback *buf, WM_lib_param * param, childInfo * info);


BOOL    wm_lib_get_own_macaddress(MACADDRESS * ma);





/* initialization functions*/



/* functions we plan to delete*/
void    wm_lib_add_child_list(WMstartParentCallback *arg);
void    wm_lib_delete_child_list(WMstartParentCallback *arg);
void    wm_lib_add_parent_list(WMstartScanCallback *arg);
void    wm_lib_add_parent_listEx(WMStartScanExCallback *arg);
void    wm_lib_delete_parent_list(WMstartScanCallback *arg);
void    wm_lib_parent_found_count_reset(void);
void    wm_lib_set_my_aid(int aid);

/* functions for both the parent and child devices*/
void    wm_lib_comm_init(void);
int     wm_lib_start(void);
const char *wm_lib_get_wlib_version(void);
void    wm_lib_set_mode(u16 mode);
void    wm_lib_set_recvbuffer(u8 *buf);
void    wm_lib_set_sendbuffer(u8 *buf);
void    wm_lib_set_callback(WM_LIB_CALLBACK callback);
int     wm_lib_get_recvbuffer_size(void);
int     wm_lib_get_sendbuffer_size(void);
void    wm_lib_set_keySharing_mode(int flag);
void    wm_lib_set_contSend_mode(int flag);
void    wm_lib_set_multiboot_mode(int flag);
void    wm_lib_set_ni_mode(int flag);
void    wm_lib_set_ggid(u32 ggid);
u32     wm_lib_get_ggid(void);
void    wm_lib_set_tgid(u16 tgid);
u16     wm_lib_get_tgid(void);
void    wm_lib_set_scan_bssid(u16 bssid0, u16 bssid1, u16 bssid2);


/* Parent Functions*/
void    wm_lib_set_gameinfo_gamename(char *str);
void    wm_lib_set_gameinfo_username(char *user_name);
u32     wm_lib_get_parent_gameinfo_ggid(int id);
u16     wm_lib_get_parent_gameinfo_tgid(int id);
u16    *wm_lib_get_parent_gameinfo_usergameinfo(int id);
u16     wm_lib_get_parent_gameinfo_usergameinfosize(int id);

void    wm_lib_set_parent_send_size(u16 size);
void    wm_lib_set_parent_channel(u16 ch);
void    wm_lib_set_beacon_period(u16 period_ms);


void    wm_lib_set_parent_recv_size_per_child(u16 size);
BOOL    wm_lib_get_child_macaddress(int aid, MACADDRESS * ma);
BOOL    wm_lib_is_child_valid(int aid);
int     wm_lib_set_gameinfo(void);

/* Child Functions*/

BOOL    wm_lib_is_parent_gameinfo_valid(int id);
int     wm_lib_get_parent_gameinfo_channel(int id);
int     wm_lib_get_parent_gameinfo_parent_sendmaxsize(int id);
int     wm_lib_get_parent_gameinfo_child_sendbufsize(int id);
int     wm_lib_get_parent_gameinfo_child_recvbufsize(int id);
const char *wm_lib_get_parent_gameinfo_username(int id);
const char *wm_lib_get_parent_gameinfo_gamename(int id);


void   *WM_lib_get_mp_parent_callback_ptr(void);
void   *WM_lib_get_mp_child_callback_ptr(void);


int     wm_lib_get_parent_found_count(void);
int     wm_lib_connect_parent(int parent_no);
BOOL    wm_lib_get_parent_macaddress(int id, MACADDRESS * ma);
int     wm_lib_get_my_aid(void);

/*  get Keysharing Keyset*/
int     wm_lib_get_keyset(WMkeySet *keyset);
int     wm_lib_connect_parent_via_bssdesc(WMbssDesc *bssDesc);

void    wm_lib_set_ggidFilter(u32 ggidFilter);

/*
    get pointer to  WMbssDesc structure
*/
WMbssDesc *wm_get_parent_bssdesc(int id, WMbssDesc *bssDescp);

int     wm_lib_connect_parent_via_bssdesc(WMbssDesc *bssDescp);

/************ end where wm_tool.h was **********************/

/****************************************************/


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C"*/
#endif

#endif /* WMDEMOLIB_WM_LIB_H_*/

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
