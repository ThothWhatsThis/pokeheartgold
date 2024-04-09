/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WBT - demos - wbt-fs
  File:     func_2.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: func_2.c,v $
  Revision 1.5  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.4  02/28/2005 05:26:13  yosizaki
  do-indent.

  Revision 1.3  09/29/2004 07:28:40  yosizaki
  change OVAERLAY_CODE_BY_10000 to OVAERLAY_CODE_BY_1000

  Revision 1.2  09/29/2004 07:02:13  yosizaki
  change interface.

  Revision 1.1  09/04/2004 13:47:06  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include	<nitro.h>

#include	"func.h"


/******************************************************************************/
/* function*/

#if	defined(__cplusplus)
extern  "C"
{
#endif


    void    func_2(char *dst)
    {
        static int i = 0;
              ++i;
                (void)OS_SPrintf(dst, "func_2. called %d time%s.\n", i, (i == 1) ? "" : "s", &i);

        /* this is dummy code use to redundantly increase the code size*/
        if      ((i > 0) && (i < 0))
        {
            volatile int v;
                    OVAERLAY_CODE_BY_1000(++v);
        }
    }


#if	defined(__cplusplus)
}                                      /* extern "C"*/
#endif


/******************************************************************************/
