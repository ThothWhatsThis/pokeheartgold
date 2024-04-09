/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - demos - multiboot-wfs
  File:     util.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: util.h,v $
  Revision 1.1  06/23/2005 09:05:57  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#if	!defined(NITRO_DEMOS_MB_MULTIBOOT_WFS_UTIL_H_)
#define	NITRO_DEMOS_MB_MULTIBOOT_WFS_UTIL_H_

#ifdef	__cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include	<nitro/types.h>

/*
 * Routine for processing the entire sample framework.
 * It its not directly related to demonstrations provided by samples.
 */



/*---------------------------------------------------------------------------*
    Structure Definitions
 *---------------------------------------------------------------------------*/


/* ASCII font data*/
extern const u32 d_CharData[8 * 256];
extern const u32 d_PaletteData[8 * 16];

/* Color palette*/
#define	COLOR_BLACK                0x00
#define	COLOR_RED                  0x01
#define	COLOR_GREEN                0x02
#define	COLOR_BLUE                 0x03
#define	COLOR_YELLOW               0x04
#define	COLOR_PURPLE               0x05
#define	COLOR_LIGHT_BLUE           0x06
#define	COLOR_DARK_RED             0x07
#define	COLOR_DARK_GREEN           0x08
#define	COLOR_DARK_BLUE            0x09
#define	COLOR_DARK_YELLOW          0x0A
#define	COLOR_DARK_PURPLE          0x0B
#define	COLOR_DARK_LIGHT_BLUE      0x0C
#define	COLOR_GRAY                 0x0D
#define	COLOR_DARK_GRAY            0x0E
#define	COLOR_WHITE                0x0F


/*---------------------------------------------------------------------------*
    Structure Definitions
 *---------------------------------------------------------------------------*/


/* Key input data */
typedef struct KeyInfo
{
    u16     cnt;                       /* Raw entry value */
    u16     trg;                       /* Down trigger input*/
    u16     up;                        /* Up trigger input */
    u16     rep;                       /* Hold down input */

}
KeyInfo;


/*---------------------------------------------------------------------------*
    Function declarations
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
  Name:         UTIL_Init

  Description:  Initialize sample framework

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/

void    UTIL_Init(void);


/*---------------------------------------------------------------------------*
  Name:         KeyRead

  Description:  Acquire key data

  Arguments:    pKey : Address at which key information is stored

  Returns:      None.
 *---------------------------------------------------------------------------*/

void    KeyRead(KeyInfo * pKey);


/*---------------------------------------------------------------------------*
  Name:         ClearString

  Description:  Initializes the virtual screen for display.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/

void    ClearString(void);


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

void    PrintString(int x, int y, int palette, const char *text, ...);


/*---------------------------------------------------------------------------*
  Name:         ColorString

  Description:  Changes the color of the character string shown on the virtual screen

  Arguments:    x       - x coordinate for the beginning of the color change / 8 dot
                y       - y coordinate for the beginning of the color change / 8 dot
                length  - Number of characters to change the color of
                palette - The palette number that designates the character color 

  Returns:      None.
 *---------------------------------------------------------------------------*/

void    ColorString(int x, int y, int length, int palette);


/*---------------------------------------------------------------------------*
  Name:         ClearLine

  Description:  Initialize drawing character string

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/

void    ClearLine(void);


/*---------------------------------------------------------------------------*
  Name:         PrintLine

  Description:  In a specified line, this draws a nonvolatile character string that will remain for at least one frame.

  Arguments:    x          Display position x grid. (8 pixel units)
                y          Display position y grid. (8 pixel units)
                text        Formatted character string that receives the variable argument that follows. 

  Returns:      None.
 *---------------------------------------------------------------------------*/

void    PrintLine(int x, int y, const char *text, ...);


/*---------------------------------------------------------------------------*
  Name:         FlushLine

  Description:  Overwrites the nonvolatile character string from PrintLine() onto the normal character string drawing. 

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/

void    FlushLine(void);


/*===========================================================================*/

#ifdef	__cplusplus
}          /* extern "C" */
#endif

#endif /* NITRO_DEMOS_WBT_WBT_FS_UTIL_H_ */


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/


