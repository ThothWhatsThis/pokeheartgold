/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WBT - demos - wbt-1
  File:     bt.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: bt.c,v $
  Revision 1.19  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.18  12/15/2005 01:00:39  yosizaki
  fix about NOT_USE_ALLOC.

  Revision 1.17  02/28/2005 05:26:01  yosizaki
  do-indent.

  Revision 1.16  02/18/2005 09:26:13  yosizaki
  fix around hidden warnings.

  Revision 1.15  10/28/2004 11:06:19  yosizaki
  delete waste 'extern'.

  Revision 1.14  2004/10/25 11:14:39  yasu
  Support SDK_NO_MESSAGE

  Revision 1.13  10/19/2004 10:27:02  miya
  Made changes so that the Port reception callback usage and the print statement are displayed on the NITRO screen

  Revision 1.12  10/04/2004 08:40:35  miya
  add WBT_CancelCurrentCommand

  Revision 1.11  10/04/2004 05:49:53  miya
  change transfer size according to the programming guideline

  Revision 1.10  09/13/2004 09:12:22  miya
  Changed to used OS_TPrintf

  Revision 1.9  09/13/2004 06:39:43  miya
  Added macros.

  Revision 1.8  09/01/2004 04:00:15  miya
  Deleted PutBlock, RequestRegisterBlock, ResponseRegisterBlock, and Done commands.

  Revision 1.7  09/01/2004 00:16:56  miya
  Changed size of user data.

  Revision 1.6  08/28/2004 11:26:56  miya
  Added WBT_PutUserData function

  Revision 1.5  08/27/2004 07:08:27  miya
  Made changes to names, etc.

  Revision 1.4  08/26/2004 23:54:28  miya
  Name changes, etc

  Revision 1.3  08/24/2004 02:41:57  miya
  comments added

  Revision 1.2  08/23/2004 12:39:40  miya
  Changed a part that could be problematic with the UI

  Revision 1.1  08/23/2004 09:47:33  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitro/wbt.h>
#include "wc.h"
#include "bt.h"
#include "text.h"

#define NOT_USE_ALLOC

#ifndef	SDK_NO_MESSAGE
#define	__MY_LINE__		__LINE__
#else
#define	__MY_LINE__		0
#endif

/* reception block control data for child device*/
static int request_block_num;
static int block_info_num = 0;
static OSTick start_tick;
static int bt_loop_count = 0;
static int bt_running = 0;

/* child device receive buffers*/
static WBTBlockInfoTable block_info_table;
static WBTRecvBufTable recv_buf_table;
static WBTPacketBitmapTable recv_buf_packet_bmp_table;
static WBTAidBitmap tbmp = 1;          /* Request partner (parent)*/

/* user data send buffer for child device*/
static u8 user_data[WBT_SIZE_USER_DATA];

#ifdef NOT_USE_ALLOC
WBTBlockInfo block_info[WBT_NUM_OF_AID][NUM_OF_BT_LIST];
u8      recv_buf[WBT_NUM_OF_AID][BT_DATA_SIZE];
u32    
    recv_pkt_bmp_buf[WBT_NUM_OF_AID][WBT_PACKET_BITMAP_SIZE(BT_DATA_SIZE, BT_PARENT_PACKET_SIZE)];
#endif

/* block data for parent device*/
static WBTBlockInfoList bt_list[NUM_OF_BT_LIST];
static u8 bt_data[NUM_OF_BT_LIST][BT_DATA_SIZE];

static u8 user_char_id[NUM_OF_BT_LIST][WBT_USER_ID_LEN] = {
    "BT text information area 0",
    "BT text information area 1",
    "BT text information area 2",
#if 0
    "BT text information area 3",
    "BT text information area 4"
#endif
};


const char *command_str[] = {
    "REQ_NONE",
    "REQ_WAIT",
    "REQ_SYNC",
    "RES_SYNC",
    "REQ_GET_BLOCK",
    "RES_GET_BLOCK",
    "REQ_GET_BLOCKINFO",
    "RES_GET_BLOCKINFO",
    "REQ_GET_BLOCK_DONE",
    "RES_GET_BLOCK_DONE",
    "REQ_USER_DATA",
    "RES_USER_DATA",
    "SYSTEM_CALLBACK",
    "PREPARE_SEND_DATA",
    "REQ_ERROR",
    "RES_ERROR",
    "CANCEL"
};




static int strlen(char *str)
{
    int     i = 0;
    while (1)
    {
        if (*str != '\0')
        {
            str++;
            i++;
        }
        else
        {
            break;
        }
    }
    return i;
}


/* block registration function for parent device*/
void bt_register_blocks(void)
{
    int     i;
    char   *end_string = "This is BlockTransfer test data contents end\n";
    char   *ptr;
    int     offset;

    for (i = 0; i < NUM_OF_BT_LIST; i++)
    {
        offset = BT_DATA_SIZE - (strlen(end_string) + 1);
        *(s32 *)(&(bt_data[i][0])) = offset;
        (void)OS_SPrintf((char *)&(bt_data[i][4]),
                         "This is BlockTransfer test data contents start %d\n", i);

        ptr = (char *)(&(bt_data[i][0]) + offset);
        (void)OS_SPrintf((char *)ptr, "%s", end_string);

        (void)WBT_RegisterBlock(&(bt_list[i]), (u32)(10000 + i) /* IDs of 1000 or less are not allowed*/ ,
                                user_char_id[i], &(bt_data[i][0]), BT_DATA_SIZE, 0);
    }
}


/* block transmission end function for child device*/
void bt_stop(void)
{
    bt_running = 0;
    (void)WBT_CancelCurrentCommand(0xffff);
}


/* block transmission start function for child device*/
void bt_start(void)
{
    OSIntrMode enabled;
    static int init_flag = FALSE;
    int     i;

    enabled = OS_DisableInterrupts();

    if (bt_running)
    {
        (void)OS_RestoreInterrupts(enabled);
        return;
    }
    bt_running = 1;

#ifdef NOT_USE_ALLOC

    if (init_flag == FALSE)
    {
        init_flag = TRUE;
        for (i = 0; i < WBT_NUM_OF_AID; i++)
        {
            block_info_table.block_info[i] = &(block_info[i][0]);
            recv_buf_table.recv_buf[i] = &(recv_buf[i][0]);
            recv_buf_packet_bmp_table.packet_bitmap[i] = &(recv_pkt_bmp_buf[i][0]);
        }
    }

#else

    mfprintf(tc[2], "child bt start\n");

    if (init_flag == FALSE)

    {
        init_flag = TRUE;
        /* Initialize child device receive buffers*/
        for (i = 0; i < WBT_NUM_OF_AID; i++)
        {
            block_info_table.block_info[i] = NULL;
            recv_buf_table.recv_buf[i] = NULL;
            recv_buf_packet_bmp_table.packet_bitmap[i] = NULL;
        }
    }

    for (i = 0; i < WBT_NUM_OF_AID; i++)
    {
        if (block_info_table.block_info[i] != NULL)
        {
            OS_Free(block_info_table.block_info[i]);
            block_info_table.block_info[i] = NULL;
        }
        if (recv_buf_table.recv_buf[i] != NULL)
        {
            OS_Free(recv_buf_table.recv_buf[i]);
            recv_buf_table.recv_buf[i] = NULL;
        }
        if (recv_buf_packet_bmp_table.packet_bitmap[i] != NULL)
        {
            OS_Free(recv_buf_packet_bmp_table.packet_bitmap[i]);
            recv_buf_packet_bmp_table.packet_bitmap[i] = NULL;
        }
    }
#endif

    (void)OS_RestoreInterrupts(enabled);

    (void)WBT_RequestSync(tbmp,        /* partner that is requesting Sync (Multiple only possible for parent)*/
                          bt_callback  /* callback when ending*/
        );
}


/* callback function for block transmission*/
void bt_callback(void *arg)
{

    WBTCommand *uc = (WBTCommand *)arg;
    int     peer_aid = WBT_AidbitmapToAid(uc->peer_bmp);        /* AID of communications partner station*/
    // mfprintf(tc[2],"aid = %d\n", peer_aid);

    switch (uc->event)
    {
    case WBT_CMD_RES_SYNC:
        /* WBT_RequestSync end*/
        request_block_num = 0;
        block_info_num = uc->sync.num_of_list;  /* Number of blocks held by partner*/
        mfprintf(tc[2], "blockinfo num = %d my_packet_size = %d peer_packet_size = %d\n", block_info_num, uc->sync.my_packet_size,      /* send data size of the partner*/
                 uc->sync.peer_packet_size      /* the send data size here*/
            );
#ifndef NOT_USE_ALLOC
        mfprintf(tc[2], "info buf alloc %d\n", peer_aid);
        /* block information table initialization*/
        block_info_table.block_info[peer_aid] = (WBTBlockInfo *)OS_Alloc(sizeof(WBTBlockInfo));
#endif

        if (uc->target_bmp == 0)       /* has there been a response from all partner stations? */
        {
            (void)OS_SPrintf((char *)user_data, " %5d\n", bt_loop_count);

            /* send user data*/
            if (FALSE == WBT_PutUserData(tbmp,  /* partner that sends user data (Multiple only possible for parent)*/
                                         user_data,     /* user  data  buffer*/
                                         WBT_SIZE_USER_DATA,    /* user data size of 12 or lower*/
                                         bt_callback    /* callback when ending*/
                ))
            {
                mfprintf(tc[2], "command invoke error %d\n", __MY_LINE__);
            }
        }
        break;
    case WBT_CMD_RES_USER_DATA:

        if (uc->target_bmp == 0)       /* has there been a response from all partner stations? */
        {
            /* block list request*/
            if (FALSE == WBT_GetBlockInfo(tbmp, /* partner that makes block list request (Multiple only possible for parent)*/
                                          request_block_num /* block list number*/ ,
                                          &block_info_table,    /* block information table*/
                                          bt_callback   /* callback when ending*/
                ))
            {
                mfprintf(tc[2], "command invoke error %d\n", __MY_LINE__);
            }
        }
        break;
    case WBT_CMD_RES_GET_BLOCKINFO:

        /* end WBT_GetBlockInfo*/

        mfprintf(tc[2], "blockinfo %d done\n", uc->get.block_id);       /* obtained block list ID*/
        mfprintf(tc[2], " info id = %d\n", block_info_table.block_info[peer_aid]->id);  /* block ID*/
        mfprintf(tc[2], " info block size = %d\n", block_info_table.block_info[peer_aid]->block_size);  /* block size*/
        mfprintf(tc[2], " info = %s\n", block_info_table.block_info[peer_aid]->user_id);        /* block user definition information*/

#ifndef NOT_USE_ALLOC
        /* initialize receive buffer table*/
        recv_buf_table.recv_buf[peer_aid] =
            (u8 *)OS_Alloc((u32)block_info_table.block_info[peer_aid]->block_size);
        mfprintf(tc[2], "recv buf alloc %d\n", peer_aid);

        /* initialize buffer table for registration of packet reception number*/
        recv_buf_packet_bmp_table.packet_bitmap[peer_aid] =
            (u32 *)
            OS_Alloc((u32)
                     WBT_CalcPacketbitmapSize(block_info_table.block_info[peer_aid]->block_size));

        mfprintf(tc[2], "recv pkt bmp size = %d\n",
                 WBT_CalcPacketbitmapSize(block_info_table.block_info[peer_aid]->block_size));
#endif

        if (uc->target_bmp == 0)       /* has there been a response from all partner stations? */
        {

            /* block reception request*/
            if (FALSE == WBT_GetBlock(tbmp,     /* partner that makes the block reception request (Multiple only possible for parent)*/
                                      block_info_table.block_info[peer_aid]->id /* block ID*/ ,
                                      &recv_buf_table,  /* receive buffer table*/
                                      (u32)block_info_table.block_info[peer_aid]->block_size,   /* block size*/
                                      &recv_buf_packet_bmp_table,       /* buffer table for registration of packet reception number*/
                                      bt_callback       /* callback when ending*/
                ))
            {
                mfprintf(tc[2], "command invoke error %d\n", __MY_LINE__);
            }
            else
            {
                start_tick = OS_GetTick();      /* start time measurement*/
            }

        }

        break;
    case WBT_CMD_RES_GET_BLOCK:
        /* end WBT_GetBlock*/

        mfprintf(tc[2], "get block %d done\n", uc->get.block_id);       /* received block ID*/
        mfprintf(tc[2], " time %d msec\n", OS_TicksToMilliSeconds(OS_GetTick() - start_tick));

        mfprintf(tc[2], " %s\n", &(recv_buf_table.recv_buf[peer_aid][4]));      /* received block contents*/
        {
            u32     offset;
            offset = *(u32 *)&(recv_buf_table.recv_buf[peer_aid][0]);
            mfprintf(tc[2], " %s\n", (char *)(&(recv_buf_table.recv_buf[peer_aid][offset])));
        }

#ifndef NOT_USE_ALLOC
        /* reception buffer table deallocation*/
        mfprintf(tc[2], "recv buf free %d\n", peer_aid);
        OS_Free(recv_buf_table.recv_buf[peer_aid]);
        recv_buf_table.recv_buf[peer_aid] = NULL;

        /* deallocation of buffer table for registration of the packet reception number*/
        OS_Free(recv_buf_packet_bmp_table.packet_bitmap[peer_aid]);
        recv_buf_packet_bmp_table.packet_bitmap[peer_aid] = NULL;

        OS_Free(block_info_table.block_info[peer_aid]);
        block_info_table.block_info[peer_aid] = NULL;

        {
            mfprintf(tc[2], "info buf alloc %d\n", peer_aid);
            /* block information table initialization*/
            block_info_table.block_info[peer_aid] = (WBTBlockInfo *)OS_Alloc(sizeof(WBTBlockInfo));
        }
#endif

        if (uc->target_bmp == 0)
        {                              /* has there been a response from all requested partner stations? */

            request_block_num++;

            if (request_block_num < block_info_num)
            {

                /* block list request*/
                if (FALSE == WBT_GetBlockInfo(tbmp, request_block_num,  /* block list number*/
                                              &block_info_table,        /* block information table*/
                                              bt_callback       /* callback when ending*/
                    ))
                {
                    mfprintf(tc[2], "command invoke error %d\n", __MY_LINE__);
                }
            }
            else
            {
                request_block_num = 0;

                bt_loop_count++;
                if (bt_loop_count > 99999)
                {
                    bt_loop_count = 0;
                }

                (void)OS_SPrintf((char *)user_data, " %05d\n", bt_loop_count);

                /* send user data*/
                if (FALSE == WBT_PutUserData(tbmp,      /* partner that sends user data (Multiple only possible for parent)*/
                                             user_data, /* user  data  buffer*/
                                             WBT_SIZE_USER_DATA,        /* user data size of 12 or lower*/
                                             bt_callback        /* callback when ending*/
                    ))
                {
                    mfprintf(tc[2], "command invoke error %d\n", __MY_LINE__);
                }
            }
        }
        break;
    case WBT_CMD_REQ_NONE:
        mfprintf(tc[2], "WBT user none\n");
        break;
    case WBT_CMD_REQ_USER_DATA:
        mfprintf(tc[2], "get user data = %s\n", uc->user_data.data);
        break;
    case WBT_CMD_REQ_GET_BLOCK_DONE:
        mfprintf(tc[2], "get peer getblockdone %d done from %d\n", uc->blockdone.block_id,
                 peer_aid);
        break;
    case WBT_CMD_REQ_SYNC:
        mfprintf(tc[2], "get peer sync from %d\n", peer_aid);
        break;
    case WBT_CMD_RES_ERROR:
        mfprintf(tc[2], "get req error %d from %d\n", peer_aid, uc->result);
        break;
    case WBT_CMD_REQ_ERROR:
        mfprintf(tc[2], "get res error %d from %d\n", peer_aid, uc->result);
        break;
    case WBT_CMD_CANCEL:
        mfprintf(tc[2], "get canncel [%s] command from %d\n", command_str[uc->command], peer_aid);
        break;
    default:
        mfprintf(tc[2], "WBT callback unknown %d\n", uc->event);
        break;
    }
}
