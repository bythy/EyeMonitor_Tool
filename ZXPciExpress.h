//**********************************************************************
//**********************************************************************
//**                                                                                                              **
//**     Copyright (c) 2015 Shanghai Zhaoxin Semiconductor Co., Ltd.                     **
//**                                                                                                              **
//**     Author: Chris J.Wang  --  chrisjwang@zhaoxin.com                                  **
//**********************************************************************
//**********************************************************************


#ifndef __ZX_PCI_EXPRESS_H__
#define __ZX_PCI_EXPRESS_H__

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/SortLib.h>


#include <Protocol/PciIo.h>
#include <Library/PciCf8Lib.h>
#include <Library/PciExpressLib.h>
#include <Library/PciLib.h>

EFI_STATUS
EFIAPI
PciScanX(VOID);

EFI_STATUS
PCIeSearchCapOffset(
	IN UINT8 Busx,
	IN UINT8 Devx,
	IN UINT8 Funx,
	IN UINT8 PcieCap,
	IN UINT16 CapId,
	OUT UINT16* pOffset);

VOID
PCIeDumpCfg(
	UINT64 PcieBase,
	UINT8 Busx,
	UINT8 Devx, 
	UINT8 Funx, 
	UINT16 DCount);

VOID
PCIeDumpMem(
	UINT32 MemBase, 
	UINT16 DCount);



#endif
