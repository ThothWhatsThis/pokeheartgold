#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - CARD - demos - card-1trom
# File:     Makefile
#
# Copyright 2005 Nintendo. All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law. They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.1  2005/04/05 11:44:53  yosizaki
# initial upload.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=	


#----------------------------------------------------------------------------


SRCS		=	main.c

TARGET_BIN	=	main.srl

#SRCDIR		=	# using default
#LCFILE		=	# using default


#    If you explicitly specify the default defaultNITRO_ROMSPEED, 
# this gets reflected in the default .rsf file.
#    If you have prepared your own .rsf file, this is described 
# in the Property section as: RomSpeedType  {1TROM|MROM}

NITRO_ROMSPEED	= 1TROM



include	$(NITROSDK_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====

