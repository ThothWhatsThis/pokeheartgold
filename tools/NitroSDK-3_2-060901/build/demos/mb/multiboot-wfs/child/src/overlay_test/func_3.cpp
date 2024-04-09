/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - demos - multiboot-wfs
  File:     func_3.cpp

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: func_3.cpp,v $
  Revision 1.1  06/23/2005 09:05:39  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include	<nitro.h>

#include	"func.h"


/******************************************************************************/
/* local implement */

namespace
{

/* class for the constructor/destructor test*/
    class   Foo
    {
      private:
        const int line_;
      public:
                Foo(int line):line_(line)
        {
            OS_TPrintf("func_3_cpp::Foo::constructor called. (object is at func_3_cpp:%d)\n",
                       line_);
        }
               ~Foo()
        {
            OS_TPrintf("func_3_cpp::Foo::destructor called. (object is at func_3_cpp:%d)\n", line_);
        }
    };

}                                      // unnamed namespace


/******************************************************************************/
/* variable */

static int i = 0;

/*
 * Constructed at the time of FS_StartOverlay().
 * Destructed at the time of FS_EndOverlay().
 */

static Foo foo1(__LINE__);


/******************************************************************************/
/* function */

#if	defined(__cplusplus)
extern  "C"
{
#endif


    void    func_3(char *dst)
    {
        /*
         * Constructed when first called.
         * If construction has occurred, destruction occurs at the time of FS_EndOverlay().
         */

        static Foo foo2(__LINE__);

              ++i;
                (void)OS_SPrintf(dst, "func_3. called %d time%s.\n", i, (i == 1) ? "" : "s", &i);
    }


#if	defined(__cplusplus)
}                                      /* extern "C" */
#endif


/******************************************************************************/

