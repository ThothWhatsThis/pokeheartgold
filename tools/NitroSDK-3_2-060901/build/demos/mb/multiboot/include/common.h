/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - demos - multiboot
  File:     common.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: common.h,v $
  Revision 1.11  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.10  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.9  2004/10/05 06:47:54  sato_masaki
  Added setup menu. Made several small revisions.

  Revision 1.8  10/04/2004 13:40:25  terui
  Standardized game group IDs as type u32.

  Revision 1.7  09/30/2004 08:28:35  yosizaki
  add SDK_MAKEGGID_SYSTEM macro.

  Revision 1.6  2004/09/30 03:05:17  sato_masaki
  - Changed so that name of parent device is obtained from NVRAM.
  - Changed so that you cycle through channels when initializing.    

  Revision 1.5  2004/09/10 03:02:47  sato_masaki
  Changed comments about channel

  Revision 1.4  09/10/2004 02:29:51  miya
  Changed output channel

  Revision 1.3  2004/09/08 00:23:27  sato_masaki
  Changed register number of MBGameRegistry to 2

  Revision 1.2  2004/08/25 04:28:20  sato_masaki
  Mounting MB_CommStartSending function    

  Revision 1.1  08/10/2004 07:32:03  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#if !defined(NITRO_MB_DEMO_MULTIBOOT_DATA_H_)
#define NITRO_MB_DEMO_MULTIBOOT_DATA_H_


#include <nitro.h>
#include <nitro/mb.h>


/*
 * Definitions common to entire multiboot demo
 */


/******************************************************************************/
/* constant*/

/*
 * Game group ID
 *
 * Because specification of GGID has been enabled for individual files
 * in the MBGameRegistry structure, specification with MB_Init()
 * and the specification with MB_Init() may lose its significance in the future.    
 */
#define SDK_MAKEGGID_SYSTEM(num)    (0x003FFF00 | (num))
#define MY_GGID         SDK_MAKEGGID_SYSTEM(0x20)

/*
 * Channel which the parent distributes
 *
 * The multiboot child device runs through all available channels,    
 * so any of the values permitted by the WM * library (currently 1, 7, and 13) can be used.    
 *
 * This is made variable when avoiding communication congestion * for the user application.    
 * The timing of the channel change depends on the application,    
 * but for example, a possible method is to set the trigger by a user pressing "restart"    
 * when the response is bad.    
 */
#define PARENT_CHANNEL  13

/* the maximum number of child device lists to display*/
#define DISP_LIST_NUM   (15)

/* DMA number to allocate to the MB library*/
#define MB_DMA_NO       2

/* the total number of games that this demo registers*/
#define GAME_PROG_MAX   2

/* the multiboot game program information that this demo provides*/
extern const MBGameRegistry mbGameList[GAME_PROG_MAX];

/* the game sequence state value of this demo*/
enum
{
    STATE_NONE,
    STATE_MENU,
    STATE_MB_PARENTINIT,
    STATE_MB_PARENT
};


/******************************************************************************/
/* variable*/

/* the work region to be allocated to the MB library*/
extern u32 cwork[MB_SYSTEM_BUF_SIZE / sizeof(u32)];

/* the game sequence state of this demo*/
extern u8 prog_state;


/******************************************************************************/
/* function*/

/* detect key trigger*/
u16     ReadKeySetTrigger(u16 keyset);

/* offset portion rotation of the val value in the min - max bounds*/
BOOL    RotateU8(u8 *val, u8 min, u8 max, s8 offset);

/* parent device initialization*/
void    ParentInit(void);

/* parent device main process in each single frame*/
void    ParentMain(void);

/* initialize the file buffer pointer*/
void    InitFileBuffer(void);

/* set parent deviceMP send size*/
void    SetParentSendSize(u16 p_size);

/* get parent device MP send size*/
u16     GetParentSendSize(void);

/* set maximum number of child device connections*/
void    SetMaxEntry(u8 num);

/* get the maximum number of child device connections*/
u8      GetMaxEntry(void);

/* configure parent device auto operations*/
void    SetAutoOperation(u8 fAccept, u8 fSend, u8 fBoot);

#endif /* !defined(NITRO_MB_DEMO_MULTIBOOT_DATA_H_) */
