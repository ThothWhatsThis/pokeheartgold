/*---------------------------------------------------------------------------*
  Project:  NitroSDK - wireless_shared - demos - wfs
  File:     wfs_common.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wfs_common.c,v $
  Revision 1.18  06/27/2006 08:04:26  yosizaki
  fix about initialization.

  Revision 1.17  05/22/2006 02:35:24  yosizaki
  add temporary test code.

  Revision 1.16  05/21/2006 23:55:55  yosizaki
  change WFS_Init() to inline function.

  Revision 1.15  02/27/2006 13:17:10  yosizaki
  fix WFS_InitParent().
  add WBT_CancelCurrentCommand() on disconnection.
  fix WFSi_MoveList().

  Revision 1.14  01/18/2006 02:12:39  kitase_hirotake
  do-indent

  Revision 1.13  01/10/2006 02:51:04  adachi_hiroaki
  All hexadecimal notation output was expressly labeled with 0x. /([^xX])(%\d*[xX][^hH])/ -> \10x\2

  Revision 1.12  12/26/2005 07:15:30  yosizaki
  fix  about wfsi_work pointer.

  Revision 1.11  11/14/2005 08:32:35  yosizaki
  fix  about wfsi_work pointer.

  Revision 1.10  10/28/2005 11:25:15  seiki_masashi
  Supported addition of WM_STATECODE_DISCONNECTED_FROM_MYSELF.
  Supported addition of WM_STATECODE_PORT_INIT.

  Revision 1.9  10/20/2005 08:12:06  yosizaki
  fix WFS_Init().
  change to allocate wok-memory in WFS_Init.

  Revision 1.8  05/09/2005 04:38:07  yosizaki
  fix around WFS_End().

  Revision 1.7  02/28/2005 05:26:35  yosizaki
  do-indent.

  Revision 1.6  02/18/2005 09:14:36  yosizaki
  fix around hidden warnings.

  Revision 1.5  02/07/2005 00:14:08  yosizaki
  fix WFSi_MoveList.
  Delayed internal state update until after initialization completes.

  Revision 1.4  02/04/2005 11:02:15  yasu
  Revised the copyright date

  Revision 1.3  02/01/2005 03:56:08  yosizaki
  fix around WFSi_SendMessage.

  Revision 1.2  12/02/2004 04:09:52  terui
  Add port callback NULL clear on WFS_End.

  Revision 1.1  11/16/2004 01:12:55  yosizaki
  moved from /demos/wbt/wbt-fs.


 -[old log at /demos/wbt/wbt-fs/wfs]-

  Revision 1.23  10/25/2004 04:20:09  yosizaki
  add WFS_InitParent and WFS_InitChild.
  add some getter functions.

  Revision 1.22  10/18/2004 00:17:29  yosizaki
  fix WFSi_OnChildSystemCallback().

  Revision 1.21  10/15/2004 09:02:09  yosizaki
  merge from branch.

  Revision 1.20  10/13/2004 02:17:39  yosizaki
  fix WFS_end in child-mode. (add async-cancelling)

  Revision 1.19  09/29/2004 07:31:45  yosizaki
  change WFS_Init.
  change assertion of getter functions.

  Revision 1.18  09/29/2004 07:11:38  yosizaki
  merge from branch.

  Revision 1.17  09/15/2004 09:10:25  yosizaki
  add Seek&Read.

  Revision 1.16  09/15/2004 01:11:06  yosizaki
  add WFS_SetPacketSize().

  Revision 1.15  09/11/2004 08:11:01  yosizaki
  fix around recv_idle.

  Revision 1.14  09/11/2004 04:13:10  yosizaki
  unify MP-size definition.

  Revision 1.13  09/09/2004 14:15:48  yosizaki
  fix WFS_GetCurrentDownloadProgress().

  Revision 1.12  09/09/2004 05:03:18  yosizaki
  fix file page-cache.

  Revision 1.11  09/07/2004 04:08:59  yosizaki
  change to delay ack-timing.

  Revision 1.10  09/07/2004 03:05:51  yosizaki
  source code Maintenance.

  Revision 1.9  09/06/2004 08:47:48  yosizaki
  fix archive. (around result of FS_ReadFile())

  Revision 1.8  09/06/2004 06:23:41  yosizaki
  add double-buffering system.

  Revision 1.7  09/04/2004 13:23:55  yosizaki
  change to load TABLE-memory with WFSi_Alloc.

  Revision 1.6  09/04/2004 08:29:53  yosizaki
  change around PREPARE-response.

  Revision 1.5  09/04/2004 01:45:21  yosizaki
  delete obsolete codes and waste parameters.

  Revision 1.4  09/03/2004 12:24:05  yosizaki
  change names. (WBT_RegisterUserDefinedBlock, WBT_UnRegisterBlock)

  Revision 1.3  09/03/2004 11:35:48  yosizaki
  support page-cache (test version)

  Revision 1.2  09/03/2004 09:58:38  yosizaki
  add wfs_thread.c

  Revision 1.1  09/03/2004 01:10:17  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

#include    "wfs.h"
#include    "wfs_private.h"


/*
 * Implementation notes:
 * The parent side will not return responses for individual requests,
 * is will always send them on the WBT_PutUserData() empty state timing.
 *  Make sure that the child does not send the next request without waiting for a response.
 */

/******************************************************************************/
/* Constant*/

/*
 * Default WBT block list user definition ID.
 * Basically, WFS uses PutUserData to control.
 * It does not communicate via this information region.
 */
static const u8 wfsi_def_user_data[WBT_USER_ID_LEN] = { 0, };


/******************************************************************************/
/* Variables*/

/* WFS work variable*/
WFSWork *wfsi_work;
static BOOL init_flag = FALSE;
BOOL    wfsi_debug_enable;


/******************************************************************************/
/* functions*/

static void WFSi_ReallocBitmap(WFSChildContext * p_child, int size);


/* message *****************************************************************/

/* message send callback function*/
static void WFSi_OnSendMessageDone(void *arg)
{
    WFSWork *const pw = wfsi_work;

    if (pw && (pw->state != WFS_STATE_STOP))
    {
        if (pw->is_parent)
        {
            WBTCommand *callback = (WBTCommand*)arg;
            /* Cancel due to disconnection*/
            if (callback->event == WBT_CMD_CANCEL)
            {
            }
            /* Send complete for all players*/
            if (!callback->target_bmp)
            {
                pw->context.parent->msg_busy = FALSE;
                WFSi_SendAck();
            }
        }
    }
}

/*
 * Send message using WBT user data command.
 */
static BOOL WFSi_SendMessage(u8 type, u16 bitmap, u32 id, u32 flag /* = 0 */ )
{
    WFSiMessage packet;
    packet.type = type;
    packet.flag = flag;
    packet.arg = id;
    packet.pck_h = (u8)(wfsi_work->parent_packet_size >> 8);
    packet.pck_l = (u8)(wfsi_work->parent_packet_size >> 0);
    return WBT_PutUserData(bitmap, &packet, WBT_SIZE_USER_DATA, WFSi_OnSendMessageDone);
}

/* Send if there are unsent Acks (parent)*/
void WFSi_SendAck(void)
{
    WFSWork *const pw = wfsi_work;
    WFSParentContext *const p_parent = pw->context.parent;

    /* ignore if the parent device and PutUserData are in sendable states*/
    if (pw && pw->is_parent)
    {
        if (!p_parent->msg_busy)
        {

            /* Show the current valid communications state in each type of bitmap*/
            p_parent->ack_bitmap &= p_parent->all_bitmap;
            p_parent->sync_bitmap &= p_parent->all_bitmap;
            p_parent->busy_bitmap &= p_parent->all_bitmap;
            p_parent->deny_bitmap &= p_parent->all_bitmap;

            /* Wait to change packet size until all processes being received are complete*/
            if (p_parent->is_changing && !p_parent->busy_count)
            {
                /* change packet size*/
                BOOL    ret;
                const int size_p = p_parent->new_packet_size;
                const int size_c = pw->child_packet_size;
                p_parent->is_changing = FALSE;
                pw->parent_packet_size = size_p;
                ret = WBT_SetPacketSize(size_p, size_c);
                WFS_DEBUG_OUTPUT("%s to WBT_SetPacketSize(%d, %d)\n",
                                 ret ? "succeeded" : "failed", size_p, size_c);
                /* overall denial response for requests with the old size*/
                if (p_parent->deny_bitmap)
                {
                    WFS_DEBUG_OUTPUT("send : deny openfile(0x%08X)\n", p_parent->deny_bitmap);
                    if (WFSi_SendMessage
                        (WFS_MSG_OPENFILE_ACK, (u16)p_parent->deny_bitmap, 0, FALSE))
                    {
                        p_parent->msg_busy = TRUE;
                        p_parent->deny_bitmap = 0;
                    }
                }
            }

            /* corresponds as-is to the requests received before the pachet size change*/
            else
            {
                int     bitmap = p_parent->ack_bitmap;

                /* ignore this if there are no response contents*/
                if (bitmap)
                {

                    WFSiMessage *msg = NULL;
                    int     i;
                    const int sync = p_parent->sync_bitmap;
                    const BOOL is_sync = (sync && ((bitmap & sync) == sync));

                    /* respond all at once if there is a uniform child device group that received the synchronous designation*/
                    if (is_sync)
                        bitmap = sync;
                    /* otherwise, respond as normal*/
                    else
                        bitmap &= ~sync;

                    /* search for relevant child devices that can respond*/
                    for (i = 0;; ++i)
                    {
                        const int bit = (1 << i);
                        if (bit > bitmap)
                            break;
                        if ((bit & bitmap) != 0)
                        {
                            /* search for child devices that can respond in order from the lowest aid*/
                            if (!msg)
                                msg = &p_parent->recv_msg[i];
                            /* send all at once if same type of response*/
                            else if ((msg->type == p_parent->recv_msg[i].type) &&
                                     (msg->arg == p_parent->recv_msg[i].arg))
                                ;
                            /* If not, then hold this time for the child device*/
                            else
                                bitmap &= ~bit;
                        }
                    }

                    /*
                     * NOTE:
                     *   In the application-side bug, gThe request contents differ even though synchronization was specifiedh,
                     *   it is not possible to determine whether
                     *   - simply that some of the synchronization contents have deviated
                     *   - if they have deviated, which will recover if acknowledged first
                     *   - It is impossible to determine that it is not the thread but the
                     *   request content that has a fatal difference.
                     *   Therefore, recovery will take place with the workaround of
                     *   gContinually issue a warning and auto-release the synchronization specificationh.
                     */
                    if (is_sync && (bitmap != sync))
                    {
                        p_parent->sync_bitmap = 0;
                        OS_TWarning("[WFS] specified synchronous-access failed! "
                                    "(then synchronous-setting was reset)");
                    }

                    /* Send the response selected this time*/
                    if (msg)
                    {
                        switch (msg->type)
                        {
                        case WFS_MSG_OPENFILE_REQ:
                            msg->type = WFS_MSG_OPENFILE_ACK;
                            break;
                        case WFS_MSG_CLOSEFILE_REQ:
                            msg->type = WFS_MSG_CLOSEFILE_ACK;
                            p_parent->busy_bitmap &= ~bitmap;
                            break;
                        }
                        if (WFSi_SendMessage((u8)msg->type, (u16)bitmap, msg->arg, TRUE))
                        {
                            p_parent->msg_busy = TRUE;
                            p_parent->ack_bitmap &= ~bitmap;
                        }
                    }
                }
            }
        }
    }
}

/*
 * File is ready, so respond to OPENFILE_REQ.
 * If it is a new file, register it.
 */
void WFSi_SendOpenAck(WFSParentContext * p, WFSiFileList * trg, BOOL ret)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();

    if (wfsi_work && wfsi_work->state != WFS_STATE_STOP)
    {

        /* Added the sections for which preparation is complete to the response enabled bitmap*/
        int     bitmap = trg->req_bitmap;
        trg->req_bitmap = 0;
        p->ack_bitmap |= bitmap;

        /* Transfer to the registered list if a new file*/
        if (trg->ref <= 0)
        {
            /*
             * If the access region is within the cache page range,
             * specify the cache pointer as-is.
             * If it is larger, use a callback from WBT
             * to perform the paging process and operate in virtual memory.
             */
            const u32 id = trg->own_id;
            const u32 size = FS_GetLength(&trg->file);
            const void *src = (size <= WFS_FILE_CACHE_SIZE) ? trg->cache : NULL;
            (void)WBT_RegisterBlock(&trg->info, id, wfsi_def_user_data, src, (int)size, 0);
            WFSi_FromBusyToAlive(p, trg);
        }

        {                              /* increase the reference count*/
            int     i;
            for (i = 0;; ++i)
            {
                const int bit = (1 << i);
                if (bit > bitmap)
                    break;
                if ((bit & bitmap) != 0)
                {
                    ++trg->ref;
                    /* store the block ID for the response*/
                    p->recv_msg[i].arg = trg->own_id;
                }
            }
        }

        (void)ret;
    }

    (void)OS_RestoreInterrupts(bak_cpsr);
}


/* list operations *****************************************************************/

/* search for currently registered files*/
WFSiFileList *WFSi_FindAlive(WFSParentContext * p, u32 top, u32 len)
{
    WFSiFileList *pb;
    for (pb = p->alive_list; pb; pb = pb->next)
    {
        if ((pb->stat == WFS_FILE_STAT_ALIVE) &&
            (FS_GetFileImageTop(&pb->file) == top) && (FS_GetLength(&pb->file) == len))
            break;
    }
    return pb;
}

/* search for files currently being prepared for registration*/
WFSiFileList *WFSi_FindBusy(WFSParentContext * p_parent, u32 src, u32 len)
{
    WFSiFileList *trg;
    /* Find files that are currently being prepared for registration*/
    for (trg = p_parent->busy_list; trg; trg = trg->next)
    {
        if ((trg->stat == WFS_FILE_STAT_OPENING) && (trg->rom_src == src) && (trg->rom_len == len))
            break;
    }
    /* If parent, add from free list to busy list.*/
    if (!trg)
    {
        trg = WFSi_FromFreeToBusy(p_parent);
        trg->ref = 0;
        trg->rom_src = src;
        trg->rom_len = len;
        WFSi_NotifyBusy();
    }
    return trg;
}

/* Search for the registered file with the designated ID*/
static WFSiFileList *WFSi_FindAliveForID(WFSParentContext * p_parent, u32 id)
{
    WFSiFileList *trg;
    for (trg = p_parent->alive_list; trg; trg = trg->next)
    {
        if ((trg->stat == WFS_FILE_STAT_ALIVE) && (trg->info.data_info.id == id))
            break;
    }
    return trg;
}

/* Transfer to the list interval*/
static void WFSi_MoveList(WFSiFileList ** src, WFSiFileList ** dst, WFSiFileList * trg)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    /* look for self in the move origin list, insert at top of move destination list*/
    WFSiFileList **pp = src;
    for (; *pp; pp = &(*pp)->next)
    {
        if (*pp == trg)
        {
            (*pp) = trg->next;
            for (; *dst; dst = &(*dst)->next)
            {
            }
            *dst = trg;
            trg->next = NULL;
            break;
        }
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*
 * Transition the file list state from "empty" to "getting ready".
 * Increment the parent reference count.
 */
WFSiFileList *WFSi_FromFreeToBusy(WFSParentContext * p_parent)
{
    WFSiFileList *const trg = p_parent->free_list;
    /* an empty definitely exists*/
    SDK_ASSERT(trg);
    WFSi_MoveList(&(p_parent->free_list), &(p_parent->busy_list), trg);
    trg->stat = WFS_FILE_STAT_OPENING;
    ++p_parent->busy_count;
    return trg;
}

/*
 * Transition the file list state from "getting ready" to "registered".
 */
void WFSi_FromBusyToAlive(WFSParentContext * p, WFSiFileList * trg)
{
    WFSi_MoveList(&(p->busy_list), &(p->alive_list), trg);
    trg->stat = WFS_FILE_STAT_ALIVE;
}

/*
 * Transition the file list state from "registered" to "deallocating".
 */
void WFSi_FromAliveToBusy(WFSParentContext * p, WFSiFileList * trg)
{
    WFSi_MoveList(&(p->alive_list), &(p->busy_list), trg);
    trg->stat = WFS_FILE_STAT_CLOSING;
}

/*
 * Transition the file list state from "getting ready" to "empty".
 * Decrement the parent reference count.
 */
void WFSi_FromBusyToFree(WFSParentContext * p, WFSiFileList * trg)
{
    WFSi_MoveList(&(p->busy_list), &(p->free_list), trg);
    trg->stat = WFS_FILE_STAT_FREE;
    --p->busy_count;
}

/*
 * Read a specified file (child )
 */
void WFSi_ReadRequest(FSFile *p_file)
{
    WFSChildContext *const p_child = wfsi_work->context.child;
    /* record the designated file and first make the OpenFile request*/
    WFSi_ReallocBitmap(p_child, (int)p_file->arg.readfile.len);
    wfsi_work->target_file = p_file;
    if (!WFSi_SendMessage(WFS_MSG_OPENFILE_REQ,
                          WFS_BITMAP_TO_PARENT,
                          p_child->base_offset + p_file->prop.file.pos, p_file->arg.readfile.len))
    {
        OS_TPanic("WBT_PutUserData() failed! (in WFSi_ReadRequest)\n");
    }
}


/* WM *************************************************************************/

static void WFSi_SetMPData(void);
static void WFSi_OnSetMPDataDone(void *arg);
static void WFSi_PortCallback(void *arg);

#ifndef WFS_TEST_IMPL
/* parent device / child device shared packet send process*/
static void WFSi_SetMPData(void)
{
    WFSWork *const pw = wfsi_work;

    u16     size;
    if (pw->is_parent)
        size = (u16)pw->parent_packet_size;
    else
        size = (u16)pw->child_packet_size;

    /* confirm that sending and receiving is complete for the previous frame*/
    if (pw->send_idle)
    {
        int     ret;
        void   *buf = pw->send_buf;
        if (pw->is_parent)
            size = (u16)WBT_MpParentSendHook(buf, size);
        else
            size = (u16)WBT_MpChildSendHook(buf, size);
        ret = WM_SetMPDataToPort(WFSi_OnSetMPDataDone, buf, size, 0xFFFF,
                                 (u16)pw->port, WM_PRIORITY_LOW);
        pw->is_running = (ret == WM_ERRCODE_OPERATING);
        pw->send_idle = !wfsi_work->is_running;
    }
}

/* Process when SetMPData completes*/
static void WFSi_OnSetMPDataDone(void *arg)
{
    WFSWork *const pw = wfsi_work;

    (void)arg;

    if (pw && (pw->state != WFS_STATE_STOP))
    {
        pw->send_idle = TRUE;
        WFSi_SendAck();
        WFSi_SetMPData();
    }
}

/* port reception callback*/
static void WFSi_PortCallback(void *arg)
{
    WFSWork *const pw = wfsi_work;
    WFSParentContext *const p_parent = pw->context.parent;

    if (pw && (pw->state != WFS_STATE_STOP))
    {

        const WMPortRecvCallback *const p_recv = (const WMPortRecvCallback *)arg;

        switch (p_recv->state)
        {

        case WM_STATECODE_CONNECTED:
            /* if stopped on the parent device, initiate child device connection*/
            if (pw->is_parent && (pw->state == WFS_STATE_READY))
            {
                if (!pw->is_running)
                    WFSi_SetMPData();
            }
            break;

        case WM_STATECODE_DISCONNECTED:
            if (pw->is_parent && (pw->state == WFS_STATE_READY))
            {
                /* delete the detected child device used by the WBT port from the bitmap*/
                const int aid = p_recv->aid;
                p_parent->all_bitmap &= ~(1 << aid);
                /* Cancels WBT command*/
                (void)WBT_CancelCurrentCommand((u16)(1 << aid));
            }
            break;

        case WM_STATECODE_PORT_RECV:
            {                          /* hook the packet received from the partner to WBT*/
                const void *src = p_recv->data;
                const int len = p_recv->length;
                if (pw->is_parent && (pw->state == WFS_STATE_READY))
                {
                    /* add the detected child device used by the WBT port to the bitmap*/
                    const int aid = p_recv->aid;
                    p_parent->all_bitmap |= (1 << aid);
                    WBT_MpParentRecvHook(src, len, aid);
                }
                else
                {
                    WBT_MpChildRecvHook(src, len);
                }
            }
            break;

        case WM_STATECODE_PORT_INIT:
        case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
            break;
        }

    }
}

#else

static void WFSi_SetMPData(void)
{
    return;
}

static void WFSi_PortCallback(void *arg)
{
    return;
}

void WFS_SendMpFrame()
{
    WFSWork *const pw = wfsi_work;

    u16     size;
    if (pw->is_parent)
        size = (u16)pw->parent_packet_size;
    else
        size = (u16)pw->child_packet_size;

    /* confirm that sending and receiving is complete for the previous frame*/
    //if (pw->send_idle)
    {
        int     ret;
        void   *buf = pw->send_buf;
        if (pw->is_parent)
            size = (u16)WBT_MpParentSendHook(buf, size);
        else
            size = (u16)WBT_MpChildSendHook(buf, size);
        ret = MP_SendDataToPort(buf, size, 0xFFFF,
                               (u16)pw->port);
        pw->is_running = TRUE;//(ret == WM_ERRCODE_OPERATING);
        pw->send_idle = !wfsi_work->is_running;
    }
    {
        if (pw && (pw->state != WFS_STATE_STOP))
        {
            pw->send_idle = TRUE;
            WFSi_SendAck();
        }
    }
}

/*
u16 WFS_SendHook(void* buf, int len)
{
    WFSWork *const pw = wfsi_work;

    {
        int     ret;
        u16     size;
        if (pw->is_parent)
            size = (u16)WBT_MpParentSendHook(buf, len);
        else
            size = (u16)WBT_MpChildSendHook(buf, len);
        ret = MP_SendDataToPort(buf, size, 0xFFFF,
                               (u16)pw->port);
        pw->is_running = TRUE;//(ret == WM_ERRCODE_OPERATING);
        pw->send_idle = !wfsi_work->is_running;
    }
}

void WFS_SendDoneHook(void *arg)
{
    WFSWork *const pw = wfsi_work;

    (void)arg;

    if (pw && (pw->state != WFS_STATE_STOP))
    {
        pw->send_idle = TRUE;
        WFSi_SendAck();
    }
}
*/

/* port reception callback*/
void WFS_RecvHook(void* src, int len, int aid)
{
    WFSWork *const pw = wfsi_work;
    WFSParentContext *const p_parent = pw->context.parent;

    if (pw && (pw->state != WFS_STATE_STOP))
    {
        if (pw->is_parent)
        {
            /* add the detected child device used by the WBT port to the bitmap*/
            p_parent->all_bitmap |= (1 << aid);
            WBT_MpParentRecvHook(src, len, aid);
        }
        else
        {
            WBT_MpChildRecvHook(src, len);
        }
    }
}


#endif

/* WBT ************************************************************************/

static void WFSi_OnParentSystemCallback(void *arg);
static void WFSi_OnChildSystemCallback(void *arg);


/* parent device callback function*/
static void WFSi_OnParentSystemCallback(void *arg)
{
    WBTCommand *uc = (WBTCommand *)arg;
    const int peer_aid = WBT_AidbitmapToAid(uc->peer_bmp);

    switch (uc->command)
    {

    default:
        WFS_DEBUG_OUTPUT("invalid code %d\n", uc->command);
        break;

    case WBT_CMD_SYSTEM_CALLBACK:
        /* indication*/

        switch (uc->event)
        {
        case WBT_CMD_REQ_USER_DATA:
            /* WFS-specific message notification*/
            {
                const WFSiMessage *const msg = (const WFSiMessage *)uc->user_data.data;

                /* save the received message*/
                wfsi_work->context.parent->recv_msg[peer_aid] = *msg;

                switch (msg->type)
                {

                case WFS_MSG_OPENFILE_REQ:
                    /* OPENFILE request*/
                    WFS_DEBUG_OUTPUT("recv : WFS_MSG_OPENFILE_REQ(0x%08X[0x%8X]) from 0x%02X\n",
                                     msg->arg, msg->flag, uc->peer_bmp);
                    {
                        const u32 src = msg->arg;
                        const u32 len = msg->flag;
                        const int packet_size = (int)((msg->pck_h << 8) | msg->pck_l);

                        WFSParentContext *const p_parent = wfsi_work->context.parent;

                        WFS_DEBUG_OUTPUT("       packet-size(%d)\n", packet_size);

                        /*
                         * For requests for the old size of a packet while that packed size is being changed,
                         * you will be made to wait until the processing be accepted completes, then the response will be a denial.
                         */
                        if (p_parent->is_changing || (packet_size != wfsi_work->parent_packet_size))
                        {
                            p_parent->is_changing = TRUE;
                            p_parent->deny_bitmap |= (1 << peer_aid);
                            p_parent->busy_bitmap |= (1 << peer_aid);
                        }
                        /*
                         * If in normal status, search list for identical request content.
                         */
                        else
                        {
                            WFSiFileList *trg;

                            /* if currently registered, reference that*/
                            trg = WFSi_FindAlive(p_parent, src, len);
                            if (trg)
                            {
                                p_parent->ack_bitmap |= (1 << peer_aid);
                                p_parent->recv_msg[peer_aid].arg = trg->own_id;
                                ++trg->ref;
                            }
                            else
                            {
                                /*
                                 * If in preparation, reference it.
                                 * If not add new task.
                                 */
                                trg = WFSi_FindBusy(p_parent, src, len);
                                trg->req_bitmap |= (1 << peer_aid);
                            }
                            p_parent->busy_bitmap |= (1 << peer_aid);
                        }
                    }
                    break;

                case WFS_MSG_CLOSEFILE_REQ:
                    /* CLOSEFILE request*/
                    WFS_DEBUG_OUTPUT("recv : WFS_MSG_CLOSEFILE_REQ(0x%08X) from 0x%02X\n", msg->arg,
                                     uc->peer_bmp);
                    {
                        WFSParentContext *const p_parent = wfsi_work->context.parent;

                        {
                            OSIntrMode bak_cpsr = OS_DisableInterrupts();

                            /* immediately return only the response message*/
                            p_parent->ack_bitmap |= (1 << peer_aid);

                            {          /* simply ignore if there is no registered file*/
                                WFSiFileList *trg = WFSi_FindAliveForID(p_parent, msg->arg);
                                if (trg)
                                {
                                    /* if the last reference has been deleted, go to deallocation process*/
                                    /* Has this been crammed into WFSi_FindAliveForID()? */
                                    if (--trg->ref <= 0)
                                    {
                                        trg->req_bitmap = 0;
                                        (void)WBT_UnregisterBlock(msg->arg);
                                        WFSi_FromAliveToBusy(p_parent, trg);
                                        WFSi_NotifyBusy();
                                    }
                                }
                            }
                            (void)OS_RestoreInterrupts(bak_cpsr);
                        }
                    }
                    break;

                }
            }
            break;

        case WBT_CMD_REQ_SYNC:
            /*
             * Synch notification from child
             * No particular operation needs to be done.
             */
            break;

        case WBT_CMD_PREPARE_SEND_DATA:
            /*
             * Virtual page file preparation request
             * Provide data pointer if it can be ready at this point
             * If not, prepare for the next request.
             */
            {
                WFSParentContext *const p_parent = wfsi_work->context.parent;
                WFSiFileList *trg;

                WBTPrepareSendDataCallback *const p_prep = &uc->prepare_send_data;

                const u32 id = p_prep->block_id;

                p_prep->data_ptr = NULL;

                /* Determine whether file has been registered*/
                trg = WFSi_FindAliveForID(p_parent, id);
                if (trg)
                {

                    const int width = p_prep->own_packet_size;
                    const int bak_req_seq = (int)trg->ack_seq;
                    const int new_req_seq = p_prep->block_seq_no;
                    int     cur_page;

                    /* if there is a cache reloading, check for completion*/
                    if ((trg->busy_page != WFS_FILE_CACHE_LINE) && !FS_IsBusy(&trg->file))
                        trg->busy_page = WFS_FILE_CACHE_LINE;

                    /* current request number responds next*/
                    trg->ack_seq = (u32)new_req_seq;

                    /* search for the page that hits the cache in the previous request number*/
                    {
                        const int pos = width * bak_req_seq;
                        for (cur_page = 0; cur_page < WFS_FILE_CACHE_LINE; ++cur_page)
                        {
                            if (cur_page != trg->busy_page)
                            {
                                const int ofs = pos - trg->page[cur_page];
                                if ((ofs >= 0) && (ofs + width <= WFS_FILE_CACHE_SIZE))
                                {
                                    p_prep->block_seq_no = bak_req_seq;
                                    p_prep->data_ptr = trg->cache[cur_page] + ofs;
                                    WFS_DEBUG_OUTPUT("       response now!(%5d)\n", bak_req_seq);
                                    break;
                                }
                            }
                        }
                        if (cur_page >= WFS_FILE_CACHE_LINE)
                            WFS_DEBUG_OUTPUT("           cache missed!(%5d)\n", bak_req_seq);
                    }

                    /* Determine whether will cache miss the curent request number*/
                    if (trg->busy_page == WFS_FILE_CACHE_LINE)
                    {
                        int     i;
                        int     pos = width * new_req_seq;
                        for (i = 0; i < WFS_FILE_CACHE_LINE; ++i)
                        {
                            const int ofs = pos - trg->page[i];
                            if ((ofs >= 0) && (ofs + width <= WFS_FILE_CACHE_SIZE))
                            {
                                break;
                            }
                        }
                        /* If a cache miss, round-robin reload*/
                        if (i >= WFS_FILE_CACHE_LINE)
                        {
                            int     new_page = trg->last_page;
                            do
                            {
                                ++new_page;
                                if (new_page >= WFS_FILE_CACHE_LINE)
                                    new_page = 0;
                            }
                            while (new_page == cur_page);
                            trg->last_page = new_page;
                            trg->busy_page = new_page;
                            pos &= ~(CARD_ROM_PAGE_SIZE - 1);
                            trg->page[new_page] = pos;
                            (void)FS_SeekFile(&trg->file, pos, FS_SEEK_SET);
                            (void)FS_ReadFileAsync(&trg->file, trg->cache[new_page],
                                                   WFS_FILE_CACHE_SIZE);
                            WFS_DEBUG_OUTPUT("               ... and now loading!(%5d)\n",
                                             new_req_seq);
                        }
                    }
                }

            }
            break;

        case WBT_CMD_REQ_GET_BLOCK_DONE:
            break;

        default:
            WFS_DEBUG_OUTPUT("sys-callback code %d\n", uc->event);
            break;
        }
        break;
    }
}

/*
 * Reallocate bitmap that can receive the specified file size.
 * If size < 0, unconditionally reallocate current settings
 */
static void WFSi_ReallocBitmap(WFSChildContext * p_child, int size)
{
    if (size < 0)
    {
        size = (int)p_child->max_file_size;
        p_child->max_file_size = 0;
    }
    if (p_child->max_file_size < size)
    {
        p_child->max_file_size = (u32)size;
        WFS_DEBUG_OUTPUT("WFSi_ReallocBitmap...(%d:%d)->%d\n",
                         wfsi_work->parent_packet_size, p_child->max_file_size,
                         WBT_PACKET_BITMAP_SIZE(p_child->max_file_size,
                                                wfsi_work->parent_packet_size));
        WFSi_Free(p_child->recv_pkt_bmp_buf);
        p_child->recv_pkt_bmp_buf = (u32 *)(WFSi_Alloc((int)(sizeof(u32) *
                                                             WBT_PACKET_BITMAP_SIZE(p_child->
                                                                                    max_file_size,
                                                                                    wfsi_work->
                                                                                    parent_packet_size))));
        p_child->recv_buf_packet_bmp_table.packet_bitmap[0] = p_child->recv_pkt_bmp_buf;
    }
    SDK_ASSERT(p_child->recv_pkt_bmp_buf);
}


/* child device -side callback function*/
void WFSi_OnChildSystemCallback(void *arg)
{
    WBTCommand *uc = (WBTCommand *)arg;
    int     peer_aid = WBT_AidbitmapToAid(uc->peer_bmp);
    WFSChildContext *const p_child = wfsi_work->context.child;

    switch (uc->command)
    {
    case WBT_CMD_REQ_SYNC:
        /* WBT_RequestSync complete*/
        if (!uc->target_bmp)
        {
            /*
             * Acquire MP packet size set by parent.
             * This size is used for sequence number management
             * and by WM_SetMPDataToPort. Both are required.
             */
            wfsi_work->parent_packet_size = uc->sync.peer_packet_size + WBT_PACKET_SIZE_MIN;
            wfsi_work->child_packet_size = uc->sync.my_packet_size + WBT_PACKET_SIZE_MIN;
            WFS_DEBUG_OUTPUT("Sync(%d, %d)\n",
                             wfsi_work->parent_packet_size, wfsi_work->child_packet_size);
            /*
             * The first time acquire total size of FAT+FNT+OVT information.
             * The parent device side normally guarantees that this is
             * at the top of the block list (index=0)
             */
            if (!wfsi_work->table)
            {
                (void)WBT_GetBlockInfo(WFS_BITMAP_TO_PARENT,
                                       0, &p_child->block_info_table, WFSi_OnChildSystemCallback);
            }
            /*
             * In the case of doing sync again, it will be considered that OpenFile was denied.
             * Therefore it will retry the same request.
             */
            else if (wfsi_work->target_file)
            {
                WFSi_ReallocBitmap(p_child, -1);
                WFSi_ReadRequest(wfsi_work->target_file);
            }
        }
        break;

    case WBT_CMD_REQ_GET_BLOCKINFO:
        /* WBT_GetBlockInfo complete*/
        /*
         * Load FAT+FNT+OVT information.
         */
        wfsi_work->table_size = (u32)p_child->block_info_table.block_info[peer_aid]->block_size;
        wfsi_work->table = (u8 *)WFSi_Alloc((int)wfsi_work->table_size);
        SDK_ASSERT(wfsi_work->table);
        WFSi_ReallocBitmap(p_child, (int)wfsi_work->table_size);
        p_child->recv_buf_table.recv_buf[peer_aid] = wfsi_work->table;
        (void)WBT_GetBlock(WFS_BITMAP_TO_PARENT,
                           WFS_FILE_TO_BLOCK(WFS_FILE_TABLEINFO),
                           &p_child->recv_buf_table,
                           wfsi_work->table_size,
                           &p_child->recv_buf_packet_bmp_table, WFSi_OnChildSystemCallback);
        break;

    case WBT_CMD_REQ_GET_BLOCK:
        /*
         * Complete WBT_GetBlock
         * This becomes a receive complete notification for
         * either FAT+FNT+OVT information, or a normal file.
         */
        if (!uc->target_bmp)
        {
            /* rebuild archive if table load is complete*/
            if (wfsi_work->state != WFS_STATE_READY)
            {
                DC_FlushRange(wfsi_work->table, wfsi_work->table_size);
                p_child->base_offset = WFSi_ReplaceRomArchive(wfsi_work->table);
                /* notify with a callback that there is a transfer to a state where FS is usable*/
                wfsi_work->state = WFS_STATE_READY;
                if (wfsi_work->state_func)
                {
                    (*wfsi_work->state_func) (NULL);
                }
            }
            /* for all else,  ReadFile complete*/
            else
            {
                /* CloseFile request*/
                if (!WFSi_SendMessage(WFS_MSG_CLOSEFILE_REQ,
                                      WFS_BITMAP_TO_PARENT, p_child->block_id, TRUE))
                {
                    OS_TPanic("WBT_PutUserData() failed! (in WFSi_ReadRequest)\n");
                }
            }
        }
        break;

    default:
        WFS_DEBUG_OUTPUT("invalid code %d\n", uc->command);
        break;

        /* indication*/
    case WBT_CMD_SYSTEM_CALLBACK:

        switch (uc->event)
        {
        case WBT_CMD_REQ_USER_DATA:

            /* notification*/
            {
                const WFSiMessage *const msg = (const WFSiMessage *)uc->user_data.data;

                switch (msg->type)
                {

                case WFS_MSG_OPENFILE_ACK:
                    WFS_DEBUG_OUTPUT("recv : WFS_MSG_OPENFILE_ACK(0x%08X)[%c] from 0x%02X\n",
                                     msg->arg, (msg->flag ? 'T' : 'F'), uc->peer_bmp);

                    /* if denied, the packet size will have changed, so retry from Sync*/
                    if (!msg->flag)
                    {
                        /* Sync command makes first contact with the parent device*/
                        WFS_DEBUG_OUTPUT("recv : openfile kicked(packet-size changed)\n");
                        (void)WBT_RequestSync(WFS_BITMAP_TO_PARENT, WFSi_OnChildSystemCallback);
                    }
                    /* In line with permit response, begin file receive*/
                    else
                    {
                        /* Start reception with the received block ID*/
                        p_child->block_id = msg->arg;
                        p_child->recv_buf_table.recv_buf[peer_aid] =
                            wfsi_work->target_file->arg.readfile.dst;
                        WFS_DEBUG_OUTPUT("send : GetBlock(0x%08X, %d)\n",
                                         p_child->block_id,
                                         wfsi_work->target_file->arg.readfile.len);
                        (void)WBT_GetBlock(WFS_BITMAP_TO_PARENT,
                                           p_child->block_id,
                                           &p_child->recv_buf_table,
                                           wfsi_work->target_file->arg.readfile.len,
                                           &p_child->recv_buf_packet_bmp_table,
                                           WFSi_OnChildSystemCallback);
                    }
                    break;

                case WFS_MSG_CLOSEFILE_ACK:
                    WFS_DEBUG_OUTPUT("recv : WFS_MSG_CLOSEFILE_ACK(0x%08X)[%c] from 0x%02X\n",
                                     msg->arg, (msg->flag ? 'T' : 'F'), uc->peer_bmp);
                    {                  /* In line with Closefile response complete receive*/
                        FSFile *const trg = wfsi_work->target_file;
                        FSArchive *const p_arc = FS_GetAttachedArchive(wfsi_work->target_file);
                        wfsi_work->target_file = NULL;
                        /* advance the file pointer here*/
                        trg->prop.file.pos += trg->arg.readfile.len;
                        FS_NotifyArchiveAsyncEnd(p_arc, FS_RESULT_SUCCESS);
                    }
                    break;

                }
            }
            break;

        default:
            WFS_DEBUG_OUTPUT("sys-callback code %d\n", uc->event);
            break;
        }
        break;
    }
}


/* interface ******************************************************************/

/*---------------------------------------------------------------------------*
  Name:         WFSi_InitCommon

  Description:  initialize Wireless File System common parameters.

  Arguments:    port             available WM port for WFS
                callback         callback of various state-notifications.
                allocator        dynamic memory allocator for WFS
                allocator_arg    argument of 'allocator'

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WFSi_InitCommon(int port, WFSStateCallback callback,
                            WFSAllocator allocator, void *allocator_arg)
{
    WFSWork *pw;

    pw = (*allocator) (allocator_arg, sizeof(WFSWork), NULL);
    if (!pw)
    {
        OS_TPanic("cannot allocate enough memory for WFS");
    }
    wfsi_work = pw;

    /*
     * Parent and child both need an FS.
     * If not yet initialized, do it here.
     * (The parent side will probably be initialized when multibooting)
     */
    FS_Init(FS_DMA_NOT_USE);
    pw->port = (u16)port;
    pw->send_idle = FALSE;
    pw->is_running = FALSE;

    pw->state = WFS_STATE_STOP;
    pw->state_func = callback;
    pw->alloc_func = allocator;
    pw->alloc_arg = allocator_arg;
    pw->target_file = NULL;
    pw->table_size = 0;
    pw->table = NULL;

    pw->parent_packet_size = 0;
    pw->child_packet_size = WBT_PACKET_SIZE_MIN;
    wfsi_debug_enable = FALSE;

    /*
     * Set the callback that drives basic WBT operation.
     * fail if WM is still in a STOP state
     */
    if (WM_SetPortCallback((u16)port, WFSi_PortCallback, NULL) != WM_ERRCODE_SUCCESS)
    {
        OS_Panic("wireless file-system : " "WFS_Init failed! (not called WM_Initialize yet)");
    }
}

/*---------------------------------------------------------------------------*
  Name:         WFS_InitParent

  Description:  initialize Wireless File System parent parameters.

  Arguments:    port             available WM port for WFS
                callback         callback of various state-notifications.
                allocator        dynamic memory allocator for WFS
                allocator_arg    argument of 'allocator'
                parent_packet    available WM packet for WFS
                p_rom            target file which directs SRL-program
                                 to export FAT/FNT and OVT.
                                 if specified NULL, use own.
                use_parent_fs    if TRUE, ignore p_rom's FAT/FNT and use own.
                                 this setting enables both independent
                                 program to share FAT/FNT (except OVT).
                                 if p_rom is NULL, always as TRUE.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_InitParent(int port, WFSStateCallback callback,
                    WFSAllocator allocator, void *allocator_arg, int parent_packet,
                    FSFile *p_rom, BOOL use_parent_fs)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    if (!init_flag)
    {
        init_flag = TRUE;
        WFSi_InitCommon(port, callback, allocator, allocator_arg);
        (void)OS_RestoreInterrupts(bak_cpsr);

        {
            WFSWork *const pw = wfsi_work;
            WFSParentContext *const p = pw->context.parent;

            pw->is_parent = TRUE;

            MI_CpuClear8(p, sizeof(*p));
            DC_FlushRange(p, sizeof(*p));
            DC_WaitWriteBufferEmpty();

            {
                /* Add all block information to the empty list*/
                enum
                { BLOCK_TOTAL_COUNT = WFS_FILE_HANDLE_MAX };
                WFSiFileList *pb = &p->list[0];
                p->free_list = pb;
                for (; ; pb = pb->next)
                {
                    pb->next = pb + 1;
                    FS_InitFile(&pb->file);
                    pb->ref = 0;
                    pb->stat = WFS_FILE_STAT_FREE;
                    pb->own_id = (u32)(WFS_FILE_TABLEINFO + 1 + pb - &p->list[0]);
                    if (pb->next >= &p->list[BLOCK_TOTAL_COUNT])
                    {
                        pb->next = NULL;
                        break;
                    }
                }
            }

            p->sync_bitmap = 0;
            p->ack_bitmap = 0;
            p->msg_busy = FALSE;
            p->alive_list = NULL;
            p->busy_list = NULL;
            pw->parent_packet_size = parent_packet;

            p->all_bitmap = 1;
            p->busy_bitmap = 0;

            p->is_changing = FALSE;
            p->deny_bitmap = 0;
            p->busy_count = 0;
            p->new_packet_size = pw->parent_packet_size;

            DC_FlushRange(p, sizeof(*p));
            DC_WaitWriteBufferEmpty();

            /*
             * Initialize WBT
             *
             * If the WBT packet size is reduced smaller than the actual maximum size,
             * it can be incuded in data sharing, etc. for single MP network communications, but
             * if that is done, the regular maximum transmission size will be lowered.
             * WFS has the lowest priority and runs in the background.
             * Therefore allocating the maximum size possible will not cause any particular problems.
             */
            WBT_InitParent(pw->parent_packet_size,
                           pw->child_packet_size, WFSi_OnParentSystemCallback);

            /*
             * Reads FNT+FAT+OVT information, and registers it as a file.
             * The following selections are possible:
             * (1) Normal independent child program: p_rom && !use_parent_fs
             * (2) File shared type child program: p_rom && use_parent_fs
             * (3) Parent clone boot: !p_rom   * Implementation under study
             */
            WFSi_LoadTables(p_rom, use_parent_fs);
            {
                WFSiFileList *pb = p->free_list;
                BOOL    ret = FALSE;
                p->free_list = pb->next;
                pb->next = p->alive_list;
                p->alive_list = pb;
                pb->ref = 1;
                MI_CpuClear8(pb, sizeof(*pb));
                ret = WBT_RegisterBlock(&pb->info,
                                        (u32)WFS_FILE_TO_BLOCK(WFS_FILE_TABLEINFO),
                                        wfsi_def_user_data, pw->table,
                                        (int)wfsi_work->table_size, 0);
                SDK_ASSERT(ret);
            }
            /* If WFS_Start is called during initialization, immediately start network communications*/
            bak_cpsr = OS_DisableInterrupts();
            pw->state = WFS_STATE_IDLE;
            if (pw->send_idle)
            {
                WFS_Start();
            }
            (void)OS_RestoreInterrupts(bak_cpsr);

            /* start up the task thread that automatically processes the child device request*/
            WFSi_CreateTaskThread();
        }
    }
    else
    {
        (void)OS_RestoreInterrupts(bak_cpsr);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WFS_InitChild

  Description:  initialize Wireless File System child parameters.

  Arguments:    port             available WM port for WFS
                callback         callback of various state-notifications.
                allocator        dynamic memory allocator for WFS
                allocator_arg    argument of 'allocator'

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_InitChild(int port, WFSStateCallback callback, WFSAllocator allocator, void *allocator_arg)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    if (!init_flag)
    {
        init_flag = TRUE;
        WFSi_InitCommon(port, callback, allocator, allocator_arg);
        (void)OS_RestoreInterrupts(bak_cpsr);

        {
            WFSWork *const pw = wfsi_work;
            WFSChildContext *const p = pw->context.child;

            pw->is_parent = FALSE;
            DC_FlushRange(p, sizeof(*p));
            DC_WaitWriteBufferEmpty();

            {                          /* Initialize receive array for GetBlockInfo, GetBlock*/
                int     i;
                for (i = 0; i < WBT_NUM_OF_AID; ++i)
                {
                    p->block_info_table.block_info[i] = &p->block_info[i];
                    p->recv_buf_table.recv_buf[i] = NULL;
                    p->recv_buf_packet_bmp_table.packet_bitmap[i] = NULL;
                }
            }

            /* initialize the information that is known only when receiving table information from the parent device*/
            p->recv_pkt_bmp_buf = NULL;
            p->max_file_size = 0;
            p->block_id = 0;

            DC_FlushRange(p, sizeof(*p));
            DC_WaitWriteBufferEmpty();

            /*
             * Initialize WBT
             *
             * Actual process flow:
             * (1) FS_ReadFile(Async) -> wfs_archive
             * (2) wfs_archive -> WBT
             * (3) WBT -> FS_NotifyArchiveAsyncEnd
             * After this, file access is complete.
             */
            WBT_InitChild(WFSi_OnChildSystemCallback);
            /* If WFS_Start is called during initialization, immediately start network communications*/
            bak_cpsr = OS_DisableInterrupts();
            pw->state = WFS_STATE_IDLE;
            if (pw->send_idle)
            {
                WFS_Start();
            }
            (void)OS_RestoreInterrupts(bak_cpsr);
        }
    }
    else
    {
        (void)OS_RestoreInterrupts(bak_cpsr);
    }
}

void WFS_Start(void)
{
    OSIntrMode bak_cpsr;
    SDK_ASSERT(init_flag);

    bak_cpsr = OS_DisableInterrupts();
    /* If initializing, extend the network communications start process*/
    if (WFS_GetStatus() != WFS_STATE_IDLE)
    {
        wfsi_work->send_idle = TRUE;
    }
    /* If not, start network communications*/
    else
    {
        wfsi_work->send_idle = TRUE;
        /* The child device side gets it own AID*/
        if (!wfsi_work->is_parent)
        {
            WFSChildContext *const p = wfsi_work->context.child;
            int     ret = WM_ReadStatus(p->status);
            SDK_ASSERT(ret == WM_ERRCODE_SUCCESS);
            WBT_SetOwnAid(p->status->aid);
            /* Sync command makes first contact with the parent device*/
            (void)WBT_RequestSync(WFS_BITMAP_TO_PARENT, WFSi_OnChildSystemCallback);
        }
        /* Immeditately transfer the parent device side to a READY state*/
        else
        {
            wfsi_work->state = WFS_STATE_READY;
            if (wfsi_work->state_func)
            {
                (*wfsi_work->state_func) (NULL);
            }
        }
        /* configure the first data after connecting*/
        WFSi_SetMPData();
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
}


void WFS_End(void)
{
    WFSWork *const pw = wfsi_work;

    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    if (init_flag)
    {

        /* deallocate table  data*/
        if (pw->table)
        {
            WFSi_Free(pw->table);
            pw->table = NULL;
            pw->table_size = 0;
        }

        pw->state = WFS_STATE_STOP;

        /* for child device, stop if processing and deallocate the bitmap*/
        if (!wfsi_work->is_parent)
        {
            WFSChildContext *const p_child = pw->context.child;
            if (pw->target_file)
            {
                FSFile *const trg = pw->target_file;
                FSArchive *const p_arc = FS_GetAttachedArchive(trg);
                pw->target_file = NULL;
                FS_NotifyArchiveAsyncEnd(p_arc, FS_RESULT_ERROR);
            }
            WFSi_Free(p_child->recv_pkt_bmp_buf);
        }
        /* for parent device, end file management thread*/
        else
        {
            WFSi_EndTaskThread();
        }

        /* end WBT*/
        WBT_End();

        /* Add/remove callback for driving WBT (ignore if WM has ended)*/
        (void)WM_SetPortCallback(wfsi_work->port, NULL, NULL);

        WFSi_Free(wfsi_work);
        wfsi_work = NULL;
        init_flag = FALSE;

    }
    (void)OS_RestoreInterrupts(bak_cpsr);
}


/*
 * Acquire current status.
 */
WFSState WFS_GetStatus(void)
{
    return wfsi_work ? wfsi_work->state : WFS_STATE_STOP;
}

/*---------------------------------------------------------------------------*
  Name:         WFS_GetCurrentBitmap

  Description:  Gets the current child device group bitmap that WBT is aware of.
                This function can only be called by parent.

  Arguments:    None.

  Returns:      Currently recognized child devices.
 *---------------------------------------------------------------------------*/
int WFS_GetCurrentBitmap(void)
{
    if (!init_flag || !wfsi_work->is_parent)
        return 0;
    return wfsi_work->context.parent->all_bitmap;
}

/*---------------------------------------------------------------------------*
  Name:         WFS_GetSyncBitmap

  Description:  Gets bitmap of child group with accessd sync specified.
                This function can only be called by parent.

  Arguments:    None.

  Returns:      Child devices that are specified to synchronize access
 *---------------------------------------------------------------------------*/
int WFS_GetSyncBitmap(void)
{
    if (!init_flag || !wfsi_work->is_parent)
        return 0;
    return wfsi_work->context.parent->sync_bitmap;
}

/*---------------------------------------------------------------------------*
  Name:         WFS_GetPacketSize

  Description:  Gets MP communication packet size of parent set by WFS.

  Arguments:    None.

  Returns:      MP communication packet size of parent set by WFS.
 *---------------------------------------------------------------------------*/
int WFS_GetPacketSize(void)
{
    if (!init_flag)
        return 0;
    return wfsi_work->parent_packet_size;
}

/*---------------------------------------------------------------------------*
  Name:         WFS_GetBusyBitmap

  Description:  Gets bitmap of child devices that are currently accessing parent.
                This function can only be called by parent.

  Arguments:    None.

  Returns:      Child devices that are currently accessing parent.
 *---------------------------------------------------------------------------*/
int WFS_GetBusyBitmap(void)
{
    if (!init_flag || !wfsi_work->is_parent)
        return 0;
    return wfsi_work->context.parent->busy_bitmap;
}

/*---------------------------------------------------------------------------*
  Name:         WFS_IsBusy

  Description:  Determines whether or not the child device of a designated aid is being accessed.
                This function can only be called by parent.

  Arguments:    aid              aid of child to check.

  Returns:      If the designated child device is being accessed, TRUE. Otherwise, FALSE.
 *---------------------------------------------------------------------------*/
BOOL WFS_IsBusy(int aid)
{
    return ((1 << aid) & WFS_GetBusyBitmap()) != 0;
}

/* Get the progress sequence of the current ReadFile file. (child device)*/
BOOL WFS_GetCurrentDownloadProgress(int *current_count, int *total_count)
{
    if (!init_flag || wfsi_work->is_parent || (wfsi_work->state != WFS_STATE_READY)
        || !wfsi_work->target_file)
        return FALSE;
    return
        WBT_GetCurrentDownloadProgress(wfsi_work->context.child->block_id,
                                       0, current_count, total_count);
}

/*
 * Sets the packet size sent from the parent. (parent only)
 * This function can only be called before calling WFS_Start(),
 * Calling can only take place when all child devices
 * are in a state where connection has completed properly.
 * If called at any other time, we do not guarantee the operation.
 */
void WFS_SetPacketSize(int size)
{
    SDK_ASSERT(init_flag);
    SDK_ASSERT(wfsi_work->is_parent);
    SDK_ASSERT(size >= WBT_PACKET_SIZE_MIN);

    {
        WFSParentContext *const p = wfsi_work->context.parent;
        OSIntrMode bak_cpsr = OS_DisableInterrupts();
        p->new_packet_size = size;
        p->is_changing = TRUE;
        /* If still busy, immediately reset size.*/
        if (wfsi_work->state != WFS_STATE_READY)
        {
            p->is_changing = FALSE;
            p->deny_bitmap = 0;
            wfsi_work->parent_packet_size = p->new_packet_size;
        }
        (void)OS_RestoreInterrupts(bak_cpsr);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WFS_EnableSync

  Description:  Configures the settings of the child device group that takes access synchronization on the parent device side.
                By synchronizing the response towards child device groups for which
                the access of identical file groups is clearly guaranteed, this function can
                make use of the WBT library attributes to achieve efficient transmissions.
                However, if the synchronization start timing is not logically safe,
                you will need to watch for the response to the child device group being off and deadlocks.
                This function can only be called by parent.

  Arguments:    sync_bitmap      The aid bitmap of the child device group that takes the access synchronization.
                                 The lowest bit 1 indicating the parent device itself is ignored.
                                 By assigning 0 for this value, synchronicity does not occur.
                                 This state by default.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_EnableSync(int sync_bitmap)
{
    SDK_ASSERT(init_flag && wfsi_work->is_parent);

    {
        OSIntrMode bak_cpsr = OS_DisableInterrupts();

        WFSParentContext *const p_parent = wfsi_work->context.parent;

        p_parent->sync_bitmap = (sync_bitmap & ~1);

        (void)OS_RestoreInterrupts(bak_cpsr);
    }
}

/*
 * Enables / Disables WFS internal debug output.
 */
void WFS_SetDebugMode(BOOL e)
{
//      SDK_ASSERT(init_flag);
    wfsi_debug_enable = e;
}
