#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - MB - demos - multiboot-PowerSave
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
# Revision 1.1  2005/04/28 07:15:06  yosizaki
# initial upload.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=	


# The sample program of the wireless library wirelessMB makes use
# of wireless features, so you need to prepare a communications 
# environment (wireless or wired LAN) with a number of 
# development machines.
# The mb_child.srl program in the $NitroSDK/bin/ARM9-TS/Rom/ 
# directory is a sample program that provides the same features
# as the DS Download child in the IPL of a retail machine.  Load 
# this binary to the other machines using the same method as the
# sample program and execute on these machines at the same time.


#----------------------------------------------------------------------------

SRCS		=	common.c data.c dispfunc.c menu.c parent.c main.c

TARGET_NEF	=	main.nef
TARGET_BIN	=	main.srl
ROM_SPEC	=	main.rsf
LCFILE_SPEC	=	main.lsf

ifeq	($(NITRO_PLATFORM),TEG)
MAKELCF_FLAGS	+=	-DADDRESS_MAIN=0x02004000
else
MAKELCF_FLAGS	+=	-DADDRESS_MAIN=0x02000000
endif

# makerom settings
MAKEROM_ROMROOT		=	./data
MAKEROM_ROMFILES	=	*.srl


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

