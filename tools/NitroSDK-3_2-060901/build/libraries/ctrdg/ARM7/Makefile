#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - libraries_sp - ctrdg
# File:     Makefile
#
# Copyright 2003-2005 Nintendo. All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law. They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.3  2005/03/06 23:52:59  terui
# Update copyright.
#
# Revision 1.2  2005/03/05 02:02:04  terui
# Made changes so RELEASE build options are available for DEBUG builds
#
# Revision 1.1  2004/09/13 10:45:20  yada
# initial release.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=

#----------------------------------------------------------------------------

# build ARM & THUMB libraries
NITRO_CODEGEN_ALL =	TRUE

# Codegen for sub processer
NITRO_PROC	=	ARM7

SRCDIR		=	../common/src src

SRCS		=	ctrdg.c	\
			ctrdg_sp.c		\


TARGET_LIB	=	libctrdg_sp$(NITRO_LIBSUFFIX).a


#----------------------------------------------------------------------------

# Implemented DEBUG-version library so DEBUG builds
# are built like RELEASE-version builds

ifdef	NITRO_DEBUG
NITRO_BUILD_TYPE	= RELEASE
endif

include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS	=	$(TARGETS)
INSTALL_DIR	=	$(NITRO_INSTALL_LIBDIR)


#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(NITROSDK_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====

