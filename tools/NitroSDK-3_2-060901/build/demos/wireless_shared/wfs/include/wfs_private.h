/*---------------------------------------------------------------------------*
  Project:  NitroSDK - wireless_shared - demos - wfs
  File:     wfs_private.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wfs_private.h,v $
  Revision 1.9  05/22/2006 02:33:11  yosizaki
  small fix.

  Revision 1.8  02/27/2006 13:15:06  yosizaki
  increase WFS_FILE_HANDLE_MAX.

  Revision 1.7  01/18/2006 02:12:39  kitase_hirotake
  do-indent

  Revision 1.6  10/20/2005 08:12:22  yosizaki
  change to allocate wok-memory in WFS_Init.

  Revision 1.5  07/07/2005 05:45:36  yada
  add warn_padding to WFSiFileList

  Revision 1.4  02/28/2005 05:26:34  yosizaki
  do-indent.

  Revision 1.3  02/18/2005 09:15:45  yosizaki
  Revised copyright year

  Revision 1.2  02/18/2005 09:14:31  yosizaki
  fix around hidden warnings.

  Revision 1.1  11/16/2004 01:13:04  yosizaki
  moved from /demos/wbt/wbt-fs.


 -[old log at /demos/wbt/wbt-fs/wfs]-

  Revision 1.16  10/25/2004 04:18:14  yosizaki
  change WFSi_LoadTables.

  Revision 1.15  09/29/2004 07:10:11  yosizaki
  merge from branch.

  Revision 1.14  09/15/2004 09:10:45  yosizaki
  add Seek&Read.

  Revision 1.13  09/15/2004 01:11:12  yosizaki
  add WFS_SetPacketSize().

  Revision 1.12  09/11/2004 04:13:35  yosizaki
  add comments.
  unify MP-size definition.

  Revision 1.11  09/09/2004 05:04:17  yosizaki
  change to block memory-allocating critical sections.

  Revision 1.10  09/07/2004 04:08:53  yosizaki
  change to delay ack-timing.

  Revision 1.9  09/07/2004 03:03:14  yosizaki
  optimize around message.

  Revision 1.8  09/06/2004 06:23:46  yosizaki
  add double-buffering system.

  Revision 1.7  09/04/2004 13:25:46  yosizaki
  change to load TABLE-memory with WFSi_Alloc.

  Revision 1.6  09/04/2004 08:29:58  yosizaki
  change around PREPARE-response.

  Revision 1.5  09/04/2004 01:45:14  yosizaki
  delete obsolete codes and waste parameters.

  Revision 1.4  09/03/2004 12:22:10  yosizaki
  support page-cache. (wbt.c Revision 1.12 or later)

  Revision 1.3  09/03/2004 11:35:41  yosizaki
  support page-cache (test version)

  Revision 1.2  09/03/2004 09:57:50  yosizaki
  add wfs_thread.c

  Revision 1.1  09/03/2004 01:12:10  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef	NITRO_BUILD_DEMOS_WIRELESSSHARED_WFS_INCLUDE_WFS_PRIVATE_H_
#define	NITRO_BUILD_DEMOS_WIRELESSSHARED_WFS_INCLUDE_WFS_PRIVATE_H_


#include <nitro/wbt.h>
#include <nitro/wm.h>
#include <nitro/fs.h>


#if	defined(__cplusplus)
extern  "C"
{
#endif


/*****************************************************************************/
/* constant*/

/*
 * Total number of files that the parent can handle simultaneously.
 *
 * Parent uses sizeof(WFSiFileList) memory of for each one
 * file handle.
 * In WBT, the number of targets that each child can simultaneously GetBlock is always only one, and
 * considering cases where the end operation of the last time may interfere with the start operation this time,
 * if parent provides twice as many file handles as the number of children, that will be sufficient.
 */
#define	WFS_FILE_HANDLE_MAX	(WM_NUM_MAX_CHILD * 2)


/*
 * Cache size the parent allocates for FS access of each file.
 *
 * Generally, it is efficient if the integral multiple of the CARD page is taken.
 * (as long as .rsf is uniquely set and the alignment size is not changed)
 * Also, taking into consideration the crossing of the page boundary, at least 2+ pages are necessary.
 * WFS always runs as follows:
 * respond to previous request and (if necessary)
 * prepare current request. Therefore, even if you increase the cache size
 * beyond what is necessary, wireless transfer efficiency does not improve.
 */
#define	WFS_FILE_CACHE_SIZE	(CARD_ROM_PAGE_SIZE * 2)


/*
 * Number of cache pages the parent allocates for single file.
 *
 * WFS always runs as follows:
 * respond to previous request and (if necessary)
 * prepare the current request. Therefore, at least two pages are necessary.
 * When file untransferred region become fragmented in bad communication state,
 * by increasing this page number, the frequency of cache errors decreases.
 * So it is possible to decrease verbose CARD access by parent.
 */
#define	WFS_FILE_CACHE_LINE	2


/*
 * Maximum file size that child can receive.
 *
 * To simplify matters, in this sample it is set to 4 Mbytes,
 * the same as main memory.
 */
#define	WFS_FILE_SIZE_MAX	(4 * 1024 * 1024)


/*
 * Bitmap value where child specifies data send target.
 *
 * The WBT can also transfer from any child to multiple targets, but
 * the current sample simply uses a request from child to parent.
 */
#define	WFS_BITMAP_TO_PARENT	1


/*
 * Messages with which WFS sends notification using  WBT_PutUserData().
 */
#define	WFS_MSG_OPENFILE_REQ	0
#define	WFS_MSG_OPENFILE_ACK	1
#define	WFS_MSG_CLOSEFILE_REQ	2
#define	WFS_MSG_CLOSEFILE_ACK	3


/*
 * States of file handles that the parent manages for child.
 */
    typedef enum
    {
        WFS_FILE_STAT_FREE,            /* empty*/
        WFS_FILE_STAT_OPENING,         /* preparing file*/
        WFS_FILE_STAT_ALIVE,           /* file accessable*/
        WFS_FILE_STAT_CLOSING          /* closing file*/
    }
    WFSiFileStatus;


/*
 * Extended file index.
 *
 * In WFS, all requests from child are assigned in CARD-ROM region,
 * and handled as WBT block ID after locking on parent.
 *
 * By customizing this command,
 * It is possible to handle non-CARD-ROM main memory and
 *  data in VRAM as files.
 * (In that case, overrides the FS_COMMAND_OPENFILEDIRECT
 *  command in the user procedure of child archive.)
 */
#define	WFS_FILE_INDEX_OFFSET	0x10000
#define	WFS_FILE_TO_BLOCK(id)	((id) + WFS_FILE_INDEX_OFFSET)
#define	WFS_BLOCK_TO_FILE(id)	((id) - WFS_FILE_INDEX_OFFSET)
#define	WFS_FILE_TABLEINFO	0x10000 /* FNT, FAT, OVT * 2*/


/******************************************************************************/
/* macro*/

/*
 * Switches debug output.
 * If this symbol is defined, log outputs the internal operation of sample.
 */
#define	WFS_DEBUG_OUTPUT_ON


/*
 * Macro that debug outputs for testing only with the Debug/Release version and when specified.
 */
#if	defined(WFS_DEBUG_OUTPUT_ON) && !defined(SDK_FINALROM)
#define	WFS_DEBUG_OUTPUT	if(wfsi_debug_enable)	OS_TPrintf
#else
#define	WFS_DEBUG_OUTPUT(...)	(void)0
#endif


/*
 * Static declaration macro.
 * Mainly used for validity determinatoin of structure size.
 */
#define	WFS_STATIC_ASSERT(expr)	\
    static void wfs_static_assert ## __LINE__ ( char is[(expr) ? +1 : -1] )


/******************************************************************************/
/* struct*/

/* WFS_MSG_* message structure*/
    typedef struct
    {
        u32     type:4;                /* WFS_MSG_**/
        u32     pck_h:4;               /* (packet_packet_size >> 8) & 0x0F */
        u32     flag:24;               /* multi-purpose flag*/
        u32     arg;                   /* multi-purpose argument*/
        u8      pck_l;                 /* (packet_packet_size >> 0) & 0xFF */
        u8      reserved[3];
    }
    WFSiMessage;


/*
 * Singly linked list structure controlling parent file handle.
 * Binding FS file and WBT block.
 */
#pragma warn_padding off
    typedef struct WFSiFileList
    {
        /* core information*/
        struct WFSiFileList *next;
        WBTBlockInfoList info;
        FSFile  file;

        /* state management*/
        WFSiFileStatus stat;           /* file handle state*/
        int     ref;                   /* reference count*/
        int     req_bitmap;            /* child waiting for a response to the request*/

        /* ROM region lock information*/
        u32     own_id;                /* block ID (fixed)*/
        u32     rom_src;               /* starting ROM address*/
        u32     rom_len;               /* ROM read size*/

        /* cache page information*/
        u32     ack_seq;               /* the sequence number that should be returned next*/
        int     busy_page;             /* cache page number that is currently reloading*/
        int     last_page;             /* last reloaded cache page*/
        int     page[WFS_FILE_CACHE_LINE];      /* current cache page position*/
        //u8  padding1[4 + (28 - (sizeof(int) * WFS_FILE_CACHE_LINE) % 32) % 32];
        u8      cache[WFS_FILE_CACHE_LINE][WFS_FILE_CACHE_SIZE] ATTRIBUTE_ALIGN(32);
    }
    WFSiFileList;
#pragma warn_padding reset

/*
 * Variable for controlling internal state of child.
 *
 * Even if the memory is allocated so that the communication is always possible with 16 devices,
 * the requested size does not become a large one. Therefore,
 * statically allocate for concise control.
 */
    typedef struct
    {
        WBTBlockInfoTable block_info_table;
        WBTRecvBufTable recv_buf_table;
        WBTPacketBitmapTable recv_buf_packet_bmp_table;
        WBTBlockInfo block_info[WBT_NUM_OF_AID];
        WMStatus status[1] ATTRIBUTE_ALIGN(32);
        u32    *recv_pkt_bmp_buf;
        u32     base_offset;
        u32     max_file_size;
        u32     block_id;
    }
    WFSChildContext;


/*
 * Variable for controlling internal state of parent.
 */
    typedef struct
    {
        /* details of the newest requests from each child*/
        WFSiMessage recv_msg[WBT_NUM_OF_AID];

        /* List of temporary files for access based on their states*/
        WFSiFileList list[WFS_FILE_HANDLE_MAX];
        WFSiFileList *free_list;
        WFSiFileList *alive_list;
        WFSiFileList *busy_list;

        /* response bitmap from the parent*/
        int     ack_bitmap;            /* target that can respond*/
        int     sync_bitmap;           /* target designated as synchronous*/
        BOOL    msg_busy;              /* sending current response message (only one at a time possible)*/

        /* child state management bitmap*/
        int     all_bitmap;            /* connection detection state*/
        int     busy_bitmap;           /* state being accessed*/

        /* management when changing packet size*/
        BOOL    is_changing;           /* changing current size*/
        int     busy_count;            /* reference count with old size*/
        int     deny_bitmap;           /* denied response target of old size*/
        int     new_packet_size;       /* new size*/

    }
    WFSParentContext;


/*
 * Variables for controlling states of entire WFS.
 */
    typedef struct
    {
        BOOL    is_parent;
        BOOL    send_idle;
        BOOL    is_running;

        WFSState state;
        WFSStateCallback state_func;
        WFSAllocator alloc_func;
        void   *alloc_arg;

        FSFile *target_file;

        u8     *table;
        u32     table_size;

        int     parent_packet_size;
        int     child_packet_size;

        u16     port;

        u8      padding1[14];

        u8      send_buf[1024] ATTRIBUTE_ALIGN(32);

        union
        {
            WFSParentContext parent[1] ATTRIBUTE_ALIGN(32);
            WFSChildContext child[1] ATTRIBUTE_ALIGN(32);
        }
        context;

    }
    WFSWork;


    extern WFSWork *wfsi_work;
    extern BOOL wfsi_debug_enable;


/******************************************************************************/
/* functions*/

/*
 * Allocates dynamic memory in WFS.
 */
    static inline void *WFSi_Alloc(int size)
    {
        void   *ret;
        OSIntrMode bak_cpsr = OS_DisableInterrupts();
                ret = (*wfsi_work->alloc_func) (wfsi_work->alloc_arg, (u32)size, NULL);
                (void)OS_RestoreInterrupts(bak_cpsr);
                WFS_DEBUG_OUTPUT("WFS::alloc(%d)->0x%08X\n", size, (u32)ret);
                return ret;
    }

/*
 * Releases dynamic memory in WFS.
 */
    static inline void WFSi_Free(void *ptr)
    {
        if (ptr)
        {
            OSIntrMode bak_cpsr = OS_DisableInterrupts();
            (void)(*wfsi_work->alloc_func) (wfsi_work->alloc_arg, 0, ptr);
            (void)OS_RestoreInterrupts(bak_cpsr);
        }
        WFS_DEBUG_OUTPUT("WFS::free(0x%08X)\n", (u32)ptr);
    }


/* list operations (exclusively for parent) *******************************************************/

/* search for currently registered files*/
    WFSiFileList *WFSi_FindAlive(WFSParentContext * p, u32 top, u32 len);

/* search for files currently being prepared for registration*/
    WFSiFileList *WFSi_FindBusy(WFSParentContext * p, u32 top, u32 len);

/* Change from empty to preparing*/
    WFSiFileList *WFSi_FromFreeToBusy(WFSParentContext * p_parent);

/* Change from preparing to registered*/
    void    WFSi_FromBusyToAlive(WFSParentContext * p, WFSiFileList * trg);

/* Change from registered to deallocating*/
    void    WFSi_FromAliveToBusy(WFSParentContext * p, WFSiFileList * trg);

/* Change from deallocating to empty*/
    void    WFSi_FromBusyToFree(WFSParentContext * p, WFSiFileList * trg);


/* message (exclusively for parent) *******************************************************/

/* respond to OPENFILE*/
    void    WFSi_SendOpenAck(WFSParentContext * p, WFSiFileList * trg, BOOL ret);

/* Send if there are unsent Acks (parent)*/
    void    WFSi_SendAck(void);


/* thread (exclusively for parent) *********************************************************/

/* start up file management thread*/
    void    WFSi_CreateTaskThread(void);

/* notify file management thread of work*/
    void    WFSi_NotifyBusy(void);

/* end file management thread*/
    void    WFSi_EndTaskThread(void);


/* other *********************************************************************/

/*
 * Load FNT, FAT, and OVT in memory. (parent only)
 * This file has been prepared at the outset so that
 * child can connect to parent any time.
 */
    void    WFSi_LoadTables(FSFile *p_rom, BOOL use_parent_fs);

/*
 * Reconstruction of ROM archive and replacing overlay table. (child only)
 * Returns base offset of child ROM address for parent.
 */
    u32     WFSi_ReplaceRomArchive(void *p_table);

/*
 * Reads the specified file. (child only)
 */
    void    WFSi_ReadRequest(FSFile *p_file);


/*****************************************************************************/


#if	defined(__cplusplus)
}                                      /* extern "C"*/
#endif


#endif /* NITRO_BUILD_DEMOS_WIRELESSSHARED_WFS_INCLUDE_WFS_PRIVATE_H_*/
