/*---------------------------------------------------------------------------*
  Project:  NitroSDK - wireless_shared - demos - wfs
  File:     wfs_thread.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wfs_thread.c,v $
  Revision 1.8  05/22/2006 02:35:24  yosizaki
  add temporary test code.

  Revision 1.7  02/27/2006 12:50:33  yosizaki
  change FS_OpenFileDirect to FS_CreateFileFromRom

  Revision 1.6  01/18/2006 02:12:39  kitase_hirotake
  do-indent

  Revision 1.5  01/10/2006 02:51:04  adachi_hiroaki
  All hexadecimal notation output was expressly labeled with 0x. /([^xX])(%\d*[xX][^hH])/ -> \10x\2

  Revision 1.4  07/22/2005 07:25:46  yosizaki
  fix around OSThredQueue.

  Revision 1.3  05/09/2005 04:31:40  yosizaki
  change around SDK_NO_THREAD.

  Revision 1.2  02/28/2005 05:26:35  yosizaki
  do-indent.

  Revision 1.1  11/16/2004 01:12:55  yosizaki
  moved from /demos/wbt/wbt-fs.


 -[old log at /demos/wbt/wbt-fs/wfs]-

  Revision 1.9  09/29/2004 07:11:23  yosizaki
  merge from branch.

  Revision 1.8  09/15/2004 09:10:25  yosizaki
  add Seek&Read.

  Revision 1.7  09/09/2004 05:03:18  yosizaki
  fix file page-cache.

  Revision 1.6  09/07/2004 03:05:51  yosizaki
  source code Maintenance.

  Revision 1.5  09/06/2004 06:23:37  yosizaki
  add double-buffering system.

  Revision 1.4  09/04/2004 08:29:53  yosizaki
  change around PREPARE-response.

  Revision 1.3  09/04/2004 01:45:21  yosizaki
  delete obsolete codes and waste parameters.

  Revision 1.2  09/03/2004 11:35:48  yosizaki
  support page-cache (test version)

  Revision 1.1  09/03/2004 09:58:10  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro.h>
#include <nitro/fs.h>
#include <nitro/card.h>

#include "wfs.h"
#include "wfs_private.h"


#if	defined(SDK_NO_THREAD)
SDK_ERR("cannot build WFS module with SDK_NO_THREAD!\n");
#endif

/********************************************************************/
/* Variables*/

typedef struct
{
    u8      stack[0x400];
    OSThread th[1];
    OSThreadQueue th_q[1];
#if defined(OS_SIZEOF_OSTHREADQUEUE) && (OS_SIZEOF_OSTHREADQUEUE == 16)
    u8      padding[2];
#endif
}
WFSiTask;

static WFSiTask wfsi_task[1] ATTRIBUTE_ALIGN(4);


/********************************************************************/
/* functions*/

/* notify file management thread of work*/
void WFSi_NotifyBusy(void)
{
    OS_WakeupThreadDirect(wfsi_task->th);
}

/* file management thread*/
static void WFSi_TaskThread(void *arg)
{
    WFSiTask *const p_task = (WFSiTask *) arg;
    WFSParentContext *const p_parent = wfsi_work->context.parent;

    int     i;

    WFS_DEBUG_OUTPUT("task : started\n");
    for (;;)
    {
        WFSiFileList *pb = NULL;

        {                              /* wait for the busy state task*/
            OSIntrMode bak_cpsr = OS_DisableInterrupts();
            while ((pb = p_parent->busy_list) == NULL)
            {
                /* end the thread if already ended*/
                if (wfsi_work->state == WFS_STATE_STOP)
                {
                    (void)OS_RestoreInterrupts(bak_cpsr);
                    return;
                }
#ifndef WFS_TEST_IMPL
                OS_SleepThread(NULL);
#else
                OS_Sleep(50);
#endif
            }
            (void)OS_RestoreInterrupts(bak_cpsr);
        }

        /* when OPENFILE is new*/
        if (pb->stat == WFS_FILE_STAT_OPENING)
        {
            WFS_DEBUG_OUTPUT("task : openfile(0x%08X)\n", pb->own_id);
            {                          /* open file and read first page to the cache*/
                (void)FS_CreateFileFromRom(&pb->file, pb->rom_src, pb->rom_len);
                for (i = 0; i < WFS_FILE_CACHE_LINE; ++i)
                {
                    pb->page[i] = WFS_FILE_CACHE_SIZE * i;
                    (void)FS_ReadFile(&pb->file, pb->cache[i], WFS_FILE_CACHE_SIZE);
                }
                pb->ack_seq = 0;
                pb->last_page = WFS_FILE_CACHE_LINE - 1;
                pb->busy_page = WFS_FILE_CACHE_LINE;
            }
            {                          /* Notify WBT, and move to the registered list*/
                OSIntrMode bak_cpsr = OS_DisableInterrupts();
                WFSi_SendOpenAck(p_parent, pb, TRUE);
                (void)OS_RestoreInterrupts(bak_cpsr);
            }
        }
        /* When deleting CLOSEFILE*/
        else
        {
            WFS_DEBUG_OUTPUT("task : closefile(0x%08X)\n", pb->info.data_info.id);
            {                          /* simply close file*/
                (void)FS_CloseFile(&pb->file);
            }
            {                          /* move to empty list*/
                OSIntrMode bak_cpsr = OS_DisableInterrupts();
                WFSi_FromBusyToFree(p_parent, pb);
                (void)OS_RestoreInterrupts(bak_cpsr);
            }
        }
    }
}

/* start up file management thread*/
void WFSi_CreateTaskThread(void)
{
    WFSiTask *const p = wfsi_task;

    /* start up task thread*/
    OS_InitThreadQueue(p->th_q);
    OS_CreateThread(p->th, WFSi_TaskThread, p, p->stack + sizeof(p->stack), sizeof(p->stack), 15);
    OS_WakeupThreadDirect(p->th);
}

/* end file management thread*/
void WFSi_EndTaskThread(void)
{
    WFSiTask *const p = wfsi_task;

    /* wait for thread to end*/
    while (!OS_IsThreadTerminated(p->th))
    {
        OS_WakeupThreadDirect(p->th);
        OS_JoinThread(p->th);
    }
}
