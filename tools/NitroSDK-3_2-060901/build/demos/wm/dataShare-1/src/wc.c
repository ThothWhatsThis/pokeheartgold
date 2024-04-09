/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - dataShare-1
  File:     wc.c

  Copyright 2003-2005 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wc.c,v $
  Revision 1.30  2005/02/28 05:26:12  yosizaki
  do-indent.

  Revision 1.29  2004/10/22 11:05:54  terui Improvement:
  wc module has been cut to wireless shared region.  

  Revision 1.28  2004/10/18 04:17:46  terui Improvement:
  Added a specification to randomize the beacon interval and the scan interval.

  Revision 1.27  2004/10/05 09:45:29  terui Improvement:
  Integrated MAC address with u8-type array.

  Revision 1.26  2004/10/04 13:40:25  terui Improvement:
  Integrated game group ID with u32-type.

  Revision 1.25  2004/09/30 08:45:09  seiki_masashi
  Set official value assigned to GGID.

  Revision 1.24  2004/09/28 09:08:04  terui Improvement:
  Corrected the bug where there was no provision for when length comparison of WMBssDesc is in units of words.  

  Revision 1.23  2004/09/27 13:32:48  terui Improvement:
  ectified the situation where GameInfo of the previous scan remained when scanning.  

  Revision 1.22  2004/09/15 12:21:17  terui Improvement:
  Changed functions and definitions related to debug output

  Revision 1.21  2004/09/10 12:08:10  seiki_masashi
  Changed it so the number of bytes sent for key sharing is concealed in the library

  Revision 1.20  2004/09/10 11:53:07  terui Improvement:
  Support specifications that divide processes during initialization and termination into three stages

  Revision 1.19  2004/09/10 04:36:35  seiki_masashi
  Reduced send/receive bufer to the minimal size required

  Revision 1.18  2004/09/10 02:48:25  seiki_masashi
  Small fix.

  Revision 1.17  2004/09/10 01:19:50  seiki_masashi
  Changed so the maximum number of children that can be connected is set with WC_MAX_NUM_CHILD

  Revision 1.16  2004/09/09 12:41:32  seiki_masashi
  Revised so that the maximum send data size for children is an appropriate size

  Revision 1.15  2004/09/03 04:42:13  seiki_masashi
  Changed the errcode for failed sends to WM_ERRCODE_SEND_FAILED

  Revision 1.14  2004/09/01 05:51:05  seiki_masashi
  Support added for WM_ERRCODE_INVALID_POLLBITMAP in MP.ind

  Revision 1.13  2004/09/01 05:17:11  seiki_masashi
  Supported WM_ERRCODE_INVALID_POLLBITMAP

  Revision 1.12  2004/08/30 01:57:30  seiki_masashi
  Changed WM_STATECODE_CHILD_CONNECTED to WM_STATECODE_CONNECTED

  Revision 1.11  2004/08/27 08:03:16  terui Improvement:
  Changed process when beacon is lost, and when connection ends.  

  Revision 1.10  2004/08/23 04:41:01  ooe
  Support for the changes to the specification of WM_StartConnect

  Revision 1.9  2004/08/20 04:31:55  terui Improvement:
  Added debugging output when the maximum number of connected children is exceeded.  

  Revision 1.8  2004/08/19 15:34:23  miya
  modified WMGameInfo structure.

  Revision 1.7  2004/08/19 02:55:17  terui Improvement:
  Revised in accordance with introduction of connection control method by entry.  

  Revision 1.6  2004/08/18 00:28:35  seiki_masashi
  Small fix

  Revision 1.5  2004/08/17 12:26:37  seiki_masashi
  Changed to specify send buffer in WM_StartMP argument.
  Revised in accordance with the removal of WM_ERRCODE_FRAME_GAP.

  Revision 1.4  2004/08/12 14:57:11  seiki_masashi
  Supported the WM_StepDataSharing return value: WM_ERRCODE_FRAME_GAP.

  Revision 1.3  2004/08/11 08:01:29  terui Improvement:
  Added argument to WcInit and restructured to use with WM_StartDataSharing

  Revision 1.2  2004/08/10 06:38:24  terui Improvement:
  Added WcGetAid. Adjusted source, including revising comments and deleted unnecessary functions  

  Revision 1.1  2004/08/10 00:41:54  terui Improvement:
  Initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

