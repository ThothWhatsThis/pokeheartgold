#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - MB - demos - cloneboot
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
# Revision 1.4  2004/11/24 11:06:20  yosizaki
# add comment about parent-section.
# delete parent_section.c
#
# Revision 1.3  2004/11/24 10:31:11  yasu
# Change folder name holding libsyacall.a
#
# Revision 1.2  2004/11/24 10:05:14  yosizaki
# change LCFILE_TEMPLATE to use /specfiles.
#
# Revision 1.1  2004/11/15 06:20:48  yosizaki
# initial update.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=


# To use the multiboot functionality, the MB library samples require several development  
# machines having the same communication environment (wired or wireless).  
# The mb_child.bin program, in the directory  
# is a sample that provides the same functionality as the  
# multiboot child on the final production model.  
# Use the same method as used in the sample program to read this binary into the other units,  
# and execute it in these units at the same time.  

#----------------------------------------------------------------------------


#----------------------------------------------------------------------------
# The source files used with this program are shown below.
#----------------------------------------------------------------------------

SRCDIR		= ./src
INCDIR		= ./include

SRCS		= main.c		\
		  common.c		\
		  disp.c		\
		  font.c		\
		  child.c		\
		  parent.c		\
		  gmain.c

# To simplify the samples, use WH modules.

WH_DIR		=  $(ROOT)/build/demos/wireless_shared/wh
SRCDIR		+= $(WH_DIR)
INCDIR		+= $(WH_DIR)
SRCS		+= $(WH_DIR)/wh.c

# To simplify the samples, use MBP modules.

MBP_DIR		=  $(ROOT)/build/demos/wireless_shared/mbp
SRCDIR		+= $(MBP_DIR)
INCDIR		+= $(MBP_DIR)
SRCS		+= $(MBP_DIR)/mbp.c


#----------------------------------------------------------------------------
# The general application settings are shown below.
#----------------------------------------------------------------------------

TARGET_NEF	= main.nef
TARGET_BIN	= main.srl
ROM_SPEC	= main.rsf
NITRO_MAKEROM	= TRUE

# When doing complex processing in an MB callback,  
# make the IRQ stack size in the lcf file is sufficiently large.  
# Since OS_Printf() uses a lot of stack space, in the callback,  
# use OS_TPrintf(), which is lightweight, whenever possible.  
ifeq	($(NITRO_PLATFORM),TEG)
MAKELCF_FLAGS	+= -DADDRESS_MAIN=0x02004000
else
MAKELCF_FLAGS	+= -DADDRESS_MAIN=0x02000000
endif
# LCFILE_SPEC	= main.lsf

# To compress each segment of a program, enable this flag.
# The compressed segments will have smaller binary images on the CARD-ROM,  
# and will load faster from the CARD-ROM or wireless connection.  
# However, when executed, they require extra CPU time to decompress.  
NITRO_COMPRESS	= TRUE

# When using overlays for the wireless download child,  
# you must enable NITRO_DIGEST.  
NITRO_DIGEST	= TRUE


#----------------------------------------------------------------------------
# Special settings for the clone boot parent are shown below.
#----------------------------------------------------------------------------

# For security purposes, specify a link-setting template  
# for the clone boot that creates a .parent section exclusively for the parent's use.  
#
# The contents of the .parent section are not sent to the child machine in DS download play.
# By placing functions exclusive to and required by the parent in this section,  
# a machine that intercepts a transmission cannot duplicate the parent.  
# When positioning the code, please note the following:
#   -Do not include anything other than the .text/.rodata section
#   -Include important routines required only by the parent  
# 
# Code in the .parent section is arranged as follows.  
# 
#     #include <nitro/parent_begin.h>
#
#     void  ForParentOnly(void)
#     {
#         /* Processing exclusive to the parent goes here */
#     }
#
#     #include <nitro/parent_end.h>
#
# In this sample demo, all the functions in parent.c are placed in the .parent section.
# For details, see the source code.

LCFILE_TEMPLATE	= $(NITRO_SPECDIR)/ARM9-TS-cloneboot-C.lcf.template

# Specify the official system call library distributed  
# for each commercial application.  
# For this sample, the SDK version (lib/ARM9-TS/etc/libsyscall.a)  
# has been placed in this directory provisionally.  
# If you make an application based on this demo, overwrite the provisional  
# Overwrite this provisional system call with the
# libsyscal.a that is being distributed.

LIBSYSCALL		= ./etc/libsyscall.a
LIBSYSCALL_CHILD	= ./etc/libsyscall_child.bin


#----------------------------------------------------------------------------


include	$(NITROSDK_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------
# Here we describe the pre-authentication process used to run the application on the production model.
#----------------------------------------------------------------------------

# The following a is rough outline of pre-authentication.
# For details, see "DS Download Play".
# (/docs/TechnicalNotes/AboutMultiBoot.doc)
#
# (0) If clone boot, use the emuchild command to prepare the program to send.
#     If not, use main.srl without modification.
#     In this makefile, the prepared program is bin/sign.srl.
#
# (1) To obtain the authentication code, send tmp/sign.srl, which was prepared in Step 0, to the server.
#     You must do this by hand using your browser.
#     In this makefile, the received authentication code is bin/sign.sgn.
#
# (2) Using the attachsign command, generate the final program, linking  
#     bin/sign.sgn obtained in Step 1 with main.srl.  
#     In this makefile, the final file is main_with_sign.srl.  
#

# For a clone boot, do not send the parent program as per normal DS download play.  
# Instead send the program processed with emuchild to the server.  
presign:
	$(EMUCHILD) \
		bin/$(NITRO_BUILDTYPE)/$(TARGET_BIN) \
		$(LIBSYSCALL_CHILD) \
		bin/sign.srl

# The process of including the obtained authentication code in the binary is  the same in both clone boot and normal operation.
postsign:
	$(ATTACHSIGN) \
		bin/$(NITRO_BUILDTYPE)/$(TARGET_BIN) \
		bin/sign.sgn  \
		main_with_sign.srl

#----------------------------------------------------------------------------

do-build:	$(TARGETS)

include	$(NITROSDK_ROOT)/build/buildtools/modulerules

ifdef	NITRO_ELFTOBIN
$(TARGETS): $(ELFTOBIN_ARM7)

else	#NITRO_ELFTOBIN
$(TARGETS): $(MAKEROM_ARM7)

endif	#NITRO_ELFTOBIN


#===== End of Makefile =====

