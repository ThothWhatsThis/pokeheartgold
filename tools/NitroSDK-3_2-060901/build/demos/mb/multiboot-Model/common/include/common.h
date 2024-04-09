/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - demos - multiboot-Model
  File:     common.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: common.h,v $
  Revision 1.3  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.2  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.1  09/16/2004 14:07:11  takano_makoto
  Initial Update

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef MB_DEMO_COMMON_H_
#define MB_DEMO_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>

//============================================================================
//  Function Declarations
//============================================================================

void    CommonInit();
void    ReadKey(void);
u16     GetPressKey(void);
u16     GetTrigKey(void);
void    InitAllocateSystem(void);

/*---------------------------------------------------------------------------*
  Name:         IS_PAD_PRESS

  Description:  Key determination

  Arguments:    Key flag to determine

  Returns:      If specified key is held down, TRUE     
                Otherwise, FALSE
 *---------------------------------------------------------------------------*/
static inline BOOL IS_PAD_PRESS(u16 flag)
{
    return (GetPressKey() & flag) == flag;
}

/*---------------------------------------------------------------------------*
  Name:         IS_PAD_TRIGGER

  Description:  Determines key trigger

  Arguments:    Key flag to determine

  Returns:      If specified key trigger is enabled, TRUE    
                Otherwise, FALSE
 *---------------------------------------------------------------------------*/
static inline BOOL IS_PAD_TRIGGER(u16 flag)
{
    return (GetTrigKey() & flag) == flag;
}


#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif // MB_DEMO_COMMON_H_
