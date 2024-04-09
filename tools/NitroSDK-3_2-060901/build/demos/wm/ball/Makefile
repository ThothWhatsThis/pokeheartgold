#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - WM - demos
# File:     Makefile
#
# Copyright 2003,2004 Nintendo. All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law. They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.2  2004/07/26 02:07:38  terui Improvement:
# Deleted libwm.a from local library
#
# Revision 1.1  2004/07/23 15:11:24  terui Improvement:
# Revisions and additions.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=

#----------------------------------------------------------------------------

SRCS			=	main.c data.c keysharing.c ball.c

TARGET_BIN		=	main.srl

LINCLUDES		=	../wmDEMOlib/include

LLIBRARY_DIRS	=	../wmDEMOlib/lib/$(NITRO_BUILDTYPE)

LLIBRARIES		=	libwmDEMO$(NITRO_LIBSUFFIX).a

include	$(NITROSDK_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====

