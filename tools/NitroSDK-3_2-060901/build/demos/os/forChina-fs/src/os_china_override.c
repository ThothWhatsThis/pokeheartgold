/*---------------------------------------------------------------------------*
  Project:  NitroSDK - demos - OS - forChina-fs
  File:     os_china_override.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_china_override.c,v $
  Revision 1.5  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.4  11/21/2005 10:53:40  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.3  10/20/2005 08:33:33  yosizaki
  update graphics data.

  Revision 1.2  07/11/2005 03:03:01  yosizaki
  disable sound.

  Revision 1.1  2005/06/02 07:57:47  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro.h>


/*****************************************************************************/
/* constant*/

/* Startup display fade and stop frame count*/
#define     OSi_FORCHINA_FADE_SPAN          32
#define     OSi_FORCHINA_DISP_SPAN          64

/* Index to data images managed by using table*/
typedef enum
{
    IMAGE_NOTES_CHR,
    IMAGE_NOTES_SCR,
    IMAGE_NOTES_DIGIT,
    IMAGE_LOGO_CHR,
    IMAGE_LOGO_SCR,
    IMAGE_LOGO_PAL,
    IMAGE_LOGO_VOICE,
    IMAGE_MAX
}
ImageIndex;


/*****************************************************************************/
/* declaration*/

static u8 *LoadImage(ImageIndex index, u32 *p_size);
static void WaitForNextFrame(void);
static void VBlankIntr(void);
static void SetISBNString(const char **isbn);


/*****************************************************************************/
/* function*/

/*---------------------------------------------------------------------------*
  Name:         LoadImage

  Description:  Allocates memory of sufficient size and loads the specified file.

  Arguments:    index   -   Index to the file to load.
                p_size  -   Pointer to u32 that stores the file size.
                            If not needed, you can specify NULL and it will be ignored.

  Returns:      Memory that is allocated inside the call and where the file is loaded.
                When ending OS_ShowAttentionChina(),
                each heap will be deleted with OS_ClearAlloc() so
                you don't need to explicitly free the memory returned here.
 *---------------------------------------------------------------------------*/
static u8 *LoadImage(ImageIndex index, u32 *p_size)
{
    void   *ptr = NULL;
    /* *INDENT-OFF**/
    static const char *(path_table[IMAGE_MAX]) =
    {
        "notes_forChina_chrData.bin",
        "notes_forChina_scrData.bin",
        "digit_chrData.bin",
        "logo_forChina_chrData.bin",
        "logo_forChina_scrData.bin",
        "logo_forChina_palData.bin",
        "logo_forChina_voice.bin",
    };
    /* *INDENT-ON**/ 
    const char *path = path_table[index];

    FSFile  file[1];

    FS_InitFile(file);
    if (!FS_OpenFile(file, path))
    {
        OS_TPanic("failed to open file \"%s\"", path);
    }
    else
    {
        u32     len = FS_GetLength(file);
        ptr = OS_Alloc(len);
        if (!ptr)
        {
            OS_TPanic("failed to allocate memory for image \"%s\"", path);
        }
        else
        {
            if (p_size)
            {
                *p_size = len;
            }
            if (FS_ReadFile(file, ptr, (int)len) != len)
            {
                OS_TPanic("failed to read file \"%s\"", path);
            }
        }
        (void)FS_CloseFile(file);
    }

    return (u8 *)ptr;
}

/*---------------------------------------------------------------------------*
  Name:         WaitForNextFrame

  Description:  Processing when system is opened and closed, V blank wait, and sound update processing.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WaitForNextFrame(void)
{
    /* Confirm that cover is closed*/
    if (PAD_DetectFold())
    {
        PM_GoSleepMode(PM_TRIGGER_COVER_OPEN | PM_TRIGGER_CARD, 0, 0);
    }
    OS_WaitVBlankIntr();
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  V blank interrupt vector in exclusive startup control screen.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         SetISBNString

  Description:  Reflect the ISBN number etc. in a prescribed location in the screen data.

  Arguments:    isbn    -   Character string array about the ISBN number etc.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SetISBNString(const char **isbn)
{
    s32     i, j;
    struct
    {
        u8      x, y;                  /* Offset from top of display*/
        u8      length;                /* Display character count*/
    }
    pos[] =
    {
        {
        80, 144, 13}
        ,                              /* ISBN*/
        {
        120, 158, 12}
        ,                              /* Joint registration number*/
        {
        103, 175, 4}
        ,                              /* New sound output tube (left)*/
        {
        144, 175, 4}
        ,                              /* New sound output tube (right)*/
    }
    ;
    const int count = sizeof(pos) / sizeof(*pos);       /* Total number of lines*/
    const int digit_id = 0;            /* Digit display character lead ID ("0123456789-")*/

    GXOamAttr *dst = (GXOamAttr *)HW_DB_OAM;
    for (i = 0; i < count; i++)
    {
        for (j = 0; j < pos[i].length; ++j)
        {
            dst->attr01 = (u32)(((pos[i].x + j * 8) << 16) | (pos[i].y << 0));
            if (isbn[i][j] == '-')
            {
                dst->attr2 = (u16)(digit_id + 10);
            }
            else if ((isbn[i][j] >= '0') && (isbn[i][j] <= '9'))
            {
                dst->attr2 = (u16)(digit_id + isbn[i][j] - '0');
            }
            else
            {
                dst->attr2 = (u16)(digit_id + 11);
            }
            ++dst;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_ShowAttentionChina

  Description:  Displays logo for China and warning screen for prescribed time.

  Arguments:    isbn    -   Character string array about the ISBN number etc.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_ShowAttentionChina(const char **isbn)
{
    /* Structure for saving register*/
    struct
    {
        u32     gx_dispcnt;
        u32     gxs_dispcnt;
        u16     gx_powcnt;
        u16     gx_dispstat;
        u16     gx_bright;
        u16     gxs_bright;
        u16     gx_bg0cnt;
        u16     gxs_bg0cnt;
        u32     gx_bg0ofs;
        u32     gxs_bg0ofs;
        u8      gx_vramcnt_a;
        u8      gx_vramcnt_c;
        u8      gx_vramcnt_d;
        u8      reserved[1];

        OSIrqFunction irqFunction;
        OSIrqMask irqMask;
        BOOL    irq;
        OSIntrMode interrupts;

    }
    shelter;

    /* Pre-processing*/
    {
        /* Save each register*/
        shelter.gx_dispstat = reg_GX_DISPSTAT;
        shelter.gx_dispcnt = reg_GX_DISPCNT;
        shelter.gxs_dispcnt = reg_GXS_DB_DISPCNT;
        shelter.gx_bright = reg_GX_MASTER_BRIGHT;
        shelter.gxs_bright = reg_GXS_DB_MASTER_BRIGHT;
        shelter.gx_powcnt = reg_GX_POWCNT;
        shelter.gx_vramcnt_a = reg_GX_VRAMCNT_A;
        shelter.gx_vramcnt_c = reg_GX_VRAMCNT_C;
        shelter.gx_vramcnt_d = reg_GX_VRAMCNT_D;
        shelter.gx_bg0cnt = reg_G2_BG0CNT;
        shelter.gxs_bg0cnt = reg_G2S_DB_BG0CNT;
        shelter.gx_bg0ofs = reg_G2_BG0OFS;
        shelter.gxs_bg0ofs = reg_G2S_DB_BG0OFS;

        /* Stop display*/
        reg_GX_DISPCNT = 0;
        reg_GXS_DB_DISPCNT = 0;

        /* Initialize power control*/
        if (!(shelter.gx_powcnt & REG_GX_POWCNT_LCD_MASK))
        {
            /* When changing LCD enable from OFF to ON, wait 100ms*/
            SVC_WaitByLoop(HW_CPU_CLOCK_ARM9 / 40);
        }
        reg_GX_POWCNT = (u16)(REG_GX_POWCNT_DSEL_MASK | REG_GX_POWCNT_E2DG_MASK |
                              REG_GX_POWCNT_E2DGB_MASK | REG_GX_POWCNT_LCD_MASK);

        /* Initialization of master brightness*/
        reg_GX_MASTER_BRIGHT = (u16)((1 << REG_GX_MASTER_BRIGHT_E_MOD_SHIFT) | 16);
        reg_GXS_DB_MASTER_BRIGHT = reg_GX_MASTER_BRIGHT;

        /* Initialization of memory management and FS*/
        {
            void   *tmp;
            OSHeapHandle hh;
            tmp = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
            OS_SetArenaLo(OS_ARENA_MAIN, tmp);
            hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
            hh = OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
            FS_Init(FS_DMA_NOT_USE);
        }

        /* Prepare display of logo screen*/
        {
            reg_GX_VRAMCNT_A =
                (u8)((1 << REG_GX_VRAMCNT_A_MST_SHIFT) | (1 << REG_GX_VRAMCNT_A_E_SHIFT));
            reg_G2_BG0CNT =
                (u16)((GX_BG_SCRSIZE_TEXT_256x256 << REG_G2_BG0CNT_SCREENSIZE_SHIFT) |
                      (GX_BG_COLORMODE_256 << REG_G2_BG0CNT_COLORMODE_SHIFT) | (GX_BG_SCRBASE_0xf000
                                                                                <<
                                                                                REG_G2_BG0CNT_SCREENBASE_SHIFT)
                      | (GX_BG_CHARBASE_0x00000 << REG_G2_BG0CNT_CHARBASE_SHIFT) | (GX_BG_EXTPLTT_01
                                                                                    <<
                                                                                    REG_G2_BG0CNT_BGPLTTSLOT_SHIFT)
                      | (0 << REG_G2_BG0CNT_PRIORITY_SHIFT));
            reg_G2_BG0OFS = 0;
            reg_GX_DISPCNT |= ((GX_BGMODE_0 << REG_GX_DISPCNT_BGMODE_SHIFT) |
                               (GX_PLANEMASK_BG0 << REG_GX_DISPCNT_DISPLAY_SHIFT));

            /* Load character data into VRAM*/
            {
                void   *const src = LoadImage(IMAGE_LOGO_CHR, NULL);
                MI_UncompressLZ16(src, (u32 *)HW_BG_VRAM);
            }
            /* Load screen data into VRAM*/
            {
                u8     *const src = LoadImage(IMAGE_LOGO_SCR, NULL);
                s32     i;
                s32     j;
                u16     temp;

                SVC_CpuClearFast(0, (u32 *)(HW_BG_VRAM + 0xf000), 1536);
                for (i = 0; i < 12; i++)
                {
                    for (j = 0; j < 12; j++)
                    {
                        temp = (u16)(src[(i * 12) + j]);
                        *(u16 *)(HW_BG_VRAM + 0xf194 + (i * 64) + (j * 2)) = temp;
                    }
                }
            }
            /* Load palette data into VRAM*/
            {
                u32     size;
                void   *const src = LoadImage(IMAGE_LOGO_PAL, &size);
                SVC_CpuCopyFast(src, (u32 *)(HW_BG_PLTT), size);
            }
        }

        /* Prepare to display Notes screen*/
        {
            reg_GX_VRAMCNT_C =
                (u8)((4 << REG_GX_VRAMCNT_C_MST_SHIFT) | (1 << REG_GX_VRAMCNT_C_E_SHIFT));
            reg_GX_VRAMCNT_D =
                (u8)((4 << REG_GX_VRAMCNT_D_MST_SHIFT) | (1 << REG_GX_VRAMCNT_D_E_SHIFT));
            reg_G2S_DB_BG0CNT =
                (u16)((GX_BG_SCRSIZE_TEXT_256x256 << REG_G2S_DB_BG0CNT_SCREENSIZE_SHIFT) |
                      (GX_BG_COLORMODE_16 << REG_G2S_DB_BG0CNT_COLORMODE_SHIFT) |
                      (GX_BG_SCRBASE_0xf000 << REG_G2S_DB_BG0CNT_SCREENBASE_SHIFT) |
                      (GX_BG_CHARBASE_0x00000 << REG_G2S_DB_BG0CNT_CHARBASE_SHIFT) |
                      (GX_BG_EXTPLTT_01 << REG_G2S_DB_BG0CNT_BGPLTTSLOT_SHIFT) | (0 <<
                                                                                  REG_G2S_DB_BG0CNT_PRIORITY_SHIFT));
            reg_G2S_DB_BG0OFS = 0;
            reg_GXS_DB_DISPCNT |= ((GX_BGMODE_0 << REG_GXS_DB_DISPCNT_BGMODE_SHIFT) |
                                   ((GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ) <<
                                    REG_GXS_DB_DISPCNT_DISPLAY_SHIFT));

            /* Load character data into VRAM*/
            {
                u32     size;
                void   *src;
                MIUnpackBitsParam param = { 0, 1, 4, 0, 0 };
                /* BG text*/
                src = LoadImage(IMAGE_NOTES_CHR, &size);
                param.srcNum = (u16)size;
                SVC_UnpackBits(src, (u32 *)(HW_DB_BG_VRAM), &param);
                /* OBJ text*/
                src = LoadImage(IMAGE_NOTES_DIGIT, &size);
                param.srcNum = (u16)size;
                SVC_UnpackBits(src, (u32 *)(HW_DB_OBJ_VRAM), &param);
                SVC_CpuClear(0xC0, (void *)HW_DB_OAM, HW_OAM_SIZE, 32);
            }
            /* Load screen data into VRAM*/
            {
                void   *const src = LoadImage(IMAGE_NOTES_SCR, NULL);
                MI_UncompressLZ16(src, (u32 *)(HW_DB_BG_VRAM + 0xf000));
            }
            SetISBNString(isbn);
            /* Edit palette*/
            *((u16 *)(HW_DB_BG_PLTT + 0)) = 0x0000;
            *((u16 *)(HW_DB_BG_PLTT + 2)) = 0x7fff;
            *((u16 *)(HW_DB_OBJ_PLTT + 0)) = 0x7fff;
            *((u16 *)(HW_DB_OBJ_PLTT + 2)) = 0x0000;
        }

        /* Starts display*/
        reg_GX_DISPCNT |= (u32)(GX_DISPMODE_GRAPHICS << REG_GX_DISPCNT_MODE_SHIFT);
        reg_GXS_DB_DISPCNT |= (u32)(REG_GXS_DB_DISPCNT_MODE_MASK);

        /* Interrupt settings*/
        reg_GX_DISPSTAT |= REG_GX_DISPSTAT_VBI_MASK;
        shelter.irqFunction = OS_GetIrqFunction(OS_IE_V_BLANK);
        OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
        shelter.irqMask = OS_EnableIrqMask(OS_IE_V_BLANK);
        shelter.irq = OS_EnableIrq();
        shelter.interrupts = OS_EnableInterrupts();

    }

    /* Main process*/
    {
        s32     i;
        u16     pad_old;
        u16     pad;

        /* Fade in*/
        for (i = 0; i < OSi_FORCHINA_FADE_SPAN; i++)
        {
            reg_GX_MASTER_BRIGHT = (u16)((1 << REG_GX_MASTER_BRIGHT_E_MOD_SHIFT) |
                                         16 - ((i + 1) * 16 / OSi_FORCHINA_FADE_SPAN));
            reg_GXS_DB_MASTER_BRIGHT = reg_GX_MASTER_BRIGHT;
            WaitForNextFrame();
        }

        /* Skip based on button input*/
        pad_old = PAD_Read();
        for (i = 0; i < OSi_FORCHINA_DISP_SPAN; i++)
        {
            pad = PAD_Read();
            if (~pad_old & pad)
            {
                /* If any button push trigger was detected, skip*/
                break;
            }
            pad_old = pad;
            WaitForNextFrame();
        }

        /* Fade out*/
        for (i = 0; i < OSi_FORCHINA_FADE_SPAN; i++)
        {
            reg_GX_MASTER_BRIGHT = (u16)((1 << REG_GX_MASTER_BRIGHT_E_MOD_SHIFT) |
                                         (i + 1) * 16 / OSi_FORCHINA_FADE_SPAN);
            reg_GXS_DB_MASTER_BRIGHT = reg_GX_MASTER_BRIGHT;
            WaitForNextFrame();
        }
    }

    /* Post-processing*/
    {

        /* End FS and memory management*/
        {
            FS_End();
            OS_ClearAlloc(OS_ARENA_MAIN);
        }

        /* Stop display*/
        reg_GX_DISPCNT &= ~(GX_DISPMODE_GRAPHICS << REG_GX_DISPCNT_MODE_SHIFT);
        reg_GXS_DB_DISPCNT &= ~REG_GXS_DB_DISPCNT_MODE_MASK;

        /* Restore interrupt-related items*/
        (void)OS_RestoreInterrupts(shelter.interrupts);
        (void)OS_RestoreIrq(shelter.irq);
        (void)OS_SetIrqMask(shelter.irqMask);
        OS_SetIrqFunction(OS_IE_V_BLANK, shelter.irqFunction);
        reg_GX_DISPSTAT &= ~REG_GX_DISPSTAT_VBI_MASK;

        /* Clear used VRAM (cannot restore initial values because the size is too big)*/
        SVC_CpuClearFast(0, (u32 *)HW_BG_VRAM, 0x10000);
        SVC_CpuClearFast(0, (u32 *)HW_BG_PLTT, 0x200);
        SVC_CpuClearFast(0, (u32 *)HW_DB_BG_VRAM, 0x10000);
        SVC_CpuClearFast(0, (u32 *)HW_DB_BG_PLTT, 0x04);
        SVC_CpuClearFast(0, (u32 *)HW_DB_OAM, HW_OAM_SIZE);

        /* Restore each register*/
        reg_G2S_DB_BG0OFS = shelter.gxs_bg0ofs;
        reg_G2_BG0OFS = shelter.gx_bg0ofs;
        reg_G2S_DB_BG0CNT = shelter.gxs_bg0cnt;
        reg_G2_BG0CNT = shelter.gx_bg0cnt;
        reg_GX_VRAMCNT_D = shelter.gx_vramcnt_d;
        reg_GX_VRAMCNT_C = shelter.gx_vramcnt_c;
        reg_GX_VRAMCNT_A = shelter.gx_vramcnt_a;
        reg_GX_POWCNT = shelter.gx_powcnt;
        if (!(shelter.gx_powcnt & REG_GX_POWCNT_LCD_MASK))
        {
            /* When changing LCD enable from ON to OFF, wait 100ms*/
            SVC_WaitByLoop(HW_CPU_CLOCK_ARM9 / 40);
        }
        reg_GXS_DB_MASTER_BRIGHT = shelter.gxs_bright;
        reg_GX_MASTER_BRIGHT = shelter.gx_bright;
        reg_GXS_DB_DISPCNT = shelter.gxs_dispcnt;
        reg_GX_DISPCNT = shelter.gx_dispcnt;
        reg_GX_DISPSTAT = shelter.gx_dispstat;
    }
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
