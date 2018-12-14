//**********************************************************************
//**********************************************************************
//**                                                                                                              **
//**     Copyright (c) 2015 Shanghai Zhaoxin Semiconductor Co., Ltd.                     **
//**                                                                                                              **
//**     Author: Chris J.Wang  --  chrisjwang@zhaoxin.com                                  **
//**********************************************************************
//**********************************************************************


#ifndef __ZX_LIB_FILE_H__
#define __ZX_LIB_FILE_H__


#include <Library/ShellLib.h>


#define NOT_FOUND_STRING 0xFFFFFFFF


VOID
FileTest(CHAR16 * param);

UINTN
GetParamData(CHAR16 *FileName, IN CHAR16 *Cmd, IN OUT CHAR16* Data);

UINTN
GetParamDataX(CHAR16 *FileName, IN CHAR16 *Cmd, UINT8 Column,IN OUT CHAR16* Data);

EFI_STATUS
WriteIIPos(CHAR16 *FileName, CHAR16 *StrWr, UINT64 pos);
EFI_STATUS
WriteII(CHAR16 *FileName, CHAR16 *StrWr);

EFI_STATUS
TestOpenFile(CHAR16 *FileName);


#endif

