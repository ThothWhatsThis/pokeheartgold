/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - wep-1
  File:     key.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: key.h,v $
  Revision 1.2  01/18/2006 02:11:21  kitase_hirotake
  do-indent

  Revision 1.1  2005/03/25 07:41:37  seiki_masashi
  Added dynamic setting of WEP Key to dataShare-Model and newly registered it as wep-1 demo

  $NoKeywords$
 *---------------------------------------------------------------------------*/

#ifndef __KEY_H__
#define __KEY_H__

#define KEY_REPEAT_START 25            // number of frames until key repeat starts
#define KEY_REPEAT_SPAN  10            // number of key repeat interval frames

typedef struct KeyInfo
{
    u16     cnt;                       // unprocessed input values
    u16     trg;                       // push trigger input
    u16     up;                        // release trigger input
    u16     rep;                       // push and hold repeat input
}
KeyInfo;

extern void updateKeys(void);
extern KeyInfo *getKeyInfo(void);

#endif
