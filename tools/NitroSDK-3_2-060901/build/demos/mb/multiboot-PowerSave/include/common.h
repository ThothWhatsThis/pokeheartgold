/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - demos - multiboot-PowerSave
  File:     common.h

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: common.h,v $
  Revision 1.2  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.1  04/28/2005 07:15:17  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#if !defined(NITRO_MB_DEMO_MULTIBOOT_POWERSAVE_COMMON_H_)
#define NITRO_MB_DEMO_MULTIBOOT_POWERSAVE_COMMON_H_


#include <nitro.h>


/*****************************************************************************/
/* variable*/

/* Configures power-save mode*/
extern BOOL g_power_save_mode;


/*****************************************************************************/
/* function*/

/*---------------------------------------------------------------------------*
  Name:         ReadKeySetTrigger

  Description:  detect key trigger

  Arguments:    None.

  Returns:      The bit set of the key input that was pressed during this call, and not pressed during the last call.
                 
 *---------------------------------------------------------------------------*/
u16     ReadKeySetTrigger(void);

/*---------------------------------------------------------------------------*
  Name:         MenuMode

  Description:  Initial settings menu for the DS Single-Card parent device

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MenuMode(void);

/*---------------------------------------------------------------------------*
  Name:         ParentMode

  Description:  DS Single-Card play parent processing.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    ParentMode(void);


#endif /* !defined(NITRO_MB_DEMO_MULTIBOOT_POWERSAVE_COMMON_H_) */
