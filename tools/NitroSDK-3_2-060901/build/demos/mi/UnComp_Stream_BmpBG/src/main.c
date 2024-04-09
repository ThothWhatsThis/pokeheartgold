/*---------------------------------------------------------------------------*
  Project:  NitroSDK - demos - MI - UnComp_Stream_BmpBG
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.16  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.15  11/21/2005 10:53:27  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.14  06/27/2005 11:10:12  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.13  02/28/2005 05:26:19  yosizaki
  do-indent.

  Revision 1.12  02/18/2005 13:47:49  seiki_masashi
  warning measures

  Revision 1.11  01/18/2005 00:20:07  takano_makoto
  fix Copyright header.

  Revision 1.10  01/14/2005 06:03:23  takano_makoto
  deleted an unnecessary cache clear

  Revision 1.9  12/08/2004 05:28:01  takano_makoto
  Buffer for uncompression was 32-byte aligned

  Revision 1.8  12/08/2004 04:29:00  takano_makoto
  Load position for asynchronous reading of files was 512-byte aligned

  Revision 1.7  12/07/2004 12:29:31  takano_makoto
  Made correction of cache is flushed

  Revision 1.6  12/01/2004 08:11:12  takano_makoto
  Added sample of asynchronous reading of CARD

  Revision 1.5  12/01/2004 00:02:27  takano_makoto
  Function name change: MI_UncompReadXX -> MI_ReadUncompXX

  Revision 1.4  11/30/2004 14:07:39  takano_makoto
  Added sample that extracts directly in VRAM

  Revision 1.3  11/30/2004 08:36:04  takano_makoto
  Changed name of MI_InitUncompContextHuff to MI_InitUncompContextHuffman

  Revision 1.2  11/30/2004 08:01:37  takano_makoto
  small fix in comment.

  Revision 1.1  11/30/2004 07:59:58  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// A sample that displays image uncompressed by streaming:
//
// If display mode is VRAM display mode, a bitmap image on VRAM is displayed.
// This sample loads different four images compressed different format to VRAM A-D,
// and displays them by turns.
//
//---------------------------------------------------------------------------

#include <nitro.h>
#include <nitro/mi/uncomp_stream.h>
#include "DEMO.h"

#define TEMP_BUF_SIZE   512

static u8 *data_uncomp_buf;
static u8 doubleBuf[2][TEMP_BUF_SIZE] ATTRIBUTE_ALIGN(32);

static void InitAlloc(void)
{
    void   *tempLo;
    OSHeapHandle hh;

    tempLo = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
    OS_SetArenaLo(OS_ARENA_MAIN, tempLo);
    hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
    if (hh < 0)
    {
        OS_Panic("ARM9: Fail to create heap...\n");
    }
    hh = OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
}


void NitroMain(void)
{
    int     vram_slot = 0, count = 0;
    u8     *tmpBuf = doubleBuf[0];

    //---------------------------------------------------------------------------
    // Initialize:
    // They enable IRQ interrupts, and initialize VRAM.
    //---------------------------------------------------------------------------
    DEMOInitCommon();
    DEMOInitVRAM();
    InitAlloc();
    FS_Init(3);
    OS_InitTick();

    //---------------------------------------------------------------------------
    // Map VRAM bank A-D onto LCDC.
    //---------------------------------------------------------------------------
    GX_SetBankForLCDC(GX_VRAM_LCDC_A | GX_VRAM_LCDC_B | GX_VRAM_LCDC_C | GX_VRAM_LCDC_D);

    //---------------------------------------------------------------------------
    // Download images
    //---------------------------------------------------------------------------

    //===========================
    // Uncompressed data
    //===========================
    {
        FSFile  file;
        u32     read_len;

        // Open the file
        FS_InitFile(&file);
        if (!FS_OpenFile(&file, "/data/image1.bin"))
        {
            OS_TPrintf(" Open File /data/image1.bin fail\n");
            return;
        }
        // Secure a region for loading the data
        read_len = FS_GetLength(&file);
        data_uncomp_buf = (u8 *)OS_Alloc(read_len);

        // Transfer file
        (void)FS_ReadFile(&file, data_uncomp_buf, 256 * 192 * 2);

        // Transfer to VRAM
        MI_DmaCopy32(3, data_uncomp_buf, (void *)HW_LCDC_VRAM_A,
                     256 * 192 * sizeof(unsigned short));

        OS_Free(data_uncomp_buf);
        (void)FS_CloseFile(&file);

    }

    //===========================
    // Run-length compressed data
    //===========================
    {
        FSFile  file;
        MIUncompContextRL context;
        s32     read_len;
        u32     data_size;

        // Open the file
        FS_InitFile(&file);
        if (!FS_OpenFile(&file, "/data/image2_RL.bin"))
        {
            OS_TPrintf(" Open File /data/image2_RL.bin fail\n");
            return;
        }

        // Get the header part of the compressed data
        (void)FS_ReadFile(&file, tmpBuf, sizeof(MICompressionHeader));

        // Secure a region for uncompressed data
        data_size = MI_GetUncompressedSize(tmpBuf);
        data_uncomp_buf = (u8 *)OS_Alloc(data_size);

        // Initialization of the context
        MI_InitUncompContextRL(&context, data_uncomp_buf, (MICompressionHeader *)tmpBuf);

        while (1)
        {
            // Read an arbitrary size of data and perform streaming uncompression
            read_len = FS_ReadFile(&file, tmpBuf, TEMP_BUF_SIZE);
            if (read_len <= 0)
            {
                OS_TPanic("can't read file\n");
            }
            // If extracting in memory, use the high-speed 8bit-access function.
            if (MI_ReadUncompRL8(&context, tmpBuf, (u32)read_len) == 0)
            {
                break;
            }
        }

        // Transfer to VRAM
        DC_FlushRange(data_uncomp_buf, data_size);      // Flush the cache
        /* IO register is accessed using DMA operation, so cache Wait is not needed*/
        // DC_WaitWriteBufferEmpty();
        MI_DmaCopy32(3, data_uncomp_buf, (void *)HW_LCDC_VRAM_B,
                     256 * 192 * sizeof(unsigned short));

        OS_Free(data_uncomp_buf);
        (void)FS_CloseFile(&file);

    }

    //===========================
    // LZ77 compressed data -- Sample of direct uncompression to VRAM
    //===========================
    {
        FSFile  file;
        MIUncompContextLZ context;
        s32     read_len;

        // Open the file
        FS_InitFile(&file);
        if (!FS_OpenFile(&file, "/data/image3_LZ.bin"))
        {
            OS_TPrintf(" Open File /data/image3_LZ.bin fail\n");
            return;
        }

        // Get the header part of the compressed data
        (void)FS_ReadFile(&file, tmpBuf, sizeof(MICompressionHeader));

        // If extracting directly to VRAM, initialize the context and specify VRAM for the dest address
        MI_InitUncompContextLZ(&context, (void *)HW_LCDC_VRAM_C, (MICompressionHeader *)tmpBuf);

        while (1)
        {
            // Load an arbitrary size of data and perform streaming uncompression
            read_len = FS_ReadFile(&file, tmpBuf, TEMP_BUF_SIZE);
            if (read_len <= 0)
            {
                OS_TPanic("can't read file\n");
            }

            // If extracting directly to VRAM, use the 16bit-access function.
            if (MI_ReadUncompLZ16(&context, tmpBuf, (u32)read_len) == 0)
            {
                break;
            }
        }

        (void)FS_CloseFile(&file);
    }

    //===========================
    // Huffman compressed data -- Sample that executes extraction while reading data asynchronously from Card.
    //===========================
    {
        FSFile  file;
        MIUncompContextHuffman context;
        u32     data_size;
        s32     read_len[2];
        u8      read_select;
        u8      uncomp_select;

        // Open the file
        FS_InitFile(&file);
        if (!FS_OpenFile(&file, "/data/image4_HUFF.bin"))
        {
            OS_TPrintf(" Open File /data/image4_HUFF.bin fail\n");
            return;
        }

        // Get the header part of the compressed data
        (void)FS_ReadFile(&file, tmpBuf, sizeof(MICompressionHeader));

        // Secure a region for uncompressed data
        data_size = MI_GetUncompressedSize(tmpBuf);
        data_uncomp_buf = (u8 *)OS_Alloc(data_size);

        // Initialization of the context
        MI_InitUncompContextHuffman(&context, data_uncomp_buf, (MICompressionHeader *)tmpBuf);

        read_select = 0;
        uncomp_select = 1;
        read_len[0] = 0;
        read_len[1] = 0;

        // To read files asynchronously, the load location must be 512Byte aligned.
        // The 4byte header portion is loaded first, so the remaining 512 - 4Byte should be read.
        read_len[read_select] =
            FS_ReadFileAsync(&file, doubleBuf[read_select], 512 - sizeof(MICompressionHeader));

        /* Executes extraction while asynchronously reading from the card*/
        while (1)
        {
            // Switch double buffers
            read_select ^= 0x1;
            uncomp_select ^= 0x1;

            // Wait for end of reading of Card.
            (void)FS_WaitAsync(&file);

            // Read an arbitrary size of data and perform streaming uncompression
            read_len[read_select] = FS_ReadFileAsync(&file, doubleBuf[read_select], TEMP_BUF_SIZE);

            if (read_len[uncomp_select] == 0)
            {
                continue;
            }
            if (read_len[uncomp_select] == -1)
            {
                OS_TPanic("can't read file\n");
            }

            if (MI_ReadUncompHuffman
                (&context, doubleBuf[uncomp_select], (u32)read_len[uncomp_select]) == 0)
            {
                // Extraction of data completed
                break;
            }
        }

        // Transfer to VRAM
        DC_FlushRange(data_uncomp_buf, data_size);      // Flush the cache
        /* IO register is accessed using DMA operation, so cache Wait is not needed*/
        // DC_WaitWriteBufferEmpty();
        MI_DmaCopy32(3, data_uncomp_buf, (void *)HW_LCDC_VRAM_D,
                     256 * 192 * sizeof(unsigned short));

        OS_Free(data_uncomp_buf);
        (void)FS_CloseFile(&file);
    }

    //---------------------------------------------------------------------------
    // Set graphics mode VRAM display mode
    //---------------------------------------------------------------------------
    GX_SetGraphicsMode(GX_DISPMODE_VRAM_A,      // display VRAM-A
                       (GXBGMode)0,    // dummy
                       (GXBG0As)0);    // dummy

    DEMOStartDisplay();
    while (1)
    {

#ifdef SDK_AUTOTEST
        EXT_TestScreenShot(100, 0xEFA8B182);
        EXT_TestTickCounter();
#endif //SDK_AUTOTEST

        OS_WaitVBlankIntr();           // Waiting the end of V-Blank interrupt

        //---------------------------------------------------------------------------
        // Change the VRAM slot displayed every 90 frames
        //---------------------------------------------------------------------------
        if (count++ > 90)
        {
            vram_slot++;
            vram_slot &= 0x03;
            switch (vram_slot)
            {
            case 0:
                GX_SetGraphicsMode(GX_DISPMODE_VRAM_A,  // display VRAM-A
                                   (GXBGMode)0, // dummy
                                   (GXBG0As)0); // dummy
                break;
            case 1:
                GX_SetGraphicsMode(GX_DISPMODE_VRAM_B,  // display VRAM-B
                                   (GXBGMode)0, // dummy
                                   (GXBG0As)0); // dummy
                break;
            case 2:
                GX_SetGraphicsMode(GX_DISPMODE_VRAM_C,  // display VRAM-C
                                   (GXBGMode)0, // dummy
                                   (GXBG0As)0); // dummy
                break;
            case 3:
                GX_SetGraphicsMode(GX_DISPMODE_VRAM_D,  // display VRAM-D
                                   (GXBGMode)0, // dummy
                                   (GXBG0As)0); // dummy
                break;
            }
            // reset a counter
            count = 0;
        }
    }
}

//---------------------------------------------------------------------------
// V-Blank interrupt function:
//
// Interrupt handlers are registered on the interrupt table by OS_SetIRQFunction.
// OS_EnableIrqMask selects IRQ interrupts to enable, and
// OS_EnableIrq enables IRQ interrupts.
// Notice that you have to call 'OS_SetIrqCheckFlag' to check a V-Blank interrupt.
//---------------------------------------------------------------------------
void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK); // checking V-Blank interrupt
}
