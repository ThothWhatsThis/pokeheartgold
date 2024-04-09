/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - demos - card-1
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.13  04/03/2006 01:58:59  yosizaki
  add CARD_BACKUP_TYPE_FLASH_8MBITS.

  Revision 1.12  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.11  11/21/2005 10:18:58  kitase_hirotake
  Deleted OS_InitPrintServer and OS_PrintServer.

  Revision 1.10  10/20/2005 07:47:05  yosizaki
  add CARD_BACKUP_TYPE_FLASH_4MBITS.

  Revision 1.9  09/02/2005 07:39:23  yosizaki
  add CARDResult codes.

  Revision 1.8  07/05/2005 00:58:03  yosizaki
  add FRAM.

  Revision 1.7  06/15/2005 09:33:16  yosizaki
  add CARD_BACKUP_TYPE_EEPROM_512KBITS.

  Revision 1.6  04/06/2005 11:17:13  yosizaki
  small fix about CARD_GetResultCode().

  Revision 1.5  04/05/2005 07:24:32  yosizaki
  change around CARD_IdentifyBackup().

  Revision 1.4  02/28/2005 05:26:03  yosizaki
  do-indent.

  Revision 1.3  02/18/2005 08:02:11  yosizaki
  Revised copyright year

  Revision 1.2  02/18/2005 07:30:10  yosizaki
  fix around hidden warnings.

  Revision 1.1  09/09/2004 12:50:30  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro.h>
#include <nitro/card.h>
#include <nitro/fs.h>


/* routine for screen rendering*/
#include "draw.h"


/* Variables for the CARD library  ******************************************************/

/*
 * ID for locking CARD backup.
 * For exclusive use of CARD hardware resources with FS library, etc.
 * Used with CARD_LockBackup() / CARD_UnlockBackup().
 */
static u16 card_lock_id;

/*
 * The result value when there is a CARD access function error.
 * The return value of CARD_GetResultCode() gets changed by CARD_UnlockBackup(), etc., so
 * be careful when access processes and error handling are spaced far apart.
 */
static CARDResult last_result = CARD_RESULT_SUCCESS;

/*
 * Flag indicating whether write-test has been executed.
 */
static BOOL is_test_run;


/* Sample demo function  **********************************************************/

/*---------------------------------------------------------------------------*
  Name:         GetPadDown

  Description:  Get the key input of the keypress timing

  Arguments:    None.

  Returns:      Flag for the key pressed between the time the function was called last time and the time the function was called this time.
 *---------------------------------------------------------------------------*/
static u32 GetPadDown(void)
{
    static u32 btn = 0;
    static u32 btn_bak = 0;

    btn_bak = btn;
    btn = PAD_Read();
    return (u32)(~btn_bak & btn);
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  V-blank callback

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         InitApp

  Description:  OS etc., application base part initialization

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitApp(void)
{
    void   *tmp;
    OSHeapHandle hh;

    /* OS initialization*/
    OS_Init();
    OS_InitThread();
    OS_InitTick();
    /* arena initialization*/
    tmp = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
    OS_SetArenaLo(OS_ARENA_MAIN, tmp);
    hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
    if (hh < 0)
        OS_Panic("ARM9: Fail to create heap...\n");
    hh = OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
    /* Enable interrupts*/
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
}

/*---------------------------------------------------------------------------*
  Name:         GetCardResultString

  Description:  Obtains the result value of the CARD function using character string

  Arguments:    result           CARDResult result value.

  Returns:      Pointer to character string that describes result value.
 *---------------------------------------------------------------------------*/
static const char *GetCardResultString(CARDResult result)
{
    switch (result)
    {
    case CARD_RESULT_SUCCESS:
        return "success";
    case CARD_RESULT_FAILURE:
        return "failure";
    case CARD_RESULT_INVALID_PARAM:
        return "invalid param";
    case CARD_RESULT_UNSUPPORTED:
        return "unsupported";
    case CARD_RESULT_TIMEOUT:
        return "timeout";
    case CARD_RESULT_CANCELED:
        return "canceled";
    case CARD_RESULT_NO_RESPONSE:
        return "no response";
    case CARD_RESULT_ERROR:
        return "error";
    default:
        return "unknown error";
    }
}

/*---------------------------------------------------------------------------*
  Name:         SelectDevice

  Description:  Device  selection screen

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SelectDevice(void)
{
    /* *INDENT-OFF**/
    static const struct
    {
        CARDBackupType type;
        const char *comment;
    }
    types_table[] =
    {
        { CARD_BACKUP_TYPE_EEPROM_4KBITS,   "EEPROM    4 kb"},
        { CARD_BACKUP_TYPE_EEPROM_64KBITS,  "EEPROM   64 kb"},
        { CARD_BACKUP_TYPE_EEPROM_512KBITS, "EEPROM  512 kb"},
        { CARD_BACKUP_TYPE_FLASH_2MBITS,    "FLASH     2 Mb"},
        { CARD_BACKUP_TYPE_FLASH_4MBITS,    "FLASH     4 Mb"},
        { CARD_BACKUP_TYPE_FLASH_8MBITS,    "FLASH     8 Mb"},
        { CARD_BACKUP_TYPE_FRAM_256KBITS,   "FRAM    256 kb"},
    };
    /* *INDENT-ON**/
    enum
    { types_table_max = sizeof(types_table) / sizeof(*types_table) };

    int     cur = 0;
    BOOL    error = FALSE;
    BOOL    end = FALSE;
    int     i;

    while (!end)
    {

        u32     btn_down = GetPadDown();

        DrawBegin();
        ClearFrame(RGB555_BLACK);

        /* move the cursor with the Up and Down keys*/
        if (btn_down & PAD_KEY_DOWN)
        {
            error = FALSE;
            if (++cur >= types_table_max)
                cur -= types_table_max;
        }
        if (btn_down & PAD_KEY_UP)
        {
            error = FALSE;
            if (--cur < 0)
                cur += types_table_max;
        }

        /*
         * Choose currently selected device with A Button.
         *
         * Be sure to set this correctly, because the library cannot determine
         * whether the specified device is actually mounted.
         */
        if (btn_down & PAD_BUTTON_A)
        {
            CARD_LockBackup((u16)card_lock_id);
            end = CARD_IdentifyBackup(types_table[cur].type);
            if (!end)
            {
                error = TRUE;
                last_result = CARD_GetResultCode();
            }
            CARD_UnlockBackup(card_lock_id);
        }

        /* character string display*/
        SetTextColor(RGB555_GREEN);
        DrawText(10, 40, "select device!");
        for (i = 0; i < types_table_max; ++i)
        {
            DrawText(20, 60 + 10 * i, types_table[i].comment);
        }
        FillRect(10, 60, 10, i * 10, RGB555_BLACK);
        DrawText(10, 60 + 10 * cur, ">");
        if (error)
        {
            /* error display*/
            SetTextColor(RGB555_RED);
            DrawText(10, 120, "error!");
            DrawText(10, 130, "result:\"%s\"", GetCardResultString(last_result));
        }
        DrawEnd();
    }
}

/*---------------------------------------------------------------------------*
  Name:         TestWriteAndVerify

  Description:  write test screen

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void TestWriteAndVerify(void)
{
    const u32 page_size = CARD_GetBackupPageSize();
    const u32 total_size = CARD_GetBackupTotalSize();

    u32     pos = 0;

    BOOL    end = FALSE;

    /* display each type of information*/
    DrawBegin();
    ClearFrame(RGB555_BLACK);
    SetTextColor(RGB555_WHITE);
    if (CARD_IsBackupEeprom())
    {
        DrawText(10, 10, "EEPROM");
    }
    else if (CARD_IsBackupFlash())
    {
        DrawText(10, 10, "FLASH");
    }
    else if (CARD_IsBackupFram())
    {
        DrawText(10, 10, "FRAM");
    }
    DrawText(10, 20, "page:%d  total:%d", page_size, total_size);
    FillRect(10, 40, 256, 100, RGB555_BLACK);
    SetTextColor(RGB555_BLUE);
    MoveTo(0 + 8, 35);
    LineTo(256 - 8, 35);
    LineTo(256 - 8, 182 - 8);
    LineTo(0 + 8, 182 - 8);
    LineTo(0 + 8, 35);
    DrawEnd();

    while (!end)
    {

        u32     btn_down = GetPadDown();

        DrawBegin();

        /* Start with the A button if nothing has started yet*/
        if (!is_test_run)
        {
            if (btn_down & PAD_BUTTON_A)
            {
                is_test_run = TRUE;
            }
            SetTextColor(RGB555_WHITE);
            DrawText(10, 40, "press A-BUTTON to test");
        }

        /* stop test with B button*/
        if (btn_down & PAD_BUTTON_B)
        {
            end = TRUE;
        }

        /* testing*/
        if (is_test_run)
        {

            u8      tmp_buf[256];

            FillRect(10, 40, 256 - 10 * 2, 100, RGB555_BLACK);

            SetTextColor(RGB555_GREEN);
            DrawText(10, 40, "now testing...");
            DrawText(10, 50, "address:%d-%d", pos, pos + sizeof(tmp_buf));

            /*
             * Lock CARD resources so backup can be accessed later.
             *
             * This blocks card access from the FS and other modules
             * until CARD_UnlockBackup() is called. But beware of
             * process deadlock.
             *
             */
            CARD_LockBackup((u16)card_lock_id);
            {
                OSTick  tick;

                int     i;

                for (i = 0; i < sizeof(tmp_buf); ++i)
                    tmp_buf[i] = (u8)(pos * 3 + i);

                tick = OS_GetTick();

                /* Write asynchronously + verify content check*/
                if (CARD_IsBackupEeprom())
                {
                    CARD_WriteAndVerifyEepromAsync(pos, tmp_buf, sizeof(tmp_buf), NULL, (void *)0);
                }
                else if (CARD_IsBackupFlash())
                {
                    CARD_WriteAndVerifyFlashAsync(pos, tmp_buf, sizeof(tmp_buf), NULL, (void *)0);
                }
                else if (CARD_IsBackupFram())
                {
                    CARD_WriteAndVerifyFramAsync(pos, tmp_buf, sizeof(tmp_buf), NULL, (void *)0);
                }

                /*
                 * Wait for the completion here
                 *
                 * Depending on the type of device and the specified size,
                 the blocking can go on for a very long time.
                 * For this reason, you should modify the wait procedure to match
                 * the framework of the user application.
                 * (For example, judge based on one CARD_TryWaitBackupAsync() in each frame)
                 */
                (void)CARD_WaitBackupAsync();
                last_result = CARD_GetResultCode();

                if (last_result != CARD_RESULT_SUCCESS)
                {
                    /* if there is an error, end for now*/
                    end = TRUE;
                }
                else
                {
                    /* display the time if successful*/
                    tick = OS_GetTick() - tick;
                    DrawText(10, 60, "%6d[ms]", OS_TicksToMilliSeconds(tick));
                    /* move the test address to the next address*/
                    pos += sizeof(tmp_buf);
                    if (pos + sizeof(tmp_buf) > total_size)
                    {
                        end = TRUE;
                    }
                }
            }
            CARD_UnlockBackup(card_lock_id);

        }

        DrawEnd();
    }
}

/*---------------------------------------------------------------------------*
  Name:         ShowResult

  Description:  test results display screen

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ShowResult(void)
{

    BOOL    end = FALSE;

    while (!end)
    {

        u32     btn_down = GetPadDown();

        DrawBegin();

        /* Go back with A / B / START buttons*/
        if (btn_down & (PAD_BUTTON_A | PAD_BUTTON_B | PAD_BUTTON_START))
        {
            end = TRUE;
        }

        if (last_result == CARD_RESULT_SUCCESS)
        {
            /* success display*/
            SetTextColor(RGB555_WHITE);
            DrawText(10, 70, "done! (success)");
        }
        else
        {
            /* error display*/
            SetTextColor(RGB555_RED);
            DrawText(10, 70, "error!");
            DrawText(10, 80, "result:\"%s\"", GetCardResultString(last_result));
        }

        DrawEnd();
    }
}

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main entry point

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{

    {
        /* sample application initialization*/
        InitApp();
        InitDraw();
        SetClearColor(RGB555_CLEAR);
    }

    {
        /* CARD library initialization*/
        s32     ret;

        CARD_Init();
        ret = OS_GetLockID();
        if (ret == OS_LOCK_ID_ERROR)
        {
            OS_Panic("demo fatal error! OS_GetLockID() failed");
        }
        card_lock_id = (u16)ret;
    }

    /* screen transition*/
    for (;;)
    {
        /* device  selection screen*/
        SelectDevice();
        /* test start*/
        is_test_run = FALSE;
        TestWriteAndVerify();
        /* display results*/
        if (is_test_run)
            ShowResult();
    }

}
