/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - include
  File:     file.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: myrom.h,v $
  Revision 1.4  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.3  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.2  02/28/2005 05:26:27  yosizaki
  do-indent.

  Revision 1.1  07/09/2004 00:59:51  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#if	!defined(NITRO_FS_DEMO_ARC_1_MYROM_H_)
#define NITRO_FS_DEMO_ARC_1_MYROM_H_

#include <nitro.h>
#include <nitro/fs.h>
#include <nitro/card.h>


#ifdef __cplusplus
extern "C" {
#endif


/* duplicate ROM archive structure*/
typedef struct MyRomArchive
{
    /* main body of the archive*/
    FSArchive arc[1];
    /* DMA channel to use*/
    u32     default_dma_no;
    /* lock ID of the card/cartridge*/
    u32     card_lock_id;

}
MyRomArchive;


/* initialization of duplicate ROM archive*/
void    MyRom_Init(MyRomArchive * p_rom);

/* DMA designation and registration / loading of archive*/
BOOL    MyRom_Create(MyRomArchive * p_rom,
                     const char *name, int name_len,
                     u32 base, const CARDRomRegion *fnt, const CARDRomRegion *fat);

/* get current default DMA*/
u32     MyRom_GetDefaultDMA(const MyRomArchive * p_rom);

/* configure default DMA*/
u32     MyRom_SetDefaultDMA(MyRomArchive * p_rom, u32 dma_no);


#ifdef __cplusplus
} /* extern "C"*/
#endif


#endif /* NITRO_FS_FILE_H_*/
