/*
  Project:  NitroSDK - wireless_shared - demos - wh
  File:     wh_config.h

  Copyright 2003-2005 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wh_config.h,v $
  Revision 1.2  2005/02/28 05:26:35  yosizaki
  do-indent.

  Revision 1.1  2004/12/20 07:17:48  takano_makoto
  Separated out the wireless communications parameters used in every application to create wh_config.h.  

  $NoKeywords$
 */

#ifndef __WH_CONFIG_H__
#define __WH_CONFIG_H__


// DMA number used for wireless communications
#define WH_DMA_NO                 2

// Maximum number of children (this number does not include the parent)
#define WH_CHILD_MAX              15

// Maximum size of data that can be shared
#define WH_DS_DATA_SIZE           12

// The maximum size of data sent in one transmission.
// If performing normal communications with the addition of data sharing,  
// increase this value by that amount only.   You need to add the header  
// and footer parts that are added to transmission of multiple packets.  
// For more details, see docs/TechnicalNotes/WirelessManager.doc.
// GUIDELINE: Guideline standard point (6.3.2)  
// A time of 5600 microseconds or less is recommended when calculating the
// required time for one MP transmission using the Wireless manager (WM)  reference -> tables & information -> calculation sheet for wireless  communications times.
#define WH_PARENT_MAX_SIZE      (WH_DS_DATA_SIZE * (1 + WH_CHILD_MAX) + 4)
#define WH_CHILD_MAX_SIZE       (WH_DS_DATA_SIZE)

// Port to use for normal MP communications
#define WH_DATA_PORT              14

// Priority to use for normal MP communications
#define WH_DATA_PRIO              WM_PRIORITY_NORMAL

// Port to use for data sharing
#define WH_DS_PORT                13


#endif // __WH_CONFIGH_H__

