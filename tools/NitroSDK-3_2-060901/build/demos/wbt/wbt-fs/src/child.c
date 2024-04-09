/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WBT - demos - wbt-fs
  File:     child.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: child.c,v $
  Revision 1.7  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.6  11/14/2005 09:35:46  yosizaki
  change about FS_RedFile().

  Revision 1.5  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.4  02/18/2005 09:26:34  yosizaki
  fix around hidden warnings.

  Revision 1.3  11/16/2004 06:36:22  yosizaki
  change to use FS_LoadOverlayImageAsync.

  Revision 1.2  10/28/2004 13:10:16  yosizaki
  fix comments.

  Revision 1.1  10/25/2004 10:19:00  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitro/wm.h>
#include <nitro/wbt.h>
#include <nitro/fs.h>

#include    "wfs.h"

#include    "util.h"
#include    "wc.h"
#include    "func.h"
#include    "common.h"


/*---------------------------------------------------------------------------*
    Internal Constant Definitions
 *---------------------------------------------------------------------------*/

/* overlay test array*/
FS_EXTERN_OVERLAY(main_overlay_1);
FS_EXTERN_OVERLAY(main_overlay_2);
FS_EXTERN_OVERLAY(main_overlay_3);

static FSOverlayID ovl_id[] = {
    FS_OVERLAY_ID(main_overlay_1),
    FS_OVERLAY_ID(main_overlay_2),
    FS_OVERLAY_ID(main_overlay_3),
};

enum
{ OVL_ID_MAX = sizeof(ovl_id) / sizeof(*ovl_id) };
typedef void (*FUNC_TABLE) (char *);

const FUNC_TABLE func_table[] = {
    func_1,
    func_2,
    func_3,
};


/*---------------------------------------------------------------------------*
    internal variable definitions
 *---------------------------------------------------------------------------*/

/* for file operations*/
static FSFile file[1];
static char tmp_buf[0x40000];

static int file_index;
static int file_max;
static char file_path[8][256];

/* overlay test management variables*/
static BOOL is_overlay_test = FALSE;
static BOOL is_overlay_loaded = FALSE;
static int current_overlay = 0;
static FSOverlayInfo ovl_info[1];


/*---------------------------------------------------------------------------*
    Internal function definitions
 *---------------------------------------------------------------------------*/

static void ModeChildFile(const KeyInfo * p_key);       /* child file test*/
static void ModeChildOverlay(const KeyInfo * p_key);    /* child overlay test*/


/*---------------------------------------------------------------------------*
  Name:         CreateFileList

  Description:  Enumerates files from a directory.

  Arguments:    pe         The FSDirEntry structure to be recursively shared

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CreateFileList(FSDirEntry *pe)
{
    FSFile  d;
    FS_InitFile(&d);
    if (FS_SeekDir(&d, &pe->dir_id))
    {
        while (FS_ReadDir(&d, pe))
        {
            if (file_max >= sizeof(file_path) / sizeof(*file_path))
                break;
            if (pe->is_directory)
                CreateFileList(pe);
            else
            {
                FSFile  f;
                FS_InitFile(&f);
                if (FS_OpenFileFast(&f, pe->file_id))
                {
                    if (FS_GetPathLength(&f) <= (int)sizeof(*file_path))
                    {
                        if (FS_GetPathName(&f, file_path[file_max], sizeof(*file_path)))
                        {
                            ++file_max;
                        }
                    }
                    (void)FS_CloseFile(&f);
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         PrepareOverlay

  Description:  Loads the currently selected overlay.
                However, it proceeds to the step where the selected overlays are loaded asynchronously
                and FS_StartOverlay() is not called.

  Arguments:    new_overlay  The index of the newly-loaded overlay

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void PrepareOverlay(int new_overlay)
{
    /*
     * When loading an overlay,
     * all other overlays competing for the region
     * must be unloaded.
     */
    if (is_overlay_loaded)
    {
        is_overlay_loaded = FALSE;
        (void)FS_UnloadOverlayImage(ovl_info);
    }

    current_overlay = new_overlay;

    /*
     * First, get the overlay information.
     * The table has completed reception via WFS, so
     * these functions immediately return control.
     */
    (void)FS_LoadOverlayInfo(ovl_info, MI_PROCESSOR_ARM9, ovl_id[current_overlay]);
    {
        /*
         * Then load actual data into the overlay module region.
         * These can be done simply by calling FS_LoadOverlay(), but since it is
         * a synchronous process via wireless, it will take an extremely long time.
         *
         * In order for the application framework also to work during wait time,
         * you must either utilize FS_LoadOverlayImageAsync() as shown below,
         * or prepare a dedicated thread that calls FS_LoadOverlay().
         */
        if (!FS_LoadOverlayImageAsync(ovl_info, file))
        {
            OS_Panic("FS_LoadOverlayImageAsync() failed!");
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         ModeChild

  Description:  child communications screen

  Arguments:    key         Key input

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ModeChild(const KeyInfo * p_key)
{
    static BOOL init_flag = FALSE;
    if (!init_flag)
    {
        init_flag = TRUE;
        /* Initialize object for file test*/
        FS_InitFile(file);
    }
    {
        BOOL    bak_state = is_overlay_test;
        if (is_overlay_test)
            ModeChildOverlay(p_key);
        else
            ModeChildFile(p_key);
        if (bak_state != is_overlay_test)
        {
            ClearLine();
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         ModeChildFile

  Description:  The file load test process on the child communications screen.

  Arguments:    key         Key input

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ModeChildFile(const KeyInfo * p_key)
{
    int     i;

    PrintString(8, 1, COLOR_WHITE, "Child mode");
    PrintString(2, 3, COLOR_WHITE, "ReadFile test.");
    PrintString(2, 19, COLOR_WHITE, "Press B to move Overlay test.");
    PrintString(2, 20, COLOR_WHITE, "Press START to disconnect.");

    if (WFS_GetStatus() == WFS_STATE_READY)
    {

        /* generate list of files*/
        if (file_max == 0)
        {
            FSFile  d;
            FSDirEntry e;
            FS_InitFile(&d);
            (void)FS_FindDir(&d, "");
            MI_WriteByte(e.name + 0, (u8)'\0');
            (void)FS_TellDir(&d, &e.dir_id);
            CreateFileList(&e);
        }

        /* select the file ID that should be loaded*/
        if (p_key->trg & PAD_KEY_UP)
        {
            if (--file_index < 0)
            {
                file_index = file_max - 1;
            }
        }
        if (p_key->trg & PAD_KEY_DOWN)
        {
            if (++file_index > file_max - 1)
            {
                file_index = 0;
            }
        }

        PrintString(3, 5, COLOR_GREEN, "<UP & DOWN key>");
        PrintString(3, 6, COLOR_WHITE, "select testing file");
        PrintString(2, 7 + file_index, COLOR_WHITE, ">");
        for (i = 0; i < file_max; ++i)
        {
            PrintString(4, 7 + i, COLOR_WHITE, file_path[i]);
        }
        PrintString(3, 15, COLOR_GREEN, "<press A button>");

        /* state in which the file read is empty*/
        if (!FS_IsFile(file))
        {

            /* go to overlay test with B button*/
            if (p_key->trg & PAD_BUTTON_B)
            {
                is_overlay_test = TRUE;
                PrepareOverlay(current_overlay);
            }

            /* read selected file with A button*/
            else if (p_key->trg & PAD_BUTTON_A)
            {

                if (!FS_OpenFile(file, file_path[file_index]))
                {
                    file_index = 0;
                    OS_TPrintf("No file:\"%s\"\n", file_path[file_index]);
                }
                else
                {
                    /*
                     * Go to read the file.
                     *
                     * Setting to asynchronous mode will immediately return the process, but
                     * this sample performs a synchronous process here in order to get the actual
                     * transfer speed.
                     * Be aware that if called in this way from the main thread
                     * in the actual game application,
                     * the game sequence will be blocked for a very long time,
                     * and there will be a negative impact on the overall framework.
                     *
                     * Refer to the overlay load process section to see
                     * how to load in asynchronous mode.
                     */
                    int     ret;
                    int     bps = 0;
                    int     rest;
                    u64     milli;
                    OSTick  t;
                    t = OS_GetTick();
                    rest = (int)(FS_GetLength(file) - FS_GetPosition(file));

                    PrintString(3, 16, COLOR_WHITE, "reading... %d BYTE", rest);

                    /*
                     * As long as the WFS_End function is not called, synchronous operations are not stopped,
                     * so in the synchronous version, when it corresponds to a disconnection notification, polling is carried out as follows.
                     * With the exception of the CPU time being occupied by polling without transferring threads,
                     * this operation is basically equivalent to the FS_ReadFile function.
                     *
                     * If a thread separate from synchronous operations is giving a fixed disconnect notification,
                     * this kind of operation is not necessary as you can use the FS_ReadFile function directly.
                     * (Note that you cannot call the WFS_End function
                     * with an interrupt handler.)
                     */
                    {
                        ret = FS_ReadFileAsync(file, tmp_buf, sizeof(tmp_buf));
                        while (FS_IsBusy(file))
                        {
                            if (WcGetStatus() == WC_STATUS_READY)
                            {
                                WFS_End();
                                ret = -1;
                            }
                        }
                    }
                    /*
                     * In the sample, it only goes to Panic when being suspended,
                     * but it's possible to go to the return operation with the application.
                     * (The FS_GetResultCode function returns FS_RESULT_ERROR.)
                     */
                    if (ret != rest)
                    {
                        OS_Panic("FS_ReadFileAsync() returned invalid length!");
                    }
                    milli = OS_TicksToMilliSeconds(OS_GetTick() - t);
                    if (milli)
                    {
                        u64     d = ret * 8;
                        d *= 1000;
                        bps = (int)(d / milli);
                        PrintLine(3, 16, "read done %d BYTE", ret);
                        PrintLine(3, 17, "%d [bps]/%d [Bps]", bps, bps / 8);
                    }
                    (void)FS_CloseFile(file);
                }
            }
        }
    }

    if (p_key->trg & PAD_BUTTON_START)
    {
        WcEnd();
    }
}

/*---------------------------------------------------------------------------*
  Name:         ModeChildOverlay

  Description:  The file overlay test process on the child communications screen.

  Arguments:    key         Key input

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ModeChildOverlay(const KeyInfo * p_key)
{
    PrintString(8, 1, COLOR_WHITE, "Child mode");
    PrintString(2, 3, COLOR_WHITE, "Overlay test.");
    PrintString(2, 19, COLOR_WHITE, "Press B to move ReadFile test.");
    PrintString(2, 20, COLOR_WHITE, "Press START to disconnect.");

    if (WFS_GetStatus() == WFS_STATE_READY)
    {

        PrintString(3, 5, COLOR_GREEN, "<UP & DOWN key>");
        PrintString(3, 6, COLOR_WHITE, "select overlay");
        PrintString(18, 6 + current_overlay, COLOR_WHITE, ">");
        PrintString(20, 6, COLOR_WHITE, "func_1");
        PrintString(20, 7, COLOR_WHITE, "func_2");
        PrintString(20, 8, COLOR_WHITE, "func_3");

        PrintString(3, 10, COLOR_GREEN, "<press A button>");
        PrintString(3, 11, COLOR_BLUE, "(call overlay func)");

        /* if in a state where the file read is empty, overlay operations are possible*/
        if (!FS_IsFile(file))
        {

            /* go to ReadFile test with B button*/
            if (p_key->trg & PAD_BUTTON_B)
            {
                is_overlay_test = FALSE;
            }
            else
            {
                /* switch overlay with up and down*/
                int     new_overlay = current_overlay;

                if (p_key->trg & PAD_KEY_DOWN)
                {
                    if (++new_overlay >= OVL_ID_MAX)
                        new_overlay -= OVL_ID_MAX;
                }
                if (p_key->trg & PAD_KEY_UP)
                {
                    if (--new_overlay < 0)
                        new_overlay += OVL_ID_MAX;
                }
                if (current_overlay != new_overlay)
                {
                    PrepareOverlay(new_overlay);
                }
                /* call current function with A button*/
                else if (p_key->trg & PAD_BUTTON_A)
                {
                    char    tmp[64];
                    (*(func_table[current_overlay])) (tmp);
                    PrintLine(3, 11, tmp);
                }
            }
        }
        else
        {
            /* display progress if receiving overlay*/
            if (FS_IsBusy(file))
            {
                int     current, total;
                PrintLine(2, 14, "loading...%8d BYTE", FS_GetLength(file));
                if (WFS_GetCurrentDownloadProgress(&current, &total))
                {
                    PrintLine(2, 15, "progress %8d / %8d", current, total);
                }
            }
            /* start up overlay if read is complete*/
            else
            {
                if (!is_overlay_loaded)
                {
                    PrintLine(2, 14, "loading done. %8d BYTE", FS_GetLength(file));
                    PrintLine(2, 15, "");
                    FS_StartOverlay(ovl_info);
                    (void)FS_CloseFile(file);
                    is_overlay_loaded = TRUE;
                }
            }
        }
    }

    if (p_key->trg & PAD_BUTTON_START)
    {
        WcEnd();
    }
}


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
