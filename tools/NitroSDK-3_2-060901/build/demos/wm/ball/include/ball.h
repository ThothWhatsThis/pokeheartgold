/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - ball
  File:     ball.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ball.h,v $
  Revision 1.5  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.4  02/28/2005 05:26:01  yosizaki
  do-indent.

  Revision 1.3  07/28/2004 02:42:40  terui
  Fix padding of structure

  Revision 1.2  2004/07/27 08:25:56  yasu
  Add #pragma  warn_padding off

  Revision 1.1  07/23/2004 15:11:39  terui
  Revisions and additions.

  Revision 1.3  06/23/2004 10:01:30  Sato
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef BALL_H_
#define BALL_H_

#ifdef	__cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include <nitro.h>

//--------------------------------------------------------------------------------
// Assign OBJ numbers
#define PARENT_STRING_OBJNO	( 0)
#define CHILD_STRING_OBJNO	( 6)
#define PLIST_STRING_OBJNO	(11)
#define CLIST_STRING_OBJNO	(17)

#define CURSOR_OBJNO		(23)
#define PARENT_NAME_OBJNO	(24)
#define BALL_OBJNO			(60)
#define MESSAGE_OBJNO		(80)

typedef struct
{
    u16     x;
    u16     y;
    u8      col;
    u8      reserved[3];               // for padding 4byte align

}
BallData;

extern GXOamAttr oamBak[128];

extern BallData ball[16];

extern void InitBall(u16 aid);

extern void ChangeBallPalette(u16 aid, u8 pal);

extern void ClearBallAll(void);

extern void LocateBall(u16 aid, u16 x, u16 y);

extern void ClearBall(u16 aid);

extern void DispBall(void);

//--------------------------------------------------------------------------------
//      set OBJ
//
extern void ObjSet(int objNo, int x, int y, int charNo, int paletteNo);

//--------------------------------------------------------------------------------
//      Set OBJ character string
//
extern void ObjSetString(int startobjNo, int x, int y, char *string, int paletteNo);

//--------------------------------------------------------------------------------
//      clear OBJ
//
extern void ObjClear(int objNo);

//--------------------------------------------------------------------------------
//      Clear OBJ (specified range)
//
extern void ObjClearRange(int start, int end);

//--------------------------------------------------------------------------------
//      set message
//
extern void ObjSetMessage(char *message);


/*===========================================================================*/

#ifdef	__cplusplus
}          /* extern "C"*/
#endif

#endif /* BALL_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
