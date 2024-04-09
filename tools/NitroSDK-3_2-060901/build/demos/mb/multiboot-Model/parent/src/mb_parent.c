/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - demos - multiboot-Model
  File:     mb_parent.c

  Copyright 2003-2005 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_parent.c,v $
  Revision 1.30  2005/02/28 05:26:19  yosizaki
  do-indent.

  Revision 1.29  2004/11/12 09:01:50  yosizaki
  this file was moved to /demos/wireless_shared/mbp.

  Revision 1.28  2004/11/12 05:21:33  takano_makoto
  Replaced the IsSameMacAddr function with the WM_IsBssidEqual function.

  Revision 1.27  2004/11/12 04:38:32  takano_makoto
  Removed unnecessary include headers.

  Revision 1.26  2004/11/10 01:12:53  takano_makoto
  Changed to use WM_CopyBssid.

  Revision 1.25  2004/11/02 18:08:42  takano_makoto
  fixed comments.

  Revision 1.24  2004/11/02 09:15:23  takano_makoto
  Changed so that MI_CpuCopy is not used to save MAC address.

  Revision 1.23  2004/10/28 01:31:09  takano_makoto
  Moved wmhigh to wh under wireless_shared.

  Revision 1.22  2004/10/27 02:07:41  takano_makoto
  Revised so that when an illegal child CONNECTs, instead of doing a Disconnect, will instantly Kick at the time of the request.  

  Revision 1.21  2004/10/26 09:18:32  takano_makoto
  Added MB_DisconnectChild

  Revision 1.19  2004/10/21 00:42:42  yosizaki
  add SHARED_WH switch.

  Revision 1.18  2004/10/18 11:44:06  yosizaki
  change to use MB_StartParentEx/MB_EndEx (switch)

  Revision 1.17  2004/10/08 01:26:28  takano_makoto
  When acquiring user information during Init, copy src and dest were in reverse order. Fixed.  

  Revision 1.16  2004/10/05 09:45:29  terui Improvement:
  Integrated MAC address with u8-type array.

  Revision 1.15  2004/10/05 07:49:36  takano_makoto
  Revised so that child Accept processes in mb_parent.c callback.

  Revision 1.14  2004/10/05 02:15:05  takano_makoto
  Changed to wait for entry state and start simultaneous download on all children.  

  Revision 1.13  2004/10/04 13:40:25  terui Improvement:
  Integrated game group ID with u32-type.

  Revision 1.12  2004/10/04 10:09:08  takano_makoto
  Added MB_COMM_PSTATE_WAIT_TO_SEND state.

  Revision 1.11  2004/09/21 10:52:25  takano_makoto
  fix ASSERT in MBP_GetChildBmp().

  Revision 1.10  2004/09/21 09:12:49  yasu
  fix typo in MBP_GetChildBmp()

  Revision 1.9  2004/09/21 06:57:03  takano_makoto
  Added processing for the MB_COMM_PSTATE_ERROR error code

  Revision 1.8  2004/09/17 14:10:39  takano_makoto
  fix bugs in MBP_GetPlayerNo.

  Revision 1.7  2004/09/17 11:35:24  takano_makoto
  Changed the order the function definitions are listed  


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

