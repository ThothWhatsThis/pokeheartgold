/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - demos - favorite-color
  File:     main.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.7  01/18/2006 02:11:29  kitase_hirotake
  do-indent

  Revision 1.6  11/21/2005 10:54:19  kitase_hirotake
  Changed SVC_WaitVBlankIntr to OS_WaitVBlankIntr

  Revision 1.5  06/27/2005 11:10:12  yosizaki
  add comment about DC_WaitWriteBufferEmpty().

  Revision 1.4  03/25/2005 01:29:48  yosizaki
  fix FillRect() to use explicit Z-sort.
  add comment.

  Revision 1.3  02/28/2005 05:26:10  yosizaki
  do-indent.

  Revision 1.2  10/08/2004 00:40:48  yosizaki
  fix V-BlankIntr during initialization.
  small fix around symbols' name, comments, and so on.

  Revision 1.1  10/07/2004 10:20:14  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include "DEMO.h"


/*****************************************************************************/
/* prototype*/

/* information for display of color name*/
typedef struct ColorProp_tag
{
    int     index;
    const char *name;
    GXRgb   color;
    u8      padding[4 - sizeof(GXRgb)];
}
ColorProp;


/*****************************************************************************/
/* macro*/

#define	COLOR_PROP_DEFINE(name)       \
{                                     \
    OS_FAVORITE_COLOR_ ## name,       \
    "OS_FAVORITE_COLOR_" #name,       \
    OS_FAVORITE_COLOR_VALUE_ ## name, \
}


/*****************************************************************************/
/* const*/

extern const u32 d_CharData[8 * 256];
extern const u32 d_PaletteData[8 * 16];

/* Constants used for screen display*/
enum
{
    ICON_MAX_X = 4,
    ICON_MAX_Y = OS_FAVORITE_COLOR_MAX / 4,
    ICON_WX = 32, ICON_WY = 12,
    SPACE_X = 16, SPACE_Y = 16,
    BORDER = 2,
    MARGIN = 16,
    FRAME_OX = 24, FRAME_OY = 48,
    FRAME_WX = ICON_WX * ICON_MAX_X + SPACE_X * (ICON_MAX_X - 1) + MARGIN * 2,
    FRAME_WY = ICON_WY * ICON_MAX_Y + SPACE_Y * (ICON_MAX_Y - 1) + MARGIN * 2
};

/* color table information*/
static const ColorProp prop[OS_FAVORITE_COLOR_MAX] = {
    COLOR_PROP_DEFINE(GRAY),
    COLOR_PROP_DEFINE(BROWN),
    COLOR_PROP_DEFINE(RED),
    COLOR_PROP_DEFINE(PINK),
    COLOR_PROP_DEFINE(ORANGE),
    COLOR_PROP_DEFINE(YELLOW),
    COLOR_PROP_DEFINE(LIME_GREEN),
    COLOR_PROP_DEFINE(GREEN),
    COLOR_PROP_DEFINE(DARK_GREEN),
    COLOR_PROP_DEFINE(SEA_GREEN),
    COLOR_PROP_DEFINE(TURQUOISE),
    COLOR_PROP_DEFINE(BLUE),
    COLOR_PROP_DEFINE(DARK_BLUE),
    COLOR_PROP_DEFINE(PURPLE),
    COLOR_PROP_DEFINE(VIOLET),
    COLOR_PROP_DEFINE(MAGENTA),
};


/*****************************************************************************/
/* variable*/

static u16 gScreen[32 * 32];           /* Virtual screen*/


/*****************************************************************************/
/* function*/

/*---------------------------------------------------------------------------*
  Name:         ClearString

  Description:  Initializes the virtual screen for display.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ClearString(void)
{
    MI_CpuClearFast(gScreen, sizeof(gScreen));
}


/*---------------------------------------------------------------------------*
  Name:         PrintString

  Description:  Displays a character string of up to 32 characters on the virtual screen

  Arguments:    x       - The x coordinate that positions the start of the character string / 8 dot
                y       - The y coordinate that positions the start of the character string / 8 dot
                palette - The palette number that designates the character color
                text    - The displayed character string
                - Subsequent variable arguments

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void PrintString(int x, int y, int palette, const char *text, ...)
{
    va_list vlist;
    static char temp[64 + 2];
    u8      d;
    int     i;

    va_start(vlist, text);
    (void)OS_VSNPrintf(temp, sizeof(temp) - 1, text, vlist);
    va_end(vlist);
    *(u16 *)&temp[32] = 0x0000;
    for (i = 0; (d = MI_ReadByte(temp + i)) != '\0'; ++i)
    {
        if ((d == '\r') || (d == '\n'))
            break;
        gScreen[((y * 32) + x + i) % (32 * 32)] = (u16)((palette << 12) | d);
    }
}


/*---------------------------------------------------------------------------*
  Name:         FillRect

  Description:  render a rectangle in the designated location

  Arguments:    x       - x coordinate of left edge of rectangle
                y       - y coordinate of top edge of rectangle
                z       - z coordinate that specifies layer of rectangle
                wx      - x width of rectangle
                wy      - y width of rectangle
                col     - Rectangle display color

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void FillRect(int x, int y, int z, int wx, int wy, GXRgb col)
{
    G3_PolygonAttr(GX_LIGHTMASK_NONE,  // no lights
                   GX_POLYGONMODE_MODULATE,     // modulation mode
                   GX_CULL_NONE,       // cull none
                   0,                  // polygon ID(0 - 63)
                   31,                 // alpha(0 - 31)
                   0                   // OR of GXPolygonAttrMisc's value
        );

    G3_Begin(GX_BEGIN_QUADS);
    {
        fx16    ox = (fx16)(-FX16_ONE + (x * FX16_ONE * 2) / GX_LCD_SIZE_X);
        fx16    oy = (fx16)(+FX16_ONE - (y * FX16_ONE * 2) / GX_LCD_SIZE_Y);
        fx16    oz = (fx16)(+z);
        fx16    tx = (fx16)(ox + (wx * FX16_ONE * 2) / GX_LCD_SIZE_X);
        fx16    ty = (fx16)(oy - (wy * FX16_ONE * 2) / GX_LCD_SIZE_Y);

        G3_Color(col);
        G3_Vtx(ox, oy, oz);
        G3_Color(col);
        G3_Vtx(ox, ty, oz);
        G3_Color(col);
        G3_Vtx(tx, ty, oz);
        G3_Color(col);
        G3_Vtx(tx, oy, oz);
    }
    G3_End();
}


/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  Main entry point to application.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    //---------------------------------------------------------------------------
    // Initialization:
    // This sample set the basic 3D settings, DEMOInit() not called.
    //---------------------------------------------------------------------------
    DEMOInitCommon();

    (void)OS_DisableIrq();

    DEMOInitVRAM();
    DEMOInitDisplay3D();

    //---------------------------------------------------------------------------
    // 2D text settings
    //---------------------------------------------------------------------------
    GX_LoadBG1Char(d_CharData, 0, sizeof(d_CharData));
    GX_LoadBGPltt(d_PaletteData, 0, sizeof(d_PaletteData));

    //---------------------------------------------------------------------------
    // Start display
    //---------------------------------------------------------------------------
    (void)OS_EnableIrq();
    DEMOStartDisplay();


    {                                  /*
                                        * Determine if the defined value and the displayed character string are really in proper correspondence
                                        * (This process is used to confirm the validity of the SDK implementation)
                                        */
        const GXRgb *p_col = OS_GetFavoriteColorTable();
        int     i;
        for (i = 0; i < OS_FAVORITE_COLOR_MAX; ++i)
        {
            if (prop[i].index != i)
            {
                OS_TPanic("ColorProp[%d] definition is wrong!", i);
            }
            else if (prop[i].color != p_col[i])
            {
                OS_TPanic("ColorProp[%d] definition or OS_GetFavoriteColorTable() is wrong!", i);
            }
            else if (prop[i].color != OS_GetFavoriteColor(i))
            {
                OS_TPanic("OS_GetFavoriteColor(i) is wrong!", i);
            }
        }
    }

    {                                  /*
                                        * Display each color value
                                        * Move the selection with +Control Pad and display the color definition name
                                        */
        int     i;
        int     cur_x = 0, cur_y = 0;
        int     frame = 0;

        DEMOReadKey();
        for (;; ++frame)
        {
            /* Rectangle layer relative control*/
            int     layer = 3;
            //---------------------------------------------------------------------------
            // Reset the states of 3D graphics
            //---------------------------------------------------------------------------
            G3X_Reset();

            //---------------------------------------------------------------------------
            // In this sample, the camera matrix and the projection matrix are set as
            // an identity matrices for simplicity.
            //---------------------------------------------------------------------------
            G3_MtxMode(GX_MTXMODE_PROJECTION);
            G3_Identity();
            G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
            G3_Identity();

            ClearString();

            DEMOReadKey();

            /* render the background*/
            FillRect(FRAME_OX, FRAME_OY, layer--, FRAME_WX, FRAME_WY, GX_RGB(16, 16, 16));
            FillRect(FRAME_OX + BORDER, FRAME_OY + BORDER, layer--,
                     FRAME_WX - BORDER * 2, FRAME_WY - BORDER * 2, GX_RGB(31, 31, 31));

            /* move the cursor with +Control Pad*/
            if (DEMO_IS_TRIG(PAD_KEY_RIGHT))
            {
                if (++cur_x >= ICON_MAX_X)
                    cur_x -= ICON_MAX_X;
            }
            if (DEMO_IS_TRIG(PAD_KEY_LEFT))
            {
                if (--cur_x < 0)
                    cur_x += ICON_MAX_X;
            }
            if (DEMO_IS_TRIG(PAD_KEY_DOWN))
            {
                if (++cur_y >= ICON_MAX_Y)
                    cur_y -= ICON_MAX_Y;
            }
            if (DEMO_IS_TRIG(PAD_KEY_UP))
            {
                if (--cur_y < 0)
                    cur_y += ICON_MAX_Y;
            }

            {                          /* display the selected color with a list*/
                const int index = cur_x + cur_y * ICON_MAX_X;

                PrintString(1, 1, 0xF, "index : %d", index);
                PrintString(1, 3, 0xF, "%s", prop[index].name);

                for (i = 0; i < OS_FAVORITE_COLOR_MAX; ++i)
                {
                    const int x = FRAME_OX + MARGIN + (i % ICON_MAX_X) * (ICON_WX + SPACE_X);
                    const int y = FRAME_OY + MARGIN + (i / ICON_MAX_X) * (ICON_WY + SPACE_Y);
                    GXRgb   gnd;
                    if (i == index)
                    {
                        const int grad = ((frame & 32) == 0) ? (frame & 31) : 31 - (frame & 31);
                        gnd = GX_RGB(grad, grad, grad);
                    }
                    else
                    {
                        gnd = GX_RGB(31, 31, 31);
                    }
                    FillRect(x - MARGIN / 2, y - MARGIN / 2, layer - 0,
                             ICON_WX + MARGIN, ICON_WY + MARGIN, gnd);
                    FillRect(x - MARGIN / 4, y - MARGIN / 4, layer - 1,
                             ICON_WX + MARGIN / 2, ICON_WY + MARGIN / 2, GX_RGB(31, 31, 31));
                    FillRect(x, y, layer - 2, ICON_WX, ICON_WY, prop[i].color);
                }
            }

            // swapping the polygon list RAM, the vertex RAM, etc.
            G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_Z);

            OS_WaitVBlankIntr();       // Waiting the end of V-Blank interrupt
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
    /* Reflect virtual screen in VRAM*/
    DC_FlushRange(gScreen, sizeof(gScreen));
    /* IO register is accessed using DMA operation, so cache Wait is not needed*/
    // DC_WaitWriteBufferEmpty();
    GX_LoadBG1Scr(gScreen, 0, sizeof(gScreen));

    OS_SetIrqCheckFlag(OS_IE_V_BLANK); // checking V-Blank interrupt
}
