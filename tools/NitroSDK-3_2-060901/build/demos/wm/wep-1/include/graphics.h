/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - wep-1
  File:     graphics.h

  Copyright 2003-2005 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: graphics.h,v $
  Revision 1.1  2005/03/25 07:41:37  seiki_masashi
  Added dynamic WEP Key settings to dataShare-Model and registered as the 
  new demo wep-1

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

