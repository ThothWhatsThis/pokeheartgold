/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - ball
  File:     keysharing.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: keysharing.c,v $
  Revision 1.16  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.15  02/28/2005 05:26:01  yosizaki
  do-indent.

  Revision 1.14  02/17/2005 10:08:44  seiki_masashi
  Revised Copyright notation

  Revision 1.13  02/17/2005 08:09:12  ooe
  Added wm_lib_set_ggidFilter

  Revision 1.12  09/30/2004 08:44:35  seiki_masashi
  Configured the proper values allocated to the GGID

  Revision 1.11  09/10/2004 12:08:10  seiki_masashi
  Changed it so the number of bytes sent for key sharing is concealed in the library.

  Revision 1.10  09/09/2004 12:55:59  seiki_masashi
  Revised the maximum send data size for parent and child devices to an appropriate size

  Revision 1.9  2004/08/30 05:39:55  sato_masaki
  Changed so that game names and user names can be set it userGameInfo
  Set in Beacon with wm_lib_set_gameinfo

  Revision 1.8  2004/08/23 07:27:27  yasu
  Workaround for pushing A Button in IPL

  Revision 1.7  08/07/2004 05:12:28  seiki_masashi
  Deleted meaningless SetMPData

  Revision 1.6  2004/07/29 12:53:56  ikedae
  Deleted unnecessary parameters

  Revision 1.4  2004/07/29 05:41:21  ikedae
  Revised memory deallocation section    

  Revision 1.3  2004/07/29 05:13:51  ikedae
  Changed libParam settings and performed error check

  Revision 1.2  07/28/2004 12:04:56  ikedae
  Made changes related to WM_lib_SetMPData

  Revision 1.1  07/23/2004 15:11:55  terui
  Revisions and additions.

  Revision 1.6  06/28/2004 02:21:03  Sato
  Added initialization for userGameInfo
  Changed multiboot logic
  Revised static void *MASK_ROM_ADDR(u32 offset) in my_romfile.c to support multiboot
  (offset had not been added)

  Revision 1.5  06/23/2004 10:01:30  Sato
  (none)

  Revision 1.3  06/22/2004 04:12:01  Sato
  Changed to accept keysharing only for those with game named "KS Demo     "
  Fixed bugs related to parent selection interface

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include "data.h"


#include <nitro/wm.h>
#include <wm_lib.h>

#include "ball.h"
#include "keysharing.h"
#include <string.h>

#define MAX_SEND_BUFFER_SIZE (256*2*2)
#define MAX_RECV_BUFFER_SIZE (256*2*2)
static u8 libParam_sendBuf[MAX_SEND_BUFFER_SIZE] ATTRIBUTE_ALIGN(32);
static u8 libParam_recvBuf[MAX_RECV_BUFFER_SIZE * 15 * 2] ATTRIBUTE_ALIGN(32);
static u16 parent_channel = 1;
static u8 parent_no = 0;
static u8 mp_on = 1;                   // Flag to send MP for each frame

static const u8 user_name[8] ATTRIBUTE_ALIGN(4) = "RANDED  ";
static const u8 game_name[16] ATTRIBUTE_ALIGN(4) = "KS Demo         ";


//============================================================================
// Function Prototype
//
static u16 GetKeySetByID(WMkeySet *kset, u16 id);
static u16 ReadKeySetTrigger(u16 keyset);

static void parent_callback(u16 type, void *arg);       // Callback for WM_lib (parent)
static void child_callback(u16 type, void *arg);        // Callback for WM_lib (child)

//----------------------------------------------------------------------------
// Get key set using specified ID    
//
static u16 GetKeySetByID(WMkeySet *kset, u16 id)
{
    return kset->key[id];
}

//----------------------------------------------------------------------------
// Detect key trigger set
//
static u16 ReadKeySetTrigger(u16 keyset)
{
    static u16 cont = 0xffff;          /* measures for A button press inside IPL*/
    u16     trigger = (u16)(keyset & (keyset ^ cont));
    cont = keyset;
    return trigger;
}


//============================================================================
// Initialize parent
//
void KsParentInit(void)
{
    u16     i;
    int     errcode;

    wm_lib_comm_init();                // Initialize WM_lib parameters
    wm_lib_set_parent_channel(parent_channel);
    wm_lib_set_gameinfo_username((char *)user_name);
    wm_lib_set_gameinfo_gamename((char *)game_name);
    wm_lib_set_mode(WM_MODE_MP_PARENT);
    wm_lib_set_parent_send_size(0);    // Parent max transmission size (Byte)
    wm_lib_set_parent_recv_size_per_child(0);   // child max transmission size (Byte)
    wm_lib_set_callback(parent_callback);

    // Set send and receive buffers
    wm_lib_set_sendbuffer((u8 *)libParam_sendBuf);
    wm_lib_set_recvbuffer((u8 *)libParam_recvBuf);

    wm_lib_set_ggid(0x003fff12);       // Value assigned to each application by Nintendo
    wm_lib_set_keySharing_mode(1);     // Perform key sharing
    wm_lib_set_contSend_mode(0);       // Do not send continuously

    errcode = wm_lib_start();          // WM_lib initialization
    if (WM_ERRCODE_SUCCESS != errcode)
        OS_Printf("Cannot Initialize WM!\n");

    ClearBallAll();
    for (i = 0; i < 16; ++i)
        InitBall(i);

    parent_no = 0;

    prog_state = STATE_KS_PARENT;

    ObjSetMessage("      Parent Mode       ");

    (void)wm_lib_set_gameinfo();

}


//============================================================================
// Main Processing for parent
//
void KsParentMain(void)
{
    u16     keyData, i;
    WMkeySet keySet;

    //---- Load pad data
    keyData = ReadKeySetTrigger(PAD_Read());

    //----------------------------------------------------
    // Get key set
    if (WM_ERRCODE_SUCCESS == wm_lib_get_keyset(&keySet))
    {
        for (i = 0; i <= WM_NUM_MAX_CHILD; ++i)
        {
            u16     key = GetKeySetByID(&keySet, i);
            // Move ball
            if (key)
            {
                if (key & PAD_KEY_RIGHT)
                    ball[i].x += 1;
                if (key & PAD_KEY_LEFT)
                    ball[i].x -= 1;
                if (key & PAD_KEY_DOWN)
                    ball[i].y += 1;
                if (key & PAD_KEY_UP)
                    ball[i].y -= 1;

                ChangeBallPalette(i, (u8)((i == wm_lib_get_my_aid())? 1 : 6));
            }
        }
    }


    //----------------------------------------------------
    // Button B processing
    if (keyData & PAD_BUTTON_B)
    {
        if (WM_lib_End() == WM_ERRCODE_SUCCESS)
        {
            prog_state = STATE_NONE;
        }
    }


    DispBall();

}

//============================================================================
// Initialize child
//
void KsChildInit(void)
{
    u16     i;
    int     errcode;

    wm_lib_comm_init();
    wm_lib_set_callback(child_callback);
    wm_lib_set_mode(WM_MODE_MP_CHILD);
    wm_lib_set_ggidFilter(0xffffffff); // no GGID filter

    wm_lib_set_keySharing_mode(1);

    errcode = wm_lib_start();

    if (WM_ERRCODE_SUCCESS != errcode)
        OS_Printf("Cannot Initialize WM!\n");

    ClearBallAll();
    for (i = 0; i < 16; ++i)
        InitBall(i);

    parent_no = 0;
    prog_state = STATE_KS_CHILDSCAN;

}


//============================================================================
// Scan child    
//
void KsChildScan(void)
{
    u16     keyData;
    int     i, j, errcode;
    u16     parent_num = (u16)wm_lib_get_parent_found_count();

    //---- Load pad data
    keyData = ReadKeySetTrigger(PAD_Read());

    if (0 < parent_num)
        ObjSetMessage("Select Parent to Connect");
    else
        ObjSetMessage("    No Parent Found     ");

    if (0 != parent_num)               // If parent found ...
    {
        if (parent_no >= parent_num - 1)
            parent_no = (u8)(parent_num - 1);

        // Use button A to start processing connection
        if (keyData & PAD_BUTTON_A)
        {
            // Allocate send and receive buffers for parent designation    
            wm_lib_set_sendbuffer((u8 *)libParam_sendBuf);
            wm_lib_set_recvbuffer((u8 *)libParam_recvBuf);

            // Request connection to parent
            errcode = wm_lib_connect_parent(parent_no);
            if (WM_ERRCODE_SUCCESS != errcode)
                OS_Printf("Cannot connnect to parent!\n");
            prog_state = STATE_NONE;
        }

        // Display parent list
        ObjSetString(PLIST_STRING_OBJNO, 24 * 8, 5 * 8, "P LIST", 7);
        for (i = 0; i < parent_num; i++)
        {
            if (TRUE == wm_lib_is_parent_gameinfo_valid(i))
            {
                char   *uname = (char *)(wm_lib_get_parent_gameinfo_username(i));

                for (j = 0; j < 8; j++)
                    ObjSet(PARENT_NAME_OBJNO + 8 * i + j, 24 * 8 + j * 8, (7 + i) * 8, uname[j], 5);

            }
        }

        ObjClearRange(PARENT_NAME_OBJNO + 8 * i, PARENT_NAME_OBJNO + 32 - 1);


        // Move parent selection cursor
        if (keyData & PAD_KEY_DOWN)
        {
            if (parent_no < parent_num - 1)
                parent_no++;
        }
        else if (keyData & PAD_KEY_UP)
        {
            if (parent_no > 0)
                parent_no--;
        }

        ObjSet(CURSOR_OBJNO, 23 * 8, (7 + parent_no) * 8, '@', 1);
    }

    //----------------------------------------------------
    // Button B processing
    if (keyData & PAD_BUTTON_B)
    {
        if (WM_lib_End() == WM_ERRCODE_SUCCESS)
        {
            prog_state = STATE_NONE;
        }
    }

}


//============================================================================
// Main processing for child
//
void KsChildMain(void)
{
    u16     keyData, i;
    WMkeySet keySet;

    //---- Load pad data
    keyData = ReadKeySetTrigger(PAD_Read());

    //----------------------------------------------------
    // Get key set
    if (WM_ERRCODE_SUCCESS == wm_lib_get_keyset(&keySet))
    {
        // At first, show key data for 4 devices    
        for (i = 0; i <= WM_NUM_MAX_CHILD; ++i)
        {
            u16     key = GetKeySetByID(&keySet, i);
            // Move ball and display coordinates
            if (key)
            {
                if (key & PAD_KEY_RIGHT)
                    ball[i].x += 1;
                if (key & PAD_KEY_LEFT)
                    ball[i].x -= 1;
                if (key & PAD_KEY_DOWN)
                    ball[i].y += 1;
                if (key & PAD_KEY_UP)
                    ball[i].y -= 1;

                ChangeBallPalette(i, (u8)((i == wm_lib_get_my_aid())? 1 : 6));
            }
        }
    }

    //----------------------------------------------------
    // Button B processing
    if (keyData & PAD_BUTTON_B)
    {
        if (WM_lib_End() == WM_ERRCODE_SUCCESS)
        {
            prog_state = STATE_NONE;
        }
    }

    DispBall();

}




//=================================================================
// Parent callback for KS demo
static void parent_callback(u16 type, void *arg)
{
#pragma unused(	arg )

    switch (type)
    {
        //-----------------------------------------------------------------
    case WM_TYPE_CHILD_CONNECTED:
        break;

        //-----------------------------------------------------------------
    case WM_TYPE_CHILD_DISCONNECTED:
        break;

        //-----------------------------------------------------------------
    case WM_TYPE_MP_PARENT_SENT:
        break;

        //-----------------------------------------------------------------
    case WM_TYPE_MP_PARENT_RECV:
        break;

        //-----------------------------------------------------------------
    case WM_TYPE_DISCONNECT_COMPLETE:
        break;

        //-----------------------------------------------------------------
    case WM_TYPE_DISCONNECT_FAILED:
        break;

        //-----------------------------------------------------------------
        // Complete end-processing with WM_lib_End()
    case WM_TYPE_END_COMPLETE:
        prog_state = STATE_SELECT;
        break;

    case WM_TYPE_MP_SEND_ENABLE:
        if (mp_on)
        {
#if 0
            u16     size;
            static u16 counter = 0;
            int     errcode;
            size = 2;
            // Set send data
            ((u16 *)libParam_sendBuf)[0] = ++counter;
            DC_FlushRange((u16 *)libParam_sendBuf, size);
            // Issue API
            errcode = WM_lib_SetMPData((u16 *)libParam_sendBuf, size);
            if (errcode == WM_ERRCODE_SUCCESS)
            {

            }
#endif
        }
        break;


    default:
        break;
    }

}



static void child_callback(u16 type, void *arg)
{
    switch (type)
    {
        //-----------------------------------------------------------------
    case WM_TYPE_PARENT_FOUND:

        break;

        //-----------------------------------------------------------------
    case WM_TYPE_PARENT_NOT_FOUND:
        break;

        //-----------------------------------------------------------------
    case WM_TYPE_CONNECT_FAILED:

        break;

        //-----------------------------------------------------------------
    case WM_TYPE_CONNECTED_TO_PARENT:
        {
            WMstartConnectCallback *callback = (WMstartConnectCallback *)arg;
            wm_lib_set_my_aid(callback->aid);
            ObjSetMessage("      Child  Mode       ");
            prog_state = STATE_KS_CHILD;        // Change main function
        }
        break;

        //-----------------------------------------------------------------
    case WM_TYPE_DISCONNECTED_FROM_PARENT:
        if (WM_ERRCODE_SUCCESS == WM_lib_End())
        {

        }
        break;

        //-----------------------------------------------------------------
    case WM_TYPE_MP_CHILD_SENT:
        break;

        //-----------------------------------------------------------------
    case WM_TYPE_MP_CHILD_RECV:
        break;

        //-----------------------------------------------------------------
        // Complete end-processing with WM_lib_End()
    case WM_TYPE_END_COMPLETE:

        prog_state = STATE_SELECT;

        break;

    case WM_TYPE_MP_SEND_ENABLE:
        if (mp_on)
        {
#if 0
            u16     size;
            static u16 counter = 0;
            int     errcode;
            size = 2;
            // Set send data
            ((u16 *)libParam_sendBuf)[0] = ++counter;
            DC_FlushRange((u16 *)libParam_sendBuf, size);
            // Issue API
            errcode = WM_lib_SetMPData((u16 *)libParam_sendBuf, size);
            if (errcode == WM_ERRCODE_SUCCESS)
            {

            }
#endif
        }
        break;


    default:
        break;
    }
}


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
