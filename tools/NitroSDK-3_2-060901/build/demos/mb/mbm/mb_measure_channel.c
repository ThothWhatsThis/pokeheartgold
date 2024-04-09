/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_child.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_measure_channel.c,v $
  Revision 1.10  01/18/2006 02:11:28  kitase_hirotake
  do-indent

  Revision 1.9  02/28/2005 05:26:19  yosizaki
  do-indent.

  Revision 1.8  02/18/2005 11:08:01  yosizaki
  fix around hidden warnings.

  Revision 1.7  12/09/2004 08:15:19  takano_makoto
  Made correction so that OS_GetVBlankCount function is used for obtaining a type of random number.    

  Revision 1.6  12/08/2004 14:27:30  takano_makoto
  Determines with a random number if the channel with the same electric wave usage rate existed.

  Revision 1.5  10/22/2004 04:10:30  takano_makoto
  Changed the name of MBM_MeasureChannelEx to MBM_MeasureChannelInIdle

  Revision 1.4  10/22/2004 02:54:54  takano_makoto
  Made changes so that the state is cleared before the user callback

  Revision 1.3  10/22/2004 02:39:57  takano_makoto
  Added MBM_MeasureChannelEx

  Revision 1.2  09/28/2004 09:52:44  takano_makoto
  Added debug print

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/wm.h>

#include "mb_measure_channel.h"

#define MBM_DEBUG

#ifdef MBM_DEBUG
#define PRINTF OS_TPrintf
#else
#define PRINTF(...) ((void)0)
#endif

#define MBM_MEASURE_DMA_NO                 2

// Random number macro
#define RAND()  ( sRand = sRand + 69069UL + 12345 )
#define RAND_INIT(x) ( sRand = (u32)(x) )

enum
{
    MBM_STATE_INIT,                    // Initial state
    MBM_STATE_MEASURING,               // Start state
    MBM_STATE_END_MEASURE,             // End measuring    
    MBM_STATE_END,                     // Wireless OFF state
    MBM_STATE_ERR                      // Error
};


enum
{
    ERRCODE_SUCCESS = 0,               // Succeeded
    ERRCODE_NOMORE_CHANNEL,            // No more channels were found beyond this    
    ERRCODE_API_ERR                    // WM API execution error
};

//===========================================================================
// Declare variables
//===========================================================================

static u32 sRand;

static MBMCallbackFunc sUserCallbackFunc = NULL;
static int mbm_measure_state = MBM_STATE_END;
// For communication channel storage
static s16 sChannel;
static u16 sChannelBitmap;
static u16 sChannelBusyRatio;

static BOOL sUseInIdle = FALSE;


//===========================================================================
// Function Prototype Declarations
//===========================================================================

static int wmi_measure_channel(WMCallbackFunc callback, u16 channel);
static void wm_callback(void *arg);
static void start_measure_channel(void);
static u16 state_in_measure_channel(u16 channel);
static void state_out_measure_channel(void *arg);
static void state_in_wm_end(void);
static void user_callback(s16 num);
static void change_mbm_state(u16 state);
static void callback_ready(s16 result);
static s16 select_channel(u16 bitmap);

//===========================================================================
// function definitions
//===========================================================================

/* ----------------------------------------------------------------------
  Checks the signal use rate
  ---------------------------------------------------------------------- */
static inline int wmi_measure_channel(WMCallbackFunc callback, u16 channel)
{
#define MBM_MEASURE_TIME         30    // The time interval in ms for picking up the signal for making one communication for one frame
#define MBM_MEASURE_CS_OR_ED     3     // The logical sum of the carrier sense and the ED value
#define MBM_MEASURE_ED_THRESHOLD 17    // The recommended ED threshold value thought through experiments to be effective.

    /*
     * An experimental value considered to be empirically valid has been used
     * as a parameter for obtaining the radio usage rate.
     */
    return WM_MeasureChannel(callback, // callback settings
                             MBM_MEASURE_CS_OR_ED,      // CS or ED
                             MBM_MEASURE_ED_THRESHOLD,  // Invalid when only carrier sense
                             channel,  // One of the channels obtained with WM_GetAllowedChannel
                             MBM_MEASURE_TIME); //The search time for 1 channel (in ms)    
}



/*---------------------------------------------------------------------------*
  Name:         MBM_MeasureChannel

  Description:  Searches for the channel with the lowest usage rate.
                Call from a wireless OFF state.    
                Returns a callback after internally measuring the radio and setting it to a wireless OFF state.

  Arguments:    wm_buffer   WM Work Memory
                callback    Designates the user callback to call when the search has finished    

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBM_MeasureChannel(u8 *wm_buffer, MBMCallbackFunc callback)
{
    sUseInIdle = FALSE;
    sUserCallbackFunc = callback;

    // Starts the initialization sequence.
    if (mbm_measure_state != MBM_STATE_END)
    {
        user_callback(MBM_MEASURE_ERROR_ILLEGAL_STATE);
        return;
    }

    if (WM_Initialize(wm_buffer, wm_callback, MBM_MEASURE_DMA_NO) != WM_ERRCODE_OPERATING)
    {
        user_callback(MBM_MEASURE_ERROR_INIT_API);
        return;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MBM_MeasureChannelInIdle

  Description:  Searches for the channel with the lowest usage rate.
                Call from an idle state.    
                Returns a callback after internally measuring and setting the radio to the IDLE state.

  Arguments:    callback    Designates the user callback to call when the search has finished    

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBM_MeasureChannelInIdle(MBMCallbackFunc callback)
{
    sUseInIdle = TRUE;
    sUserCallbackFunc = callback;

    // Starts the initialization sequence.
    if (mbm_measure_state != MBM_STATE_END)
    {
        user_callback(MBM_MEASURE_ERROR_ILLEGAL_STATE);
        return;
    }

    start_measure_channel();
}

/* ----------------------------------------------------------------------
  WM callback
  ---------------------------------------------------------------------- */
static void wm_callback(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    switch (cb->apiid)
    {
    case WM_APIID_INITIALIZE:
        /* WM_Initialize callback*/
        {
            WMCallback *cb = (WMCallback *)arg;
            if (cb->errcode != WM_ERRCODE_SUCCESS)
            {
                user_callback(MBM_MEASURE_ERROR_INIT_CALLBACK);
                return;
            }
        }
        change_mbm_state(MBM_STATE_INIT);
        start_measure_channel();
        break;

    case WM_APIID_MEASURE_CHANNEL:
        /* WM_MeasureChannel callback*/
        state_out_measure_channel(arg);
        break;

    case WM_APIID_END:
        change_mbm_state(MBM_STATE_END);
        user_callback(sChannel);
        break;

    default:
        OS_TPanic("Get illegal callback");

        break;
    }
}

/* ----------------------------------------------------------------------
  Begin searching for the radio usage rate
  ---------------------------------------------------------------------- */
static void start_measure_channel(void)
{
#define MAX_RATIO 100                  // The channel use rate is between 0 and 100
    u16     result;
    u8      macAddr[6];

    OS_GetMacAddress(macAddr);
    RAND_INIT(OS_GetVBlankCount() + *(u16 *)&macAddr[0] + *(u16 *)&macAddr[2] + *(u16 *)&macAddr[4]);   // Random number initialization
    RAND();

    sChannel = 0;
    sChannelBitmap = 0;
    sChannelBusyRatio = MAX_RATIO + 1;

    result = state_in_measure_channel(1);       // Check in order from channel 1

    if (result == ERRCODE_NOMORE_CHANNEL)
    {
        // There are not any channels available
        callback_ready(MBM_MEASURE_ERROR_NO_ALLOWED_CHANNEL);
        return;
    }

    if (result == ERRCODE_API_ERR)
    {
        // Error complete
        callback_ready(MBM_MEASURE_ERROR_MEASURECHANNEL_API);
        return;
    }

    // Begin measuring the radio usage rate
    change_mbm_state(MBM_STATE_MEASURING);
}


/*---------------------------------------------------------------------------*
  Name:         state_in_measure_channel

  Arguments:    channel     The channel number to start the search at

  Returns:      ERRCODE_SUCCESS        - Searching
                ERRCODE_NOMORE_CHANNEL - There are no more channels to search
                ERRCODE_API_ERR        - WM_MeasureChannel API callback error
 *---------------------------------------------------------------------------*/
static u16 state_in_measure_channel(u16 channel)
{
    u16     allowedCannel;

    allowedCannel = WM_GetAllowedChannel();

    while (((1 << (channel - 1)) & allowedCannel) == 0)
    {
        channel++;
        if (channel > 16)
        {
            /* when all allowed channels have finished being searched*/
            return ERRCODE_NOMORE_CHANNEL;
        }
    }

    if (wmi_measure_channel(wm_callback, channel) != WM_ERRCODE_OPERATING)
    {
        return ERRCODE_API_ERR;
    }
    return ERRCODE_SUCCESS;
}


static void state_out_measure_channel(void *arg)
{
    u16     result;
    u16     channel;
    WMMeasureChannelCallback *cb = (WMMeasureChannelCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        callback_ready(MBM_MEASURE_ERROR_MEASURECHANNEL_CALLBACK);
        return;
    }

    channel = cb->channel;
    PRINTF("CH%d = %d\n", cb->channel, cb->ccaBusyRatio);

    if (cb->ccaBusyRatio < sChannelBusyRatio)
    {
        sChannelBusyRatio = cb->ccaBusyRatio;
        sChannelBitmap = (u16)(1 << (channel - 1));
    }
    else if (cb->ccaBusyRatio == sChannelBusyRatio)
    {
        sChannelBitmap |= (u16)(1 << (channel - 1));
    }

    result = state_in_measure_channel(++channel);

    if (result == ERRCODE_NOMORE_CHANNEL)
    {
        // The channel search ends
        callback_ready(select_channel(sChannelBitmap));
        return;
    }

    if (result == ERRCODE_API_ERR)
    {
        // Error complete
        callback_ready(MBM_MEASURE_ERROR_MEASURECHANNEL_API);
        return;
    }

    // Do nothing if ERRCODE_SUCCESS

}


/* ----------------------------------------------------------------------
  Turn off wireless before the callback
  ---------------------------------------------------------------------- */
static void callback_ready(s16 result)
{
    sChannel = result;
    if (sUseInIdle)
    {
        change_mbm_state(MBM_STATE_END);
        user_callback(result);
    }
    else
    {
        state_in_wm_end();
        change_mbm_state(MBM_STATE_END_MEASURE);
    }
}

/* ----------------------------------------------------------------------
  End WM
  ---------------------------------------------------------------------- */
static void state_in_wm_end(void)
{
    if (WM_End(wm_callback) != WM_ERRCODE_OPERATING)
    {
        OS_TPanic("fail WM_End");
    }
}


/* ----------------------------------------------------------------------
  Change the MBM internal state
  ---------------------------------------------------------------------- */
static void change_mbm_state(u16 state)
{
#ifdef MBM_DEBUG
    static const char *STATE_STR[] = {
        "INIT",
        "MEASURING",                   // Start state
        "END_MEASURE",                 // End measuring    
        "END",                         // Wireless OFF state
        "ERR"                          // Error
    };
#endif

    PRINTF("%s -> ", STATE_STR[mbm_measure_state]);
    mbm_measure_state = state;
    PRINTF("%s\n", STATE_STR[mbm_measure_state]);
}

/* ----------------------------------------------------------------------
  Determine the channel
  ---------------------------------------------------------------------- */
static s16 select_channel(u16 bitmap)
{
    s16     i;
    s16     channel = 0;
    u16     num = 0;
    u16     select;

    for (i = 0; i < 16; i++)
    {
        if (bitmap & (1 << i))
        {
            channel = (s16)(i + 1);
            num++;
        }
    }

    if (num <= 1)
    {
        return channel;
    }

    // If there are multiple channels of the same electric wave usage rate
    select = (u16)(((RAND() & 0xFF) * num) / 0x100);

    channel = 1;

    for (i = 0; i < 16; i++)
    {
        if (bitmap & 1)
        {
            if (select == 0)
            {
                return (s16)(i + 1);
            }
            select--;
        }
        bitmap >>= 1;
    }

    return 0;
}

/* ----------------------------------------------------------------------
  Call the user callback
  ---------------------------------------------------------------------- */
static void user_callback(s16 type)
{
    MBMCallback arg;

    if (!sUserCallbackFunc)
    {
        return;
    }

    if (type > 0)
    {
        arg.errcode = MBM_MEASURE_SUCCESS;
        arg.channel = (u16)type;
    }
    else
    {
        arg.errcode = type;
        arg.channel = 0;
    }
    sUserCallbackFunc(&arg);
}
