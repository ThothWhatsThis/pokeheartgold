#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - WVR - demos
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
# Revision 1.1  2005/01/14 06:25:50  terui
# Moved from tests to demos.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

# Creates child boot binary
SUBDIRS		=	child


# To use the multiboot functionality, the MB library samples require several development  
# machines having the same communication environment (wired or wireless).  
# The mb_child.bin program, in the directory  
# is a sample that provides the same functionality as the  
# multiboot child on the final production model.  
# Use the same method as used in the sample program to read this binary into the other units,  
# and execute it in these units at the same time.  


#----------------------------------------------------------------------------

INCDIR		=	common/include parent/include
SRCDIR		=	common/src parent/src
SRCS		=	main.c      \
			common.c    \
			disp.c      \
			font.c      \
			gmain.c

LLIBRARIES	=	libwvr$(CODEGEN).a

# WH module was moved to wireless_shared.

WH_DIR		=	$(ROOT)/build/demos/wireless_shared/wh
SRCDIR		+=	$(WH_DIR)
INCDIR		+=	$(WH_DIR)
SRCS		+=	$(WH_DIR)/wh.c

# MBP was moved to wireless_shared.

MBP_DIR		=	$(ROOT)/build/demos/wireless_shared/mbp
SRCDIR		+=	$(MBP_DIR)
INCDIR		+=	$(MBP_DIR)
SRCS		+=	$(MBP_DIR)/mbp.c


TARGET_NEF	=	main.nef
TARGET_BIN	=	main.srl

ROM_SPEC	=	main.rsf

# When doing complex processing in an MB callback,  
# make the IRQ stack size in the lcf file is sufficiently large.  
# Since OS_Printf() uses a lot of stack space, in the callback,  
# use OS_TPrintf(), which is lightweight, whenever possible.  

# ifeq	($(NITRO_PLATFORM),TEG)
# MAKELCF_FLAGS	+=	-DADDRESS_MAIN=0x02004000
# else
# MAKELCF_FLAGS	+=	-DADDRESS_MAIN=0x02000000
# endif
# LCFILE_SPEC	=	main.lsf

# makerom settings
MAKEROM_ROMROOT		=	./data
MAKEROM_ROMFILES	=	*.srl

#------------------------------------------------------#
# The component definition that the wireless library places inside VRAM  #
MY_COMPONENT_NAME	= ichneumon
MAKEROM_ARM7	= $(NITRO_COMPONENTSDIR)/$(MY_COMPONENT_NAME)/$(NITRO_BUILDTYPE_ARM7)/$(MY_COMPONENT_NAME)_sub$(TS_VERSION).nef
#------------------------------------------------------#

include	$(NITROSDK_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------

do-build:	$(TARGETS)

include	$(NITROSDK_ROOT)/build/buildtools/modulerules

ifdef	NITRO_ELFTOBIN
$(TARGETS): $(ELFTOBIN_ARM7)

else	#NITRO_ELFTOBIN
$(TARGETS): $(MAKEROM_ARM7)

endif	#NITRO_ELFTOBIN


#===== End of Makefile =====

