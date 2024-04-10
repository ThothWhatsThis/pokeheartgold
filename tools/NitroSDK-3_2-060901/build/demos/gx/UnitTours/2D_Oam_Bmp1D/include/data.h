/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/2D_Oam_Bmp1D
  File:     data.h

  Copyright 2003-2005 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: data.h,v $
  Revision 1.4  2005/02/28 05:26:04  yosizaki
  do-indent.

  Revision 1.3  2004/04/07 01:23:26  yada
  Fixed header comment

  Revision 1.2  2004/04/06 12:48:07  yada
  Fixed header comment

  Revision 1.1  2004/03/01 09:01:02  takano_makoto
  Initial check-in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/*
 ******************************************************************************
 *
 *  Project    :
 *               nitroSDK sample program
 *  Title      :
 *               Display a 1D mapping bitmap OBJ  
 *  Explanation:
 *               Display an OBJ that has 1D mapping bitmap data  
 *
 ******************************************************************************
 */
#ifndef OBJ_DATA_H
#define OBJ_DATA_H

extern const unsigned short bitmapOBJ_directcolor_Texel[128 * 128 * 2];

#define IMAGE_DATA	(bitmapOBJ_directcolor_Texel)
#define IMAGE_DATA_SIZE	(sizeof(bitmapOBJ_directcolor_Texel))

#endif
