/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - demos - multiboot-wfs
  File:     func_2.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: func_2.c,v $
  Revision 1.1  06/23/2005 09:05:39  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include	<nitro.h>

#include	"func.h"


/******************************************************************************/
/* function */

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
}                                      /* extern "C" */
#endif


/******************************************************************************/

