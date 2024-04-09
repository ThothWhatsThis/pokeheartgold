/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - demos - file-1
  File:     foo.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: foo.c,v $
  Revision 1.5  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.4  02/28/2005 05:26:01  yosizaki
  do-indent.

  Revision 1.3  04/13/2004 12:08:47  yosizaki
  fix header comment.

  Revision 1.2  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.1  2004/01/15 07:32:17  yasu
  Test for ROM Image construction

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include	<nitro.h>

void    function_Foo(void);
void function_Foo(void)
{
    OS_Printf("Hello, I am Foo.\n");
    return;
}
