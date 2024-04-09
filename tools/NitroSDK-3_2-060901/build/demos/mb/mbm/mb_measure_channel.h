/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - include
  File:     mb_measure_channel.h

  Copyright 2004-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_measure_channel.h,v $
  Revision 1.7  01/18/2006 02:11:28  kitase_hirotake
  do-indent

  Revision 1.6  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.5  02/28/2005 05:26:19  yosizaki
  do-indent.

  Revision 1.4  10/22/2004 04:10:30  takano_makoto
  Changed the name of MBM_MeasureChannelEx to MBM_MeasureChannelInIdle

  Revision 1.3  10/22/2004 02:54:53  takano_makoto
  Made changes so that the state is cleared before the user callback

  Revision 1.2  10/22/2004 02:39:57  takano_makoto
  Added MBM_MeasureChannelEx

  Revision 1.1  09/28/2004 06:50:04  takano_makoto
  initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef _MBM_MEASURE_CHANNEL_H_
#define _MBM_MEASURE_CHANNEL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>

/* constant definitions*/

typedef enum
{
    MBM_MEASURE_SUCCESS = 0,           // Get the channel
    MBM_MEASURE_ERROR_INIT_API = -1,   // 
    MBM_MEASURE_ERROR_INIT_CALLBACK = -2,       // 
    MBM_MEASURE_ERROR_MEASURECHANNEL_API = -3,  // 
    MBM_MEASURE_ERROR_MEASURECHANNEL_CALLBACK = -4,     // 
    MBM_MEASURE_ERROR_RESET_API = -5,  // 
    MBM_MEASURE_ERROR_RESET_CALLBACK = -6,      // 
    MBM_MEASURE_ERROR_END_API = -7,    // 
    MBM_MEASURE_ERROR_END_CALLBACK = -8,        // 
    MBM_MEASURE_ERROR_NO_ALLOWED_CHANNEL = -9,  // 
    MBM_MEASURE_ERROR_ILLEGAL_STATE = -10       // 
}
MBMErrCode;


typedef struct
{
    s16     errcode;                   // Error code (MBMErrCode)
    u16     channel;                   // The channel with the lowest usage rate
}
MBMCallback;

/* Type Definitions*/
typedef void (*MBMCallbackFunc) (MBMCallback *);


/* function definitions*/

// This function is called before the wireless initialization state, and it searches for an optimal channel    
void    MBM_MeasureChannel(u8 *wm_buffer, MBMCallbackFunc callback);
// This function is called in the IDLE state, and it searches for an optimal channel    
void    MBM_MeasureChannelInIdle(MBMCallbackFunc callback);



#ifdef __cplusplus
}
#endif

#endif /* _MBM_MEASURE_CHANNEL_H_ */
