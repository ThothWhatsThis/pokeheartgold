/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - dataShare-Model
  File:     main.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.h,v $
  Revision 1.7  01/18/2006 02:11:21  kitase_hirotake
  do-indent

  Revision 1.6  11/25/2005 01:35:27  seiki_masashi
  Changed the structure name from GameInfo to MyGameInfo

  Revision 1.5  11/21/2005 00:41:10  adachi_hiroaki
  Made the parent device selection screen display the number of players joining and their nicknames

  Revision 1.4  02/28/2005 05:26:12  yosizaki
  do-indent.

  Revision 1.3  11/05/2004 09:16:48  sasakis
  Improved the behavior of windows, and arranged codes.

  Revision 1.2  11/05/2004 04:27:40  sasakis
  Added the lobby screen and parent selection screen, and made the necessary modifications (such as scan related).

  Revision 1.1  11/02/2004 01:07:34  sasakis
  Made a few fixes in addition to restoring a section with some meaningless code

  $NoKeywords$
 *---------------------------------------------------------------------------*/

#ifndef __MAIN_H__
#define __MAIN_H__

enum
{
    SYSMODE_SELECT_ROLE,               // Role (start as a parent or a child) selection screen
    SYSMODE_OPTION,                    // Option screen
    SYSMODE_SELECT_PARENT,             // Parent selection screen (only for child)
    SYSMODE_LOBBY,                     // Lobby screen (only for parent)
    SYSMODE_LOBBYWAIT,                 // Lobby standby screen (only for child)
    SYSMODE_SELECT_CHANNEL,            // Channel selection screen
    SYSMODE_SCAN_PARENT,               // Parent search screen
    SYSMODE_ERROR,                     // Error report screen
    SYSMODE_PARENT,                    // Parent mode screen
    SYSMODE_CHILD,                     // Child mode screen
    SYSMODE_NUM
};

enum
{
    SHARECMD_NONE = 0,                 // Nothing specific (normal)
    SHARECMD_EXITLOBBY,                // Signal for ending the lobby screen
    SHARECMD_NUM
};

typedef struct ShareData_
{
    unsigned int command:3;            // Instruction (used for switching the game state at once)
    unsigned int level:2;              // Radio receiving strength
    unsigned int data:3;               // For graph
    unsigned int key:16;               // Key data
    unsigned int count:24;             // Frame count
}
ShareData;

typedef struct MyGameInfo_
{
    u8      nickName[10 * 2];
    u8      nickNameLength;
    u8      entryCount;
    u16     periodicalCount;
}
MyGameInfo;

struct PRScreen_;

extern BOOL isDataReceived(int index);
extern ShareData *getRecvData(int index);
extern ShareData *getSendData(void);
extern void changeSysMode(int s);
extern struct PRScreen_ *getInfoScreen(void);

#endif
