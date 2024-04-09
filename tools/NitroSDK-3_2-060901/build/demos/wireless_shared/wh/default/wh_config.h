/*
  Project:  NitroSDK - wireless_shared - demos - wh - default
  File:     wh_config.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wh_config.h,v $
  Revision 1.4  04/11/2006 00:01:31  okubata_ryoma
  Copyright fix

  Revision 1.3  04/10/2006 13:18:57  yosizaki
  support WH_MP_FREQUENCY.

  Revision 1.2  12/21/2005 02:36:44  adachi_hiroaki
  Changed identifier in "define" statement

  Revision 1.1  12/21/2005 02:20:17  adachi_hiroaki
  Changed the location of wh_config.h.

  Revision 1.2  02/28/2005 05:26:35  yosizaki
  do-indent.

  Revision 1.1  12/20/2004 07:17:48  takano_makoto
  Separated out the wireless communications parameters used in each application as wh_config.h.

  $NoKeywords$
 */

#ifndef _DEFAULT_WH_CONFIG_H__
#define _DEFAULT_WH_CONFIG_H__


// DMA number used for wireless communications
#define WH_DMA_NO                 2

// Maximum number of children (this number does not include the parent)
#define WH_CHILD_MAX              15

// Maximum size of data that can be shared
#define WH_DS_DATA_SIZE           12

// The maximum size of data sent in one transmission.
// If performing normal communications with the addition of data sharing,
// increase this value by that amount only. You need to add the header
// and footer parts that are added to transmission of multiple packets.
// For more details, see docs/TechnicalNotes/WirelessManager.doc.
// GUIDELINE: Guideline Standard Point (6.3.2)
// A time of 5600 microseconds or less is recommended when calculating the
// required time for one MP transmission using the Wireless manager (WM)  reference -> tables & information -> calculation sheet for wireless  communications times.
#define WH_PARENT_MAX_SIZE      (WH_DS_DATA_SIZE * (1 + WH_CHILD_MAX) + 4)
#define WH_CHILD_MAX_SIZE       (WH_DS_DATA_SIZE)

// upper limit of MP communications in one picture frame
// when multiple protocols, like data sharing and block transfer, are in parallel,
// this value must be set to a value larger than 1 (or to 0 for no limitations).
// Without that, the only protocol that will be allowed to run MP
// communications will be the one with the highest priority.
#define WH_MP_FREQUENCY           1

// Port to use for normal MP communications
#define WH_DATA_PORT              14

// Priority to use for normal MP communications
#define WH_DATA_PRIO              WM_PRIORITY_NORMAL

// Port to use for data sharing
#define WH_DS_PORT                13



#endif // _DEFAULT_WH_CONFIG_H__
