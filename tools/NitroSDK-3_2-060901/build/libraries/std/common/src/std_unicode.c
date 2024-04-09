/*---------------------------------------------------------------------------*
  Project:  NitroSDK - libraries - STD
  File:     std_unicode.c

  Copyright 2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: std_unicode.c,v $
  Revision 1.3  08/10/2006 07:53:08  yosizaki
  add STDResult and change API interface.

  Revision 1.2  07/24/2006 12:28:08  yosizaki
  beautify indent.
  change of STD-API interface.

  Revision 1.1  07/05/2006 07:43:43  kitase_hirotake
  Added the sjis <-> unicode conversion library to std

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

#include <nitro/std/unicode.h>
#include "sjis2unicode.h"
#include "unicode2sjis.h"


/*****************************************************************************/
/* function*/

/*---------------------------------------------------------------------------*
  Name:         STD_ConvertStringSjisToUnicode

  Description:  Converts a Shift-JIS character string to a Unicode character string.

  Arguments:    dst               Conversion destination buffer.
                                  The storage process is ignored if NULL is specified.
                dst_len           Pointer that stores and passes the maximum character count for the conversion destination buffer,
                                  and receives the number of characters actually stored.
                                  Ignored when NULL is given.
                src               Conversion source buffer.
                src_len           Pointer that stores and passes the maximum character count that should be converted,
                                  and receives the number of characters actually converted.
                                  The end-of-string position takes priority over this specification.
                                  When a negative value is stored and passed or NULL is given,
                                  the character count is revised to be the number of characters to the end of the string.
                callback          The callback called if there are any characters that can't be converted.
                                  When NULL is specified, the conversion process ends
                                  at the position of the character that cannot be converted.

  Returns:      Result of the conversion process.
 *---------------------------------------------------------------------------*/
STDResult STD_ConvertStringSjisToUnicode(u16 *dst, int *dst_len,
                                         const char *src, int *src_len,
                                         STDConvertUnicodeCallback callback)
{
    STDResult retval = STD_RESULT_SUCCESS;
    if (!src)
    {
        retval = STD_RESULT_INVALID_PARAM;
    }
    else
    {
		int     src_pos = 0;
		int     dst_pos = 0;
		int     src_max = (src_len && (*src_len >= 0)) ? *src_len : 0x7FFFFFFF;
		int     dst_max = (dst && dst_len && (*dst_len >= 0)) ? *dst_len : 0x7FFFFFFF;

        /* Process one character at a time until either of the buffer length reaches the end*/
        while ((dst_pos < dst_max) && (src_pos < src_max))
        {
            u16     dst_tmp[4];
            int     dst_count = 0;
            int     src_count;
            u32     c1 = (u8)src[src_pos];
			/* Prioritize the search for the end point over the string length*/
			if (!c1)
			{
				break;
			}
			/* ASCII [00, 7E] */
			else if (c1 <= 0x7E)
			{
				dst_tmp[0] = (u16)c1;
				src_count = 1;
				dst_count = 1;
			}
			/* Half-width kana [A1, DF]*/
			else if ((c1 >= 0xA1) && (c1 <= 0xDF))
			{
				dst_tmp[0] = (u16)(c1 + (0xFF61 - 0xA1));
				src_count = 1;
				dst_count = 1;
			}
			/* ShiftJIS*/
			else if (STD_IsSjisCharacter(&src[src_pos]))
			{
				src_count = 2;
				if (src_pos + src_count <= src_max)
				{
					/*
					 * The range of conversion table is
					 * First byte { 81 - 9F, E0 -FC }, Second byte { 40 - FF }
					 * To avoid subtraction at table calculation for the sake of efficiency,
					 * the second byte { 7F, FD, FE, FF } will be left and not omitted.
					 */
					u32     c2 = (u8)src[src_pos + 1];
					c1 -= 0x81 + ((c1 >= 0xE0) ? (0xE0 - 0xA0) : 0);
					dst_tmp[0] = sjis2unicode_array[c1 * 0xC0 + (c2 - 0x40)];
					dst_count = (dst_tmp[0] ? 1 : 0);
				}
			}
			/* Call callback if a non-convertible character appear*/
			if (dst_count == 0)
			{
				if (!callback)
				{
					retval = STD_RESULT_CONVERSION_FAILED;
				}
				else
				{
					src_count = src_max - src_pos;
					dst_count = sizeof(dst_tmp) / sizeof(*dst_tmp);
					retval = (*callback) (dst_tmp, &dst_count, &src[src_pos], &src_count);
				}
				if (retval != STD_RESULT_SUCCESS)
				{
					break;
				}
			}
			/* End here if the conversion source or the conversion target goes beyond the end point.*/
			if ((src_pos + src_count > src_max) || (dst_pos + dst_count > dst_max))
			{
				break;
			}
			/* Store the conversion result if the buffer is enabled*/
			if (dst)
			{
				int     i;
				for (i = 0; i < dst_count; ++i)
				{
					MI_StoreLE16(&dst[dst_pos + i], (u16)dst_tmp[i]);
				}
			}
			src_pos += src_count;
			dst_pos += dst_count;
		}
		/* Return the conversion character count*/
		if (src_len)
		{
			*src_len = src_pos;
		}
		if (dst_len)
		{
			*dst_len = dst_pos;
		}
	}
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         STD_ConvertStringUnicodeToSjis

  Description:  Converts a Unicode character string to a ShiftJIS character string.

  Arguments:    dst               Conversion destination buffer.
                                  The storage process is ignored if NULL is specified.
                dst_len           Pointer that stores and passes the maximum character count for the conversion destination buffer,
                                  and receives the number of characters actually stored.
                                  Ignored when NULL is given.
                src               Conversion source buffer.
                src_len           Pointer that stores and passes the maximum character count that should be converted,
                                  and receives the number of characters actually converted.
                                  The end-of-string position takes priority over this specification.
                                  When a negative value is stored and passed or NULL is given,
                                  the character count is revised to be the number of characters to the end of the string.
                callback          The callback called if there are any characters that can't be converted.
                                  When NULL is specified, the conversion process ends
                                  at the position of the character that cannot be converted.

  Returns:      Result of the conversion process.
 *---------------------------------------------------------------------------*/
STDResult STD_ConvertStringUnicodeToSjis(char *dst, int *dst_len,
                                         const u16 *src, int *src_len,
                                         STDConvertSjisCallback callback)
{
    STDResult retval = STD_RESULT_SUCCESS;
    if (!src)
    {
        retval = STD_RESULT_INVALID_PARAM;
    }
    else
    {
		int     src_pos = 0;
		int     dst_pos = 0;
		int     src_max = (src_len && (*src_len >= 0)) ? *src_len : 0x7FFFFFFF;
		int     dst_max = (dst && dst_len && (*dst_len >= 0)) ? *dst_len : 0x7FFFFFFF;
		/* Process one character at a time until either of the buffer length reaches the end*/
		while ((dst_pos < dst_max) && (src_pos < src_max))
		{
			char    dst_tmp[4];
			int     dst_count = 0;
			int     src_count = 1;
			u32     w = MI_LoadLE16(&src[src_pos]);
			/* Prioritize the search for the end point over the string length*/
			if (!w)
			{
				break;
			}
			/* Extended characters (personal area)*/
			else if ((w >= 0xE000) && (w < 0xF8FF))
			{
				const u32 sjis_page = 188UL;
				const u32 offset = w - 0xE000;
				u32     c1 = offset / sjis_page;
				u32     c2 = offset - c1 * sjis_page;
				dst_tmp[0] = (char)(c1 + 0xF0);
				dst_tmp[1] = (char)(c2 + ((c2 < 0x3F) ? 0x40 : 0x41));
				dst_count = 2;
			}
			else
			{
				/*
				 * The range of conversion table is
				 * [0000-0480), [2000-2680), [3000-3400), [4E00-9FA8), [F928-FFE6).
				 * Simply deletes the area filled with 0000 from the largest to smallest and truncates.
				 */
				/* *INDENT-OFF**/
                static const int table[][2] =
                {
                    {0x0000,0x0480 - 0x0000,},
                    {0x2000,0x2680 - 0x2000,},
                    {0x3000,0x3400 - 0x3000,},
                    {0x4E00,0x9FA8 - 0x4E00,},
                    {0xF928,0xFFE6 - 0xF928,},
                };
				enum { table_max = sizeof(table) / (sizeof(int) * 2) };
				/* *INDENT-ON**/
				int     i;
				int     index = 0;
				for (i = 0; i < table_max; ++i)
				{
					const int offset = (int)(w - table[i][0]);
					/* Invalid range*/
					if (offset < 0)
					{
						break;
					}
					/* Valid range*/
					else if (offset < table[i][1])
					{
						index += offset;
						dst_tmp[0] = (char)unicode2sjis_array[index * 2 + 0];
						if (dst_tmp[0])
						{
							dst_tmp[1] = (char)unicode2sjis_array[index * 2 + 1];
							dst_count = dst_tmp[1] ? 2 : 1;
						}
						break;
					}
					/* Other range*/
					else
					{
						index += table[i][1];
					}
				}
			}
			/* Call callback if a non-convertible character appear*/
			if (dst_count == 0)
			{
				if (!callback)
				{
					retval = STD_RESULT_CONVERSION_FAILED;
				}
				else
				{
					src_count = src_max - src_pos;
					dst_count = sizeof(dst_tmp) / sizeof(*dst_tmp);
					retval = (*callback) (dst_tmp, &dst_count, &src[src_pos], &src_count);
				}
				if (retval != STD_RESULT_SUCCESS)
				{
					break;
				}
			}
			/* End here if the conversion source or the conversion target goes beyond the end point.*/
			if ((src_pos + src_count > src_max) || (dst_pos + dst_count > dst_max))
			{
				break;
			}
			/* Store the conversion result if the buffer is enabled*/
			if (dst)
			{
				int     i;
				for (i = 0; i < dst_count; ++i)
				{
					MI_StoreLE8(&dst[dst_pos + i], (u8)dst_tmp[i]);
				}
			}
			src_pos += src_count;
			dst_pos += dst_count;
		}
		/* Return the conversion character count*/
		if (src_len)
		{
			*src_len = src_pos;
		}
		if (dst_len)
		{
			*dst_len = dst_pos;
		}
	}
    return retval;
}
