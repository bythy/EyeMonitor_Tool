//**********************************************************************
//**********************************************************************
//**                                                                                                              **
//**     Copyright (c) 2015 Shanghai Zhaoxin Semiconductor Co., Ltd.                     **
//**                                                                                                              **
//**     Author: Chris J.Wang  --  chrisjwang@zhaoxin.com                                  **
//**********************************************************************
//**********************************************************************


#ifndef __ZX_EYE_MONITOR_MODE3_TEST_H__
#define __ZX_EYE_MONITOR_MODE3_TEST_H__

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/SortLib.h>

VOID
DebugMode3_SinglePresetScan(UINT32 EphyBase, UINT8 LaneNum,UINT8 Presetx);


VOID
DebugMode3_SingleCursorScan(UINT32 EphyBase, UINT8 LaneNum,UINT8 Pre,UINT8 Cur,UINT8 Pos);


VOID
DebugMode3_AllCursorScan(UINT32 EphyBase, UINT8 LaneNum);

VOID
DebugMode3_SetRxEqAutoMode(IN CHAR16 **param);




VOID
RxEqAutoModeParam(UINTN BaseAddr,UINT8 LaneNum,UINT16 P1,UINT16 P2,UINT16 P3,UINT16 P4,UINT16 P5 );

VOID
RxEqAutoMode(UINTN BaseAddr,UINT8 LaneNum,UINT8 DC, UINT8 HF, UINT8 OS, UINT8 W1, UINT8 W2);

VOID
RxEqAutoModeOrgEQTNVTH(UINTN BaseAddr,UINT8 LaneNum,UINT16 Valuex);





#endif
