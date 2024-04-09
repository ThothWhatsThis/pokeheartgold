/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SPI - demos
  File:     monkey.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: monkey.c,v $
  Revision 1.10  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.9  02/28/2005 05:26:23  yosizaki
  do-indent.

  Revision 1.8  08/20/2004 02:44:31  terui
  Revised so that PM_SetAmp is also done when TS_VERSION=0.

  Revision 1.7  08/04/2004 23:44:28  terui
  Changed LCD backlight operation functions

  Revision 1.6  08/02/2004 01:07:04  yada
  PM_GAINAMP_80 -> PM_AMPGAIN_80

  Revision 1.5  07/31/2004 12:46:19  terui
  Added code to initialize PMIC for mic

  Revision 1.4  06/10/2004 09:41:25  terui
  Moved sampling timing determination from thread to timer handler.
  Moved timer start to the thread.    
  Added mechanism that allows thread to determine the sampling device from the message.    

  Revision 1.3  2004/06/10 05:23:46  yasu
  Change TP_WaitCalibratedResult -> TP_GetCalibratedResult

  Revision 1.2  06/03/2004 11:17:29  terui
  Changed the sampling rate.

  Revision 1.1  06/02/2004 09:53:25  terui
  Initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    "monkey.h"
#include    <nitro/spi.h>

#ifdef  SDK_TS
#if     ( SDK_TS_VERSION >= 100 )
#include    <nitro/spi/common/pm_common.h>
#include    <nitro/spi/ARM9/pm.h>
#endif
#endif


/*---------------------------------------------------------------------------*
    Internal function definitions
 *---------------------------------------------------------------------------*/
static void MonkeyThread(void *arg);
static void MonkeySamplingAlarm(void *arg);
static void MonkeyMicCallback(MICResult result, void *arg);
static void MonkeyTpCallback(TPRequestCommand command, TPRequestResult result, u16 index);


/*---------------------------------------------------------------------------*
    internal variable definitions
 *---------------------------------------------------------------------------*/
static MonkeyWork monkey;


/*---------------------------------------------------------------------------*
  Name:         MonkeyInit

  Description:  Start the SPI device sampling thread.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MonkeyInit(void)
{
    // Create a thread for sampling only.    
    OS_InitMessageQueue(&(monkey.msg_q), monkey.msg_buf, MONKEY_MESSAGE_ARRAY_MAX);
    OS_CreateThread(&(monkey.thread),
                    MonkeyThread,
                    0,
                    (void *)(monkey.stack + (MONKEY_STACK_SIZE / sizeof(u32))),
                    MONKEY_STACK_SIZE, MONKEY_THREAD_PRIORITY);
    OS_WakeupThreadDirect(&(monkey.thread));
}

/*---------------------------------------------------------------------------*
  Name:         MonkeyGetNewTpData

  Description:  Gets the newest touch panel sampling data.
                Does not care whether the data has been read before.    

  Arguments:    num   - Specifies the amount of data to get.
                array - Specifies the array that gets the data.    
                        Stores the data starting at the beginning of the array in order of newness.    

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MonkeyGetNewTpData(s32 num, TPData *array)
{
    s32     i;
    s32     index;

    index = (s32)(monkey.tpIndex);
    for (i = 0; i < num; i++)
    {
        index = (index + (MONKEY_TP_ARRAY_MAX - 1)) % MONKEY_TP_ARRAY_MAX;
        array[i] = monkey.tpBuf[index];
    }
}

/*---------------------------------------------------------------------------*
  Name:         MonkeyGetNewMicData

  Description:  Gets the newest mic sampling data.
                Does not care whether the data has been read before.    

  Arguments:    num   - Specifies the amount of data to get.
                array - Specifies the array that gets the data.    
                        Stores the data starting at the beginning of the array in order of newness.    

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MonkeyGetNewMicData(s32 num, u16 *array)
{
    s32     i;
    s32     index;

    index = (s32)(monkey.micIndex);
    for (i = 0; i < num; i++)
    {
        index = (index + (MONKEY_MIC_ARRAY_MAX - 1)) % MONKEY_MIC_ARRAY_MAX;
        array[i] = monkey.micBuf[index];
    }
}

/*---------------------------------------------------------------------------*
  Name:         MonkeyThread

  Description:  The thread that performs sampling

  Arguments:    arg     -   Unused.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MonkeyThread(void *arg)
{
#pragma unused( arg )

    OSMessage msg;

    // Initialize the microphone API
    {
        MIC_Init();
        monkey.micIndex = 0;

#ifdef  SDK_TS
        // Initialize PMIC
        PM_Init();
        // AMP on
        (void)PM_SetAmp(PM_AMP_ON);
#if ( SDK_TS_VERSION >= 100 )
        // Adjust AMP gain
        (void)PM_SetAmpGain(PM_AMPGAIN_80);
#endif
#if ( SDK_TS_VERSION == 100 )
        // Turn off LCD backlight to deal with noise.
        (void)PM_SetBackLight(PM_LCD_ALL, PM_BACKLIGHT_OFF);
#endif
#endif

    }
    // Initialize the touch panel API.
    {
        TPCalibrateParam calibrate;

        TP_Init();
        if (TP_GetUserInfo(&calibrate))
        {
            TP_SetCalibrateParam(&calibrate);
        }
        else
        {
            OS_Panic("Can't find TP calibration data.");
        }
        TP_SetCallback(MonkeyTpCallback);
        monkey.tpIndex = 0;
    }

    // Start the sampling timer    
    OS_CreateAlarm(&(monkey.alarm));
    monkey.timerCount = 0;
    OS_SetPeriodicAlarm(&(monkey.alarm),
                        OS_GetTick(),
                        (MONKEY_MIC_SPAN_TICK * MONKEY_SAMPLING_SPAN_LINE),
                        MonkeySamplingAlarm, NULL);

    while (TRUE)
    {
        // Stop the thread until message is received.
        (void)OS_ReceiveMessage(&(monkey.msg_q), &msg, OS_MESSAGE_BLOCK);

        // Touch panel sampling
        if ((u32)msg == MONKEY_MESSAGE_TYPE_TP)
        {
            TP_RequestSamplingAsync();
        }
        // Mic sampling
        else if ((u32)msg == MONKEY_MESSAGE_TYPE_MIC)
        {
            if (MIC_RESULT_SUCCESS != MIC_DoSamplingAsync(MIC_SAMPLING_TYPE_12BIT,
                                                          &(monkey.micBuf[monkey.micIndex]),
                                                          MonkeyMicCallback, NULL))
            {
                OS_Printf("Monkey: MIC request failure.\n");
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         MonkeySamplingAlarm

  Description:  Alarm handler that controls when to send message to the thread.

  Arguments:    arg     -   Unused.

  Returns:      None.
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
    Periodic timer and sample timing chart

    -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
MIC   >----           >----           >----           >----           >----
TP          >--------                       >--------                       >--

 *---------------------------------------------------------------------------*/
static void MonkeySamplingAlarm(void *arg)
{
#pragma unused( arg )

    if (0 == (monkey.timerCount % 8))
    {
        (void)OS_SendMessage(&(monkey.msg_q), (void *)MONKEY_MESSAGE_TYPE_MIC, OS_MESSAGE_BLOCK);
    }
    else if (3 == (monkey.timerCount % 16))
    {
        (void)OS_SendMessage(&(monkey.msg_q), (void *)MONKEY_MESSAGE_TYPE_TP, OS_MESSAGE_BLOCK);
    }

    monkey.timerCount++;
}

/*---------------------------------------------------------------------------*
  Name:         MonkeyMicCallback

  Description:  Callback function that responds to mic sampling requests.    
                Normally, when sampling is finished, it proceeds to the next
                buffer position of the data storage destination.

  Arguments:    result - Processing result for mic operation request.
                arg     -   Unused.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MonkeyMicCallback(MICResult result, void *arg)
{
#pragma unused( arg )

    if (result == MIC_RESULT_SUCCESS)
    {
        // Advance to the next data storage buffer position.    
        monkey.micIndex = (u16)((monkey.micIndex + 1) % MONKEY_MIC_ARRAY_MAX);
    }
    else
    {
        // Failure because influence of sound prevented previous sample from completing.    
        OS_Printf("Monkey: MIC request failure.\n");
    }
}

/*---------------------------------------------------------------------------*
  Name:         MonkeyTpCallback

  Description:  Callback function that responds to touch panel sampling requests.
                Normally, when sampling is finished, it proceeds to the next
                buffer position of the data storage destination.

  Arguments:    command - Indicates the operation request command to respond to. Not used.
                result  - Processing result for touch panel operation requests.
                index   - Index for auto sampling. Not used.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MonkeyTpCallback(TPRequestCommand command, TPRequestResult result, u16 index)
{
#pragma unused( command , index )

    if (result == TP_RESULT_SUCCESS)
    {
        // Convert raw sampling values to screen coordinates.    
        (void)TP_GetCalibratedResult(&(monkey.tpBuf[monkey.tpIndex]));
        // Advance to the next data storage buffer position.    
        monkey.tpIndex = (u16)((monkey.tpIndex + 1) % MONKEY_TP_ARRAY_MAX);
    }
    else
    {
        // Failure because influence of sound prevented previous sample from completing.    
        OS_Printf("Monkey: TP request failure.\n");
    }
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
