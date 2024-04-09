#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - WM - demos - wireless-all
# File:     Makefile
#
# Copyright 2006 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.1  04/10/2006 13:07:23  yosizaki
# initial upload.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=


#----------------------------------------------------------------------------
# The source files used with this program are shown below.
#----------------------------------------------------------------------------

SRCDIR		= ./src
INCDIR		= ./include

# common parent processes
SRCS		=	main.c	\
				common.c	\
				test.c

# wireless driver processes (simple WH wrapper)
SRCS		+=	wh_measure.c	\
				wh_download.c	\
				wh_datasharing.c

# Uses the WH (wireless sample module).
WH_DIR		=  $(ROOT)/build/demos/wireless_shared/wh
LINCLUDES	+= $(WH_DIR)
SRCS		+= $(WH_DIR)/wh.c

# Uses the MDP (DS Download Play sample module).
MBP_DIR		=  $(ROOT)/build/demos/wireless_shared/mbp
LINCLUDES	+= $(MBP_DIR)
SRCS		+= $(MBP_DIR)/mbp.c

# Uses the WFS (wireless file system sample module).
WFS_DIR		=  $(ROOT)/build/demos/wireless_shared/wfs
LINCLUDES	+= $(WFS_DIR)/include
SRCS		+= \
	$(WFS_DIR)/src/wfs_common.c	\
	$(WFS_DIR)/src/wfs_archive.c	\
	$(WFS_DIR)/src/wfs_thread.c


#----------------------------------------------------------------------------
# The resources used when building this program are shown below.
#----------------------------------------------------------------------------

# The directory that stores the data from .dat to .o format in bin2obj.exe.
DATA_DIR	= ./data/image

# The wireless link strength icon.
WLICON_DIR	= $(ROOT)/data/wl_icons
WLICON_BMP	= $(wildcard $(WLICON_DIR)/wireless_strength_level_*_b.bmp)
WLICON_OBJS	= $(patsubst $(WLICON_DIR)/wireless_strength_level_%_b.bmp,$(OBJDIR)/linkimg%.o,$(WLICON_BMP))
EXT_OBJS	+=	$(WLICON_OBJS)

# BG character for ASCII text display.
BGCHAR_OBJS	= $(patsubst $(DATA_DIR)/%.dat,$(OBJDIR)/%.o,$(wildcard $(DATA_DIR)/*.dat))
EXT_OBJS	+=	$(BGCHAR_OBJS)


#----------------------------------------------------------------------------
# The general application settings are shown below.
#----------------------------------------------------------------------------

TARGET_NEF	= main.nef
TARGET_BIN	= main.srl
NITRO_MAKEROM	= TRUE
MAKEROM_ROMROOT		= ./data/rom
MAKEROM_ROMFILES	= *

# To compress each segment of a program, enable this flag.
# The compressed segments will have smaller binary images on the CARD-ROM,
# and will load faster from the CARD-ROM or wireless connection.
# However, when executed, they require extra CPU time to decompress.

NITRO_COMPRESS	= TRUE

# When using overlays for the wireless download child,  
# NITRO_DIGEST must be enabled for security purposes.

NITRO_DIGEST	= TRUE


#----------------------------------------------------------------------------
# Special settings for the clone boot parent are shown below.
#----------------------------------------------------------------------------

# For security purposes, specify a link-setting template
# for the clone boot that creates a .parent section exclusively for the parent's use.

LCFILE_TEMPLATE	= $(NITRO_SPECDIR)/ARM9-TS-cloneboot-C.lcf.template

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
# In this sample program, all of the DS Download Play parent process functions
# are placed in the .parent section. For more details, see the source code.


# Specify the official system call library distributed
# for each commercial application.
# For this sample, the SDK version (lib/ARM9-TS/etc/libsyscall.a)
# has been placed in this directory in advance.
# If you make an application based on this demo, overwrite the provisional
# system call with the libsyscal.a that is
# being distributed.

LIBSYSCALL		= ./etc/libsyscall.a
LIBSYSCALL_CHILD	= ./etc/libsyscall_child.bin


#----------------------------------------------------------------------------


include	$(NITROSDK_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------
# Here we describe the signature process used to run the application on the production model.
#----------------------------------------------------------------------------

# The following a is rough outline of signature processes.
# For details, see "DS Download Play".
# (/docs/TechnicalNotes/AboutMultiBoot.pdf)
#
# (0) If clone boot, use the emuchild command to prepare the program to send.
#     If not, use main.srl without modification.
#     In this makefile, the prepared program is bin/sign.srl.
#
# (1) To obtain the signature code, send tmp/sign.srl, which was prepared in Step 0, to the server.
#     You must do this by hand using your browser.
#     In this makefile, the received signature code is bin/sign.sgn.
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

# The process of including the obtained signature code in the binary is the same in both clone boot and normal operation.
postsign:
	$(ATTACHSIGN) \
		bin/$(NITRO_BUILDTYPE)/$(TARGET_BIN) \
		bin/sign.sgn  \
		main_with_sign.srl

#----------------------------------------------------------------------------

do-build:	$(TARGETS)


include	$(NITROSDK_ROOT)/build/buildtools/modulerules

# The bitmap for the link strength icon is converted to binary data.
$(DATA_DIR)/linkimg%.dat : $(WLICON_DIR)/wireless_strength_level_%_b.bmp
	$(NTEXCONV) -no -bg -bgb -bgf palette16 $< >/dev/null && \
	cat	$(basename $<).nbfp $(basename $<).nbfc > $(basename $@).dat && \
	rm	$(basename $<).nbfp $(basename $<).nbfc $(basename $<).nbfs

# Al of the binary data is linked as a constant array.
$(OBJDIR)/%.o : $(DATA_DIR)/%.dat
	$(BIN2OBJ) --compatible $< $@ >/dev/null


#===== End of Makefile =====
