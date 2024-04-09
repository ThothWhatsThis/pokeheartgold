/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - demos - overlay-staticinit
  File:     top_menu.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: top_menu.c,v $
  Revision 1.3  01/18/2006 02:12:29  kitase_hirotake
  do-indent

  Revision 1.2  02/28/2005 05:26:03  yosizaki
  do-indent.

  Revision 1.1  01/19/2005 07:44:45  yosizaki
  initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include	"mode.h"
#include	"draw.h"

/*
 * in order to designate NitroStaticInit() in the static initializer,
 * include this header.
 */
#include <nitro/sinit.h>


/*****************************************************************************/
/* constant*/

/* the overlay ID of each mode to be selected from the top menu*/
FS_EXTERN_OVERLAY(mode_1);
FS_EXTERN_OVERLAY(mode_2);
FS_EXTERN_OVERLAY(mode_3);

enum
{
    MENU_MODE_1,
    MENU_MODE_2,
    MENU_MODE_3,
    MENU_BEGIN = 0,
    MENU_END = MENU_MODE_3,
    MENU_MAX = MENU_END + 1
};


/*****************************************************************************/
/* variable*/

/* menu cursor*/
static int menu_cursor;


/*****************************************************************************/
/* function*/

/*---------------------------------------------------------------------------*
  Name:         MyUpdateFrame

  Description:  Updates the internal state by one frame in current mode.    

  Arguments:    frame_count      Frame count of current operation.
                input            Input data array.    
                player_count     Current number of players. (Valid number of input elements)    
                own_player_id    Own player number.

  Returns:      If current mode ends in current frame, FALSE.
                Otherwise, TRUE.
 *---------------------------------------------------------------------------*/
static BOOL MyUpdateFrame(int frame_count,
                          const InputData * input, int player_count, int own_player_id)
{
    (void)frame_count;
    (void)player_count;

    /* select menu with Up and Down keys*/
    if (IS_INPUT_(input[own_player_id], PAD_KEY_UP, push))
    {
        if (--menu_cursor < MENU_BEGIN)
        {
            menu_cursor = MENU_END;
        }
    }
    if (IS_INPUT_(input[own_player_id], PAD_KEY_DOWN, push))
    {
        if (++menu_cursor > MENU_END)
        {
            menu_cursor = MENU_BEGIN;
        }
    }
    /* decide with the A button*/
    return !IS_INPUT_(input[own_player_id], PAD_BUTTON_A, push);
}

/*---------------------------------------------------------------------------*
  Name:         MyDrawFrame

  Description:  Update internal state by one frame in current mode.    

  Arguments:    frame_count      Frame count of current operation.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MyDrawFrame(int frame_count)
{
    int     i;

    (void)frame_count;

    /* display file name and a simple description of operations*/
    SetTextColor(GX_RGB(31, 31, 31));
    DrawText(0, 10, "%s", __FILE__);
    DrawText(30, 40, "up/down: select menu");
    DrawText(30, 50, "   A   : run selected mode");
    /* menu and cursor display*/
    SetTextColor(GX_RGB(31, 31, 16));
    for (i = MENU_BEGIN; i <= MENU_END; ++i)
    {
        DrawText(40, 80 + i * 10, "%c mode %d", ((i == menu_cursor) ? '>' : ' '), i + 1);
    }
}

/*---------------------------------------------------------------------------*
  Name:         MyEndFrame

  Description:  Ends current mode.

  Arguments:    p_next_mode      To explicitly indicate the next mode,    
                                 overwrite the ID in the address pointed to by this pointer.    
                                 If no mode is specified, the mode that called the current
                                 mode will be selected.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MyEndFrame(FSOverlayID *p_next_mode)
{
    /* expressly configure and return the selected mode*/
    switch (menu_cursor)
    {
    case MENU_MODE_1:
        *p_next_mode = FS_OVERLAY_ID(mode_1);
        break;
    case MENU_MODE_2:
        *p_next_mode = FS_OVERLAY_ID(mode_2);
        break;
    case MENU_MODE_3:
        *p_next_mode = FS_OVERLAY_ID(mode_3);
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NitroStaticInit

  Description:  Function for auto-initialization as static initializer.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void NitroStaticInit(void)
{
    UpdateFrame = MyUpdateFrame;
    DrawFrame = MyDrawFrame;
    EndFrame = MyEndFrame;

    /* perform the necessary initialization processes for each mode here*/

    menu_cursor = 0;
    SetClearColor(GX_RGB(0, 0, 0));
}
