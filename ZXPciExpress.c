//**********************************************************************
//**********************************************************************
//**                                                                                                              **
//**     Copyright (c) 2015 Shanghai Zhaoxin Semiconductor Co., Ltd.                     **
//**                                                                                                              **
//**     Author: Chris J.Wang  --  chrisjwang@zhaoxin.com                                  **
//**********************************************************************
//**********************************************************************


#include "Top.h"
#include "ZXPciExpress.h"



/**
@Use this function to search register offset of Specific capability
@Param:
	Busx,Devx,Funx: device routing ID
	PcieCap: when 1, search PCIE capability, when 0,search PCI capability
	CapId: target capability ID
@Return:
	Offset: register offset of target capability
	EFI_STATUS: search result, 
		when EFI_NOT_FOUND, not found capability
		when EFI_DEVICE_ERROR, device register abnormal
**/
EFI_STATUS
PCIeSearchCapOffset(
	IN UINT8 Busx,
	IN UINT8 Devx,
	IN UINT8 Funx,
	IN UINT8 PcieCap,
	IN UINT16 CapId,
	OUT UINT16* pOffset)
{
	EFI_STATUS Statusx = EFI_SUCCESS;
	UINT64 PcieBar = 0xE0000000;
	
	UINT8 CapIdP = 0xFF;
	UINT16 CapIdPe= 0xFF;
	
	UINT8 NextPtrP;
	UINT16 NextPtrPe;

	
	if(PcieCap){
		CapIdPe = CapId;
	}else{
		CapIdP = (UINT8) CapId; 
	}

	if(PcieCap){
		NextPtrPe = 0x100;
		while(NextPtrPe<0xFFF){
			Print(L"       NextPtrP = 0x%x\n",NextPtrPe);	
			if(CapIdPe ==  AsiaPcieRead16(PcieBar, AsiaPciAddress(Busx,Devx,Funx,NextPtrPe))){
				//found it!
				*pOffset = NextPtrPe;
				break;
			}else{
				NextPtrPe = AsiaPcieRead16(PcieBar, AsiaPciAddress(Busx,Devx,Funx,NextPtrPe+2));
				NextPtrPe = NextPtrPe>>4;			
				if(NextPtrPe == 0){
					//end of this cap list
					*pOffset = 0;
					Statusx = EFI_NOT_FOUND;
					break;
				}
			}
		}

	}else{
		NextPtrP = AsiaPcieRead8(PcieBar, AsiaPciAddress(Busx,Devx,Funx,0x34));
		if((NextPtrP == 0xFF) ||(NextPtrP == 0x00)){
			Statusx = EFI_DEVICE_ERROR;
			goto __DevCapExit;
		}		
		while(NextPtrP<0xFF){
			Print(L"       NextPtrP = 0x%x\n",NextPtrP);	
			if(CapIdP ==  AsiaPcieRead8(PcieBar, AsiaPciAddress(Busx,Devx,Funx,NextPtrP))){
				//found it!
				*pOffset = (UINT16)NextPtrP;
				break;
			}else{
				NextPtrP = AsiaPcieRead8(PcieBar, AsiaPciAddress(Busx,Devx,Funx,NextPtrP+1));							
				if(NextPtrP == 0){
					//end of this cap list
					*pOffset = 0;
					Statusx = EFI_NOT_FOUND;
					break;
				}
			}
		}

	}
		
__DevCapExit:	
	if(Statusx != EFI_SUCCESS){
		Print(L"       [%d:%d.%d]CapId(%04xh) Not Found!\n",Busx,Devx,Funx,CapId);				
	}else{
		Print(L"       [%d:%d.%d]CapId(%04xh) at Rx%x\n",Busx,Devx,Funx,CapId,*pOffset);			
	}

	return Statusx;
}


#if 1
VOID
PCIeDumpCfg(
	UINT64 PcieBase,
	UINT8 Busx,
	UINT8 Devx, 
	UINT8 Funx, 
	UINT16 DCount)
{

	UINT16 offsetx = 0;
	UINT8 readback8;
	Print(L"[PCIE] DumpCfg([%d|%d|%d] Count:0x%08x):\n",Busx,Devx,Funx,DCount);
	Print(L"\n0x%04x: ",offsetx+1);
	for(offsetx = 0; offsetx < DCount; offsetx++){
		readback8 = AsiaPcieRead8(PcieBase, AsiaPciAddress(Busx, Devx, Funx, offsetx));
		Print(L"%02X ",readback8);
		if(offsetx != (DCount -1)){
			if((offsetx & 0xFF) == 0xFF){
				Print(L"\n\n        00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
				Print(L"\n        -----------------------------------------------");
			}
		}
		if((offsetx & 0xF) == 0xF){
			Print(L"\n0x%04x: ",offsetx+1);
		}
		
	}
	Print(L"\n");
}
#endif

#if 1
VOID
PCIeDumpMem(
	UINT32 MemBase, 
	UINT16 DCount)
{

	UINT16 offsetx = 0;
	UINT8 readback8;
	Print(L"[PCIE] DumpMem(Base:0x%08x, Count:0x%04x):\n",MemBase, DCount);
	Print(L"\n0x%04x: ",offsetx+1);
	for(offsetx = 0; offsetx < DCount; offsetx++){
		readback8 = AsiaMemoryRead8(MemBase + offsetx);
		Print(L"%02X ",readback8);
		if(offsetx != (DCount -1)){
			if((offsetx & 0xFF) == 0xFF){
				Print(L"\n\n        00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
				Print(L"\n        -----------------------------------------------");
			}
		}	
		if((offsetx & 0xF) == 0xF){
			Print(L"\n0x%04x: ",offsetx+1);
		}
		
	}
	Print(L"\n");
}

#endif

#if 1
EFI_STATUS
EFIAPI
PciScanX(VOID)
{
	UINT16 LocalVid;
	UINT32 LocalAER;
	UINT16 Addrx;
	LocalVid = PciCf8Read16(PCI_CF8_LIB_ADDRESS(0,2,0, 0x0));
	Print(L"VID = %04x\n", LocalVid);
	
	
	LocalAER = PciRead32(PCI_LIB_ADDRESS (0, 2, 0, 0x100));
	Print(L"AER = %08x\n", LocalAER);


	//MmioRead8(IN UINTN Address);
	

	PCIeSearchCapOffset(0, 2, 0, 0, 0x10, &Addrx);
	Print(L"Cap ID = 10h , Offset = 0x%x\n", Addrx);

	AsiaIoWrite8(0x80, 0x82);
	Print(L"IO 80 = 82\n");
	gBS->Stall(2000000);
	AsiaIoWrite8(0x80, 0x83);
	Print(L"IO 80 = 83\n");

	
	PCIeDumpCfg(0xE0000000, 0,2,0, 0x100);
	PCIeDumpMem(0xFE014000,0x100);
		
	return EFI_SUCCESS;
}
#endif




