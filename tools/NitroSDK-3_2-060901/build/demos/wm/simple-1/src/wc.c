/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - simple-1
  File:     wc.c

  Copyright 2003-2005 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wc.c,v $
  Revision 1.32  2005/02/28 05:26:12  yosizaki
  do-indent.

  Revision 1.31  2004/10/22 11:06:01  terui Improvement:
  wc module has been cut to wireless shared region.  

  Revision 1.30  2004/10/18 04:17:46  terui Improvement:
  Added a specification to randomize the beacon interval and the scan interval.

  Revision 1.29  2004/10/05 09:45:29  terui Improvement:
  Integrated MAC address with u8-type array.

  Revision 1.28  2004/10/04 13:40:25  terui Improvement:
  Integrated game group ID with u32-type.

  Revision 1.27  2004/09/30 08:45:31  seiki_masashi
  Set official value assigned to GGID.

  Revision 1.26  2004/09/28 09:08:04  terui Improvement:
  Corrected the bug where there was no provision for when length comparison of WMBssDesc is in units of words.  

  Revision 1.25  2004/09/27 13:32:48  terui Improvement:
  ectified the situation where GameInfo of the previous scan remained when scanning.  

  Revision 1.24  2004/09/15 12:21:09  terui Improvement:
  Changed functions and definitions related to debug output

  Revision 1.23  2004/09/10 12:08:10  seiki_masashi
  Changed it so the number of bytes sent for key sharing is concealed in the library

  Revision 1.22  2004/09/10 11:49:25  terui Improvement:
  Support specifications that divide processes during initialization and termination into three stages

  Revision 1.21  2004/09/03 04:42:13  seiki_masashi
  Changed the errcode for failed sends to WM_ERRCODE_SEND_FAILED

  Revision 1.20  2004/09/01 05:51:05  seiki_masashi
  Support added for WM_ERRCODE_INVALID_POLLBITMAP in MP.ind

  Revision 1.19  2004/09/01 05:17:27  seiki_masashi
  Supported WM_ERRCODE_INVALID_POLLBITMAP

  Revision 1.18  2004/08/30 05:37:39  seiki_masashi
  Changed send conditions to be only when the previous send completes  

  Revision 1.17  2004/08/30 02:41:19  seiki_masashi
  Small fix.

  Revision 1.16  2004/08/30 02:10:35  seiki_masashi
  Changed constant definition location

  Revision 1.15  2004/08/30 01:56:53  seiki_masashi
  Changed so Port receive callback is used

  Revision 1.14  2004/08/27 07:55:04  terui Improvement:
  Changed process when beacon is lost, and when connection ends.  

  Revision 1.13  2004/08/23 04:40:54  ooe
  Support for the changes to the specification of WM_StartConnect

  Revision 1.12  2004/08/20 04:31:55  terui Improvement:
  Added debugging output when the maximum number of connected children is exceeded.  

  Revision 1.11  2004/08/20 00:23:09  terui Improvement:
  Dealt with change to WMParentParam structure.

  Revision 1.9  2004/08/19 02:55:17  terui Improvement:
  Revised in accordance with introduction of connection control method by entry.  

  Revision 1.8  2004/08/18 06:25:19  terui Improvement:
  Changed default connection channel.

  Revision 1.7  2004/08/18 00:27:11  seiki_masashi
  Small fix

  Revision 1.6  2004/08/17 12:40:45  seiki_masashi
  Changed to specify send buffer in WM_StartMP argument.

  Revision 1.5  2004/08/09 00:48:31  terui Improvement:
  Supported changes to WM specifications.

  Revision 1.4  2004/07/28 02:48:14  terui Improvement:
  Changed calculation of amount of memory to be allocated  

  Revision 1.3  2004/07/27 08:09:30  yasu
  delete initializing of reserved area

  Revision 1.2  2004/07/24 03:21:15  yasu
  Fix at masking OS_Printf

  Revision 1.1  2004/07/23 15:28:14  terui Improvement:
  Revisions and additions.

  Revision 1.5  2004/07/23 14:46:05  terui Improvement:
  fix comment. turn off print debug switch.

  Revision 1.4  2004/07/23 01:03:55  terui Improvement:
  Adjusted code

  Revision 1.2  2004/07/15 13:54:29  seiki_masashi
  Revised for current WM specifications

  Revision 1.1  2004/07/15 12:48:42  terui Improvement:
  initial upload

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

