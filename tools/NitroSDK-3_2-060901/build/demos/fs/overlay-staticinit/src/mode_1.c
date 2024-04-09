/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - demos - overlay-staticinit
  File:     mode_1.c

  Copyright 2005 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mode_1.c,v $
  Revision 1.2  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.1  2005/01/19 07:44:45  yosizaki
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
/* constant */


/*****************************************************************************/
/* variable */


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         MyUpdateFrame

  Description:  Updates internal state by one frame in current mode.  

  Arguments:    frame_count      Current operation frame count.
                input            Array of Input info.
                player_count     Current number of players. (Number of valid input elements)  
                own_player_id    Own player number.

  Returns:      If current mode ends in current frame, FALSE.
                Otherwise, TRUE.
 *---------------------------------------------------------------------------*/
static BOOL MyUpdateFrame(int frame_count,
                          const InputData * input, int player_count, int own_player_id)
{
    (void)frame_count;
    (void)player_count;

    /* end if anything is pressed */
    return !IS_INPUT_(input[own_player_id], ~0, push);
}

/*---------------------------------------------------------------------------*
  Name:         MyDrawFrame

  Description:  Update internal state by one frame in current mode.  

  Arguments:    frame_count      Current operation frame count.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MyDrawFrame(int frame_count)
{
    /*  display file name and a simple description of operations  */
    SetTextColor(GX_RGB(31, 31, 31));
    DrawText(0, 10, "%s", __FILE__);
    DrawText(30, 40, "press any key to return");
    /* simple frame count display  */
    SetTextColor(GX_RGB(31, 31, 16));
    DrawText(40, 80, "frame count = %d", frame_count);
}

/*---------------------------------------------------------------------------*
  Name:         MyEndFrame

  Description:  Ends current mode.

  Arguments:    p_next_mode      To explicitly indicate the next mode,  
                                 overwrite the destination pointed to with the mode ID.  
                                 If no mode is specified, the mode that called the current
                                 mode will be selected.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MyEndFrame(FSOverlayID *p_next_mode)
{
    /* do nothing and return to the top menu */
    (void)p_next_mode;
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

    /*  perform the necessary initialization processes for each mode here */

    SetClearColor(GX_RGB(0, 0, 0));
}

