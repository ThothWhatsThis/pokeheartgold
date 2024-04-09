/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SPI - demos
  File:     monkey.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: monkey.h,v $
  Revision 1.4  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.3  02/28/2005 05:26:27  yosizaki
  do-indent.

  Revision 1.2  06/10/2004 09:37:46  terui
  Added message type. Adjusted sampling cycle.    

  Revision 1.1  06/02/2004 09:53:00  terui
  Initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef	MONKEY_H_
#define	MONKEY_H_

#ifdef	__cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include	<nitro/os.h>
#include	<nitro/spi.h>

/*---------------------------------------------------------------------------*
	constant definitions
 *---------------------------------------------------------------------------*/
#define		MONKEY_MESSAGE_ARRAY_MAX	8
#define		MONKEY_STACK_SIZE			1024
#define		MONKEY_THREAD_PRIORITY		15
#define		MONKEY_MIC_SPAN_TICK		( HW_SYSTEM_CLOCK / 64 / 60 / 263 )     //Approximately 1 line
#define		MONKEY_MIC_ARRAY_MAX		256
#define		MONKEY_TP_ARRAY_MAX			8
#define		MONKEY_SAMPLING_SPAN_LINE	4       // Line cycle of periodic time    

#define		MONKEY_MESSAGE_TYPE_MIC		1
#define		MONKEY_MESSAGE_TYPE_TP		2


/*---------------------------------------------------------------------------*
	structure definitions
 *---------------------------------------------------------------------------*/
typedef struct MonkeyWork
{
    OSMessageQueue msg_q;
    OSMessage msg_buf[MONKEY_MESSAGE_ARRAY_MAX];
    OSThread thread;
    u32     stack[MONKEY_STACK_SIZE / sizeof(u32)];
    OSAlarm alarm;
    u16     micBuf[MONKEY_MIC_ARRAY_MAX];
    u16     micIndex;
    TPData  tpBuf[MONKEY_TP_ARRAY_MAX];
    u16     tpIndex;
    u32     timerCount;

}
MonkeyWork;


/*---------------------------------------------------------------------------*
	function definitions
 *---------------------------------------------------------------------------*/

// Initialize, start thread
void    MonkeyInit(void);

// Get touch panel input value in array
void    MonkeyGetNewTpData(s32 num, TPData *array);

// Get mike input value in array
void    MonkeyGetNewMicData(s32 num, u16 *array);


/*===========================================================================*/

#ifdef	__cplusplus
}          /* extern "C"*/
#endif

#endif // MONKEY_H_

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
