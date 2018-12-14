//**********************************************************************
//**********************************************************************
//**                                                                                                              **
//**     Copyright (c) 2015 Shanghai Zhaoxin Semiconductor Co., Ltd.                     **
//**                                                                                                              **
//**     Author: Chris J.Wang  --  chrisjwang@zhaoxin.com                                  **
//**********************************************************************
//**********************************************************************

#ifndef __ZX_LIB_RW_H__
#define __ZX_LIB_RW_H__



#pragma pack(1)
typedef struct {
  UINT8 Register;
  UINT8 Function;
  UINT8 Device;
  UINT8 Bus;
  UINT32 ExtendedRegister;
} EFI_PCI_ADDRESS;
#pragma pack()



///////////////////////////////////////////////////////////////////////////////////CFG

VOID AsiaPcieModify8(UINT64 Bar, UINT64 Address, UINT8 Mask, UINT8 Value);
VOID AsiaPcieModify16(UINT64 Bar, UINT64 Address, UINT16 Mask, UINT16 Value);
VOID AsiaPcieModify32(UINT64 Bar, UINT64 Address, UINT32 Mask, UINT32 Value);


UINT8 AsiaPcieRead8(UINT64 Bar, UINT64 Address);
UINT16 AsiaPcieRead16(UINT64 Bar, UINT64 Address);
UINT32 AsiaPcieRead32(UINT64 Bar, UINT64 Address);


VOID AsiaPcieWrite8(UINT64 Bar, UINT64 Address, UINT8 Data);
VOID AsiaPcieWrite16(UINT64 Bar, UINT64 Address, UINT16 Data);
VOID AsiaPcieWrite32(UINT64 Bar, UINT64 Address, UINT32 Data);

///////////////////////////////////////////////////////////////////////////////////IO

VOID AsiaIoModify8(UINT16 Address, UINT8 Mask, UINT8 Value);
VOID AsiaIoModify16(UINT16 Address, UINT16 Mask, UINT16 Value);
VOID AsiaIoModify32(UINT16 Address, UINT32 Mask, UINT32 Value);

UINT8 AsiaIoRead8(UINT16 Address);
UINT16 AsiaIoRead16(UINT16 Address);
UINT32 AsiaIoRead32(UINT16 Address);
VOID AsiaIoWrite8(UINT16 Address, UINT8 Data);
VOID AsiaIoWrite16(UINT16 Address, UINT16 Data);
VOID AsiaIoWrite32(UINT16 Address, UINT32 Data);

///////////////////////////////////////////////////////////////////////////////////Memory

VOID AsiaMemoryWrite8(UINT64 Address, UINT8 Data);
VOID AsiaMemoryWrite16(UINT64 Address, UINT16 Data);
VOID AsiaMemoryWrite32(UINT64 Address, UINT32 Data);

UINT8 AsiaMemoryRead8(UINT64 Address);
UINT16 AsiaMemoryRead16(UINT64 Address);
UINT32 AsiaMemoryRead32(UINT64 Address);


VOID AsiaMemoryModify8(UINT64 Address, UINT8 Mask, UINT8 Value);
VOID AsiaMemoryModify16(UINT64 Address, UINT16 Mask, UINT16 Value);
VOID AsiaMemoryModify32(UINT64 Address, UINT32 Mask, UINT32 Value);

///////////////////////////////////////////////////////////////////////////////////



UINT64 AsiaPciAddress(UINT8 Bus, UINT8 Dev, UINT8 Fun, UINT16 Reg);
UINT64
AsiaGetPcieMmioAddress(UINT64 BaseAddress, UINT64 PciAddress);


#endif
