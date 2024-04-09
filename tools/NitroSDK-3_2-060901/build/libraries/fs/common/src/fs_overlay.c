/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - libraries
  File:     fs_overlay.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fs_overlay.c,v $
  Revision 1.38  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.37  08/22/2005 09:45:32  yosizaki
  fix about FS_EndOverlay.

  Revision 1.36  07/20/2005 04:28:57  yosizaki
  change implementation of FS_EndOverlay().

  Revision 1.35  07/14/2005 11:20:28  yosizaki
  fix FS_EndOverlay().

  Revision 1.34  05/30/2005 04:19:42  yosizaki
  add comments.

  Revision 1.33  04/26/2005 02:38:44  yosizaki
  change around overlay functions (not notify to the debugger on FINALROM)

  Revision 1.32  04/15/2005 08:42:58  yosizaki
  fix assertion macros format. (remove ';' )

  Revision 1.31  03/28/2005 23:52:06  seiki_masashi
  dgt/dgt.h include

  Revision 1.30  03/01/2005 01:16:03  yosizaki
    fix around check routine in FS_LoadOverlayImage*.

  Revision 1.29  02/28/2005 05:26:13  yosizaki
  do-indent.

  Revision 1.28  02/24/2005 11:49:13  yosizaki
  fix FS_LoadOverlayImage*

  Revision 1.27  2005/02/18 07:53:19  yasu
  Signed/Unsigned conversion warning control

  Revision 1.26  2005/02/18 07:26:49  yasu
  Signed/Unsigned conversion warning control

  Revision 1.25  11/17/2004 06:02:43  yosizaki
  delete implement of FS_RegisterOverlayToDebugger.

  Revision 1.24  11/16/2004 06:01:10  yosizaki
  add FS_LoadOverlayImageAsync.

  Revision 1.23  11/09/2004 12:01:53  yosizaki
  fix SDK_OVERLAY_DIGEST_NUMBER to SDK_OVERLAY_DIGEST_END.

  Revision 1.22  2004/11/09 03:11:05  yasu
  Support FS_AttachOverlayTable on TEG build

  Revision 1.21  11/08/2004 04:29:27  yosizaki
  fix FS_AttachOverlayTable. (in the case ptr == NULL)

  Revision 1.20  11/08/2004 04:08:52  yosizaki
  add FSi_CompareOverlayDigest. (from FS_StartOverlay)
  add assertion check in FS_AttachOverlayTable.

  Revision 1.19  11/05/2004 13:09:26  yosizaki
  add assertion check in FS_StartOverlay().

  Revision 1.18  2004/11/04 11:50:38  yasu
  Don't start overlay if no digest

  Revision 1.17  10/18/2004 13:50:46  yosizaki
  add FS_RegisterOverlayToDebugger.

  Revision 1.16  2004/10/07 12:21:40  yasu
  Set default keycode

  Revision 1.15  09/26/2004 23:51:30  miya
  disable digest-calc. on ARM7

  Revision 1.14  09/17/2004 06:49:20  yosizaki
  fix around commpressed overlay.

  Revision 1.13  2004/09/08 09:19:56  yasu
  Don't check digest if TEG

  Revision 1.12  09/08/2004 00:05:27  yosizaki
  enable digest-calculation.

  Revision 1.11  09/07/2004 06:36:37  yosizaki
  add umcomp-routine.

  Revision 1.10  09/04/2004 13:21:32  yosizaki
  support multiboot-overlay.

  Revision 1.9  09/02/2004 11:03:00  yosizaki
  fix FS include directives.

  Revision 1.8  08/02/2004 03:46:46  yosizaki
  fix locking bug in ARM7-overlay ( OS_LockCard -> CARD_LockRom )

  Revision 1.7  07/23/2004 01:05:34  yada
  type of lockID for lock functions becomes u16

  Revision 1.6  07/19/2004 13:19:11  yosizaki
  support IS-NITRO-DEBUGGER overlay.

  Revision 1.5  07/13/2004 08:50:53  yosizaki
  fix static-uninitalizer routine.

  Revision 1.4  07/13/2004 08:11:29  yosizaki
  erase debug-printf...

  Revision 1.3  07/13/2004 07:59:54  yosizaki
  change overlay-destructor routine.

  Revision 1.2  07/13/2004 02:54:05  yosizaki
  add argument of FS_OpenFileDirect.

  Revision 1.1  07/08/2004 13:41:00  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/os.h>
#include <nitro/mi.h>

#include <nitro/fs.h>

#include <nitro/mb.h>
#include <nitro/dgt/common.h>
#include <nitro/dgt/dgt.h>


#include "../include/util.h"
#include "../include/rom.h"

#include "../include/mw_dtor.h"

typedef enum
{
    OVERLAYPROC_ARM9,
    OVERLAYPROC_ARM7
}
OVERLAYPROC;

int     _ISDbgLib_RegistOverlayInfo(OVERLAYPROC nProc, u32 nAddrRAM, u32 nAddrROM, u32 nSize);
int     _ISDbgLib_UnregistOverlayInfo(OVERLAYPROC nProc, u32 nAddrRAM, u32 nSize);


#define FS_OVERLAY_FLAG_COMP    0x0001
#define FS_OVERLAY_FLAG_AUTH    0x0002
#define FS_OVERLAY_DIGEST_SIZE  DGT_HASH2_DIGEST_SIZE


#if defined(SDK_TS)
extern u8 SDK_OVERLAYTABLE_DIGEST[];
extern u8 SDK_OVERLAY_DIGEST[];
extern u8 SDK_OVERLAY_DIGEST_END[];
#endif

static const u8 fsi_def_digest_key[64] = {      // same values as compstatic
    0x21, 0x06, 0xc0, 0xde,
    0xba, 0x98, 0xce, 0x3f,
    0xa6, 0x92, 0xe3, 0x9d,
    0x46, 0xf2, 0xed, 0x01,

    0x76, 0xe3, 0xcc, 0x08,
    0x56, 0x23, 0x63, 0xfa,
    0xca, 0xd4, 0xec, 0xdf,
    0x9a, 0x62, 0x78, 0x34,

    0x8f, 0x6d, 0x63, 0x3c,
    0xfe, 0x22, 0xca, 0x92,
    0x20, 0x88, 0x97, 0x23,
    0xd2, 0xcf, 0xae, 0xc2,

    0x32, 0x67, 0x8d, 0xfe,
    0xca, 0x83, 0x64, 0x98,
    0xac, 0xfd, 0x3e, 0x37,
    0x87, 0x46, 0x58, 0x24,
};

static const void *fsi_digest_key_ptr = fsi_def_digest_key;
static int fsi_digest_key_len = sizeof(fsi_def_digest_key);

/********************************************************************/
/* Constant*/

/* Symbol indicating whether it is not "TEG -version ARM9" (Card access is possible)*/
#if !defined(SDK_TEG) || !defined(SDK_ARM9)
#define FS_NEED_CARD    1
#endif


/********************************************************************/
/* functions*/

/*---------------------------------------------------------------------------*
  Name:         FSi_GetOverlayBinarySize

  Description:  get file-size which contains an image of overlay module

  Arguments:    p_ovi       pointer to FSOverlayInfo

  Returns:      file-size of overlay.
 *---------------------------------------------------------------------------*/
static u32 FSi_GetOverlayBinarySize(const FSOverlayInfo *p_ovi)
{
    u32     size = (((p_ovi->header.flag & FS_OVERLAY_FLAG_COMP) != 0)
                    ? p_ovi->header.compressed : p_ovi->header.ram_size);
    return size;
}

/*---------------------------------------------------------------------------*
  Name:         FS_ClearOverlayImage

  Description:  0-clear the memory where FS_GetOverlayAddress() points,
                and invalidate its region of cache.
                this function is called in FS_LoadOverlayImage().

  Arguments:    p_ovi       pointer to FSOverlayInfo

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_ClearOverlayImage(FSOverlayInfo *p_ovi)
{
    u8     *const addr = FS_GetOverlayAddress(p_ovi);
    const u32 image_size = FS_GetOverlayImageSize(p_ovi);
    const u32 total_size = FS_GetOverlayTotalSize(p_ovi);
#if defined(SDK_ARM9)
    IC_InvalidateRange(addr, total_size);
    DC_InvalidateRange(addr, total_size);
#endif
    MI_CpuFill8(addr + image_size, 0, total_size - image_size);
}


#if defined(FS_IMPLEMENT)


/*---------------------------------------------------------------------------*
  Name:         FS_GetOverlayFileID

  Description:  get file-id which contains an image of overlay module

  Arguments:    p_ovi       pointer to FSOverlayInfo

  Returns:      file-id od overlay.
 *---------------------------------------------------------------------------*/
FSFileID FS_GetOverlayFileID(const FSOverlayInfo *p_ovi)
{
    FSFileID ret;
    ret.arc = &fsi_arc_rom;
    ret.file_id = p_ovi->header.file_id;
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_LoadOverlayInfoCore

  Description:  Internal function.
                Loads overlay information from specified rom-duplicated archive.
                Unannounced release for convenience of IPL package.

  Arguments:    p_ovi            Pointer that stores information that was read.
                target           Overlay target. (ARM9/ARM7)
                id               Overlay ID to read.
                p_arc            Archive to read to.
                offset_arm9      Start of overlay info storage position in archive.
                len_arm9         Size of overlay info in archive.
                offset_arm7      Start of overlay info storage position in archive.
                len_arm7         Size of overlay info in archive.

  Returns:      If overlay is read correctly, TRUE.
 *---------------------------------------------------------------------------*/
BOOL FSi_LoadOverlayInfoCore(FSOverlayInfo *p_ovi, MIProcessor target, FSOverlayID id,
                             FSArchive *p_arc,
                             u32 offset_arm9, u32 len_arm9, u32 offset_arm7, u32 len_arm7)
{
    CARDRomRegion pr[1];
    u32     pos;

    if (target == MI_PROCESSOR_ARM9)
    {
        pr->offset = offset_arm9;
        pr->length = len_arm9;
    }
    else
    {
        pr->offset = offset_arm7;
        pr->length = len_arm7;
    }

    pos = (u32)id *sizeof(FSOverlayInfoHeader);
    if (pos >= pr->length)
    {
        return FALSE;
    }
    else
    {
        FSFile  file[1];
        FS_InitFile(file);
        if (!FS_OpenFileDirect(file, p_arc, pr->offset + pos, pr->offset + pr->length, (u32)~0))
        {
            return FALSE;
        }
        else
        {
            if (FS_ReadFile(file, p_ovi, sizeof(FSOverlayInfoHeader)) !=
                sizeof(FSOverlayInfoHeader))
            {
                (void)FS_CloseFile(file);
                return FALSE;
            }
            (void)FS_CloseFile(file);
            p_ovi->target = target;
            /* Also when located in CARD-ROM, from here read from FAT*/
            if (!FS_OpenFileFast(file, FS_GetOverlayFileID(p_ovi)))
            {
                return FALSE;
            }
            else
            {
                p_ovi->file_pos.offset = FS_GetFileImageTop(file);
                p_ovi->file_pos.length = FS_GetLength(file);
                (void)FS_CloseFile(file);
            }
        }
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         FS_LoadOverlayInfo

  Description:  load the information of specified overlay module

  Arguments:    p_ovi       pointer to destination FSOverlayInfo
                target      ARM9 or ARM7
                id          overlay-id (FS_OVERLAY_ID(overlay-name))

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_LoadOverlayInfo(FSOverlayInfo *p_ovi, MIProcessor target, FSOverlayID id)
{
    /* if OVT is attached, read it*/
    CARDRomRegion *const pr = (target == MI_PROCESSOR_ARM9) ? &fsi_ovt9 : &fsi_ovt7;

    if (pr->offset)
    {
        const u32 pos = (u32)id * sizeof(FSOverlayInfoHeader);
        if (pos >= pr->length)
        {
            return FALSE;
        }
        else
        {
            MI_CpuCopy8((const void *)(pr->offset + pos), p_ovi, sizeof(FSOverlayInfoHeader));
            p_ovi->target = target;
            /* Also when located in CARD-ROM, from here read from FAT*/
            {
                FSFile  file[1];
                FS_InitFile(file);
                if (!FS_OpenFileFast(file, FS_GetOverlayFileID(p_ovi)))
                {
                    return FALSE;
                }
                else
                {
                    p_ovi->file_pos.offset = FS_GetFileImageTop(file);
                    p_ovi->file_pos.length = FS_GetLength(file);
                    (void)FS_CloseFile(file);
                }
            }
            return TRUE;
        }
    }
    /* else, read from default ROM*/
    else
    {
        const CARDRomRegion *const p_ovt9 = CARD_GetRomRegionOVT(MI_PROCESSOR_ARM9);
        const CARDRomRegion *const p_ovt7 = CARD_GetRomRegionOVT(MI_PROCESSOR_ARM7);
        return FSi_LoadOverlayInfoCore(p_ovi, target, id,
                                       &fsi_arc_rom,
                                       p_ovt9->offset, p_ovt9->length,
                                       p_ovt7->offset, p_ovt7->length);
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_LoadOverlayImageAsync

  Description:  load the image of overlay module without 'static initializer'.
                by this call, the memory where FS_GetOverlayAddress() points
                is set to rare initial status.

  Arguments:    p_ovi       pointer to FSOverlayInfo
                p_file      pointer to FSFile for asynchronous reading

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_LoadOverlayImageAsync(FSOverlayInfo *p_ovi, FSFile *p_file)
{
    FS_InitFile(p_file);
    if (!FS_OpenFileFast(p_file, FS_GetOverlayFileID(p_ovi)))
    {
        return FALSE;
    }
    else
    {
        s32     size = (s32)FSi_GetOverlayBinarySize(p_ovi);
        FS_ClearOverlayImage(p_ovi);
        if (FS_ReadFileAsync(p_file, FS_GetOverlayAddress(p_ovi), size) != size)
        {
            (void)FS_CloseFile(p_file);
            return FALSE;
        }
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         FS_LoadOverlayImage

  Description:  load the image of overlay module without 'static initializer'.
                by this call, the memory where FS_GetOverlayAddress() points
                is set to rare initial status.

  Arguments:    p_ovi       pointer to FSOverlayInfo

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_LoadOverlayImage(FSOverlayInfo *p_ovi)
{
    FSFile  p_file[1];

    FS_InitFile(p_file);
    if (!FS_OpenFileFast(p_file, FS_GetOverlayFileID(p_ovi)))
    {
        return FALSE;
    }
    else
    {
        s32     size = (s32)FSi_GetOverlayBinarySize(p_ovi);
        FS_ClearOverlayImage(p_ovi);
        if (FS_ReadFile(p_file, FS_GetOverlayAddress(p_ovi), size) != size)
        {
            (void)FS_CloseFile(p_file);
            return FALSE;
        }
    }
    (void)FS_CloseFile(p_file);
    return TRUE;
}

#endif

/*---------------------------------------------------------------------------*
  Name:         FSi_CompareOverlayDigest

  Description:  Compares overlay digest values.

  Arguments:    spec_digest       compare source digest value
                src               compare target data
                len               src byte size

  Returns:      TRUE if comparison matches. Otherwise, FALSE.
 *---------------------------------------------------------------------------*/
static BOOL FSi_CompareDigest(const u8 *spec_digest, void *src, int len)
{
    int     i;
    u8      digest[FS_OVERLAY_DIGEST_SIZE];
    u8      digest_key[64];

    MI_CpuClear8(digest, sizeof(digest));
    MI_CpuCopy8(fsi_digest_key_ptr, digest_key, (u32)fsi_digest_key_len);
    DGT_Hash2CalcHmac(digest, src, len, digest_key, fsi_digest_key_len);
    for (i = 0; i < sizeof(digest); i += sizeof(u32))
    {
        if (*(const u32 *)(digest + i) != *(const u32 *)(spec_digest + i))
            break;
    }
    return (i == sizeof(digest));
}


/*---------------------------------------------------------------------------*
  Name:         FS_StartOverlay

  Description:  execute 'static initializer'.
                this function needs:
                    the memory where FS_GetOverlayAddress() points is
                    set by data which FS_LoadOverlayImage() makes.

  Arguments:    p_ovi       pointer to FSOverlayInfo

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_StartOverlay(FSOverlayInfo *p_ovi)
{
    u32     rare_size = FSi_GetOverlayBinarySize(p_ovi);

#ifndef  SDK_TEG
    /* If child and authentication fails, image is invalidated*/
    if (MB_IsMultiBootChild())
    {
        BOOL    ret = FALSE;

        if ((p_ovi->header.flag & FS_OVERLAY_FLAG_AUTH) != 0)
        {
            const u32 odt_max =
                (u32)((SDK_OVERLAY_DIGEST_END - SDK_OVERLAY_DIGEST) / FS_OVERLAY_DIGEST_SIZE);
            if (p_ovi->header.id < odt_max)
            {
                const u8 *spec_digest = (SDK_OVERLAY_DIGEST +
                                         FS_OVERLAY_DIGEST_SIZE * p_ovi->header.id);
                ret = FSi_CompareDigest(spec_digest, p_ovi->header.ram_address, (int)rare_size);
            }
        }
        if (!ret)
        {
            MI_CpuClear8(p_ovi->header.ram_address, rare_size);
            OS_TPanic("FS_StartOverlay() failed! (invalid overlay-segment data)");
            return;
        }
    }
#endif

#ifndef SDK_ARM7
    /* If data is compressed, uncompress it*/
    if ((p_ovi->header.flag & FS_OVERLAY_FLAG_COMP) != 0)
    {
        MIi_UncompressBackward(p_ovi->header.ram_address + rare_size);
    }
    DC_FlushRange(FS_GetOverlayAddress(p_ovi), FS_GetOverlayImageSize(p_ovi));
#endif

#if !defined(SDK_FINALROM)
    /*
     * All segments become traceable by debugger from this moment.
     * Processing performed with FS_RegisterOverlayToDebugger()
     * must be accurately called from this location only.
     */
    (void)_ISDbgLib_RegistOverlayInfo((p_ovi->target == MI_PROCESSOR_ARM9)
                                      ? OVERLAYPROC_ARM9 : OVERLAYPROC_ARM7,
                                      (u32)FS_GetOverlayAddress(p_ovi),
                                      p_ovi->file_pos.offset, FS_GetOverlayImageSize(p_ovi));
#endif

    {                                  /*
                                        * start static initializer.
                                        */
        FSOverlayInitFunc *p = p_ovi->header.sinit_init;
        FSOverlayInitFunc *q = p_ovi->header.sinit_init_end;
        for (; p < q; ++p)
        {
            if (*p)
                (**p) ();
        }
    }

}

/*---------------------------------------------------------------------------*
  Name:         FS_EndOverlay

  Description:  execute 'static initializer'.
                by this function:
                    all the destructors in specified overlay module
                    are removed from global destructor chain and executed.

  Arguments:    p_ovi       pointer to FSOverlayInfo

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_EndOverlay(FSOverlayInfo *p_ovi)
{
    for (;;)
    {
        /*
         * First, extract the relevant dtor from the global chain.
         * Check to see if instance is in overlay range.
         * Concerning location of dtor, only when obj = NULL
         * is it subject to range checking (In 2.2 and later)
         */
        MWiDestructorChain *head = NULL, *tail = NULL;
        const u32 region_top = (u32)FS_GetOverlayAddress(p_ovi);
        const u32 region_bottom = region_top + FS_GetOverlayTotalSize(p_ovi);

        {
            OSIntrMode bak_psr = OS_DisableInterrupts();
            MWiDestructorChain *prev = NULL;
            MWiDestructorChain *base = __global_destructor_chain;
            MWiDestructorChain *p = base;
            while (p)
            {
                MWiDestructorChain *next = p->next;
                const u32 dtor = (u32)p->dtor;
                const u32 obj = (u32)p->obj;
                if (((obj == 0) && (dtor >= region_top) && (dtor < region_bottom)) ||
                    ((obj >= region_top) && (obj < region_bottom)))
                {
                    /* If appropriate, extract*/
                    if (!tail)
                    {
                        head = p;
                    }
                    else
                    {
                        tail->next = p;
                    }
                    if (base == p)
                    {
                        base = __global_destructor_chain = next;
                    }
                    tail = p, p->next = NULL;
                    if (prev)
                    {
                        prev->next = next;
                    }
                }
                else
                {
                    /* If not, go to next step*/
                    prev = p;
                }
                p = next;
            }
            (void)OS_RestoreInterrupts(bak_psr);
        }

        /* If not even one dtor, quit*/
        if (!head)
        {
            break;
        }
        /* If one or more dtor, execute from the top and destroy*/
        do
        {
            MWiDestructorChain *next = head->next;
            if (head->dtor)
            {
                (*head->dtor) (head->obj);
            }
            head = next;
        }
        while (head);

        /*
         * If the dtor is for a local-static-object that is accessed for the first time,
         * at this stage another addition will be made to the global chain.
         * This kind of lifespan control is somewhat irregular in C++,
         * but supporting it should not cause any problems.
         * Repeat test extraction as much as possible.
         */
    }

#if !defined(SDK_FINALROM)
    (void)_ISDbgLib_UnregistOverlayInfo((p_ovi->target ==
                                         MI_PROCESSOR_ARM9) ? OVERLAYPROC_ARM9 : OVERLAYPROC_ARM7,
                                        (u32)FS_GetOverlayAddress(p_ovi),
                                        FS_GetOverlayImageSize(p_ovi));
#endif

}


/*---------------------------------------------------------------------------*
  Name:         FS_UnloadOverlayImage

  Description:  unload overlay module

  Arguments:    p_ovi       pointer to FSOverlayInfo

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_UnloadOverlayImage(FSOverlayInfo *p_ovi)
{
    FS_EndOverlay(p_ovi);
    return TRUE;
}


#if defined(FS_IMPLEMENT)


/*---------------------------------------------------------------------------*
  Name:         FS_LoadOverlay

  Description:  load overlay module and initialize

  Arguments:    target      ARM9 or ARM7
                id          overlay-id (FS_OVERLAY_ID(overlay-name))

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_LoadOverlay(MIProcessor target, FSOverlayID id)
{
    FS_ASSERT_INIT(FALSE);

    {
        FSOverlayInfo ovi;
        if (!FS_LoadOverlayInfo(&ovi, target, id) || !FS_LoadOverlayImage(&ovi))
            return FALSE;
        FS_StartOverlay(&ovi);
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         FS_UnloadOverlay

  Description:  unload overlay module and clean up

  Arguments:    target      ARM9 or ARM7
                id          overlay-id (FS_OVERLAY_ID(overlay-name))

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_UnloadOverlay(MIProcessor target, FSOverlayID id)
{
    FS_ASSERT_INIT(FALSE);

    {
        FSOverlayInfo ovi;
        if (!FS_LoadOverlayInfo(&ovi, target, id) || !FS_UnloadOverlayImage(&ovi))
            return FALSE;
    }
    return TRUE;
}


#else  /* !FS_IMPLEMENT*/


/* Directly read specified address*/
static void FSi_ReadRomDirect(const void *src, void *dst, u32 len)
{
#if !defined(SDK_TS)
    if (src >= (const void *)HW_CTRDG_ROM)
    {
        // Cartridge.
        (void)OS_LockCartridge((u16)fsi_card_lock_id);
        MI_CpuCopy32(src, dst, len);
        (void)OS_UnlockCartridge((u16)fsi_card_lock_id);
    }
    else
#endif
    {
        // Card.
        (void)CARD_LockRom((u16)fsi_card_lock_id);
        CARD_ReadRom(FS_DMA_NOT_USE, src, dst, len);
        (void)CARD_UnlockRom((u16)fsi_card_lock_id);
    }
}


/*---------------------------------------------------------------------------*
  Name:         FS_LoadOverlay

  Description:  load overlay module and initialize

  Arguments:    target      ARM9 or ARM7
                id          overlay-id (FS_OVERLAY_ID(overlay-name))

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_LoadOverlay(MIProcessor target, FSOverlayID id)
{
    FS_ASSERT_INIT(FALSE);

    {
        /* ROM base address*/
        const u8 *const base = (const u8 *)
            (((OS_GetConsoleType() & OS_CONSOLE_DEV_CARD) != 0) ? 0x00000000 : HW_CTRDG_ROM);
        /* FAT table*/
        const CARDRomRegion *fat = CARD_GetRomRegionFAT();
        /* OVT offset*/
        const CARDRomRegion *ovt = CARD_GetRomRegionOVT(target);

        FSOverlayInfo ovi;
        FSArchiveFAT fat_info;

        u32     pos, index;

        /* Directly read OVT*/
        index = (u32)id;
        pos = index * sizeof(FSOverlayInfoHeader);
        if (pos >= ovt->length)
            return FALSE;
        FSi_ReadRomDirect(base + ovt->offset + pos, &ovi, sizeof(FSOverlayInfoHeader));
        ovi.target = target;

        /* Get image region from FAT*/
        index = ovi.header.file_id;
        pos = index * sizeof(FSArchiveFAT);
        if (pos >= fat->length)
            return FALSE;
        FSi_ReadRomDirect(base + fat->offset + pos, &fat_info, sizeof(FSArchiveFAT));

        /* Load here*/
        FS_ClearOverlayImage(&ovi);
        FSi_ReadRomDirect(base + fat_info.top,
                          ovi.header.ram_address, fat_info.bottom - fat_info.top);

        (void)_ISDbgLib_RegistOverlayInfo((ovi.target ==
                                           MI_PROCESSOR_ARM9) ? OVERLAYPROC_ARM9 : OVERLAYPROC_ARM7,
                                          (u32)FS_GetOverlayAddress(&ovi),
                                          (u32)(base + fat_info.top), FS_GetOverlayImageSize(&ovi));

        FS_StartOverlay(&ovi);
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         FS_UnloadOverlay

  Description:  unload overlay module and clean up

  Arguments:    target      ARM9 or ARM7
                id          overlay-id (FS_OVERLAY_ID(overlay-name))

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_UnloadOverlay(MIProcessor target, FSOverlayID id)
{
    FS_ASSERT_INIT(FALSE);
    (void)target;
    (void)id;
    return TRUE;
}

#endif /* FS_IMPLEMENT*/


/*---------------------------------------------------------------------------*
  Name:         FS_AttachOverlayTable

  Description:  attach the overlayinfo-table (OVT) to file-system.
                after setting, FS_LoadOverlayInfo()
                loads overlay-info from specified memory.

  Arguments:    target      ARM9 or ARM7
                ptr         pointer to buffer which contains OVT.
                            if ptr is NULL, reset to default(from CARD).
                len         length of OVT.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_AttachOverlayTable(MIProcessor target, const void *ptr, u32 len)
{
#ifdef	SDK_TS
    if ((ptr != NULL) && (target == MI_PROCESSOR_ARM9))
    {                                  /* Overlay table must be own*/
        int     bak_ovt_mode = DGT_SetOverlayTableMode(TRUE);
        if (!FSi_CompareDigest((const u8 *)SDK_OVERLAYTABLE_DIGEST, (void *)ptr, (int)len))
        {
            OS_TPanic("FS_AttachOverlayTable() failed! (invalid overlay-table data)");
        }
        (void)DGT_SetOverlayTableMode(bak_ovt_mode);
    }
#endif

    {
        CARDRomRegion *const pr = (target == MI_PROCESSOR_ARM9) ? &fsi_ovt9 : &fsi_ovt7;
        OSIntrMode bak_psr = OS_DisableInterrupts();

        pr->offset = (u32)ptr;
        pr->length = len;

        (void)OS_RestoreInterrupts(bak_psr);
    }
}
