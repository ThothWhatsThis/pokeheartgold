/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - makelcf
  File:     makelcf.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: makelcf.c,v $
  Revision 1.11  05/10/2006 02:06:00  yasu
  Added support for CodeWarrior 2.x

  Revision 1.10  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.9  02/28/2005 05:26:03  yosizaki
  do-indent.

  Revision 1.8  08/05/2004 13:50:01  yasu
  Support -M option

  Revision 1.7  08/05/2004 13:38:44  yasu
  Support -M option

  Revision 1.6  07/15/2004 05:20:53  yasu
  Support showing version number at help screen

  Revision 1.5  03/26/2004 05:07:11  yasu
  support variables like as -DNAME=VALUE

  Revision 1.4  02/05/2004 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.3  01/14/2004 12:38:08  yasu
  Change OverlayName->OverlayDefs

  Revision 1.2  01/07/2004 13:10:17  yasu
  fix all warning at compile -Wall

  Revision 1.1  01/05/2004 02:32:59  yasu
  Initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include	<stdio.h>
#include	<stdlib.h>              // atoi()
#include	<ctype.h>
#include	<getopt.h>              // getopt()
#include	"makelcf.h"
#include	"defval.h"
#include	"misc.h"
#include	"version.h"

/*---------------------------------------------------------------------------*
 *  MAIN
 *---------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    int     n;
    int     narg;
    int     t;
    
    while ((n = getopt(argc, argv, "hdD:M:V:")) != -1)
    {
        switch (n)
        {
        case 'h':
            goto usage;

        case 'd':
            DebugMode = TRUE;
            break;

        case 'D':
            AddDefVal(optarg);
            break;

        case 'M':
            if (!AddDefValFromFile(optarg))
            {
                fprintf(stderr, "Stop.\n");
                return 1;
            }
            break;

        case 'V':
            t = atoi(optarg);
            if (t >= 1)
            {
                char str[128];
                sprintf(str, "NITRO_LCFSPEC=%d", t);	// NITRO_LCFSPEC is defined
                AddDefVal(str);
            }
            else
            {
                fprintf(stderr, "Unknown version number [%s]. Stop.\n", optarg);
                return 1;
            }
            break;
            
        default:
            break;
        }
    }

    narg = argc - optind;

    if (narg == 2 || narg == 3)
    {
        if (ParseSpecFile(argv[optind]))
        {
            fprintf(stderr, "Stop.\n");
            return 1;
        }

        if (ParseTlcfFile(argv[optind + 1]))
        {
            fprintf(stderr, "Stop.\n");
            return 1;
        }

        if (CreateLcfFile(narg == 3 ? argv[optind + 2] : NULL))
        {
            fprintf(stderr, "Stop.\n");
            return 1;
        }
        return 0;
    }

  usage:
    {
        fprintf(stderr,
                "NITRO-SDK Development Tool - makelcf - Make linker command file\n"
                "Build %lu\n\n"
                "Usage:  makelcf [-DNAME=VALUE...] [-MDEFINES_FILE] [-V1|-V2] SPECFILE LCF-TEMPALTE [LCFILE]\n\n",
                SDK_DATE_OF_LATEST_FILE);
    }
    return 2;
}
