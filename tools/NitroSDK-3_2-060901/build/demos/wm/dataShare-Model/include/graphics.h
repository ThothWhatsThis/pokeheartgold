/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - dataShare-Model
  File:     graphics.h

  Copyright 2003-2005 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: graphics.h,v $
  Revision 1.3  2005/02/28 05:26:12  yosizaki
  do-indent.

  Revision 1.2  2004/11/05 04:27:40  sasakis
  Added the lobby screen and parent selection screen, and made the necessary modifications (such as scan related).

  Revision 1.1  2004/11/01 04:11:44  sasakis
  Added the process failure countermeasure code and such (provisional version that includes the currently working codes).  

  $NoKeywords$
 *---------------------------------------------------------------------------*/

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#define PSEUDO2D_ONE 64

extern void initGraphics(void);
extern void setupPseudo2DCamera(void);
extern void drawPseudo2DTexQuad(int sx, int sy, int width, int height, int texw, int texh);
extern void drawPseudo2DColorQuad(int sx, int sy, int width, int height, GXRgb color);

#endif

