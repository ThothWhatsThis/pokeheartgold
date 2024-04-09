#! make -f
#----------------------------------------------------------------------------
#  Project:  NitroSDK - buildtools - compiler dependant settings for CW
#  File:     commondefs.cctype.CW
#
#  Copyright 2003-2006 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: commondefs.cctype.CW,v $
# Revision 1.126  2006/06/06 01:00:21  yasu
# Changed the default CodeWarrior path
#
# Revision 1.125  2006/06/06 00:23:06  yasu
# Discontinued support for older versions of the CodeWarrior compiler
#
# Revision 1.124  06/01/2006 00:25:29  okubata_ryoma
# Made revisions so that version.h is no longer used when determining the CodeWarrior version
#
# Revision 1.123  2006/05/02 05:18:20  yasu
# typo fix
#
# Revision 1.122  05/01/2006 01:15:56  okubata_ryoma
# Made changes in line with the change to the method of comparing CW versions
#
# Revision 1.121  04/21/2006 06:22:45  okubata_ryoma
# Deleted the warning control option
#
# Revision 1.120  03/30/2006 06:28:21  okubata_ryoma
# small fix
#
# Revision 1.119  03/30/2006 04:10:53  okubata_ryoma
# prepared working and package versions
#
# Revision 1.118  03/29/2006 09:15:04  okubata_ryoma
# corrected error occurring when verinfo.cw.* was referenced before being generated
#
# Revision 1.117  03/17/2006 00:33:06  yosizaki
# support __declspec(force_export)
#
# Revision 1.116  03/01/2006 08:58:17  okubata_ryoma
# Performed undo because there was a problem with the arithmetic operation library that was changed.
#
# Revision 1.115  02/21/2006 01:37:00  okubata_ryoma
# Changed arithmetic operation library
#
# Revision 1.114  02/20/2006 09:41:18  okubata_ryoma
# small fix
#
# Revision 1.113  02/17/2006 04:58:00  okubata_ryoma
# Added support for CW2.0beta
#
# Revision 1.112  02/14/2006 08:40:53  okubata_ryoma
# Added support for CW2.0beta
#
# Revision 1.111  02/13/2006 08:31:03  okubata_ryoma
# Added support for CW2.0beta
#
# Revision 1.110  11/22/2005 09:11:51  yosizaki
# support SDK_ENABLE_ARM7_PRINT.
#
# Revision 1.109  11/21/2005 02:21:51  terui
# Add SDK_SVC_WAITVBLANK_COMPATIBLE macro flag.
#
# Revision 1.108  2005/07/20 04:51:57  yasu
# Corrected mistaken setting "enable use of pre-compile headers by default"
#
# Revision 1.107  2005/07/20 04:46:26  yasu
# Enable used of pre-compile headers by default
#
# Revision 1.106  2005/06/21 09:56:03  yasu
# Added NITRO_UTEST
#
# Revision 1.105  2005/06/20 12:26:11  yasu
# Changed Surffix to Suffix
#
# Revision 1.104  2005/06/17 03:12:06  yasu
# small fix in comment
#
# Revision 1.103  2005/04/25 10:55:03  yasu
# Switched FPLIB to fastI
#
# Revision 1.102  2005/04/25 08:57:50  yasu
# Enabled re-definition of CW_LIBFP settings in Makefile.
#
# Revision 1.101  2005/03/04 09:39:45  yasu
# Revised the default CodeWarrior path
#
# Revision 1.100  02/18/2005 14:12:17  seiki_masashi
# Revised Copyright notation
#
# Revision 1.99  2005/02/18 13:37:30  yasu
# Added support for switching the warning display with the NITRO_WARNING_STRICT macro
#
# Revision 1.98  2005/02/18 09:01:53  yasu
# Enhanced the warning displays
#
# Revision 1.97  2004/12/27 03:51:22  yasu
# Change CC->MWCC , AS->MWAS, LD->MWLD and AR->MWAR
#
# Revision 1.96  2004/10/05 02:29:54  yasu
# Add LMACRO_FLAGS to MACRO_FLAGS
#
# Revision 1.95  09/24/2004 08:49:02  takano_makoto
# Changed SDK_CW_WARNOFF_SAFESTRB to OFF when it is TS.
#
# Revision 1.94  09/04/2004 09:30:36  yosiokat
# Added SDK_SMALL_BUILD.
#
# Revision 1.93  2004/08/27 05:23:26  yasu
# Set default codewarrior version to 0.6.1
#
# Revision 1.92  2004/08/10 00:13:46  yasu
# Support CodeWarrior 0.6.1
#
# Revision 1.91  2004/08/09 11:30:41  yasu
# Don't set default value of NITRO_TS_VERSION on commondefs to support CW
#
# Revision 1.90  2004/07/31 02:57:38  yasu
# support NITRO_TS_VERSION
#
# Revision 1.89  2004/07/27 09:56:07  yasu
# Stop passing SDK_VERSION_WL value thru make option.
#
# Revision 1.88  2004/07/27 06:07:30  yasu
# Let default CW version number back to 0.5
#
# Revision 1.87  2004/07/13 12:33:02  yasu
# Clean up CW_LIB settings
#
# Revision 1.86  2004/07/10 10:14:08  yasu
# Arrange some macros for Marionea Library
#
# Revision 1.85  2004/07/10 04:18:26  yasu
# Update default CodeWarrior path to V0.6
#
# Revision 1.84  2004/06/28 10:50:49  yasu
# Set "SDK_CW_WA_LIBCXX" as workaround flag
#
# Revision 1.83  2004/06/03 09:10:05  yasu
# Add NITRO_NO_OPT_G to switch to enable -g option for CC/LD debug info
#
# Revision 1.82  2004/06/03 01:11:38  yasu
# Change ARM7's optimizing policy from 'speed' to 'space'
#
# Revision 1.81  2004/05/21 05:11:19  yasu
# Invalidate CW_WA_** for CW 0.5
#
# Revision 1.80  2004/05/20 09:49:25  yasu
# change default CW version to 0.5
#
# Revision 1.79  2004/05/19 00:49:30  yasu
# move workaround TEG vblank problems to commondefs.cctype.CW
# add macro switch for anti-thread policy
#
# Revision 1.78  2004/05/18 02:11:38  yasu
# fix SDK_PROFILE_XXX macro
#
# Revision 1.77  2004/05/18 01:51:15  yasu
# add SDK_PROFILE_XXX macro to switch profiling method
#
# Revision 1.76  2004/05/11 10:20:08  yasu
# combine with 'ferret' as ARM7 component
#
# Revision 1.75  2004/05/11 10:13:34  yasu
# clean up comments
#
# Revision 1.74  05/01/2004 10:04:11  yada
# temporary fix for CW0.5
#
# Revision 1.73  2004/04/22 06:56:33  yasu
# workaround for CW blxeq/blxne problem
#
# Revision 1.72  2004/04/19 04:29:12  yasu
# add -Cpp_exceptions off
#
# Revision 1.71  2004/04/14 11:24:37  yasu
# add NITRO_CONSERVATIVE to degrade optimize level
#
# Revision 1.70  2004/04/14 09:39:17  yasu
# support -profile option
#
# Revision 1.69  2004/04/09 07:06:25  yasu
# fix default ARM7 component
#
# Revision 1.68  2004/04/03 08:43:35  yasu
# with MINGW, support detect CW versions
#
# Revision 1.67  2004/04/03 08:01:43  yasu
# auto-detect CW verinfo and switch strb parameters
#
# Revision 1.65  2004/03/26 13:09:09  yasu
# small fix
#
# Revision 1.64  2004/03/26 08:39:25  yasu
# support NITROSDK_LANG
#
# Revision 1.63  2004/03/26 01:44:15  yasu
# define SDK_CW_WA_OPT4 for finalrom
#
# Revision 1.62  2004/03/16 10:11:30  yasu
# set default CWFOLDER to NITRO ver 0.3
#
# Revision 1.61  2004/03/03 01:09:44  yasu
# add workaround flag SDK_CW_WA_CONSTPOOLS for 'ConstantPools.c'line: 346
#
# Revision 1.60  2004/02/25 09:14:03  yasu
# add cw stubs
#
# Revision 1.59  02/25/2004 06:21:01  nishida_kenji
# Use MSL_C_ARM_i_LE_strb.a instead of MSL_C_Thumb_LE.a.
#
# Revision 1.58  2004/02/23 07:00:51  yasu
# append SDK_CW_WA_OPT4
#
# Revision 1.57  2004/02/23 05:01:38  yasu
# axf->nef
#
# Revision 1.56  2004/02/18 01:00:56  yasu
# add switch SDK_CW_WARNOFF_SAFESTRB
#
# Revision 1.55  2004/02/16 01:24:24  yasu
# support file dependancies
#
# Revision 1.54  2004/02/14 07:33:34  yasu
# move components directory
#
# Revision 1.53  2004/02/14 06:38:08  yasu
# workaround for "components"
#
# Revision 1.52  2004/02/13 10:15:38  yasu
# support ARM7 build setting with elftobin
#
# Revision 1.51  2004/02/09 11:11:06  yasu
# change CC option to -avoid_strb all,noerr
#
# Revision 1.50  2004/02/09 01:26:25  yasu
# delete -once option
#
# Revision 1.49  2004/02/05 07:43:17  yasu
# fix definiation of CWFOLDER_IRIS
#
# Revision 1.48  2004/02/05 07:09:01  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.47  2004/02/03 06:38:47  yasu
# Added SDK_AUTOTEST
#
# Revision 1.46  2004/01/26 10:53:58  yasu
# delete SDK_CWBUG_VSNPRINTF workaround
#
# Revision 1.45  2004/01/08 01:39:34  yasu
# Add workaround (as comments) codewarrior environment value
#
# Revision 1.44  2003/12/25 12:32:43  yasu
# Clean up comments
#
# Revision 1.43  12/24/2003 09:24:35  yada
# Set -proc arm946e for BB compile (because of nightly build error).
#
# Revision 1.42  2003/12/22 04:43:46  yasu
# CW_NITRO 0.2 Hotfix
#
# Revision 1.41  2003/12/17 09:22:48  yasu
# Added SDK_CWBUG_PROC_OPT
#
# Revision 1.40  2003/12/16 10:44:32  yasu
# Revised SDK_CWBUGS_VSNPRINTF
#
# Revision 1.39  2003/12/16 05:25:12  yasu
# ARM7 version option settings
#
# Revision 1.38  2003/12/12 07:12:55  yasu
# Removed -avoid_strb all,err from ARM7 compile options
#
# Revision 1.37  2003/12/12 05:03:48  yasu
# Added a workaround to use until version that supports vsprintf strb is released.
#
# Revision 1.36  2003/12/10 05:12:48  yasu
# Changed so that the xxxx/src.c format can be used when specifying a source file.
# Added Makefile.srcs.
# Revised so that the IS Debugger library is linked if IS_NITRO_DIR has been defined.
#
# Revision 1.35  2003/12/08 09:26:43  yasu
# Added -once option
#
# Revision 1.34  2003/12/02 04:54:02  yasu
# Support for \ as path delimiter for NITROSDK_ROOT
#
# Revision 1.33  2003/12/02 02:40:23  yasu
# Added IS-NITRO-EMULATOR library definitions
#
# Revision 1.32  2003/12/01 23:50:52  yasu
# Revised LM_LICENSE_FILE
#
# Revision 1.31  2003/12/01 11:30:55  yasu
# Switched ROM creation tool to objcopy -> elftobin.
#
# Revision 1.30  2003/12/01 04:28:03  yasu
# Added old libraries in comment status
#
# Revision 1.29  2003/12/01 00:51:35  yasu
# Revised license.dat file settings
#
# Revision 1.28  2003/11/29 12:29:22  yasu
# Changed all libraries to strb version except for the library that includes vsnprintf.
#
# Revision 1.27  2003/11/29 10:48:32  yasu
# Added SDK_BB_ICE
#
# Revision 1.26  2003/11/29 04:10:21  yasu
# Revised as options
#
# Revision 1.25  2003/11/29 01:26:21  yasu
# Because vsnprintf is not working, changed library groups to old ones.
#
# Revision 1.24  2003/11/28 02:02:52  yasu
# Moved libsyscall.a to commondefs
#
# Revision 1.23  2003/11/28 02:01:12  yasu
# Replaced standard library for CW for NITRO.
#
# Revision 1.22  2003/11/27 13:43:31  yasu
# Added option for byte access
#
# Revision 1.21  2003/11/27 08:12:34  yasu
# Revised CWFolder_NITRO to CWFOLDER_NITRO
#
# Revision 1.20  2003/11/27 05:01:15  yasu
# Support for CodeWarrior for NITRO v0.1
#
# Revision 1.19  2003/11/25 00:03:03  yasu
# Added -stdinc option
#
# Revision 1.18  2003/11/21 06:32:00  yasu
# Not defining SDK_CWBUG_INLINE for SDK_DEBUG
#
# Revision 1.17  2003/11/21 04:07:21  yasu
# Added SDK_CWBUG_INLINE switch, which is used for an inline optimization bug workaround.
#
# Revision 1.16  2003/11/20 13:02:51  yasu
# To SDK_CODE_ARM that avoids SDK_ARM duplicate definition
#
# Revision 1.15  2003/11/20 00:02:47  yasu
# Added SDK_ARM/SDK_THUMB definition
#
# Revision 1.14  2003/11/17 01:31:05  yasu
# Separated C++ definitions from C definitions.
#
# Revision 1.13  2003/11/13 11:59:47  yasu
# typo correction
#
# Revision 1.12  2003/11/13 06:44:10  yasu
# Added ARM / Thumb mode compile switch
#
# Revision 1.11  2003/11/12 04:19:41  yasu
# Added definitions for ARM7. Use macro switch TARGET_PROC.
#
# Revision 1.10  2003/11/10 05:24:18  yasu
# Added build control using environmental variables.
#
# Revision 1.9	11/07/2003 08:54:49  yasu
# Added options such as -enum int
#
# Revision 1.8	11/06/2003 13:36:58  yasu
# Revised LM_LICENSE_FILE default value
#
# Revision 1.7	11/06/2003 10:39:21  yasu
# Revised compile options
#
# Revision 1.6	11/05/2003 09:57:26  yasu
# Revised NITRO_INC_DIR to NITRO_INCDIR
#
# Revision 1.5	11/05/2003 09:15:13  yasu
# Added ARFLAGS
# Enabled source switch using SDK_RELEASE/SDK_DEBUG
#
# Revision 1.4	11/05/2003 07:14:00  yasu
# Provisional completion of CodeWarrior option settings
#
# Revision 1.3	10/29/2003 06:47:28  yasu
# Support "make clean/clobber" only
#
# Revision 1.2	10/24/2003 13:19:07  yasu
# Registration of header files etc.
#
# Revision 1.1	10/23/2003 04:38:07  yasu
# initial version
#
# $NoKeywords: $
#----------------------------------------------------------------------------

EMPTY	 	?=
SPACE 		?= $(EMPTY) $(EMPTY)

#----------------------------------------------------------------------------
NITRO_STD_PCHDR	?= True

#----------------------------------------------------------------------------
CWFOLDER_IRIS	?= C:/Program Files/Metrowerks/CW for NINTENDO DS V1.2
CWFOLDER_NITRO	?= $(CWFOLDER_IRIS)
LM_LICENSE_FILE	:= $(subst /,\,$(CWFOLDER_NITRO))\license.dat
CW_ROOT		:= $(subst $(SPACE),\ ,$(subst \,/,$(CWFOLDER_NITRO)))
CW_BINDIR	:= $(CW_ROOT)/ARM_Tools/Command_Line_Tools
CW_ARMDIR	:= $(CW_ROOT)/ARM_EABI_Support

#----------------------------------------------------------------------------
SDK_VERINFO_CW_CC	:= $(NITRO_BUILDTOOLSDIR)/verinfo.cw.cc
SDK_VERINFO_CW_LD	:= $(NITRO_BUILDTOOLSDIR)/verinfo.cw.ld

ifeq	($(MAKECMDGOALS),do-build)

-include $(SDK_VERINFO_CW_CC) $(SDK_VERINFO_CW_LD)
#--------Support for CW2.0beta2 and later versions
ifndef	SDK_CW_BUILD_VERSION_CC
#--------If SDK_CW_BUILD_VERSION_CC is not defined, use make to create verinfo
MAKE_VERINFO	:= $(shell make make-verinfo)
-include $(SDK_VERINFO_CW_CC) $(SDK_VERINFO_CW_LD)
endif

ifeq		($(shell expr "$(SDK_CW_BUILD_VERSION_CC)" ">=" 3.0),1)
CW_MAJOR_VER	?= 2
endif
# "__declspec(force_export)" support
ifeq		($(shell expr "$(SDK_CW_BUILD_VERSION_CC)" ">=" 3.0),1)
SDK_CW_FORCE_EXPORT_SUPPORT	:= yes
endif
endif
CW_MAJOR_VER	?= 1

######## IRIS V0.2 Hotfix 1
######## NITRO V0.3
#CW_AVOID_STRB		:= -avoid_strb all,noerr
#
######## NITRO V0.4 or other
CW_AVOID_STRB		:= -avoid_byte strb -warn_byte none
#CW_AVOID_STRB		:= -avoid_byte strb -warn_byte all
#

#----------------------------------------------------------------------------

MWCC			:= $(CW_BINDIR)/mwccarm.exe
MWAS			:= $(CW_BINDIR)/mwasmarm.exe
MWLD			:= $(CW_BINDIR)/mwldarm.exe
MWAR			:= $(MWLD)

CC			:= $(MWCC)
AS			:= $(MWAS)
LD			:= $(MWLD)
AR			:= $(MWAR)

ELFTOBIN		:= $(CW_BINDIR)/elftobin.exe
ELFTOBIN_TYPE		:= $(NITRO_PLATFORM)$(CODEGEN_ARCH)/$(NITRO_BUILD_DIR)
ELFTOBIN_ARM9		?= $(NITRO_COMPONENTSDIR)/printserver/$(NITRO_BUILDTYPE_ARM9)/printserver.nef
ELFTOBIN_ARM7		?= $(NITRO_COMPONENTSDIR)/ferret/$(NITRO_BUILDTYPE_ARM7)/ferret.nef
ELFTOBIN_ROMHEADER	?= $(NITRO_TOOLSDIR)/elftobin/romHeader.bin

#----------------------------------------------------------------------------
#  Workaround flags
#	SDK_CW_WARNOFF_SAFESTRB	: use code32.h to wipe warning STRB out
#	SDK_CW_WA_OPT4		: workaround for -O4
#	SDK_CW_WA_CONSTPOOLS	: workaround for CW 'ConstantPools.c'line: 346
#	SDK_CW_WA_OPT_BLX	: workaround for blxeq,blxne with opt>=2
#	SDK_CW_WA_LIBCXX	: workaround for 'C++' libname in lcf
#

#--- flags ---
ifeq	($(NITRO_PLATFORM),TEG)
SDK_CW_WARNOFF_SAFESTRB := yes
else
SDK_CW_WARNOFF_SAFESTRB	:=
endif

######## IRIS V0.2 Hotfix 1
######## NITRO V0.3
######## NITRO V0.4.1
#SDK_CW_WA_OPT4		:= yes
#SDK_CW_WA_CONSTPOOLS	:= yes
#SDK_CW_WA_OPT_BLX	:= yes
######## NITRO V0.5
######## NITRO V0.5 + HotFix1
######## NITRO V0.6
#SDK_CW_WA_LIBCXX	:= yes

#-----------------------------------
#
ifdef	SDK_CW_WA_OPT_BLX
ifdef	NITRO_BLXCHECKED
#	workaround by pragma
MACRO_FLAGS	+= -DSDK_CW_WA_OPT_BLX
else
#	workaround by option
SDK_CW_AVOID_ARM_ARCH5	:= yes
endif
endif

ifdef	SDK_CW_WARNOFF_SAFESTRB
MACRO_FLAGS	+= -DSDK_CW_WARNOFF_SAFESTRB
endif

ifdef	SDK_CW_WA_OPT4
ifeq	($(NITRO_CODEGEN),ARM)
ifneq	($(NITRO_BUILD_TYPE),DEBUG)
MACRO_FLAGS	+= -DSDK_CW_WA_OPT4
endif
endif
endif

ifdef	SDK_CW_WA_CONSTPOOLS
ifeq	($(NITRO_CODEGEN),THUMB)
ifneq	($(NITRO_BUILD_TYPE),DEBUG)
MACRO_FLAGS	+= -DSDK_CW_WA_CONSTPOOLS
endif
endif
endif

ifdef	SDK_CW_FORCE_EXPORT_SUPPORT
MACRO_FLAGS   +=  -DSDK_CW_FORCE_EXPORT_SUPPORT
endif

#----------------------------------------------------------------------------
# SDK small build

ifdef	SDK_SMALL_BUILD
MACRO_FLAGS	+=	-DSDK_SMALL_BUILD
endif

#----------------------------------------------------------------------------
#  Workaround flags
#	SDK_TEG_WA_VBLANK	: workaround TEG vblank problems
#

ifeq	($(NITRO_PLATFORM),TEG)
MACRO_FLAGS	+= -DSDK_TEG_WA_VBLANK
endif


#----------------------------------------------------------------------------
######## IRIS V0.2 Hotfix 1
######## NITRO V0.3
######## NITRO V0.4
#CW_LIBNAME_RULE_A   := ARM
#CW_LIBNAME_RULE_T   := Thumb
#CW_LIBNAME_RULE_AI  := ARM_i
#CW_LIBNAME_RULE_RA  := ARM_Runtime
#CW_LIBNAME_RULE_RT  := Thumb_Runtime
#CW_LIBNAME_RULE_RAI := ARM_Runtime_i
######## NITRO V0.5 or other
CW_LIBNAME_RULE_A   := NITRO_A
CW_LIBNAME_RULE_T   := NITRO_T
CW_LIBNAME_RULE_AI  := NITRO_Ai
CW_LIBNAME_RULE_RA  := NITRO_Runtime_A
CW_LIBNAME_RULE_RT  := NITRO_Runtime_T
CW_LIBNAME_RULE_RAI := NITRO_Runtime_Ai
#

CW_INCDIRS	= $(CW_ARMDIR)/msl/MSL_C/MSL_ARM/Include		\
		  $(CW_ARMDIR)/msl/MSL_C/MSL_Common/Include		\
		  $(CW_ARMDIR)/msl/MSL_C/MSL_Common_Embedded/Math/Include \
		  $(CW_ARMDIR)/msl/MSL_C++/MSL_ARM/Include		\
		  $(CW_ARMDIR)/msl/MSL_C++/MSL_Common/Include		\
		  $(CW_ARMDIR)/msl/MSL_Extras/MSL_Common/Include	\
		  $(CW_ARMDIR)/Profiler/include
#--------Because the Runtime directory was changed in versions CW2.0beta2 and later
ifeq		($(CW_MAJOR_VER),2)
CW_INCDIRS	+= $(CW_ARMDIR)/msl/MSL_Extras/MSL_ARM/Include
endif

CW_CXX		= $(if $(SDK_CW_WA_LIBCXX),CXX,CPP)
CW_LIBCXX_DIR	= $(if $(SDK_CW_WA_LIBCXX),$(ROOT)/lib/CodeWarrior,$(CW_ARMDIR)/msl/MSL_C++/MSL_ARM/Lib)
#--------Because the Runtime directory was changed in versions CW2.0beta2 and later
ifeq		($(CW_MAJOR_VER),2)
CW_LIBDIRS	?= $(CW_ARMDIR)/msl/MSL_C/MSL_ARM/Lib			\
		   $(CW_LIBCXX_DIR)					\
		   $(CW_ARMDIR)/msl/MSL_Extras/MSL_ARM/Lib 		\
		   $(CW_ARMDIR)/Runtime/Runtime_ARM/Runtime_NITRO/Lib	\
		   $(CW_ARMDIR)/Mathlib/lib
endif
CW_LIBDIRS	?= $(CW_ARMDIR)/msl/MSL_C/MSL_ARM/Lib			\
		   $(CW_LIBCXX_DIR)					\
		   $(CW_ARMDIR)/msl/MSL_Extras/MSL_ARM/Lib 		\
		   $(CW_ARMDIR)/Runtime/Lib				\
		   $(CW_ARMDIR)/Mathlib/lib

ifeq		($(CODEGEN_PROC)-$(NITRO_PLATFORM),ARM9-TEG)
CW_SUFFIX	= _LE_strb.a
else
CW_SUFFIX	= _LE.a
endif

ifeq		($(CODEGEN_PROC),ARM9)
CW_LIBFP	?= FP_fastI_v5t_LE.a
# CW_LIBFP	?= FP_fixedI_v5t_LE.a
# CW_LIBFP	?= FP_flush0_v5t_LE.a
# CW_LIBFP	?= FP_fullI_v5t_LE.a
else
CW_LIBFP	?= FP_fastI_v4t_LE.a
# CW_LIBFP	?= FP_fixedI_v4t_LE.a
# CW_LIBFP	?= FP_flush0_v4t_LE.a
# CW_LIBFP	?= FP_fullI_v4t_LE.a
endif

CW_LIBCXX_ARM	= MSL_$(CW_CXX)_$(CW_LIBNAME_RULE_AI)$(CW_SUFFIX)

CW_LIBS_ARM	= MSL_C_$(CW_LIBNAME_RULE_AI)$(CW_SUFFIX)		\
		  MSL_Extras_$(CW_LIBNAME_RULE_AI)$(CW_SUFFIX)		\
		  $(CW_LIBCXX_ARM)					\
		  $(CW_LIBFP)						\
		  $(CW_LIBNAME_RULE_RAI)$(CW_SUFFIX)

# Workaround for Byte Access: Don't use thumb libraries if ARM9-TEG
ifeq		($(CODEGEN_PROC)-$(NITRO_PLATFORM),ARM9-TEG)

CW_LIBCXX_THUMB = $(CW_LIBCXX_ARM)

CW_LIBS_THUMB	= $(CW_LIBS_ARM)

else

CW_LIBCXX_THUMB = MSL_$(CW_CXX)_$(CW_LIBNAME_RULE_T)_LE.a

CW_LIBS_THUMB	= MSL_C_$(CW_LIBNAME_RULE_T)_LE.a			\
		  MSL_Extras_$(CW_LIBNAME_RULE_T)_LE.a			\
		  $(CW_LIBCXX_THUMB)					\
		  $(CW_LIBFP)						\
		  $(CW_LIBNAME_RULE_RT)_LE.a
endif

ifeq	($(CODEGEN_PROC),ARM9)
STUBS_LIBS	+= libstubscw$(NITRO_LIBSUFFIX).a
else #	($(CODEGEN_PROC),ARM7)
STUBS_LIBS	+= libstubscw_sp$(NITRO_LIBSUFFIX).a
endif
	$(ECHO) "==== test";

#----------------------------------------------------------------------------
#  switch by codegen target
#    NITRO_PLATFORM  TEG/TS
#    NITRO_MEMSIZE   4M/8M
#    CODEGEN_PROC    ARM7/ARM9
#    CODEGEN_CC      ARM/THUMB
#    CODEGEN_AS      ARM/THUMB
#

MACRO_FLAGS	+= -DSDK_$(NITRO_PLATFORM)  \
		   -DSDK_$(NITRO_MEMSIZE)   \
		   -DSDK_$(CODEGEN_PROC)

MACRO_FLAGS_CC	+= -DSDK_CODE_$(CODEGEN_CC)
MACRO_FLAGS_AS	+= -DSDK_CODE_$(CODEGEN_AS)


#-------------------------------------- CODEGEN_PROC ARM9/ARM7
ifeq		($(CODEGEN_PROC),ARM9)

CCFLAGS_PROC	:= -proc $(if $(SDK_CW_AVOID_ARM_ARCH5),arm7tdmi,arm946e)
ASFLAGS_PROC	:= -proc arm5TE

ifeq		($(NITRO_PLATFORM),TEG)
CCFLAGS_PROC	+= $(CW_AVOID_STRB)
endif

else	#	($(CODEGEN_PROC),ARM7)

CCFLAGS_PROC	:= -proc arm7tdmi
ASFLAGS_PROC	:= -proc arm4T

endif	#	($(CODEGEN_PROC))

#-------------------------------------- NITRO_CODEGEN ARM/THUMB

ifeq		($(NITRO_CODEGEN),ARM)
CW_LIBS		?= $(CW_LIBS_ARM)
else	#	($(NITRO_CODEGEN),THUMB)
CW_LIBS		?= $(CW_LIBS_THUMB)
endif


#-------------------------------------- CODEGEN_CC  ARM/THUMB

ifeq		($(CODEGEN_CC),THUMB)
CCFLAGS_ISET	:= -thumb
else
CCFLAGS_ISET	:= -nothumb
endif


#-------------------------------------- CODEGEN_AS  ARM/THUMB

ifeq		($(CODEGEN_AS),THUMB)
ASFLAGS_ISET	:= -16
else
ASFLAGS_ISET	:= -32
endif


CCFLAGS_ARCH	?= $(CCFLAGS_PROC) $(CCFLAGS_ISET) -nopic -nopid -interworking
ASFLAGS_ARCH	?= $(ASFLAGS_PROC) $(ASFLAGS_ISET)
LDFLAGS_ARCH	?= $(CCFLAGS_PROC) $(CCFLAGS_ISET) -nopic -nopid -interworking
ARFLAGS_ARCH	?=

#----------------------------------------------------------------------------
#  standard includes/libraries for CodeWarrior
#

export	MWCIncludes    := $(subst \;, ,$(subst $(SPACE),;,$(CW_INCDIRS)))
export	MWLibraries    := $(subst \;, ,$(subst $(SPACE),;,$(CW_LIBDIRS)))
export	MWLibraryFiles := $(subst \;, ,$(subst $(SPACE),;,$(CW_LIBS)))

#----------------------------------------------------------------------------
#  switch by C Compiler CW/SNC
#

MACRO_FLAGS	+= -DSDK_$(NITRO_CCTYPE)


#----------------------------------------------------------------------------
#  switch by DEBUG/RELEASE/FINALROM
#

MACRO_FLAGS	+= -DSDK_$(NITRO_BUILD_TYPE)

ifeq		($(NITRO_BUILD_TYPE),DEBUG)

CCFLAGS_OPT	?= -O0 -inline off
ASFLAGS_OPT	?=
LDFLAGS_OPT	?=
ARFLAGS_OPT	?=

else

CCFLAGS_OPT_O_	= -O4 -inline on,noauto

ifeq		($(CODEGEN_PROC),ARM9)
CCFLAGS_OPT_O_	+= -opt speed
else
CCFLAGS_OPT_O_	+= -opt space
endif

ifeq		($(NITRO_BUILD_TYPE),FINALROM)
CCFLAGS_OPT	?= $(CCFLAGS_OPT_O_)
ASFLAGS_OPT	?=
LDFLAGS_OPT	?=
ARFLAGS_OPT	?=
else 	#	($(NITRO_BUILD_TYPE),RELEASE)	# (default)
CCFLAGS_OPT	?= $(CCFLAGS_OPT_O_)
ASFLAGS_OPT	?=
LDFLAGS_OPT	?=
ARFLAGS_OPT	?=
endif

endif

ifndef		NITRO_NO_OPT_G
CCFLAGS_OPT	+= -g
LDFLAGS_OPT	+= -g
endif

#--------To avoid problem with size increase in versions CW2.0beta2 and later, -ipa file option is required
ifeq		($(CW_MAJOR_VER),2)
CCFLAGS_OPT	+= -ipa file
endif


#----------------------------------------------------------------------------
#  switch for code profiling
#

ifdef		NITRO_PROFILE
CCFLAGS_OPT	+= -profile
endif

ifdef		NITRO_PROFILE_TYPE
CCFLAGS_OPT	+= -DSDK_PROFILE_$(NITRO_PROFILE_TYPE)
endif


#----------------------------------------------------------------------------
#  switch for anti-thread policy
#

ifdef		NITRO_NO_THREAD
MACRO_FLAGS	+= -DSDK_NO_THREAD
endif


#----------------------------------------------------------------------------
#  switch for AUTOTEST/UNITTEST
#
ifdef		NITRO_AUTOTEST
MACRO_FLAGS	+= -DSDK_AUTOTEST
endif
ifdef		NITRO_UTEST
MACRO_FLAGS	+= -DSDK_UTEST
endif

#----------------------------------------------------------------------------
#  Switch for using system call when SVC_WaitVBlankIntr
#
ifdef		NITRO_SVC_WAITVBLANK_COMPATIBLE
MACRO_FLAGS	+= -DSDK_SVC_WAITVBLANK_COMPATIBLE
endif

#----------------------------------------------------------------------------
#  Switch for automatic PrintServer
#
ifdef		NITRO_ENABLE_ARM7_PRINT
MACRO_FLAGS	+= -DSDK_ENABLE_ARM7_PRINT
endif


#----------------------------------------------------------------------------
#  switch for TS board
#
#	Default value of NITRO_TS_VERSION is defined in <nitro/spec.h>
#
#	NITRO_TS_VERSION = [0/100/200]
#
#      0 : Prototype breadboard ~ ISD-BOX Ver.C
#    100 : ISD-BOX Ver.D
#    200 : ISD-BOX Ver.E
#
#
ifdef		NITRO_TS_VERSION
MACRO_FLAGS	+= -DSDK_TS_VERSION=$(NITRO_TS_VERSION)
endif


#----------------------------------------------------------------------------
#  my macro flags
#
MACRO_FLAGS	+= $(LMACRO_FLAGS)


#----------------------------------------------------------------------------
CCFLAGS_WARNING_FULL	= all,cmdline,illpragmas,emptydecl,possible,unusedarg,unusedvar,unused,extracomma,pedantic,hidevirtual,implicitconv,impl_int2float,impl_float2int,impl_signedunsigned,notinlined,largeargs,structclass,padding,notused,missingreturn,unusedexpr,ptrintconv,anyptrintconv,undefmacro,filecaps,sysfilecaps,tokenpasting

CCFLAGS_MSGSTYLE	?= -msgstyle std

ifdef	NITRO_WARNING_STRICT
CCFLAGS_WARNING		?= -w $(CCFLAGS_WARNING_FULL)
else
CCFLAGS_WARNING		?= -w all
endif
LDFLAGS_WARNING		?= -w on
ARFLAGS_WARNING		?= -w on

ifeq	($(NITROSDK_LANG),en)
CCFLAGS_ENCODING	?= -enc ascii
else
CCFLAGS_ENCODING	?= -enc SJIS
endif
CCFLAGS_CHARTYPE	?= -char signed

CCFLAGS_MISC	= $(CCFLAGS_MSGSTYLE) $(CCFLAGS_WARNING) $(CCFLAGS_ENCODING) $(CCFLAGS_CHARTYPE)
ASFLAGS_MISC	= $(CCFLAGS_MSGSTYLE)
LDFLAGS_MISC	= $(CCFLAGS_MSGSTYLE) $(LDFLAGS_WARNING)
ARFLAGS_MISC	= $(CCFLAGS_MSGSTYLE) $(ARFLAGS_WARNING)


#----------------------------------------------------------------------------

CCFLAGS_SYS	?= -stdinc -enum int -stdkeywords off -Cpp_exceptions off
ASFLAGS_SYS	?= -nostdinc -DSDK_ASM
LDFLAGS_SYS	?= -stdlib -map closure -main _start
ARFLAGS_SYS	?= -library


CCFLAGS		= -lang c $(CCFLAGS_ARCH) $(CCFLAGS_OPT) $(CCFLAGS_MISC) \
		  $(CCFLAGS_SYS)  $(MACRO_FLAGS) $(MACRO_FLAGS_CC)

CXXFLAGS	= -lang c++ $(CCFLAGS_ARCH) $(CCFLAGS_OPT) $(CCFLAGS_MISC) \
		  $(CCFLAGS_SYS)  $(MACRO_FLAGS) $(MACRO_FLAGS_CC)

ASFLAGS		= $(ASFLAGS_ARCH) $(ASFLAGS_OPT) $(ASFLAGS_MISC) \
		  $(ASFLAGS_SYS)  $(MACRO_FLAGS) $(MACRO_FLAGS_AS)

LDFLAGS		= $(LDFLAGS_ARCH) $(LDFLAGS_OPT) $(LDFLAGS_MISC) \
		  $(LDFLAGS_SYS)

ARFLAGS		= $(ARFLAGS_ARCH) $(ARFLAGS_OPT) $(ARFLAGS_MISC) \
		  $(ARFLAGS_SYS)


#----- End of commondefs.cctype.CW -----
