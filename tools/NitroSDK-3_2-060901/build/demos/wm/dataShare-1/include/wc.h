/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - dataShare-1
  File:     wc.h

  Copyright 2003-2005 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wc.h,v $
  Revision 1.13  2005/02/28 05:26:12  yosizaki
  do-indent.

  Revision 1.12  2004/10/22 11:05:49  terui Improvement:
  wc module has been cut to wireless shared region.  

  Revision 1.11  2004/09/30 08:46:02  seiki_masashi
  Set official value assigned to GGID.

  Revision 1.10  2004/09/10 04:36:35  seiki_masashi
  Reduced send/receive bufer to the minimal size required

  Revision 1.9  2004/09/10 01:19:50  seiki_masashi
  Changed so the maximum number of children that can be connected is set with WC_MAX_NUM_CHILD

  Revision 1.8  2004/08/18 06:26:57  terui Improvement:
  Changed default connection channel.

  Revision 1.7  2004/08/17 12:26:59  seiki_masashi
  Revised in accordance with the removal of WM_ERRCODE_FRAME_GAP.

  Revision 1.6  2004/08/12 14:57:11  seiki_masashi
  Supported the WM_StepDataSharing return value: WM_ERRCODE_FRAME_GAP.

  Revision 1.5  2004/08/11 08:00:58  terui Improvement:
  Added argument to WcInit

  Revision 1.4  2004/08/11 00:27:06  terui Improvement:
  Changed port used

  Revision 1.3  2004/08/10 06:36:48  terui Improvement:
  Added WcGetAid. Moved constant definitions from wc.c

  Revision 1.2  2004/08/10 02:11:32  terui Improvement:
  Changed size of shared data

  Revision 1.1  2004/08/10 00:41:40  terui Improvement:
  Initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

