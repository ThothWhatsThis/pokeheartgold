#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - WM - demos
# File:     Makefile
#
# Copyright 2003-2005 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: makefile,v $
# Revision 1.16  11/22/2005 01:06:16  yosizaki
# Updated copyright year
#
# Revision 1.15  11/21/2005 00:41:10  adachi_hiroaki
# Made the parent device selection screen display the number of players joining and their nicknames
#
# Revision 1.14  11/05/2004 04:27:40  sasakis
# Added the lobby screen and parent selection screen, and made the necessary modifications (such as scan related).
#
# Revision 1.13  11/02/2004 07:15:01  sasakis
# forgot to revise makefile
#
# Revision 1.12  11/02/2004 07:12:55  sasakis
# Arranged codes.
#
# Revision 1.11  11/01/2004 04:11:44  sasakis
# Added code, etc to take care of extremely slow processing. (provisional version includes code that is being worked on)
#
# Revision 1.10  10/22/2004 07:35:18 AM  sasakis
# Support for shared wh.
#
# Revision 1.9  10/21/2004 00:43:23  yosizaki
# add SHARED_WH switch.
#
# Revision 1.8  10/06/2004 05:11:35  sasakis
# Addition of a graph display, etc.
#
# Revision 1.7  09/30/2004 00:00:29  takano_makoto
# deleted local configurations
#
# Revision 1.6  09/22/2004 09:39:33  sasakis
# Added a debug screen.
#
# Revision 1.5  09/17/2004 09:13:30  sasakis
# Modified to allow selection of the least crowded channel
# by using MeasureChannel when it is a parent. (conditions are not yet determined.)
#
# Revision 1.4  09/15/2004 10:23:40  sasakis
# Added a radio reception strength icon, modification to make the data size variable, etc.
#
# Revision 1.3  09/10/2004 08:44:52  sasakis
# Added a guideline compliance process, made error-related reconsiderations, etc.
#
# Revision 1.2  09/08/2004 01:52:52  sasakis
# Error / disconnection related improvements.
#
# Revision 1.1  09/07/2004 04:59:42  sasakis
# Initial version registration.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS         =

#----------------------------------------------------------------------------

INCDIR			=	./include
SRCDIR			=	./src
SRCS            = \
                main.c          \
                print.c         \
		key.c		\
		graphics.c	\
                font.c          \
		menu.c		\
		unicode.c	\
                icon.c

WH_DIR		=  $(ROOT)/build/demos/wireless_shared/wh
SRCDIR		+= $(WH_DIR)
INCDIR		+= $(WH_DIR)
SRCS		+= $(WH_DIR)/wh.c
MACRO_FLAGS	+= -DSHARED_WH

TARGET_BIN      = main.srl

include $(NITROSDK_ROOT)/build/buildtools/commondefs

#----------------------------------------------------------------------------

do-build:       $(TARGETS)

include $(NITROSDK_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====
