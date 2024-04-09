/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - demos - multiboot
  File:     common.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: common.c,v $
  Revision 1.15  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.14  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.13  2004/10/05 06:47:54  sato_masaki
  Added setup menu. Made several small revisions.

  Revision 1.12  2004/09/30 03:05:12  sato_masaki
  - Changed so that name of parent device is obtained from NVRAM.
  - Changed so that you cycle through channels when initializing.    

  Revision 1.11  09/14/2004 23:56:23  yosizaki
  change MBGameregistry's member to Unicode.

  Revision 1.10  09/14/2004 02:54:07  miya
  with sign    

  Revision 1.9  09/13/2004 06:25:40  yosizaki
  change balldemo -> pol_toon

  Revision 1.8  2004/09/09 14:03:31  sato_masaki
  Revision due to mb library changes

  Revision 1.7  2004/09/08 00:23:27  sato_masaki
  Changed register number of MBGameRegistry to 2

  Revision 1.6  2004/08/25 04:28:20  sato_masaki
  Mounting MB_CommStartSending function    

  Revision 1.5  2004/08/23 07:27:27  yasu
  Workaround for pushing A Button in IPL

  Revision 1.4  08/19/2004 14:47:18  yosiokat
  Will try to add a line break in the MBGameRegistry game contents description

  Revision 1.3  08/19/2004 02:43:19  yosizaki
  change type of MBUserInfo.

  Revision 1.2  08/12/2004 09:57:34  yosiokat
  Changed so that the icon data is designated by the file path

  Revision 1.1  08/10/2004 07:31:53  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro.h>


#include "common.h"


/*
 * The common features used on this demo as a whole    
 */


/******************************************************************************/
/* constant*/

/* the array of program information that this demo downloads*/
const MBGameRegistry mbGameList[GAME_PROG_MAX] = {
    {
     "/em.srl",                        // The file path of the multiboot child device binary
     L"edgeDemo",                      // Game name
     L"edgemarking demo\ntesttesttest", // Game content description
     "/data/icon.char",                // The file path of the icon character data
     "/data/icon.plt",                 // The file path of the icon palette data
     0x12123434,                       // GameGroupID(GGID)
     16,                               // The maximum number of players
     },
    {
     "/pol_toon.srl",
     L"PolToon",
     L"toon rendering",
     "/data/icon.char",
     "/data/icon.plt",
     0x56567878,
     8,
     },
};


/******************************************************************************/
/* variable*/

/* the work region to be allocated to the MB library*/
u32     cwork[MB_SYSTEM_BUF_SIZE / sizeof(u32)];

/* the game sequence state of this demo*/
u8      prog_state;


/******************************************************************************/
/* function*/

/* detect key trigger*/
u16 ReadKeySetTrigger(u16 keyset)
{
    static u16 cont = 0xffff;          /* measures for A button press inside IPL*/
    u16     trigger = (u16)(keyset & (keyset ^ cont));
    cont = keyset;
    return trigger;
}

/* offset portion rotation of the val value in the min - max bounds*/
BOOL RotateU8(u8 *val, u8 min, u8 max, s8 offset)
{
    int     lVal = (int)*val - min;
    int     div = max - min + 1;

    if (div == 0)
        return FALSE;

    lVal = (lVal + offset + div) % div + min;

    *val = (u8)lVal;
    return TRUE;
}
