/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - libraries
  File:     card_pullOut.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: card_pullOut.c,v $
  Revision 1.14  08/10/2006 06:04:23  okubata_ryoma
  fix CARD_TerminateForPulledOut.

  Revision 1.13  07/24/2006 06:42:06  okubata_ryoma
  small fix

  Revision 1.12  07/21/2006 12:48:02  okubata_ryoma
  Revision to fix the system not powering off at card removal issue

  Revision 1.11  02/07/2006 08:19:36  kitase_hirotake
  Changed the CARD_UserCallback invocation conditions in CARDi_PulledOutCallback

  Revision 1.10  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.9  10/24/2005 10:49:34  yosizaki
  add CARDi_CheckPulledOutCore().

  Revision 1.8  06/01/2005 07:09:40  yosizaki
  add CARD_CheckPulledOut().

  Revision 1.7  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.6  02/18/2005 11:20:17  yosizaki
  fix around hidden warnings.

  Revision 1.5  11/10/2004 06:39:20  yada
  When card pulled out, check to fold cover, and if true then power off.

  Revision 1.4  09/28/2004 05:06:54  yada
  only fix comment

  Revision 1.3  09/27/2004 13:42:46  yada
  change polling for card remove

  Revision 1.2  09/16/2004 04:05:45  yada
  fix a little

  Revision 1.1  09/15/2004 05:48:49  yada
  first release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

#include <card_rom.h>

//---- user callback for card pulled out
static CARDPulledOutCallback CARD_UserCallback;

//---- flag to be pulled out
static BOOL CARDi_IsPulledOutFlag = FALSE;

static void CARDi_PulledOutCallback(PXIFifoTag tag, u32 data, BOOL err);
static void CARDi_SendtoPxi(u32 data, u32 wait);

/*---------------------------------------------------------------------------*
  Name:         CARD_InitPulledOutCallback

  Description:  initialize callback setting

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void CARD_InitPulledOutCallback(void)
{
    PXI_Init();

    //---- set PXI callback
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_CARD, CARDi_PulledOutCallback);

    //---- init user callback
    CARD_UserCallback = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_PulledOutCallback

  Description:  callback to receive data from PXI

  Arguments:    tag  : tag from PXI (unused)
                data : data from PXI
                err  : error bit (unused)

  Returns:      None
 *---------------------------------------------------------------------------*/
static void CARDi_PulledOutCallback(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused( tag, err )

    u32     command = data & CARD_PXI_COMMAND_MASK;

    //---- receive message 'pulled out'
    if (command == CARD_PXI_COMMAND_PULLED_OUT)
    {
        if (CARDi_IsPulledOutFlag == FALSE)
        {
            BOOL    isTerminateImm = TRUE;

            CARDi_IsPulledOutFlag = TRUE;

            //---- call user callback
            if (CARD_UserCallback)
            {
                isTerminateImm = CARD_UserCallback();
            }

            //---- terminate
            if (isTerminateImm)
            {
                CARD_TerminateForPulledOut();
            }
        }
    }
    else
    {
#ifndef SDK_FINALROM
        OS_Panic("illegal card pxi command.");
#else
        OS_Panic("");
#endif
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARD_SetPulledOutCallback

  Description:  set user callback for being pulled out card

  Arguments:    callback : callback

  Returns:      None
 *---------------------------------------------------------------------------*/
void CARD_SetPulledOutCallback(CARDPulledOutCallback callback)
{
    CARD_UserCallback = callback;
}

/*---------------------------------------------------------------------------*
  Name:         CARD_IsPulledOut

  Description:  return if have detected card pulled out

  Arguments:    None

  Returns:      TRUE if detected
 *---------------------------------------------------------------------------*/
BOOL CARD_IsPulledOut(void)
{
    return CARDi_IsPulledOutFlag;
}

//================================================================================
//         TERMINATION
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         CARD_TerminateForPulledOut

  Description:  terminate for pulling out card
                send message to do termination to ARM7

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void CARD_TerminateForPulledOut(void)
{
#ifndef SDK_TEG
    BOOL    should_be_halt = TRUE;
    //---- if folding, power off
    if (PAD_DetectFold())
    {
        /* Retry while the wireless SPI exclusive control fails.*/
        u32 res;
        while ((res = PM_ForceToPowerOff()) == SPI_PXI_RESULT_EXCLUSIVE)
        {
            /* Wait for 10ms before retry*/
            OS_SpinWait(HW_CPU_CLOCK_ARM9 / 100);
        }
        if (res == PM_RESULT_SUCCESS)
        {
            // If ARM7 is busy, there is a chance that the power off will occur before the Terminate process, preventing system shutdown.
            // If the PM_ForceToPowerOff function is successful, do not send the Terminate command to the ARM7 side.
            should_be_halt = FALSE;
        }
    }

    // The ARM7 needs to be stopped if the clamshell is not closed or the PM_ForceToPowerOff function fails, so send Terminate command.
    if (should_be_halt)
    {
        //---- send 'TERMINATE' command to ARM7, and terminate itself immediately
        CARDi_SendtoPxi(CARD_PXI_COMMAND_TERMINATE, 1);
    }
#endif

    OS_Terminate();
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CheckPulledOutCore

  Description:  Main processing for the card removal detection function.
                The card bus must be locked.

  Arguments:    id            ROM-ID read from the card

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_CheckPulledOutCore(u32 id)
{
    //---- card ID IPL had read
    vu32    iplCardID = *(vu32 *)((*(u16 *)HW_CHECK_DEBUGGER_SW ==
                                   0) ? HW_RED_RESERVED : HW_BOOT_CHECK_INFO_BUF);
    //---- if card removal has been detected, we simulate PXI-notification from ARM7
    if (id != (u32)iplCardID)
    {
        OSIntrMode bak_cpsr = OS_DisableInterrupts();
        CARDi_PulledOutCallback(PXI_FIFO_TAG_CARD, CARD_PXI_COMMAND_PULLED_OUT, FALSE);
        (void)OS_RestoreInterrupts(bak_cpsr);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARD_CheckPulledOut

  Description:  get whether system has detected pulled out card
                by comparing IPL cardID with current cardID
                (notice that once card pulled out, IDs are different absolutely)

  Arguments:    None

  Returns:      TRUE if current cardID is equal to IPL cardID
 *---------------------------------------------------------------------------*/
void CARD_CheckPulledOut(void)
{
    CARDi_CheckPulledOutCore(CARDi_ReadRomID());
}

//================================================================================
//         SEND PXI COMMAND
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         CARDi_SendtoPxi

  Description:  send data via PXI

  Arguments:    data : data to send

  Returns:      None
 *---------------------------------------------------------------------------*/
static void CARDi_SendtoPxi(u32 data, u32 wait)
{
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_CARD, data, FALSE) != PXI_FIFO_SUCCESS)
    {
        SVC_WaitByLoop((s32)wait);
    }
}
