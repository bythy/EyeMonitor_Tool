//**********************************************************************
//**********************************************************************
//**                                                                                                              **
//**     Copyright (c) 2015 Shanghai Zhaoxin Semiconductor Co., Ltd.                     **
//**                                                                                                              **
//**     Author: Chris J.Wang  --  chrisjwang@zhaoxin.com                                  **
//**********************************************************************
//**********************************************************************
#ifndef __ZX_LIB_STRING_H__
#define __ZX_LIB_STRING_H__


#define TAB  0x9



//int strcmp(const char *dest, const char *source);
char Char16Char8(IN CHAR16 *in, OUT char *out);


//char *strcpy(char *strDest, const char *strScr);
//int strcmp (const char *str1,const char *str2);




UINT32 my_strhex2dec_16(CHAR16 *str);

UINT32 my_strdec2dec_16(CHAR16 *str);


UINT32 
Str2Num(CHAR16 *str);

EFI_STATUS
Str2NumBitField(
	IN CHAR16 * input, 
	IN OUT UINT32 *FieldMask, 
	IN OUT UINT8 *BaseBit,
	IN OUT UINT8 *EndBit,
	IN OUT UINT32 *FirstPart);

#endif