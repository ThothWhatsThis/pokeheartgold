/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - demos - wireless-all
  File:     wh_measure.c

  Copyright 2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wh_measure.c,v $
  Revision 1.2  04/10/2006 13:37:12  yosizaki
  beautify source code indent.

  Revision 1.1  04/10/2006 13:07:32  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include "common.h"
#include "wh.h"


/*****************************************************************************/
/* function*/

/*---------------------------------------------------------------------------*
  Name:         MeasureChannel

  Description:  Measures the available wireless rates for each channel.
                WH can be used as the WH_SYSSTATE_IDLE state.

  Arguments:    None.

  Returns:      The channel with the lowest rate of wireless use.
 *---------------------------------------------------------------------------*/
u16 MeasureChannel(void)
{
    SDK_ASSERT(WH_GetSystemState() == WH_SYSSTATE_IDLE);
    /* channel measurement begins*/
    (void)WH_StartMeasureChannel();
    WaitWHState(WH_SYSSTATE_MEASURECHANNEL);
    /* all channel measurement complete*/
    return WH_GetMeasureChannel();
}
