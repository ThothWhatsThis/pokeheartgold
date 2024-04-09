/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - demos - cloneboot
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

  Revision 1.1  11/15/2004 06:21:41  yosizaki
  initial update.

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


/*
 * This function was NitroMain() on the multiboot-Model parent side.    
 * Called in this sample when MB_IsMultiBootChild() == FALSE.
 */
void    ParentMain(void);

/*
 * This function was NitroMain() on the multiboot-Model child side.    
 * Called in this sample when MB_IsMultiBootChild() == TRUE.
 */
void    ChildMain(void);

/*
 * This function is located in the parent section (area reserved for parent machine).    
 * It calls ParentMain().    
 */
void    ParentIdentifier(void);

/* Everything else is identical to the multiboot-model.*/

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
