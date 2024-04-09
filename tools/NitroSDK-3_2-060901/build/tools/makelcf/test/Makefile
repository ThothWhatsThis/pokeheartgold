#! make -f
#---------------------------------------------------------------------------
#  Project:  NitroSDK - tools - makelcf
#  File:     Makefile
#
#  Copyright 2003-2006 Nintendo.  All rights reserved.
#
#  These coded instructions, statements, and computer programs contain
#  proprietary information of Nintendo of America Inc. and/or Nintendo
#  Company Ltd., and are protected by Federal copyright law.  They may
#  not be disclosed to third parties or copied or duplicated in any form,
#  in whole or in part, without the prior written consent of Nintendo.
#
#  $Log: Makefile,v $
#  Revision 1.19  05/11/2006 12:55:47  yasu
#  changed creation year
#
#  Revision 1.18  05/10/2006 03:18:56  yasu
#  Check the IF-ELSE-ENDIF conditional test and test the searchsymbol sentence
#
#  Revision 1.17  08/29/2005 00:17:13  yasu
#  Updated copyright year
#
#  Revision 1.16  08/29/2005 12:29  yasu
#  Removed -d option.
#
#  Revision 1.15  08/26/2005 11:23:38  yasu
#  overlay support for ITCM/DTCM
#
#  Revision 1.14  08/26/2005 04:35:19  yasu
#  Corrected problem where the arena would become funny when using DTCM ovelay
#
#  Revision 1.13  08/05/2004 13:37:28  yasu
#  Add test code for -M option
#
#  Revision 1.12  07/19/2004 05:08:56  yasu
#  Add rule for super-clobber
#
#  Revision 1.11  07/08/2004 02:59:29  yasu
#  Test section name for multi-objects as 'Objects' parameters
#
#  Revision 1.10  06/24/2004 15:58  yasu
#  add test files
#
#  Revision 1.9  05/27/2004 09:58:58  yasu
#  fix specfiles directory to current position
#
#  Revision 1.8  02/16/2004 03:49:21  yasu
#  change switch name CODEGEN_PROC -> NITRO_PROC
#
#  Revision 1.7  02/05/2004 07:09:03  yasu
#  change SDK prefix iris -> nitro
#
#  Revision 1.6  02/03/2004 07:37:16  yasu
#  Add target "autotest"
#
#  Revision 1.5  01/15/2004 13:25:37  yasu
#  add test code
#
#  Revision 1.4  01/15/2004 10:51:24  yasu
#  change a few filenames
#
#  Revision 1.3  01/14/2004 01:59:02  yasu
#  Change filename
#
#  Revision 1.2  01/07/2004 13:11:08  yasu
#  change command casing
#
#  Revision 1.1  01/05/2004 02:32:59  yasu
#  Initial version
#
#  $NoKeywords: $
#---------------------------------------------------------------------------
include	$(NITROSDK_ROOT)/build/buildtools/commondefs

TARGET		=	simple1.lcf simple2.lcf test.autogen.lcf ARM9-TEG.autogen.lcf overlay.autogen.lcf
TARGET		+=	test2.lcf

LCFILE_TEMPLATE =	$(NITRO_INCDIR)/nitro/specfiles/$(NITRO_PROC)-$(NITRO_PLATFORM).lcf.template

%.autogen.lcf:		%.lsf $(MAKELCF) $(LCFILE_TEMPLATE)
			$(MAKELCF) $(MAKELCF_FLAGS) $< $(LCFILE_TEMPLATE) $@

simple1.lcf:		simple.lsf $(MAKELCF) simple.lcf.template
			$(MAKELCF) $(MAKELCF_FLAGS) -V1 $< simple.lcf.template $@

simple2.lcf:		simple.lsf $(MAKELCF) simple.lcf.template
			$(MAKELCF) $(MAKELCF_FLAGS) -V2 $< simple.lcf.template $@

test2.lcf:		test2.lsf $(MAKELCF) test2.def
			$(MAKELCF) $(MAKELCF_FLAGS) -Mtest2.def $< $(LCFILE_TEMPLATE) $@

MAKELCF =		$(NITRO_TOOLSDIR)/bin/makelcf.exe

.PHONY:			build install do-autotest clean clobber

define ECHO_CURDIR
			echo "==== $(CURDIR)";
endef

build:			
			@$(ECHO_CURDIR)
			@$(MAKE) $(TARGET)

install do-autotest:
			@$(ECHO_CURDIR)

clean clobber super-clobber:
			@$(ECHO_CURDIR)
			-rm -f $(TARGET) *~
