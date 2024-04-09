/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX - demos - UnitTours/3D_Pol_Vertex_Source
  File:     data.h

  Copyright 2003-2005 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: data.h,v $
  Revision 1.4  2005/02/28 05:26:05  yosizaki
  do-indent.

  Revision 1.3  2004/04/07 01:23:26  yada
  Fixed header comment

  Revision 1.2  2004/04/06 12:48:07  yada
  Fixed header comment

  Revision 1.1  2004/03/01 09:06:29  takano_makoto
  Initial check-in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/*
 ******************************************************************************
 *
 *  Project    :
 *               nitroSDK sample program
 *  Title      :
 *               Setting GX_TEXGEN_VERTEX in G3_TexImageParam()
 *              (Fixed eye point texture expression)
 *  Explanation:
 *               Uses the GX_TEXGEN_VERTEX settings in G3_TexImageParam() to perform fixed
 *               eye point texture expression.
 *
 ******************************************************************************
 */
#ifndef TEX_4PLETT_H_
#define TEX_4PLETT_H_

#include <nitro.h>

/* 4-color palette texture */
extern const u16 tex_4plett64x64[];

/* 4-color palette */
extern const u16 pal_4plett[];

#endif

