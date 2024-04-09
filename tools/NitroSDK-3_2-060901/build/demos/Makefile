#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - demos
# File:     Makefile
#
# Copyright 2003-2006 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.52  07/05/2006 07:44:46  kitase_hirotake
# Added the sjis <-> unicode conversion library to std
#
# Revision 1.51  03/28/2006 04:42:01  okubata_ryoma
# undo
#
# Revision 1.50  03/27/2006 11:42:34  okubata_ryoma
# test
#
# Revision 1.49  03/02/2006 10:04:43  okubata_ryoma
# Revised copyright
#
# Revision 1.48  03/02/2006 10:03:34  okubata_ryoma
# Added vib
#
# Revision 1.47  09/05/2005 01:22:46  yada
# add env
#
# Revision 1.46  07/07/2005 07:45:30  yasu
# Treated utest as beta
#
# Revision 1.45  06/24/2005 02:18:20  yasu
# Added utest
#
# Revision 1.44  04/13/2005 13:59:59  terui
# Corrected multiple registrations of snd in SUBDIRS.
#
# Revision 1.43  04/13/2005 03:30:31  ida
# Added snd.
#
# Revision 1.42  02/16/2005 10:38:32  terui
# made revisions so that WVR is always built
#
# Revision 1.41  01/14/2005 06:22:06  terui
# Added WVR to SUBDIRS
#
# Revision 1.40  12/22/2004 02:49:43  terui
# Add cht in SUBDIRS
#
# Revision 1.39  12/14/2004 13:19:39  takano_makoto
# Added math
#
# Revision 1.38  09/13/2004 11:46:52  yada
# add ctrdg
#
# Revision 1.37  09/13/2004 06:20:06  yosizaki
# add wbt.
#
# Revision 1.36  09/09/2004 12:48:39  yosizaki
# add card demos.
#
# Revision 1.35  08/10/2004 07:31:18  yosizaki
# add mb demo.
#
# Revision 1.34  07/24/2004 05:43:11  yasu
# Link to wm/
#
# Revision 1.33  07/13/2004 12:31:55  yasu
# delete libcxx workaround
#
# Revision 1.32  07/12/2004 09:10:13  yasu
# Support e3s libraires on TEG only.
#
# Revision 1.31  06/29/2004 00:58:54  yasu
# build libcxx again for workaround against lcf error
#
# Revision 1.30  06/23/2004 12:02:11  seiki_masashi
# add 'prc/characterRecognition-1'
#
# Revision 1.29  06/10/2004 01:42:06  yasu
# fix checklist gen
#
# Revision 1.28  06/09/2004 11:21:55  yasu
# Makefile Picks up only public demos
#
# Revision 1.27  06/09/2004 10:56:46  yasu
# Add demo check list generator
#
# Revision 1.26  05/31/2004 03:24:12  yasu
# merge e3s gpio package
#
# Revision 1.25  05/24/2004 07:02:41  yasu
# Hide ide sample, because MW provides offical ide samples
#
# Revision 1.24  05/17/2004 05:27:36  takano_makoto
# Add CP demos.
#
# Revision 1.23  05/12/2004 05:36:20  terui
# Add spi , rtc in SUBDIRS.
#
# Revision 1.22  05/06/2004 10:18:18  yasu
# fix typo
#
# Revision 1.21  03/31/2004 13:55:44  takano_makoto
# Add tips directory
#
# Revision 1.20  03/05/2004 04:22:57  yasu
# add PXI
#
# Revision 1.19  02/27/2004 05:36:37  yasu
# template_sp into private
#
# Revision 1.18  02/23/2004 06:41:51  ida
# hide snd library
#
# Revision 1.17  02/23/2004 06:33:47  yasu
# NITRO_PRIVATES -> NITRO_PRIVATE
#
# Revision 1.16  02/18/2004 06:31:54  ida
# changed snd demo to Nindendo confidential
#
# Revision 1.15  02/06/2004 08:17:48  yasu
# small fix
#
# Revision 1.14  02/05/2004 07:09:02  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.13  01/20/2004 07:02:19  yasu
# Added ide
#
# Revision 1.12  01/18/2004 05:15:31  yada
# Added MI directory
#
# Revision 1.11  01/15/2004 13:22:29  yasu
# Makefile chains to fs/
#
# Revision 1.10  12/18/2003 09:49:37  nishida_kenji
# add fx
#
# Revision 1.9  12/16/2003 08:09:08  yasu
# Temporarily disconnect the template_sp link
#
# Revision 1.8  12/16/2003 07:39:25  yasu
# Added templateSP
#
# Revision 1.7  12/11/2003 07:57:44  yasu
# _template/ -> template/ revision
#
# Revision 1.6  11/25/2003 09:52:39  yasu
# Added os
#
# Revision 1.5  11/14/2003 10:55:48  yada
# Added the pad directory
#
# Revision 1.4  11/05/2003 04:46:31  yasu
# Added _template
#
# Revision 1.3  11/04/2003 10:16:06  yasu
# Added a chain to gx
#
# Revision 1.2  10/30/2003 05:51:52  yasu
# build: deleted target
#
# Revision 1.1  10/29/2003 13:27:27  yasu
# Completed the make propogation framework
# Made all/install/clean/clobber available
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/commondefs
include	$(NITROSDK_ROOT)/build/buildtools/commondefs.cctype.CW

#----------------------------------------------------------------------------

SUBDIRS		= template		\
		  gx 			\
		  pad			\
		  os			\
		  fx			\
		  card			\
		  fs			\
		  mi			\
		  tips			\
		  spi			\
		  rtc			\
		  cp			\
		  prc			\
		  wm			\
		  wbt			\
		  mb			\
		  ctrdg			\
		  math			\
		  cht			\
		  wvr			\
		  snd			\
		  env			\
		  vib           \
		  std

ifdef	NITRO_WITH_ARM7
SUBDIRS		+= pxi			\
		   utest		\
		   template_sp
ifeq	($(NITRO_PLATFORM),TEG)
SUBDIRS		+= e3s
endif
endif

#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/modulerules

XMLFILE		=	_checklist.xml
XMLGROUP	=	^gx\\/UnitTours\\/2/GX_2D		\
			^gx\\/UnitTours\\/3/GX_3D		\
			^gx\\/UnitTours\\/[^23]/GX_MISC		\
			^[mo]/OS				\
			^[a-fh-lnp-z]/MISC

make_list:
	# Make a list of public demos
	DEMODIRS="`/bin/find . -name src -exec dirname {} \; | sed '/\/_/d' | sed 's/^..//g'`"; \
	for i in $$DEMODIRS; do									\
		if [ ! \( -e $$i'/_' -o -e $$i'/@' -o -e $$i'/../_' -o -e $$i'/../@' \) ]; then	\
			echo $$i;								\
		fi;										\
	done > $(XMLFILE).tmp
	# Make XML files
	for i in $(XMLGROUP); do \
		REGEX=`dirname $$i`; FNAME='_checklist_'`basename $$i`'.xml' ;\
		echo '<?xml-stylesheet type="text/xsl" href="_checklist.xsl"?>'               >> $$FNAME; \
		echo '<ALL>'                                                       	      >> $$FNAME; \
		gawk '/'$$REGEX'/{ print "<DEMO><NAME>"$$1"</NAME></DEMO>"; }' $(XMLFILE).tmp >> $$FNAME; \
		echo '</ALL>'                                                  	              >> $$FNAME; \
	done
	rm $(XMLFILE).tmp

#===== End of Makefile =====
