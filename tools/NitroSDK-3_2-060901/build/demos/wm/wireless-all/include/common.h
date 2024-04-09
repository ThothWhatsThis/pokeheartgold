/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - wireless-all
  File:     common.h

  Copyright 2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: common.h,v $
  Revision 1.2  04/10/2006 13:37:12  yosizaki
  beautify source code indent.

  Revision 1.1  04/10/2006 13:07:32  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef WM_DEMO_WIRELESS_ALL_COMMON_H_
#define WM_DEMO_WIRELESS_ALL_COMMON_H_


#include <nitro.h>

#include "mbp.h"
#include "wh.h"
#include "wfs.h"


/*****************************************************************************/
/* declaration*/

/* key input structure used for data sharing*/
typedef struct
{
    u32     count;                     /* Frame info*/
    u16     key;                       /* Key information*/
    u16     padding;
}
GShareData;

SDK_COMPILER_ASSERT(sizeof(GShareData) <= WH_DS_DATA_SIZE);


/*****************************************************************************/
/* constant*/

/* palette color for text*/
enum
{
    PLTT_BLACK = 0,
    PLTT_BLUE = 1,
    PLTT_RED = 2,
    PLTT_PURPLE = 3,
    PLTT_GREEN = 4,
    PLTT_CYAN = 5,
    PLTT_YELLOW = 6,
    PLTT_WHITE = 7,

    PLTT_LINK_ICON                     /* palette for the wireless link strength display icon*/
};


/*****************************************************************************/
/* function*/

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
  Name:         InitCommon

  Description:  Common initialization function.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    InitCommon(void);

/*---------------------------------------------------------------------------*
  Name:         IS_PAD_PRESS

  Description:  determines own key status

  Arguments:    the determining flag

  Returns:      TRUE if specified key is held down
                Otherwise, FALSE
 *---------------------------------------------------------------------------*/
BOOL    IS_PAD_PRESS(u16 flag);

/*---------------------------------------------------------------------------*
  Name:         IS_PAD_TRIGGER

  Description:  determines own key trigger status

  Arguments:    the determining flag

  Returns:      TRUE if specified key trigger is enabled
                Otherwise, FALSE
 *---------------------------------------------------------------------------*/
BOOL    IS_PAD_TRIGGER(u16 flag);

/*---------------------------------------------------------------------------*
  Name:         GetCurrentInput

  Description:  gets current key input from the specified player

  Arguments:    aid -- the player from whom key input is to be gotten

  Returns:      Returns a valid address when the latest data sharing has succeeded.
                Otherwise returns NULL.
 *---------------------------------------------------------------------------*/
GShareData *GetCurrentInput(int aid);

/*---------------------------------------------------------------------------*
  Name:         GetCurrentChannel

  Description:  gets the currently selected wireless channel

  Arguments:    None

  Returns:      Returns the currently selected wireless channel.
 *---------------------------------------------------------------------------*/
u16     GetCurrentChannel(void);

/*---------------------------------------------------------------------------*
  Name:         SetCurrentChannel

  Description:  Sets the current wireless channel

  Arguments:    channel -- channel to be set

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    SetCurrentChannel(u16 channel);

/*---------------------------------------------------------------------------*
  Name:         LoadLinkIcon

  Description:  Loads the link icon into VRAM

  Arguments:    id -- the load target's character ID
                palette -- the load target's palette
                level -- the link strength

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    LoadLinkIcon(int id, int palette, int level);

/*---------------------------------------------------------------------------*
  Name:         PrintString

  Description:  renders background text

  Arguments:    x -- the rendering x grid position
                y -- the rendering y grid position
                palette -- the palette index
                format -- the formatted string

  Returns:      None
 *---------------------------------------------------------------------------*/
void    PrintString(int x, int y, int palette, const char *format, ...);

/*---------------------------------------------------------------------------*
  Name:         WaitNextFrame

  Description:  initializes rendering while waiting for the next picture frame

  Arguments:    None.

  Returns:      Returns TRUE when key input is updated.
 *---------------------------------------------------------------------------*/
BOOL    WaitNextFrame(void);

/*---------------------------------------------------------------------------*
  Name:         WaitWHState

  Description:  Waits until the specified WH state

  Arguments:    state -- the state whose transition is awaited

  Returns:      None
 *---------------------------------------------------------------------------*/
void    WaitWHState(int state);

/*---------------------------------------------------------------------------*
  Name:         MeasureChannel

  Description:  Measures the available wireless rates for each channel.
                Blocking while running internal display until completion.

  Arguments:    None.

  Returns:      The channel with the lowest rate of wireless use.
 *---------------------------------------------------------------------------*/
u16     MeasureChannel(void);

/*---------------------------------------------------------------------------*
  Name:         ExecuteDownloadServer

  Description:  Runs DS Download Play parent process.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    ExecuteDownloadServer(void);

/*---------------------------------------------------------------------------*
  Name:         StartWirelessParent

  Description:  Launches data sharing parent mode.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    StartWirelessParent(void);

/*---------------------------------------------------------------------------*
  Name:         StartWirelessChild

  Description:  Launches data sharing child mode.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    StartWirelessChild(void);

/*---------------------------------------------------------------------------*
  Name:         ExecuteDataSharing

  Description:  data sharing main processes
                WFS process in background

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    ExecuteDataSharing(void);


#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif // WM_DEMO_WIRELESS_ALL_COMMON_H_
