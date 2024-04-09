/*---------------------------------------------------------------------------*
  Project:  NitroSDK - PRC - demo - include
  File:     patternDict.h

  Copyright 2003-2005 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: patternDict.h,v $
  Revision 1.5  2005/02/28 05:26:29  yosizaki
  do-indent.

  Revision 1.4  2004/11/04 07:55:47  seiki_masashi
  Corrected 'regularize' to 'normalize'

  Revision 1.3  2004/07/19 06:52:06  seiki_masashi
  Identifier names revised.   

  Revision 1.2  2004/06/30 08:49:34  seiki_masashi
  algo_simple -> algo_light
  algo_standard -> algo_fine
  algo_accurate -> algo_superfine
  Added new algo_standard

  Revision 1.1  2004/06/29 13:12:20  seiki_masashi
  move characterRecognition-1 to characterRecognition-2

  Revision 1.2  2004/06/29 02:16:09  seiki_masashi
  Assigned value for kind for each character type  

  Revision 1.1  2004/06/23 12:02:11  seiki_masashi
  add 'prc/characterRecognition-1'


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef PATTERN_DICT_H_
#define PATTERN_DICT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/prc/types.h>


#define PDIC_WHOLE_POINT_NUM             1647
#define PDIC_WHOLE_STROKE_NUM            333
#define PDIC_PATTERN_NUM                 161
#define PDIC_CODE_NUM                    117
#define PDIC_NORMALIZE_SIZE              64


extern PRCPrototypeList PrototypeList;
extern char *PatternName[PDIC_CODE_NUM];


#ifdef __cplusplus
} /* extern "C" */
#endif

/* PATTERN_DICT_H_ */
#endif

/*---------------------------------------------------------------------------*
  End of File
 *---------------------------------------------------------------------------*/

