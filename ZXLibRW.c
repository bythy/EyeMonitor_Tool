//**********************************************************************
//**********************************************************************
//**                                                                                                              **
//**     Copyright (c) 2015 Shanghai Zhaoxin Semiconductor Co., Ltd.                     **
//**                                                                                                              **
//**     Author: Chris J.Wang  --  chrisjwang@zhaoxin.com                                  **
//**********************************************************************
//**********************************************************************

#include "Top.h"
#include "ZXLibRW.h"



UINT8 GetStartBit32(UINT32 Masks)
{
	UINT32 Mx = Masks;
	UINT8 x;
	//0x00FF0000
	if(Mx == 0){
		return 0;
	}

	for(x = 0; x<32; x++){
		if((Mx>>x) == Mx){
			continue;
		}else{
			break;
		}
	}
	
	return x-1;
}

UINT8 GetEndBit32(UINT32 Masks)
{
	UINT32 Mx = Masks;
	UINT8 x;
	//0x00FF0000
	if(Mx == 0){
		return 0;
	}

	for(x = 0; x<32; x++){
		if((Mx<<x) == Mx){
			continue;
		}else{
			break;
		}
	}
	
	return x+1;
}


UINT8 GetStartBit16(UINT16 Masks)
{
	UINT16 Mx = Masks;
	UINT8 x;
	//0x00FF0000
	if(Mx == 0){
		return 0;
	}

	for(x = 0; x<16; x++){
		if((Mx>>x) == Mx){
			continue;
		}else{
			break;
		}
	}
	
	return x-1;
}

UINT8 GetEndBit16(UINT16 Masks)
{
	UINT16 Mx = Masks;
	UINT8 x;
	//0x00FF0000
	if(Mx == 0){
		return 0;
	}

	for(x = 0; x<16; x++){
		if((Mx<<x) == Mx){
			continue;
		}else{
			break;
		}
	}
	
	return x+1;
}


UINT8 GetStartBit8(UINT8 Masks)
{
	UINT8 Mx = Masks;
	UINT8 x;
	//0x00FF0000
	if(Mx == 0){
		return 0;
	}

	for(x = 0; x<8; x++){
		if((Mx>>x) == Mx){
			continue;
		}else{
			break;
		}
	}
	
	return x-1;
}

UINT8 GetEndBit8(UINT8 Masks)
{
	UINT8 Mx = Masks;
	UINT8 x;
	//0x00FF0000
	if(Mx == 0){
		return 0;
	}

	for(x = 0; x<8; x++){
		if((Mx<<x) == Mx){
			continue;
		}else{
			break;
		}
	}
	
	return x+1;
}
//#####################################################
//basic level 1





STATIC UINT32
AsiaGetCf8Value(IN UINT64 PciAddress)
{
  EFI_PCI_ADDRESS* pEfiAddress = (EFI_PCI_ADDRESS*)(&PciAddress);
  UINT8 Register;
  if(pEfiAddress->ExtendedRegister != 0)
    Register = (UINT8)(pEfiAddress->ExtendedRegister); // Just cut to byte
  else
    Register = pEfiAddress->Register;
  return (0x80000000 | (pEfiAddress->Bus << 16) | (pEfiAddress->Device << 11) |
    (pEfiAddress->Function << 8) | Register);
}

UINT64
AsiaGetPcieMmioAddress(UINT64 BaseAddress, UINT64 PciAddress)
{
  EFI_PCI_ADDRESS* pEfiAddress = (EFI_PCI_ADDRESS*)(&PciAddress);
  UINT32 Register;
  if(pEfiAddress->ExtendedRegister != 0)
    Register = (pEfiAddress->ExtendedRegister & 0x0FFF); // Just cut within 4k
  else
    Register = pEfiAddress->Register;
  return (BaseAddress | (pEfiAddress->Bus << 20) | (pEfiAddress->Device << 15) |
    (pEfiAddress->Function << 12) | Register);
}




UINT64 AsiaPciAddress(UINT8 Bus, UINT8 Dev, UINT8 Fun, UINT16 Reg)
{
  EFI_PCI_ADDRESS PciAddr;
  UINT64* pResult;
  PciAddr.Bus = Bus;
  PciAddr.Device = Dev;
  PciAddr.Function = Fun;
  if(Reg > 0xFF)
  {
    PciAddr.ExtendedRegister = Reg;
    PciAddr.Register = 0;
  }
  else
  {
    PciAddr.Register = (UINT8)Reg;
    PciAddr.ExtendedRegister = 0;
  }
  pResult = (UINT64*)(&(PciAddr));
  return *pResult;
}

//#####################################################
//basic level 2






//#####################################################
//top to use
#if 0
VOID AsiaPcieModify8(UINT64 Bar, UINT64 Address, UINT8 Mask, UINT8 Value)
{ AsiaMemoryModify8(AsiaGetPcieMmioAddress(Bar, Address), Mask, Value); }
VOID AsiaPcieModify16(UINT64 Bar, UINT64 Address, UINT16 Mask, UINT16 Value)
{ AsiaMemoryModify16(AsiaGetPcieMmioAddress(Bar, Address), Mask, Value); }
VOID AsiaPcieModify32(UINT64 Bar, UINT64 Address, UINT32 Mask, UINT32 Value)
{ AsiaMemoryModify32(AsiaGetPcieMmioAddress(Bar, Address), Mask, Value); }




// Pcie read
UINT8 AsiaPcieRead8(UINT64 Bar, UINT64 Address)
{ return AsiaMemoryRead8(AsiaGetPcieMmioAddress(Bar, Address)); }
UINT16 AsiaPcieRead16(UINT64 Bar, UINT64 Address)
{ return AsiaMemoryRead16(AsiaGetPcieMmioAddress(Bar, Address)); }
UINT32 AsiaPcieRead32(UINT64 Bar, UINT64 Address)
{ return AsiaMemoryRead32(AsiaGetPcieMmioAddress(Bar, Address)); }



VOID AsiaPcieWrite8(UINT64 Bar, UINT64 Address, UINT8 Data)
{ AsiaMemoryWrite8(AsiaGetPcieMmioAddress(Bar, Address), Data); }
VOID AsiaPcieWrite16(UINT64 Bar, UINT64 Address, UINT16 Data)
{ AsiaMemoryWrite16(AsiaGetPcieMmioAddress(Bar, Address), Data); }
VOID AsiaPcieWrite32(UINT64 Bar, UINT64 Address, UINT32 Data)
{ AsiaMemoryWrite32(AsiaGetPcieMmioAddress(Bar, Address), Data); }



UINT8 AsiaIoRead8(UINT16 Address)
{
	return IoRead8(Address);

}
UINT16 AsiaIoRead16(UINT16 Address)
{
	return IoRead16(Address);
}
UINT32 AsiaIoRead32(UINT16 Address)
{
	return IoRead32(Address);
}

VOID AsiaIoWrite8(UINT16 Address, UINT8 Data)
{
	IoWrite8(Address,Data);
}
VOID AsiaIoWrite16(UINT16 Address, UINT16 Data)
{
	IoWrite16(Address,Data);
}
VOID AsiaIoWrite32(UINT16 Address, UINT32 Data)
{
	IoWrite32(Address,Data);
}


VOID AsiaIoModify8(UINT16 Address, UINT8 Mask, UINT8 Value)
{
  UINT8 D8 = AsiaIoRead8(Address);
  D8 = (D8 & (~Mask) | Value);
  AsiaIoWrite8(Address, D8);
}
VOID AsiaIoModify16(UINT16 Address, UINT16 Mask, UINT16 Value)
{
  UINT16 D16 = AsiaIoRead16(Address);
  D16 = (D16 & (~Mask) | Value);
  AsiaIoWrite16(Address, D16);
}
VOID AsiaIoModify32(UINT16 Address, UINT32 Mask, UINT32 Value)
{
  UINT32 D32 = AsiaIoRead32(Address);
  D32 = (D32 & (~Mask) | Value);
  AsiaIoWrite32(Address, D32);
}
#else

VOID AsiaPcieModify8(UINT64 Bar, UINT64 Address, UINT8 Mask, UINT8 Value)
{ 
	UINT8 D8;
	D8 = AsiaPcieRead8(Bar, Address);
	D8 = (D8&(~Mask))|(Value);
	AsiaPcieWrite8(Bar, Address, D8);
}
VOID AsiaPcieModify16(UINT64 Bar, UINT64 Address, UINT16 Mask, UINT16 Value)
{ 
	UINT16 D16;
	D16 = AsiaPcieRead16(Bar, Address);
	D16 = (D16&(~Mask))|(Value);
	AsiaPcieWrite16(Bar, Address, D16);
}
VOID AsiaPcieModify32(UINT64 Bar, UINT64 Address, UINT32 Mask, UINT32 Value)
{ 
	UINT32 D32;
	D32 = AsiaPcieRead32(Bar, Address);
	D32 = (D32&(~Mask))|(Value);
	AsiaPcieWrite32(Bar, Address, D32);
}



// Pcie read
UINT8 AsiaPcieRead8(UINT64 Bar, UINT64 Address)
{
//Print(L"CC-0x%08x\n",AsiaGetPcieMmioAddress(Bar, Address));
return MmioRead8(AsiaGetPcieMmioAddress(Bar, Address));
}
UINT16 AsiaPcieRead16(UINT64 Bar, UINT64 Address)
{ return MmioRead16(AsiaGetPcieMmioAddress(Bar, Address));
}
UINT32 AsiaPcieRead32(UINT64 Bar, UINT64 Address)
{ return MmioRead32(AsiaGetPcieMmioAddress(Bar, Address));
}



VOID AsiaPcieWrite8(UINT64 Bar, UINT64 Address, UINT8 Data)
{ MmioWrite8(AsiaGetPcieMmioAddress(Bar, Address), Data); 
}
VOID AsiaPcieWrite16(UINT64 Bar, UINT64 Address, UINT16 Data)
{ MmioWrite16(AsiaGetPcieMmioAddress(Bar, Address), Data);
}
VOID AsiaPcieWrite32(UINT64 Bar, UINT64 Address, UINT32 Data)
{ MmioWrite32(AsiaGetPcieMmioAddress(Bar, Address),Data);
}




UINT8 AsiaIoRead8(UINT16 Address)
{
	return IoBitFieldRead8(Address, 0, 7);
	//return IoRead8(Address);

}
UINT16 AsiaIoRead16(UINT16 Address)
{
	return IoBitFieldRead16(Address, 0, 15);
	//return IoRead16(Address);
}
UINT32 AsiaIoRead32(UINT16 Address)
{
	return IoBitFieldRead32(Address, 0, 31);
	//return IoRead32(Address);
}

VOID AsiaIoWrite8(UINT16 Address, UINT8 Data)
{
	IoBitFieldWrite8(Address, 0, 7, Data);
	//IoWrite8(Address,Data);
}
VOID AsiaIoWrite16(UINT16 Address, UINT16 Data)
{
	IoBitFieldWrite16(Address, 0, 15, Data);
	//IoWrite16(Address,Data);
}
VOID AsiaIoWrite32(UINT16 Address, UINT32 Data)
{
	IoBitFieldWrite32(Address, 0, 31, Data);
	//IoWrite32(Address,Data);
}


VOID AsiaIoModify8(UINT16 Address, UINT8 Mask, UINT8 Value)
{
  UINT8 D8 = AsiaIoRead8(Address);
  D8 = (D8 & (~Mask) | Value);
  AsiaIoWrite8(Address, D8);
}
VOID AsiaIoModify16(UINT16 Address, UINT16 Mask, UINT16 Value)
{
  UINT16 D16 = AsiaIoRead16(Address);
  D16 = (D16 & (~Mask) | Value);
  AsiaIoWrite16(Address, D16);
}
VOID AsiaIoModify32(UINT16 Address, UINT32 Mask, UINT32 Value)
{
  UINT32 D32 = AsiaIoRead32(Address);
  D32 = (D32 & (~Mask) | Value);
  AsiaIoWrite32(Address, D32);
}



VOID AsiaMemoryWrite8(UINT64 Address, UINT8 Data)
{ MmioWrite8(Address,Data); }
VOID AsiaMemoryWrite16(UINT64 Address, UINT16 Data)
{ MmioWrite16(Address,Data); }
VOID AsiaMemoryWrite32(UINT64 Address, UINT32 Data)
{ MmioWrite32(Address,Data); }


UINT8 AsiaMemoryRead8(UINT64 Address)
{ return MmioRead8(Address); }
UINT16 AsiaMemoryRead16(UINT64 Address)
{ return MmioRead16(Address); }
UINT32 AsiaMemoryRead32(UINT64 Address)
{ return MmioRead32(Address); }


VOID AsiaMemoryModify8(UINT64 Address, UINT8 Mask, UINT8 Value)
{
  UINT8 D8 = AsiaMemoryRead8(Address);
  D8 = (D8 & (~Mask) | Value);
  AsiaMemoryWrite8(Address, D8);
}
VOID AsiaMemoryModify16(UINT64 Address, UINT16 Mask, UINT16 Value)
{
  UINT16 D16 = AsiaMemoryRead16(Address);
  D16 = (D16 & (~Mask) | Value);
  AsiaMemoryWrite16(Address, D16);
}
VOID AsiaMemoryModify32(UINT64 Address, UINT32 Mask, UINT32 Value)
{
  UINT32 D32 = AsiaMemoryRead32(Address);
  D32 = (D32 & (~Mask) | Value);
  AsiaMemoryWrite32(Address, D32);
}


#endif


