/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - ball
  File:     ball.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ball.c,v $
  Revision 1.3  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.2  02/28/2005 05:26:01  yosizaki
  do-indent.

  Revision 1.1  07/23/2004 15:11:55  terui
  Revisions and additions.

  Revision 1.2  06/23/2004 10:01:30  Sato
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

#include "ball.h"

GXOamAttr oamBak[128];
BallData ball[16];

static const char NumString[] = "0123456789ABCDEF";

void InitBall(u16 aid)
{
    ball[aid].x = (u16)(96 + (aid % 4) * 16);
    ball[aid].y = (u16)(68 + (aid / 4) * 16);
    ball[aid].col = 7;
}

void ChangeBallPalette(u16 aid, u8 pal)
{
    ball[aid].col = pal;
}

void ClearBallAll(void)
{
    u16     i;
    for (i = 0; i < 16; i++)
        ClearBall(i);
}


void LocateBall(u16 aid, u16 x, u16 y)
{
    ball[aid].x = x;
    ball[aid].y = y;
}


void ClearBall(u16 aid)
{
    ball[aid].x = 256;
    ball[aid].y = 192;
}

void DispBall(void)
{
    int     i;
    for (i = 0; i < 16; i++)
    {
        GXOamAttr *oam = &oamBak[BALL_OBJNO + i];

        oam->x = ball[i].x;
        oam->y = (u8)ball[i].y;
        G2_SetOBJAttr((GXOamAttr *)oam,
                      ball[i].x,
                      ball[i].y,
                      0,
                      GX_OAM_MODE_NORMAL,
                      FALSE,
                      GX_OAM_EFFECT_NONE,
                      GX_OAM_SHAPE_8x8, GX_OAM_COLOR_16, NumString[i], ball[i].col, 0);
    }
}

//--------------------------------------------------------------------------------
//      set OBJ
//
void ObjSet(int objNo, int x, int y, int charNo, int paletteNo)
{
    G2_SetOBJAttr((GXOamAttr *)&oamBak[objNo],
                  x,
                  y,
                  0,
                  GX_OAM_MODE_NORMAL,
                  FALSE,
                  GX_OAM_EFFECT_NONE, GX_OAM_SHAPE_8x8, GX_OAM_COLOR_16, charNo, paletteNo, 0);
}

//--------------------------------------------------------------------------------
//      Set OBJ character string
//
void ObjSetString(int startobjNo, int x, int y, char *string, int paletteNo)
{
    int     i;
    for (i = 0; string[i] != 0; i++)
    {
        ObjSet(startobjNo + i, x + i * 8, y, (int)string[i], paletteNo);
    }
}

//--------------------------------------------------------------------------------
//      clear OBJ
//
void ObjClear(int objNo)
{
    ObjSet(objNo, 256 * 8, 192 * 8, 0, 0);
}

//--------------------------------------------------------------------------------
//      Clear OBJ (specified range)
//
void ObjClearRange(int start, int end)
{
    int     i;
    for (i = start; i <= end; i++)
        ObjClear(i);
}

//--------------------------------------------------------------------------------
//      set message
//
void ObjSetMessage(char *message)
{
    ObjSetString(MESSAGE_OBJNO, 4 * 8, 22 * 8, message, 7);
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
