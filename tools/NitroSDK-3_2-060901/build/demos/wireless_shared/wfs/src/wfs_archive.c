/*---------------------------------------------------------------------------*
  Project:  NitroSDK - wireless_shared - demos - wfs
  File:     wfs_archive.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wfs_archive.c,v $
  Revision 1.10  08/25/2006 08:36:22  yosizaki
  fix about mixed-FAT mode.

  Revision 1.9  05/22/2006 02:34:52  yosizaki
  small fix.

  Revision 1.8  03/06/2006 08:12:01  adachi_hiroaki
  Replaced HW_ROM_HEADER_BUF with CARD_GetRomHeader()

  Revision 1.7  02/27/2006 12:50:33  yosizaki
  change FS_OpenFileDirect to FS_CreateFileFromRom

  Revision 1.6  01/18/2006 02:12:39  kitase_hirotake
  do-indent

  Revision 1.5  11/14/2005 08:32:35  yosizaki
  fix  about wfsi_work pointer.

  Revision 1.4  02/28/2005 05:26:35  yosizaki
  do-indent.

  Revision 1.3  02/18/2005 09:14:36  yosizaki
  fix around hidden warnings.

  Revision 1.2  12/15/2004 02:06:04  yasu
  untabify

  Revision 1.1  11/16/2004 01:12:55  yosizaki
  moved from /demos/wbt/wbt-fs.


 -[old log at /demos/wbt/wbt-fs/wfs]-

  Revision 1.12  10/28/2004 14:50:21  yosizaki
  fix around FSi_LoadTables.

  Revision 1.11  10/25/2004 15:20:35  yosizaki
  fix WFSi_LoadTables.

  Revision 1.10  10/25/2004 04:19:32  yosizaki
  change WFSi_LoadTables.

  Revision 1.9  09/29/2004 07:11:38  yosizaki
  merge from branch.

  Revision 1.8  09/15/2004 09:10:25  yosizaki
  add Seek&Read.

  Revision 1.7  09/15/2004 01:11:06  yosizaki
  add WFS_SetPacketSize().

  Revision 1.6  09/07/2004 03:05:51  yosizaki
  source code Maintenance.

  Revision 1.5  09/06/2004 08:47:48  yosizaki
  fix archive. (around result of FS_ReadFile())

  Revision 1.4  09/04/2004 13:23:55  yosizaki
  change to load TABLE-memory with WFSi_Alloc.

  Revision 1.3  09/04/2004 01:45:21  yosizaki
  delete obsolete codes and waste parameters.

  Revision 1.2  09/03/2004 09:58:38  yosizaki
  add wfs_thread.c

  Revision 1.1  09/03/2004 01:10:17  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro.h>
#include <nitro/fs.h>
#include <nitro/card.h>

#include "wfs.h"
#include "wfs_private.h"


/********************************************************************/
/* functions*/

/* read callback*/
static FSResult WFSi_ReadRomCallback(FSArchive *p_arc, void *dst, u32 src, u32 len)
{
    /* allow FNT / FAT to be read as-is*/
    (void)p_arc;
    MI_CpuCopy8((const void *)src, dst, len);
    return FS_RESULT_SUCCESS;
}

/* light callback*/
static FSResult WFSi_WriteRomCallback(FSArchive *p_arc, const void *src, u32 dst, u32 len)
{
    (void)p_arc;
    (void)src;
    (void)dst;
    (void)len;
    return FS_RESULT_FAILURE;
}

/* user procedure*/
static FSResult WFSi_RomArchiveProc(FSFile *p_file, FSCommandType cmd)
{
    switch (cmd)
    {

    case FS_COMMAND_READFILE:
        /* fail if in a not-prepared state*/
        if (!wfsi_work || (wfsi_work->state != WFS_STATE_READY))
        {
            return FS_RESULT_ERROR;
        }
        /* success if size 0*/
        if (p_file->arg.readfile.len == 0)
        {
            return FS_RESULT_SUCCESS;
        }
        /* begin request - GetBlock - end request*/
        WFSi_ReadRequest(p_file);
        return FS_RESULT_PROC_ASYNC;

    case FS_COMMAND_WRITEFILE:
        /* the following commands are not supported*/
        return FS_RESULT_UNSUPPORTED;

    default:
        /* all other commands use default operations*/
        return FS_RESULT_PROC_UNKNOWN;

    }

}

enum
{
    region_fat,
    region_fnt,
    region_ovt9,
    region_ovt7,
    region_max
};

/*
 * Parent loads FNT, FAT, OVT to memory.
 * This file has been prepared at the outset so that
 * child can connect to parent any time.
 */
void WFSi_LoadTables(FSFile *p_rom, BOOL use_parent_fs)
{
    /*
     * Reads FNT+FAT+OVT information, and registers it as a file.
     * Can use the following selections.
     *
     * (1) Normal independent child program: p_rom && !use_parent_fs
     * (2) File shared type child program: p_rom && use_parent_fs
     * (3) Parent clone boot: !p_rom.
     *
     * To properly support all of these, use the following procedure
     * to do merge an overlay FAT, etc.
     *
     Select an appropriate ROM header and acquire the size of FAT, FNT, OVT.
     Taking overall settings into consideration, calculate and allocate the required memory.
     *    Depending on the settings, appropriately add FAT size used for OVT.
     Load OVT/FAT/FNT.
     Depending on settings, appropriately revise the OVT File-ID and add FAT.
     *    (Special process needed only for file shared child devices)
     */

    typedef struct
    {
        u32     len;
        u32     data[1];
    }
    TableInfo;

    int     i;

    u8     *mem = NULL;
    u32     size = 0;

    const BOOL is_own = (p_rom == NULL);
    const BOOL is_mixed_fat = (!is_own && use_parent_fs);

    u32     child_base_offset;
    u32     rom_base_offset;
    const u8 *rom_header;
    u8      rom_buf[0x60];

    CARDRomRegion r[region_max];
    u32     align_len[region_max];
    TableInfo *(table_info[region_max]);


    FSFile  rom_file[1];

    /*
     Open a large file that indicates the parent itself.
     */
    FS_InitFile(rom_file);
    (void)FS_CreateFileFromRom(rom_file, 0, 0x7FFFFFFF);
    /*
     Select an appropriate ROM header and acquire the size of FAT, FNT, OVT.
     */
    {
        /* if p_rom is NULL, the parent device itself is obtained as a file*/
        if (is_own)
        {
            rom_base_offset = 0;
            child_base_offset = 0;
            rom_header = (const u8 *)CARD_GetRomHeader();
        }
        /* if not NULL, only the ROM header is obtained first*/
        else
        {
            const u32 bak_pos = FS_GetPosition(p_rom);
            rom_base_offset = bak_pos + FS_GetFileImageTop(p_rom);
            child_base_offset = rom_base_offset;
            rom_header = rom_buf;
            (void)FS_ReadFile(p_rom, rom_buf, sizeof(rom_buf));
            (void)FS_SeekFile(p_rom, (int)bak_pos, FS_SEEK_SET);
        }
        /*
         * Since the header has been acquired, calculate total memory size.
         *
         * (1) : OVT(child) + FNT(child) + FAT(child)
         * (2) : OVT(child) + FNT(parent) + FAT(parent + OVT(child))
         * (3) : OVT(parent) + FAT(parent) + FNT(parent)
         *
         * (2) is extremely complex because FAT must be converted to integrate OVT.
         */
        if (!is_mixed_fat)
        {
            /* In the case of (1) or (3), continue to reference the parent device / child device side*/
            r[region_fat] = *(const CARDRomRegion *)(rom_header + 0x48);
            r[region_fnt] = *(const CARDRomRegion *)(rom_header + 0x40);
            r[region_ovt9] = *(const CARDRomRegion *)(rom_header + 0x50);
            r[region_ovt7] = *(const CARDRomRegion *)(rom_header + 0x58);
        }
        else
        {
            /*
             * Case (2)
             * OVT references the child device, while FAT/FNT reference the parent.
             * Furthermore, the ID for the overlay file must be added to the FAT
             * by the amount of the OVT.
             * When doing this, the file_id member in the OVT must also be revised.
             */
            r[region_fat] = *(const CARDRomRegion *)(CARD_GetRomHeader() + 0x48);
            r[region_fnt] = *(const CARDRomRegion *)(CARD_GetRomHeader() + 0x40);
            r[region_ovt9].offset = (u32)(*(const u32 *)(rom_header + 0x50) + rom_base_offset);
            r[region_ovt9].length = (u32)(*(const u32 *)(rom_header + 0x54));
            r[region_ovt7].offset = (u32)(*(const u32 *)(rom_header + 0x58) + rom_base_offset);
            r[region_ovt7].length = (u32)(*(const u32 *)(rom_header + 0x5C));
            /* The file system will be the base for the parent device, so move the offset to the start*/
            child_base_offset = 0;
        }
    }
    /*
     Taking overall settings into consideration, calculate and allocate the required memory.
     *    Depending on the settings, appropriately add the FAT size for OVT.
     */
    {
        /*
         * Acquire overall size.
         * Store the base offset of the child device ROM in the first 4 bytes.
         */
        size += sizeof(u32);
        for (i = 0; i < region_max; ++i)
        {
            u32     r_len = r[i].length;
            /* for (2), add the ID for the overlay file to the FAT*/
            if ((i == region_fat) && is_mixed_fat)
            {
                int     j;
                for (j = region_ovt9; j < region_max; ++j)
                {
                    r_len += (u32)((r[j].length / sizeof(FSOverlayInfoHeader)) * sizeof(CARDRomRegion));
                }
            }
            align_len[i] = ((r_len + 31) & ~31);
            size += (u32)(sizeof(u32) + align_len[i]);
        }
        /* dynamically allocate the necessary size amount, and set the base offset at the start*/
        mem = (u8 *)WFSi_Alloc((int)size);
        SDK_ASSERT(mem);
        *(u32 *)mem = child_base_offset;
    }
    /*
     Load OVT/FAT/FNT.
     */
    {
        u8     *dst = mem + sizeof(u32);
        /* read each region*/
        for (i = 0; i < region_max; ++i)
        {
            table_info[i] = (TableInfo *) dst;
            table_info[i]->len = r[i].length;
            (void)FS_SeekFile(rom_file, (int)(child_base_offset + r[i].offset), FS_SEEK_SET);
            (void)FS_ReadFile(rom_file, table_info[i]->data, (int)table_info[i]->len);
            dst += sizeof(u32) + align_len[i];
        }
    }
    /*
     Depending on settings, appropriately revise the OVT File-ID and add FAT.
     *    (Special process needed only for file shared child devices)
     */
    {
        if (is_mixed_fat)
        {
            typedef struct
            {
                u32     top, bottom;
            }
            FatInfo;
            /* parent device -side FAT (that is ultimately adopted on the child device)*/
            FatInfo *const p_fat = (FatInfo *) table_info[region_fat]->data;
            int     fat_num = (int)(table_info[region_fat]->len / sizeof(FatInfo));
            /* Child device -side FAT position (that must convert the OVT offset)*/
            const u32 child_fat_base =
                (u32)(rom_base_offset + ((const CARDRomRegion *)(rom_header + 0x48))->offset);
            /* Perform the conversion process for the ARM9/ARM7 OVT*/
            for (i = region_ovt9; i < region_max; ++i)
            {
                FSOverlayInfoHeader *const p_ovt = (FSOverlayInfoHeader *)table_info[i]->data;
                const int ovt_num = (int)(table_info[i]->len / sizeof(FSOverlayInfoHeader));
                int     j;
                for (j = 0; j < ovt_num; ++j)
                {
                    /* To begin with, obtain the FAT of the relevant file from the child device OVT*/
                    (void)FS_SeekFile(rom_file,
                                      (int)(child_fat_base + p_ovt[j].file_id * sizeof(FatInfo)),
                                      FS_SEEK_SET);
                    (void)FS_ReadFile(rom_file, &p_fat[fat_num], sizeof(FatInfo));
                    /* convert that to the parent device base ROM offset*/
                    p_fat[fat_num].top += rom_base_offset;
                    p_fat[fat_num].bottom += rom_base_offset;
                    /* Revise the File-ID in OVT and that which was newly added*/
                    p_ovt[j].file_id = (u32)fat_num;
                    ++fat_num;
                }
            }
            /* Show the conversion results, and the total number of FATs increased for OVT integration*/
            table_info[region_fat]->len = (u32)(fat_num * sizeof(FatInfo));
        }
    }
    /*
     * In the above, table creation is completed for each of (1) - (3).
     * By going ahead and performing this complicated adjustment on the parent side,
     * the child device will resolve anything other than the ReadFile as table direct access,
     * and will request the ReadFile as base_offset + position.
     * On the parent side, that request will be evaluated as simply a ReadCard on ROM.
     */

    DC_FlushRange(mem, size);
    DC_WaitWriteBufferEmpty();
    (void)FS_CloseFile(rom_file);
    wfsi_work->table = mem;
    wfsi_work->table_size = size;
}

/*
 * Restructure ROM archive and switch overlay table on the child device
 * Returns base offset of child ROM address for parent.
 */
u32 WFSi_ReplaceRomArchive(void *p_table)
{
    u32     ret;
    CARDRomRegion r[region_max];

    FSArchive *const p_rom = FS_FindArchive("rom", 3);

    int     i;

    /* since there is no card from the start, the unload immediately completes*/
    (void)FS_UnloadArchive(p_rom);

    /* replace the user procedure*/
    FS_SetArchiveProc(p_rom, WFSi_RomArchiveProc, (u32)FS_ARCHIVE_PROC_ALL);

    /* get base offset*/
    ret = *(u32 *)p_table;
    p_table = ((u8 *)p_table) + sizeof(u32);

    /* extract information about each region from a given buffer*/
    for (i = 0; i < region_max; ++i)
    {
        r[i].offset = (u32)((u32)p_table + sizeof(u32));
        r[i].length = *(u32 *)p_table;
        p_table = (void *)(r[i].offset + ((r[i].length + 31) & ~31));
    }

    /* use the extracted region to reload the archive*/
    (void)FS_LoadArchive(p_rom, 0,
                         r[region_fat].offset, r[region_fat].length,
                         r[region_fnt].offset, r[region_fnt].length,
                         WFSi_ReadRomCallback, WFSi_WriteRomCallback);

    /* replace the OVT*/
    FS_AttachOverlayTable(MI_PROCESSOR_ARM9,
                          (const void *)r[region_ovt9].offset, r[region_ovt9].length);
    FS_AttachOverlayTable(MI_PROCESSOR_ARM7,
                          (const void *)r[region_ovt7].offset, r[region_ovt7].length);

    return ret;
}
