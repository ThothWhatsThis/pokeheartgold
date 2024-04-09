/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG
  File:     ctrdg_sp.c

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg_sp.c,v $
  Revision 1.11  2005/02/28 05:26:03  yosizaki
  do-indent.

  Revision 1.10  2004/12/20 00:59:49  terui
  Add "WVR_Shutdown" in CTRDGi_TerminateARM7

  Revision 1.9  2004/09/27 13:47:01  yada
  only arrange comments

  Revision 1.8  2004/09/16 04:18:04  yada
  fix a little

  Revision 1.7  2004/09/14 12:20:39  yada
  stop sound

  Revision 1.6  2004/09/14 11:46:40  yada
  fix a little

  Revision 1.5  2004/09/14 09:58:54  yada
  consider for SMALL_BUILD

  Revision 1.4  2004/09/14 08:53:16  yada
  separate callback to 2 function

  Revision 1.3  2004/09/14 07:22:32  yada
  fix a little

  Revision 1.2  2004/09/14 06:09:44  yada
  check cartridge pulled out in ARM7

  Revision 1.1  2004/09/13 10:45:14  yada
  initial release.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/ctrdg.h>
#include "../include/ctrdg_sp.h"
#include <nitro/snd/common/global.h>
#include <nitro/wvr.h>

//----------------------------------------------------------------------------

static CTRDGWorkSp ctw_sp;

//----------------------------------------------------------------------------

static void CTRDGi_CallbackForInitModuleInfo(PXIFifoTag tag, u32 data, BOOL err);
static void CTRDGi_CallbackForPulledOut(PXIFifoTag tag, u32 data, BOOL err);
static BOOL CTRDGi_IsNinLogoOfAgb(u16 *logop);
static void CTRDGi_TerminateARM7(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_Init

  Description:  initialize cartridge functions

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void CTRDG_Init(void)
{
    static BOOL isInitialized;

    if (isInitialized)
    {
        return;
    }
    isInitialized = TRUE;

    CTRDGi_InitCommon();

#ifndef SDK_SMALL_BUILD
    PXI_Init();
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_CTRDG, CTRDGi_CallbackForInitModuleInfo);

    CTRDGi_InitModuleInfo();

    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_CTRDG, CTRDGi_CallbackForPulledOut);

#endif // SDK_SMALL_BUILD
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_InitModuleInfo

  Description:  initialize the information of peripheral device around cartridge

                Because try lock cartridge, in case another processor had locked,
                wait till its was released.

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/

void CTRDGi_InitModuleInfo(void)
{
#ifndef SDK_SMALL_BUILD

    OSIrqMask lastIE;
    BOOL lastIME;
    static BOOL isInitialized;

    CTRDGModuleInfo *cip = CTRDGi_GetModuleInfoAddr();

    if (isInitialized)
    {
        return;
    }
    isInitialized = TRUE;

    if (!(reg_OS_PAUSE & REG_OS_PAUSE_CHK_MASK))
    {
        return;
    }

    lastIE = OS_SetIrqMask(OS_IE_MPFIFO_RECV);
    lastIME = OS_EnableIrq();

    while (ctw_sp.status != CTRDG_STATUS_INIT_MODULE_INFO)
    {
        SVC_WaitByLoop(0x100);
    }

    {
        CTRDGHeader *chp =
            (CTRDGHeader *)(HW_MAIN_MEM +
                            ((ctw_sp.
                              cmd32[0] & CTRDG_PXI_COMMAND_PARAM_MASK) >>
                             CTRDG_PXI_COMMAND_PARAM_SHIFT) * 32);

        cip->isAgbCartridge = CTRDGi_IsNinLogoOfAgb((u16 *)(chp->nintendoLogo));
    }

    //---- send message to ARM9
    CTRDGi_SendtoPxi(CTRDG_PXI_COMMAND_INIT_MODULE_INFO);

    (void)OS_RestoreIrq(lastIME);
    (void)OS_SetIrqMask(lastIE);

#endif // SDK_SMALL_BUILD
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_CallbackForInitModuleInfo

  Description:  callback to receive data from PXI

  Arguments:    tag  : tag from PXI (unused)
                data : data from PXI
                err  : error bit (unused)

  Returns:      None
 *---------------------------------------------------------------------------*/
static void CTRDGi_CallbackForInitModuleInfo(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused( tag, err )

    if ((data & CTRDG_PXI_COMMAND_MASK) == CTRDG_PXI_COMMAND_INIT_MODULE_INFO)
    {
        ctw_sp.cmd32[0] = data;
        ctw_sp.status = CTRDG_STATUS_INIT_MODULE_INFO;
    }
    else
    {
#ifndef SDK_FINALROM
        OS_Panic("illegal Cartridge pxi command.");
#else
        OS_Panic("");
#endif
    }
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_CallbackForPulledOut

  Description:  callback to receive data from PXI

  Arguments:    tag  : tag from PXI (unused)
                data : data from PXI
                err  : error bit (unused)

  Returns:      None
 *---------------------------------------------------------------------------*/
static void CTRDGi_CallbackForPulledOut(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused( tag, err )

    if ((data & CTRDG_PXI_COMMAND_MASK) == CTRDG_PXI_COMMAND_TERMINATE)
    {
        CTRDGi_TerminateARM7();
    }
    else
    {
#ifndef SDK_FINALROM
        OS_Panic("illegal Cartridge pxi command.");
#else
        OS_Panic("");
#endif
    }
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_IsNinLogoOfAgb

  Description:  is callback to receive data from PXI

  Arguments:    tag  : tag from PXI (unused)
                data : data from PXI
                err  : error bit

  Returns:      None
 *---------------------------------------------------------------------------*/

#define AGB_MASK1_OFFSET            0x9c
#define AGB_MASK2_OFFSET            0x9e
#define AGB_TITLE_NAME_OFFSET       0xa0

static BOOL CTRDGi_IsNinLogoOfAgb(u16 *logop)
{
    CTRDGHeader *chp = (CTRDGHeader *)HW_CTRDG_ROM;
    u16 *logo_orgp = (u16 *)chp->nintendoLogo;
    u16 lockID;
    BOOL retval = TRUE;
    int i;

    lockID = (u16)OS_GetLockID();

    (void)OS_LockCartridge(lockID);

    for (i = 0; i < (AGB_TITLE_NAME_OFFSET - 4) / 2; i++)
    {
        u16 cmpMask = 0xffff;
        if (i == (AGB_MASK1_OFFSET - 4) / 2)
        {
            cmpMask ^= 0x84;
        }
        else if (i == (AGB_MASK2_OFFSET - 4) / 2)
        {
            cmpMask ^= 0x03;
        }

        if ((logop[i] & cmpMask) != logo_orgp[i])
        {
            retval = FALSE;
            break;
        }
    }

    (void)OS_UnLockCartridge(lockID);

    OS_ReleaseLockID(lockID);

    return retval;
}

//================================================================================
//       for POLLING
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         CTRDG_CheckPullOut_Polling()

  Description:  polling that cartridge is pulled out

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
#define CTRDG_POLLING_INTERVAL 10
#define CTRDGi_COUNT_NOT_SET 0xffffffff

void CTRDG_CheckPullOut_Polling(void)
{
    static u32 nextCount = CTRDGi_COUNT_NOT_SET;
    static BOOL isCartridgePullOut = FALSE;
    static BOOL isFirstCheck = TRUE;
    static BOOL skipCheck = FALSE;

#ifndef SDK_SMALL_BUILD
    //---- if first calling
    if (nextCount == CTRDGi_COUNT_NOT_SET)
    {
        nextCount = OS_GetVBlankCount() + CTRDG_POLLING_INTERVAL;
        return;
    }

    //---- if skip flag  or  cartridge already pulled out  or  check time not come yet, just return.
    if (skipCheck || isCartridgePullOut || OS_GetVBlankCount() < nextCount)
    {
        return;
    }
    nextCount = OS_GetVBlankCount() + CTRDG_POLLING_INTERVAL;

    //---------------- check cartridge pulled out
    //---- check cartridge
    isCartridgePullOut = CTRDG_IsPulledOut();

    //---- if no cartridge...
    if (!CTRDG_IsExisting())
    {
        if (isFirstCheck)
        {
            skipCheck = TRUE;
            return;
        }
        isCartridgePullOut = TRUE;
    }
    isFirstCheck = FALSE;

    //---------------- if cartridge pulled out, tell that to ARM9
    if (isCartridgePullOut)
    {
        while (PXI_SendWordByFifo(PXI_FIFO_TAG_CTRDG, CTRDG_PXI_COMMAND_PULLED_OUT, FALSE) !=
               PXI_FIFO_SUCCESS)
        {
            OS_Sleep(100 /*msec */ );
        }
    }
#endif
}

//================================================================================
//         TERMINATION
//================================================================================
static void CTRDGi_TerminateARM7(void)
{
    //---- stop sound
    SND_BeginSleep();

    //---- stop wireless hardware
    WVR_Shutdown();

    //---- terminate
    OS_Terminate();
}
