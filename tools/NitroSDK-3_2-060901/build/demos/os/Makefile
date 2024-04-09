#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - OS - demos
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
# $Log: Makefile,v $
# Revision 1.43  09/09/2005 06:19:13  yada
# add argument-2
#
# Revision 1.42  07/27/2005 08:44:29  seiki_masashi
# Added entrop-1 demo
#
# Revision 1.41  07/21/2005 03:42:26  yada
# add argument-1
#
# Revision 1.40  07/14/2005 00:38:26  kitase_hirotake
# Added thread-1
#
# Revision 1.39  2005/07/12 07:55:08  yada
# add thread-8 and thread-9 demos
#
# Revision 1.38  07/08/2005 13:03:52  yada
# add thread6 and thread7 demo
#
# Revision 1.37  06/02/2005 09:00:28  yosizaki
# add forChina-fs.
#
# Revision 1.36  05/14/2005 02:08:05  terui
# Add "forChina-1"
#
# Revision 1.35  04/27/2005 03:01:09  terui
# Update copyright.
#
# Revision 1.34  04/26/2005 06:52:39  terui
# remove china-1 from SUBDIRS
#
# Revision 1.33  04/26/2005 03:35:01  terui
# Fix comment
#
# Revision 1.32  12/08/2004 06:18:31  yada
# remove interrupt-key-1
#
# Revision 1.31  10/08/2004 01:38:36  yosizaki
# add favorite-color.
#
# Revision 1.30  09/01/2004 05:48:01  yada
# add reset-1
#
# Revision 1.29  06/25/2004 06:02:35  yada
# OS_JoinThread() and  OS_IsThreadTerminate() sample
#
# Revision 1.28  06/03/2004 08:15:12  yada
# sorry, update 1.27 is mistake. ignore comment of 1.27.
# now, add some demos because profile library separated.
#
# Revision 1.27  06/03/2004 07:59:41  yada
# fix indent processing
#
# Revision 1.26  05/20/2004 02:00:30  yada
# add waitIrq-1 and waitIrq-2
#
# Revision 1.25  05/12/2004 06:15:41  yada
# delete callTrace* and exceptionDisplay-3
#
# Revision 1.24  05/10/2004 13:00:31  yada
# only arrange tab
#
# Revision 1.23  2004/05/06 10:18:18  yasu
# fix typo
#
# Revision 1.22  04/15/2004 07:52:19  yada
# add exceptionDisplay-3, callTrace-1, callTrace-2
#
# Revision 1.21  2004/04/13 00:27:24  mita_kentaro
# add backslash at the spinwait-1 line
#
# Revision 1.20  2004/04/12 10:12:06  yasu
# add cplusplus-1 demo
#
# Revision 1.19  04/05/2004 02:53:10  yada
# add mutex-2 demo
#
# Revision 1.18  03/17/2004 09:18:04  yada
# Added exceptionDisplay-2
#
# Revision 1.17  03/09/2004 08:36:10  yada
# Added valarm-1, sleep-1, exceptionDisplay-1, and exceptionDisplay2
#
# Revision 1.16  03/08/2004 10:18:47  yada
# fixed top comment
#
# Revision 1.15  2004/02/27 01:55:51  yasu
# added demo for OS_SpinWait
#
# Revision 1.14  02/25/2004 12:35:02  yada
# Changes related to switch from systemClock to Tick
#
# Revision 1.13  2004/02/05 07:09:03  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.12  02/03/2004 13:05:49  yada
# Added alarm-2
#
# Revision 1.11  02/03/2004 12:25:53  yada
# Added systemClock-1 and alarm-1
#
# Revision 1.10  01/18/2004 05:51:39  yada
# Moved interrupt-dma sample to MI
#
# Revision 1.9  01/17/2004 13:30:25  yada
# Added interrupt-dma-1
#
# Revision 1.8  01/09/2004 10:20:21  yada
# Added mutex-1
#
# Revision 1.7  01/08/2004 08:38:59  yada
# Added timer-1 and interrupt-key-1
#
# Revision 1.6  01/07/2004 09:21:49  yada
# Added Subdirectory heap-1, heap2, heap3, arena-1
#
# Revision 1.5  2003/12/16 06:30:36  yasu
# Sample of thread switch processing by thread-4 interrupt
#
# Revision 1.4  11/30/2003 10:28:08  yada
# Deleted sample of interrupt-thread
#
# Revision 1.3  2003/11/29 12:39:41  yasu
# Added thread-3 and interrupt-thread
#
# Revision 1.2  2003/11/26 01:38:29  yasu
# Added thread-2
#
# Revision 1.1  2003/11/25 09:52:58  yasu
# Newly added
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------

SUBDIRS =       thread-1 \
                thread-2 \
                thread-3 \
                thread-4 \
                thread-5 \
                thread-6 \
                thread-7 \
                thread-8 \
                thread-9 \
                thread-10 \
                mutex-1 \
                mutex-2 \
                arena-1 \
                heap-1 \
                heap-2 \
                heap-3 \
                tick-1 \
                timer-1 \
                alarm-1 \
                alarm-2 \
                valarm-1 \
                sleep-1 \
                exceptionDisplay-1 \
                exceptionDisplay-2 \
                spinwait-1 \
                cplusplus-1 \
                waitIrq-1 \
                waitIrq-2 \
                callTrace-1 \
                callTrace-2 \
                exceptionDisplay-3 \
                functionCost-1 \
                functionCost-2 \
                functionCost-3 \
                reset-1 \
                favorite-color \
                forChina-1 \
                forChina-fs \
                argument-1 \
                argument-2 \
                entropy-1

#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
