/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - demos - multiboot
  File:     parent.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: parent.c,v $
  Revision 1.39  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.38  04/11/2005 08:50:04  yosizaki
  change to use WM_GetNextTgid().

  Revision 1.37  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.36  02/18/2005 10:27:18  yosizaki
  fix around hidden warnings.

  Revision 1.35  11/18/2004 10:05:04  takano_makoto
  Got rid of the argument for setting child transfer size in MB_SetParentCommSize, MB_SetParentCommParam

  Revision 1.34  2004/10/27 13:09:58  yasu
  Revised to set tgid from the minutes and seconds in the RTC.

  Revision 1.33  2004/10/27 12:49:24  yasu
  Untabified

  Revision 1.32  2004/10/26 08:25:43  sato_masaki
  Revised to call OS_Panic when RTC acquisition fails.

  Revision 1.31  2004/10/26 04:27:56  sato_masaki
  Changed to acquire TGID value from the number of seconds in the RTC.

  Revision 1.30  10/05/2004 10:43:41  yosizaki
  fix #ifdef in ParentInit().

  Revision 1.29  2004/10/05 06:47:54  sato_masaki
  Added setup menu. Made several small revisions.

  Revision 1.28  10/04/2004 13:40:25  terui
  Standardized game group IDs as type u32.

  Revision 1.27  10/04/2004 06:14:14  yosizaki
  change around OwnerInfo. (use fixed-value when OS_GetOwnerInfo is not available)

  Revision 1.26  10/04/2004 00:37:15  yosizaki
  fix around OwnerInfo. (depending TS_VERSION)

  Revision 1.25  2004/10/02 05:05:50  sato_masaki
  message fix & small fix

  Revision 1.24  2004/09/30 03:05:12  sato_masaki
  - Changed so that name of parent device is obtained from NVRAM.
  - Changed so that you cycle through channels when initializing.    

  Revision 1.23  09/16/2004 12:59:16  yosizaki
  Fixed some comments and an implicit cast.

  Revision 1.22  2004/09/16 12:51:59  sato_masaki
  In mb.h, changed the functions and structures defined as MBi_*** to MB_***

  Revision 1.21  2004/09/15 06:30:04  sato_masaki
  Added specifications for send/receive data sizes

  Revision 1.20  2004/09/13 04:13:44  sato_masaki
  Changed MB_Init -> MBi_Init

  Revision 1.19  2004/09/10 09:51:05  sato_masaki
  Deleted case statement for the MB_COMM_PSTATE_BOOT_REQUEST callback

  Revision 1.18  09/10/2004 02:17:05  yosizaki
  add comments.

  Revision 1.17  2004/09/08 08:22:08  sato_masaki
  Changed the End->Re-initialize Button to SELECT

  Revision 1.16  2004/09/08 04:53:00  sato_masaki
  Included MB_CommStartSendingAll() and MB_CommBootRequestAll()    

  Revision 1.15  2004/09/08 01:27:02  sato_masaki
  Limited number of display characters for username

  Revision 1.14  2004/09/08 00:56:09  sato_masaki
  Modified so the buffer for the loaded segment image is allocated from heap

  Revision 1.13  2004/09/07 23:50:59  sato_masaki
  small fix

  Revision 1.12  2004/09/07 12:34:26  sato_masaki
  Added comments

  Revision 1.11  2004/09/07 12:06:04  sato_masaki
  - Added new operations
    * +Button Mover cursor to targeted child device    
    * A Button
      ENTRY REQUEST - Permit requests
      ENTRY OK          - Start game transmission
      SEND COMPLETE - Send boot request
    * B Button
      ENTRY REQUEST - Refuse requests
    * START Button
      Perform the operation defined for the A Button on the connected child device

  Revision 1.10  2004/09/06 12:03:13  sato_masaki
  - Added tgid as an argument for the MB_Init() function.

  Revision 1.9  2004/09/04 07:02:24  sato_masaki
  Added callback processing for MB_COMM_PSTATE_MEMBER_FULL

  Revision 1.8  2004/09/02 11:59:29  sato_masaki
  Changed MB_CommStartSending() into MB_CommStartSendingAll()

  Revision 1.7  2004/09/02 08:56:33  sato_masaki
  Changed so that jumps to re-intialization after boot

  Revision 1.6  2004/08/26 10:01:46  sato_masaki
  Implemented an entry permission/denial due to the MB_CommResponseRequest function.    
  While downloading in this sample, entry is set to always be denied.   

  Revision 1.5  2004/08/25 04:28:20  sato_masaki
  Mounting MB_CommStartSending function    

  Revision 1.4  2004/08/17 10:50:24  sato_masaki
  Added and revised the branching contents of the case sentence about the callback status    

  Revision 1.3  08/14/2004 09:34:05  yosiokat
  Since MB_COMM_PSTATE_END_COMPLETE no longer exists, it was commented out

  Revision 1.2  08/11/2004 05:17:25  yosizaki
  add callback-state.

  Revision 1.1  08/10/2004 07:31:53  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nitro/wm.h>
#include <nitro/mb.h>

#include "common.h"
#include "dispfunc.h"


/*
 * The basic processing on the parent side.
 *
 * Because the MB library samples use the multiboot functionality,
 * development tools of the same communications environment (wired or wireless) are required.
 * The mb_child.bin program in the * $NitroSDK/bin/ARM9-TS/Release/ directory
 * is a sample that provides the same features as the multiboot child
 * of the DS system.
 * Load this binary on the other tools as you would as demo program,
 * and run them together.
 */


/******************************************************************************/
/* variables*/

/*
 * The memory that loads the segment information of the download program.
 */

static u8 *p_segbuf[GAME_PROG_MAX];
static u8 cursor = 0;
static MBUserInfo myUser;
static u16 parent_channel = PARENT_CHANNEL;
static u16 parent_send_size = 256;
static u8 entry_max = 15;

/*
 * Switch for automatically controlling response from parent side to child in this sample.    
 *
 * Request from child waits for response from parent.
 * Therefore, by batching or individuating the timing of this response with all children,    
 * you can set operation of multiboot applications.    
 *
 * Can think of examples such as individual response when only distributing a test version, or    
 * with a multi-player game making a group response when the entry number is achieved.    
 */

static u8 auto_accept = 0;             /* automate the connection permission*/
static u8 auto_send = 0;               /* automate the download permission*/
static u8 auto_boot = 0;               /* automate the boot permission*/

/******************************************************************************/
/* function*/

/* initialize the file buffer pointer*/
void InitFileBuffer(void)
{
    int     i;

    for (i = 0; i < GAME_PROG_MAX; i++)
    {
        p_segbuf[i] = NULL;
    }

}

/* deallocate the allocated file buffers*/
static void FreeFileBuffer(void)
{
    int     i;

    for (i = 0; i < GAME_PROG_MAX; i++)
    {
        if (p_segbuf[i])
        {
            OS_Free(p_segbuf[i]);
            p_segbuf[i] = NULL;
        }

    }

}

/* generate MBUserInfo*/
static void MakeUserInfo(MBUserInfo *user, u8 favoriteColor, u8 playerNo, u16 *name,
                         u16 name_length)
{
    SDK_ASSERT(user != NULL);
    user->favoriteColor = favoriteColor;
    user->nameLength = (u8)(name_length);
    MI_CpuCopy8((char *)name, (char *)user->name, (u32)(name_length * 2));
    user->playerNo = playerNo;
}

/* start to cyclically change channels*/
static BOOL changeChannel(u16 *p_channel)
{
    u16     channel_bmp, channel, i;

    /* get channel bitmap*/
    channel_bmp = WM_GetAllowedChannel();

    /* If there are no usable channels, call OS_Panic*/
    if (channel_bmp == 0)
    {
        OS_Panic("No Available Parent channel\n");
        return FALSE;
    }

    /* If usable channels exist*/
    for (i = 0, channel = *p_channel;
         i < 16; i++, channel = (u16)((channel == 16) ? 1 : channel + 1))
    {
        if (channel_bmp & (1 << (channel - 1)))
        {
            /* When the detected channel is the same as previous one,    
               as the previous item, a different channel
               is looked for.
             */
            if (*p_channel != channel)
            {
                *p_channel = channel;
                break;
            }
        }

    }

    return TRUE;

}

/* parent device file download state notification callback*/
static void ParentStateCallback(u16 aid, u32 status, void *arg)
{

    switch (status)
    {
    case MB_COMM_PSTATE_INIT_COMPLETE:
        /* initialization process complete*/
        {
            WMStartParentCallback *p = (WMStartParentCallback *)arg;
            BgSetMessage(WHITE, "MB lib Init complete");
        }
        break;

    case MB_COMM_PSTATE_CONNECTED:
        /* notification the moment the connection comes from a child device*/
        {
            WMStartParentCallback *p = (WMStartParentCallback *)arg;

            BgSetMessage(YELLOW, "Connected [%2d]", aid);
            BgSetMessage(YELLOW, "MACAddress %04x %04x %04x",
                         p->macAddress[0], p->macAddress[1], p->macAddress[2]);
        }
        break;

    case MB_COMM_PSTATE_DISCONNECTED:
        /* Notification that the child was disconnected    */
        {
            WMStartParentCallback *p = (WMStartParentCallback *)arg;

            BgSetMessage(RED, "Disconnected [%2d]", aid);
            /* ReasonCode display at the time of disconnection*/
            //                  BgSetMessage( WHITE, "reason code : %2d", p->reason);
        }
        break;

    case MB_COMM_PSTATE_KICKED:
        /* Notify when kicking a child*/
        BgSetMessage(RED, "Entry Refused [%2d]", aid);
        break;

    case MB_COMM_PSTATE_REQ_ACCEPTED:
        /* Notify when received the download request from a child.*/
        BgSetMessage(YELLOW, "Download Request [%2d]", aid);
        break;

    case MB_COMM_PSTATE_SEND_PROCEED:
        /* Notify when starting the binary transmission to a child.*/
        BgSetMessage(CYAN, "Start Sending [%2d]", aid);
        break;

    case MB_COMM_PSTATE_SEND_COMPLETE:
        /* Notify when the binary transmission to the child completed.*/
        BgSetMessage(CYAN, "Send Completed [%2d]", aid);
        if (auto_boot == 1)
        {
            /* automatic boot request*/
            (void)MB_CommBootRequest(aid);
            BgSetMessage(WHITE, "-->Boot Request [%2d]", aid);
        }
        break;

    case MB_COMM_PSTATE_BOOT_REQUEST:
        /* notification when the boot request is sent to the child device*/
        BgSetMessage(CYAN, "Send boot request [%2d]", aid);
        break;

    case MB_COMM_PSTATE_BOOT_STARTABLE:
        /* notification when a series of processes has ended, and an application that supports multiboot can be started up*/
        BgSetMessage(YELLOW, "Boot ready [%2d]", aid);
        break;

    case MB_COMM_PSTATE_REQUESTED:
        /* Notification of the moment that the entry request came from a child*/
        BgSetMessage(YELLOW, "Entry Requested [%2d]", aid);

        if (auto_accept == 1)
        {
            /* auto-allow entry*/
            (void)MB_CommResponseRequest(aid, MB_COMM_RESPONSE_REQUEST_ACCEPT);
            BgSetMessage(WHITE, "-->Entry Accept [%2d]", aid);
        }
        break;

    case MB_COMM_PSTATE_MEMBER_FULL:
        /* notification when a child device has connected to a game with the full number of players*/
        BgSetMessage(RED, "Entry Member full [%2d]", aid);
        break;

    case MB_COMM_PSTATE_END:
        /* notification when the parent device ends*/
        BgSetMessage(WHITE, "MB lib End");
        prog_state = STATE_MENU;
        break;

    case MB_COMM_PSTATE_WAIT_TO_SEND:
        /* notification when the send wait state to the child device has started*/
        BgSetMessage(CYAN, "Waiting to send [%2d]", aid);
        if (auto_send == 1)
        {
            /* auto start send*/
            (void)MB_CommStartSending(aid);
            BgSetMessage(WHITE, "-->Start Sending [%2d]", aid);
        }
        break;

    }
}



/* parent device initialization*/
void ParentInit(void)
{
    int     i;
#if defined(SDK_TS) && ((SDK_TS_VERSION >= 200) || ( SDK_NVRAM_FORMAT >= 100 ))
    OSOwnerInfo MyInfo;

    OS_GetOwnerInfo(&MyInfo);
    MakeUserInfo(&myUser, MyInfo.favoriteColor, 0, (u16 *)MyInfo.nickName, MyInfo.nickNameLength);
#else
    /* OS_GetOwnerInfo() cannot be used with a TEG, old-format IPL, etc.*/
    OS_Warning
        ("OS_GetOwnerInfo() can not available on this platform. (favoriteColor and nickname are fixed value)");
    {
        const u8 favoriteColor = 0;
        const u16 nickname_len = 8;
        static const u16 nickname[] = L"TestUser";
        MakeUserInfo(&myUser, favoriteColor, 0, (u16 *)nickname, nickname_len);
    }
#endif

    BgClear();
    BgSetMessage(WHITE, "Initializing Parent.");

    /*
     * Starts MB parent control.
     * At this point, only all of the devices that are ready will go into a wait state.    
     */

    /* deallocate the heap previously in use before re-initializing*/
    FreeFileBuffer();

    /* multiboot  library  initialization*/
    (void)MB_Init(cwork, &myUser, MY_GGID, MB_TGID_AUTO, MB_DMA_NO);

    /* designated the data size to send / receive*/
    (void)MB_SetParentCommParam(parent_send_size, entry_max);
    (void)MB_CommSetParentStateCallback(ParentStateCallback);
    (void)MB_StartParent(parent_channel);

    BgSetMessage(WHITE, "** Parameters **");
    BgSetMessage(WHITE, "channel      : %2d", parent_channel);
    BgSetMessage(WHITE, "send size    : %3d", parent_send_size);
    BgSetMessage(WHITE, "max children : %2d", entry_max);
    BgSetMessage(WHITE, "GGID:%08x TGID:%04x", MY_GGID, MB_GetTgid());

    prog_state = STATE_MB_PARENT;
    cursor = 0;

    /*
     * Next, register the download program file information.
     *
     * Only the ARM9-static segment and the ARM7-static segment are needed    
     * when multibooting among the overall program files.    
     * (Anything else will be overlay and data files, etc.)
     *
     * Since only this information is read out from the program,   
     * first use the MB_ReadSegment() function.    
     * By getting size with the MB_GetSegmentLength() function,
     * memory is dynamically secured in this sample.
     *
     * The program file must be read out by FS_ReadFile.    
     * Normally, since the program should be kept as a file inside the CARD-ROM,    
     * there should be no problem, but if a particular multiboot system is assumed,    
     * build a proprietary archive with FSArchive, and handle it with that.    
     */
    for (i = 0; i < GAME_PROG_MAX; ++i)
    {
        FSFile  file[1];
        FS_InitFile(file);

        /*
         * Opens the child program file.
         * This file is used by MB_ReadSegment() below.
         */
        if (!FS_OpenFile(file, mbGameList[i].romFilePathp))
        {
            OS_TPrintf("ParentInit : file cannot open (mbGameList[i].romFilePathp=\"%s)\"\n",
                       mbGameList[i].romFilePathp ? mbGameList[i].romFilePathp : "(NULL)");
        }
        else
        {
            /*
             * Obtains the size of the segment information.
             * If it is not the valid download program,
             * 0 is returned for the size.
             */
            u32     length = MB_GetSegmentLength(file);
            if (length == 0)
            {
                OS_TPrintf
                    ("ParentInit : specified file may be invalid format. (mbGameList[i].romFilePathp=\"%s)\"\n",
                     mbGameList[i].romFilePathp ? mbGameList[i].romFilePathp : "(NULL)");
            }
            else
            {
                /*
                 * Secures memory for segment information.
                 * If this memory is large enough,    
                 * it can be prepared statically.    
                 */
                p_segbuf[i] = (u8 *)OS_Alloc(length);
                if (!p_segbuf[i])
                {
                    OS_TPrintf("ParentInit : memory allocation failed. (%d BYTE)\n", length);
                }
                else
                {
                    /*
                     * Extract the segment information from the file
                     * The extracted segment information must stay resident on main memory
                     * while the download program is being distributed.
                     *
                     * Even if the size was obtained successfully,    
                     * if the extraction process fails here,    
                     * then the file handler had been changed in some way.    
                     * (File was closed, seek was performed for the location, ...)
                     */
                    if (!MB_ReadSegment(file, p_segbuf[i], length))
                    {
                        OS_TPrintf("ParentInit : failed to extract segment\n");
                    }
                    else
                    {
                        /*
                         * Register the download program with
                         * the extracted segment information and MBGameRegistry.
                         *
                         */

                        if (MB_RegisterFile(&mbGameList[i], p_segbuf[i]))
                        {
                            BgSetMessage(LIGHT_GREEN, "Registered\"%s\"",
                                         mbGameList[i].romFilePathp);
                        }
                        else
                        {
                            OS_TPrintf("ParentInit : failed to register file No %d\n", i);
                        }
                    }
                }
            }
        }
        if (FS_IsFile(file))
            (void)FS_CloseFile(file);
    }

}


/* parent device main process in each single frame*/
void ParentMain(void)
{
    char    userName[MB_USER_NAME_LENGTH * 2 + 1];
    const int children_num = MB_CommGetChildrenNumber();
    const u16 keyData = ReadKeySetTrigger(PAD_Read());
    enum
    { DISP_OX = 2, DISP_OY = 3 };
    u16     i;
    OSIntrMode enabled;
    enum
    { PSTATE_NUM = 16 };
    const char *pstate_string[] = {
        "NONE        ",                //      STATE_NONE
        "INIT OK     ",                //      STATE_INIT_COMPLETE
        "CONNECTED   ",                //      STATE_CONNECTED
        "DISCONNECTED",                //      STATE_DISCONNECTED
        "KICKED      ",                //      STATE_KICKED
        "ENTRY OK    ",                //      STATE_REQ_ACCEPTED
        "SENDING     ",                //      STATE_SEND_PROCEED
        "SEND END    ",                //      STATE_SEND_COMPLETE
        "BOOT REQ    ",                //      STATE_BOOT_REQUEST
        "BOOT READY  ",                //      STATE_BOOT_STARTABLE
        "ENTRY REQ   ",                //      STATE_REQUESTED
        "MEMBER FULL ",                //      STATE_MEMBER_FULL
        "END         ",                //      STATE_END
        "ERROR       ",                //      STATE_ERROR
        "WAIT TO SEND",                //      STATE_WAIT_TO_SEND
    };

    BgPrintf(DISP_OX, DISP_OY - 2, WHITE, "CH:%2d  Max Children:%2d", parent_channel, entry_max);
    BgPutString(DISP_OX, DISP_OY - 1, WHITE, "AID USERNAME STATE        ");
    BgPutString(DISP_OX, DISP_OY + MB_MAX_CHILD + 1, WHITE, "A:operate B:refuse SEL:restart");
    BgPutString(DISP_OX, DISP_OY + MB_MAX_CHILD + 2, WHITE, "START:operate all children");

    enabled = OS_DisableInterrupts();  /* interrupts prohibited*/

    /* display list of children*/
    for (i = 0; i < entry_max; ++i)
    {
        const u16 aid = (u16)(i + 1);
        const MBUserInfo *p_child = MB_CommGetChildUser(aid);
        int     p_state;

        BgPutString(DISP_OX, (s16)(DISP_OY + i), WHITE, "                         ");

        BgPrintf(DISP_OX + 0, (s16)(DISP_OY + i), WHITE, "%2d  ", aid);

        if (p_child && p_child->nameLength)
        {
            u8      palette = p_child->favoriteColor;
            MI_CpuCopy8(p_child->name, userName, (u32)(p_child->nameLength * 2));
            BgPutStringN((s16)(DISP_OX + 4), (s16)(DISP_OY + i), palette, userName, 8);
        }

        p_state = MB_CommGetParentState(aid);

        if (p_state < PSTATE_NUM)
        {
            BgPrintf((s16)(DISP_OX + 13), (s16)(DISP_OY + i), WHITE,
                     "%s  ", (char *)pstate_string[p_state]);
        }

        BgPutChar((s16)(DISP_OX - 2), (s16)(DISP_OY + i), RED, (char)((i == cursor) ? '*' : ' '));
    }

    (void)OS_RestoreInterrupts(enabled);        /* Cancels interrupts prohibition*/

    /* all child devices are targets*/
    if (keyData & PAD_BUTTON_START)
    {
        BOOL    result = FALSE;

        if (!auto_accept)
        {
            for (i = 1; i <= entry_max; i++)
            {
                /* entry permission*/
                if (MB_COMM_PSTATE_REQUESTED == MB_CommGetParentState(i))
                {
                    result = MB_CommResponseRequest(i, MB_COMM_RESPONSE_REQUEST_ACCEPT);
                }
            }
            if (TRUE == result)
                return;
        }

        if (!auto_send)
        {
            result = MB_CommStartSendingAll();
            if (TRUE == result)
                return;
        }

        if (!auto_boot)
        {
            result = MB_CommBootRequestAll();
            if (TRUE == result)
                return;
        }
    }

    /* individually send to designated child devices*/
    if (keyData & PAD_BUTTON_A)
    {
        const u16 aid = (u16)(cursor + 1);

        BgSetMessage(WHITE, "AID%2d State:%s", aid, pstate_string[MB_CommGetParentState(aid)]);

        /* entry permission*/
        if (MB_COMM_PSTATE_REQUESTED == MB_CommGetParentState(aid))
        {
            if (!auto_accept)
            {
                (void)MB_CommResponseRequest(aid, MB_COMM_RESPONSE_REQUEST_ACCEPT);
                BgSetMessage(WHITE, "-->Entry Accept [%2d]", aid);
            }
        }
        /* start send*/
        else if (MB_COMM_PSTATE_WAIT_TO_SEND == MB_CommGetParentState(aid))
        {
            if (!auto_send)
            {
                (void)MB_CommStartSending(aid);
                BgSetMessage(WHITE, "-->Start Sending [%2d]", aid);
            }
        }
        else
            /* boot request*/
        if (TRUE == MB_CommIsBootable(aid))
        {
            if (!auto_boot)
            {
                (void)MB_CommBootRequest(aid);
                BgSetMessage(WHITE, "-->Boot Request [%2d]", aid);
            }
        }
    }
    else
        /* deny entry to the designated child device*/
    if (keyData & PAD_BUTTON_B)
    {
        const u16 aid = (u16)(cursor + 1);

        BgSetMessage(WHITE, "AID%2d State:%s", aid, pstate_string[MB_CommGetParentState(aid)]);
        if (MB_COMM_PSTATE_REQUESTED == MB_CommGetParentState(aid))
        {
            if (!auto_accept)
            {
                (void)MB_CommResponseRequest(aid, MB_COMM_RESPONSE_REQUEST_KICK);
                BgSetMessage(CYAN, "Entry Refuse [%2d]", aid);
            }
        }
    }

    /* When SELECT is pressed, end and re-initialize*/
    if (keyData & PAD_BUTTON_SELECT)
    {
        /* change channel*/
        (void)changeChannel(&parent_channel);
        MB_End();
    }

    /* change cursor number*/
    if (keyData & PAD_KEY_DOWN)
    {
        (void)RotateU8(&cursor, 0, (u8)(entry_max - 1), 1);
    }
    else if (keyData & PAD_KEY_UP)
    {
        (void)RotateU8(&cursor, 0, (u8)(entry_max - 1), -1);
    }

}

/* set parent deviceMP send size*/
void SetParentSendSize(u16 p_size)
{
    parent_send_size = p_size;
}

/* get parent device MP send size*/
u16 GetParentSendSize(void)
{
    return parent_send_size;
}

/* set maximum number of child device connections*/
void SetMaxEntry(u8 num)
{
    entry_max = num;
}

/* get the maximum number of child device connections*/
u8 GetMaxEntry(void)
{
    return entry_max;
}

/* configure parent device auto operations*/
void SetAutoOperation(u8 fAccept, u8 fSend, u8 fBoot)
{
    auto_accept = fAccept;
    auto_send = fSend;
    auto_boot = fBoot;
}
