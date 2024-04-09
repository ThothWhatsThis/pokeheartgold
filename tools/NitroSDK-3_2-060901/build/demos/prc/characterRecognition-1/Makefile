#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - demos - PRC
# File:     Makefile
#
# Copyright 2004-2005 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.4  10/04/2005 02:59:14 AM  seiki_masashi
# Made parameter data shared
#
# Revision 1.3  09/03/2004 01:28:31 PM  seiki_masashi
# From ferret to mongoose
#
# Revision 1.2  07/01/2004 02:25:48  yasu
# .bin -> .srl
#
# Revision 1.1  06/23/2004 12:02:11 PM  seiki_masashi
# add 'prc/characterRecognition-1'
#
# Revision 1.3  06/22/2004 11:19:00 AM  seiki_masashi
# Operable Version (simple only)
#
# Revision 1.2  06/17/2004 02:31:57 AM  seiki_masashi
# To facilitate recognition algorithm changes, split into prc_algo_* 
#
# Revision 1.1  06/16/2004 11:56:09 PM  seiki_masashi
# Renamed ptrcg to prc.
#
# Revision 1.1  06/15/2004 08:10:55 AM  seiki_masashi
# Made prototype program operable with dummy library 
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		= ../patterns

#----------------------------------------------------------------------------

TARGET_BIN	= main.srl
#TARGET_NEF	= main.nef

SRCDIR		= src ../patterns/src
INCDIR		= include ../patterns/include

SRCS		= main.c patterns.c

#----------------------------------------------------------------------------
include	$(NITROSDK_ROOT)/build/buildtools/commondefs

do-build:		$(TARGETS)

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
