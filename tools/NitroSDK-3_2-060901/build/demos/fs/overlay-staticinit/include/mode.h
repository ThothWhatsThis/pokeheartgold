/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - demos - overlay-staticinit
  File:     mode.h

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mode.h,v $
  Revision 1.3  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.2  02/28/2005 05:26:06  yosizaki
  do-indent.

  Revision 1.1  01/19/2005 07:44:53  yosizaki
  initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#if	!defined(NITRO_DEMOS_FS_OVERLAY_STATICINIT_MODE_H_)
#define NITRO_DEMOS_FS_OVERLAY_STATICINIT_MODE_H_


#include <nitro.h>


/*
 * Defines the overlay interface for each mode.
 * These are set by the overlay's static initializer.   
 */


#if	defined(__cplusplus)
extern  "C"
{
#endif


/********************************************************************/
/* structure */

/* input information  structure*/
    typedef struct
    {
        TPData  tp;
        u16     push_bits;             /* the key bit at the moment of the press*/
        u16     hold_bits;             /* the key bit of the state being pressed*/
        u16     release_bits;          /* the key bit at the moment of the release*/
        u16     reserved;
    }
    InputData;


/********************************************************************/
/* constant*/

/* proprietary expanded bit that displays the stylus touch state*/
#define PAD_BUTTON_TP   0x4000


/********************************************************************/
/* function*/

/* the state determination macro for the designated key*/
#define	IS_INPUT_(input, key, action)	\
	(((input).action ## _bits & (key)) != 0)


/*---------------------------------------------------------------------------*
  Name:         UpdateFrame

  Description:  Updates the internal state by one frame in current mode.    

  Arguments:    frame_count      Frame count of current operation.
                input            Input data array.    
                player_count     Current number of players. (Valid number of input elements)    
                own_player_id    Own player number.

  Returns:      If current mode ends in current frame, FALSE.
                Otherwise, TRUE.
 *---------------------------------------------------------------------------*/
    extern BOOL (*UpdateFrame) (int frame_count,
                                const InputData * input, int player_count, int own_player_id);

/*---------------------------------------------------------------------------*
  Name:         DrawFrame

  Description:  Update internal state by one frame in current mode.    

  Arguments:    frame_count      Frame count of current operation.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    extern void (*DrawFrame) (int frame_count);

/*---------------------------------------------------------------------------*
  Name:         EndFrame

  Description:  Ends current mode.

  Arguments:    p_next_mode      To explicitly indicate the next mode,    
                                 overwrite the ID in the address pointed to by this pointer.    
                                 If no mode is specified, the mode that called the current
                                 mode will be selected.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    extern void (*EndFrame) (FSOverlayID *p_next_mode);


#if	defined(__cplusplus)
}                                      /* extern "C"*/
#endif


#endif                                 /* NITRO_DEMOS_FS_OVERLAY_STATICINIT_MODE_H_ */
