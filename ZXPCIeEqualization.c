//**********************************************************************
//**********************************************************************
//**                                                                                                              **
//**     Copyright (c) 2015 Shanghai Zhaoxin Semiconductor Co., Ltd.                     **
//**                                                                                                              **
//**     Author: Chris J.Wang  --  chrisjwang@zhaoxin.com                                  **
//**********************************************************************
//**********************************************************************


#include "ZXPCIeEqualization.h"

#include "Top.h"



extern M3StFEL Mode3Def[]; 



EPHYCFG LanetoPort[NUM_EPHYCFG] = {
	{0,{PE6, PE6,PE4,PE4,PE4,PE4,PE4,PE4,PE4,PE4,PE0,PE0,PE0,PE0,PE2,PE3}},
	{1,{PE6, PE7,PE4,PE4,PE4,PE4,PE4,PE4,PE4,PE4,PE0,PE0,PE0,PE0,PE2,PE2}},
	{2,{PE6, PE7,PE4,PE4,PE4,PE4,PE5,PE5,PE5,PE5,PE0,PE0,PE0,PE0,PENA,PENA}},
	{3,{PE6, PE7,PE4,PE4,PE4,PE4,PE4,PE4,PE4,PE4,PE0,PE0,PE2,PE3,PENA,PENA}},
	{4,{PE6, PE7,PE4,PE4,PE4,PE4,PE4,PE4,PE4,PE4,PE0,PE1,PE2,PE3,PENA,PENA}},
	{5,{PE6, PE7,PE4,PE4,PE4,PE4,PE5,PE5,PE5,PE5,PE0,PE1,PE2,PE3,PENA,PENA}},
	{6,{PE6, PE7,VPI,VPI,VPI,VPI,VPI,VPI,VPI,VPI,PE0,PE0,PE0,PE0,PE4,PE5}},
	{7,{PE6, PE7,VPI,VPI,VPI,VPI,VPI,VPI,VPI,VPI,PE0,PE0,PE0,PE0,PENA,PENA}},
	{8,{PE6, PE6,VPI,VPI,VPI,VPI,VPI,VPI,VPI,VPI,PE0,PE1,PE2,PE3,PE4,PE5}},
	{9,{PE6, PE6,VPI,VPI,VPI,VPI,VPI,VPI,VPI,VPI,PE0,PE1,PE2,PE3,PENA,PENA}},
	{10,{PE6, PE7,PE4,PE4,PE4,PE4,PE4,PE4,PE4,PE4,PE0,PE0,PE0,PE0,PENA,PENA}},
	{11,{PE6, PE6,PE4,PE4,PE4,PE4,PE4,PE4,PE4,PE4,PE0,PE0,PE0,PE0,PENA,PENA}},
};






//define the window conmunicated with PEMCU
RFD WindowDef[] = {
	{PCIEPHYCFG_EQ_INT_TO_MCU_Z3,0,0}, //DOORBELL_C2P
	{PCIEPHYCFG_EQ_INT_TO_MCU_Z6,0,31}, //SCRATCH_REG_C2P
	{PCIEPHYCFG_PEMCU_USE_REG_0,0,7}, //FS_FAREND
	{PCIEPHYCFG_PEMCU_USE_REG_0+1,0,7}, //LF_FAREND
	{PCIEPHYCFG_EQ_INT_TO_MCU_Z4,0,31}, //SCRATCH_REG_P2C
	{PCIEPHYCFG_PCIE_ROMSIP_REG,0,4}, //EPHYCFG_REG
};




//
// input: 
//		BaseAddr: EPHY base address
//		lane number
// output : 
//		corresponding Port
//			[7:4]=DevNum   [3:0]=FuncNum    BusNum=0
//
UINT8 
EQ_LaneMaptoPort(UINT32 BaseAddr, UINT8 LaneNum)
{
	UINT8 EphyCfg;
	UINT8 Portx;
	RFD *LocalReg = WindowDef;
	EPHYCFG *pLaneMap = LanetoPort;

	EphyCfg = MmioBitFieldRead8(BaseAddr+((LocalReg+EPHYCFG_REG)->Offset), 
								((LocalReg+EPHYCFG_REG)->start), 
								((LocalReg+EPHYCFG_REG)->end));

	Portx = ((pLaneMap+EphyCfg)->LanePortMap[LaneNum]);
	Print(L"Lane%d  coresponding to PCIE Port D%dF%d\n",LaneNum,0xF&(Portx>>4),0xF&Portx);
	return Portx;
}



//
//
//
EFI_STATUS 
EQ_HotResetGen3Confirm(UINT32 BaseAddr, UINT8 LaneNum)
{
	UINT8 Porty;
	UINT8 GenSpeed;
	UINT8 Devx,Funcx;
	UINT8 Ltssm = 0;
	
	Porty = EQ_LaneMaptoPort(BaseAddr, LaneNum);
	Devx = 0xF&(Porty>>4);
	Funcx = 0xF&Porty;

	GenSpeed = AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,Devx,Funcx,0x52));
	GenSpeed = GenSpeed & 0xF;
	Ltssm =  AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,Devx,Funcx,0x1C3));
	Print(L"Before EQ: Gen%d, LTSSM:0x%02x\n",GenSpeed,Ltssm);

	//if current is Gen1/2, after Hotreset, will Gen3 EQ occurs?


	//Set Rx2A4[0] = 1 to request Equalization
	AsiaPcieModify8(0xE0000000, AsiaPciAddress(0,Devx,Funcx, 0x2A4),BIT0, BIT0);


	
	AsiaPcieModify8(0xE0000000, AsiaPciAddress(0,Devx,Funcx, 0x3E),BIT6, BIT6);
	gBS->Stall(500000);
	AsiaPcieModify8(0xE0000000, AsiaPciAddress(0,Devx,Funcx, 0x3E),BIT6, 0);
	gBS->Stall(100000);

	GenSpeed = AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,Devx,Funcx,0x52));
	GenSpeed = GenSpeed & 0xF;
	Ltssm =  AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,Devx,Funcx,0x1C3));
	Print(L"After HotReset: Gen%d,  LTSSM:0x%02x\n",GenSpeed,Ltssm);

	if(GenSpeed == 3){
		return EFI_SUCCESS;
	}else{
		return EFI_ABORTED;
	}
}



//
//the normal sequence is:
//Read FarendFS/LF(this function)  --> Set Param to PEMCU--> HotReset to Redo EQ --> Verify Farend Param
//
UINT8
EQ_GetFs_SpecificLane(UINT32 BaseAddr,UINT8 LaneNum)
{
	M3StFEL *pMode3Reg = Mode3Def;
	UINT8 Valuex = 0;
	Valuex = MmioBitFieldRead8(BaseAddr+((pMode3Reg+LaneNum)->Reg[FAREND_FS].Offset), 
								((pMode3Reg+LaneNum)->Reg[FAREND_FS].start), 
								((pMode3Reg+LaneNum)->Reg[FAREND_FS].end));
	
	return Valuex;
}

//
//the normal sequence is:
//Read FarendFS/LF(this function)  --> Set Param to PEMCU--> HotReset to Redo EQ --> Verify Farend Param
//
UINT8
EQ_GetLf_SpecificLane(UINT32 BaseAddr,UINT8 LaneNum)
{
	M3StFEL *pMode3Reg = Mode3Def;
	UINT8 Valuex = 0;
	Valuex = MmioBitFieldRead8(BaseAddr+((pMode3Reg+LaneNum)->Reg[FAREND_LF].Offset), 
								((pMode3Reg+LaneNum)->Reg[FAREND_LF].start), 
								((pMode3Reg+LaneNum)->Reg[FAREND_LF].end));
	
	return Valuex;
}


//
//the normal sequence is:
//Read FarendFS/LF --> Set Param to PEMCU(this function) --> HotReset to Redo EQ --> Verify Farend Param
//
EFI_STATUS
EQ_SendCmdToAdjustTxParam(UINT32 EphyBase, FTPC *pFtpc)
{
	FTPC *pLocal = pFtpc;
	RFD *LocalReg = WindowDef;

	UINT8 Vfs = 0,Vlf = 0;
	UINT32 Result = 0;

	Print(L"[PEMCU]  Start\n");
	if(pLocal->Cmd == CMD_Cursor){
		Print(L"[PEMCU]  Cmd:%d, Pre:%d, Cur:%d, Post:%d\n",pLocal->Cmd,pLocal->PresetPreCursor,pLocal->Cursor,pLocal->PostCursor);
	}
	if(pLocal->Cmd == CMD_TxPreset){
		Print(L"[PEMCU]  Cmd:%d, Preset:P%d\n",pLocal->Cmd,pLocal->Cursor);
	}
	
	//clear Result flag	
	AsiaMemoryModify8(EphyBase+((LocalReg+SCRATCH_REG_P2C)->Offset), 
						0xFF,0x00);

	//CMD cycle
	AsiaMemoryWrite8(EphyBase+( (LocalReg+SCRATCH_REG_C2P)->Offset), pLocal->Cmd);
	AsiaMemoryWrite8(EphyBase+( (LocalReg+SCRATCH_REG_C2P)->Offset)+1, pLocal->PresetPreCursor);

	//doorbell
	AsiaMemoryWrite8(EphyBase+( (LocalReg+DOORBELL_C2P)->Offset),
								1<<(( (LocalReg+DOORBELL_C2P)->start)));

	while(AsiaMemoryRead8(EphyBase+( (LocalReg+DOORBELL_C2P)->Offset)) & BIT0){
		gBS->Stall(100000);
		Print(L"[CMD cycle] wait doorbell cleared\n");
	}

	//Read FS/LF
	Vfs = MmioBitFieldRead8(EphyBase+((LocalReg+FS_FAREND)->Offset), 
								((LocalReg+FS_FAREND)->start), 
								((LocalReg+FS_FAREND)->end));
	Vlf = MmioBitFieldRead8(EphyBase+((LocalReg+LF_FAREND)->Offset), 
								((LocalReg+LF_FAREND)->start), 
								((LocalReg+LF_FAREND)->end));
	Print(L"[CMD cycle] Farend-FS:0x%02x(%d)    Farend-LF:0x%02x(%d)\n",Vfs,Vfs,Vlf,Vlf);
	
	
	//DATA cycle
	AsiaMemoryWrite8(EphyBase+( (LocalReg+SCRATCH_REG_C2P)->Offset), pLocal->Cursor);
	AsiaMemoryWrite8(EphyBase+( (LocalReg+SCRATCH_REG_C2P)->Offset)+1, pLocal->PostCursor);

	//doorbell
	AsiaMemoryWrite8(EphyBase+( (LocalReg+DOORBELL_C2P)->Offset),
								1<<(( (LocalReg+DOORBELL_C2P)->start)));


	while(AsiaMemoryRead8(EphyBase+( (LocalReg+DOORBELL_C2P)->Offset)) & BIT0){
		gBS->Stall(100000);
		Print(L"[DATA cycle] wait doorbell cleared\n");
	}

	//read rersult
	Result = MmioBitFieldRead32(EphyBase+((LocalReg+SCRATCH_REG_P2C)->Offset), 
								((LocalReg+SCRATCH_REG_P2C)->start), 
								((LocalReg+SCRATCH_REG_P2C)->end));
	Result  = Result & 0xFF;
	if(Result == 1){  //pass
		Print(L"[PEMCU] Done! \n");
		return EFI_SUCCESS;
	}else if(Result == 2){ //fail
		Print(L"[PEMCU] Cmd Failed! \n");
		return EFI_ABORTED;
	}else{	//unknown
		Print(L"[PEMCU] No Reply! (0x%02x)\n",Result);
		return EFI_UNSUPPORTED;
	}
}


#if 0
EFI_STATUS
EQ_xx(UINT32 EphyBase ,FTPC *pFtpc)
{
	EFI_STATUS Statusx = EFI_SUCCESS;
	
	FTPC *pLocal= pFtpc;
	
	
	Statusx = EQ_SendCmdToAdjustTxParam(EphyBase, pLocal);
	if(Statusx == EFI_SUCCESS){
		//verify whether 
	

		
	}else{
	
		Print(L"Failed!\n");
		return EFI_ABORTED;
	}


	return EFI_SUCCESS;
}
#endif

//
//the normal sequence is:
//Read FarendFS/LF --> Set Param to PEMCU --> HotReset to Redo EQ --> Verify Farend Param(this function)
//
EFI_STATUS
EQ_VerifyFarendParam(UINT32 BaseAddr,UINT8 LaneNum, FTPC *pFtpc)
{
	FTPC *pLocal= pFtpc;
	UINT8 Farend_Param = 0;
	M3StFEL *pMode3Reg = Mode3Def;
	EFI_STATUS Statusx = EFI_SUCCESS;


	//
	// Preset
	//
	if (pLocal->Cmd == CMD_TxPreset){
		Farend_Param = MmioBitFieldRead8(BaseAddr+((pMode3Reg+LaneNum)->Reg[FAREND_TX_PRESET].Offset), 
					((pMode3Reg+LaneNum)->Reg[FAREND_TX_PRESET].start), 
					((pMode3Reg+LaneNum)->Reg[FAREND_TX_PRESET].end));
		Print(L"[FAREND] TxPreset = 0x%02x(%d)  - expect:0x%02x\n",Farend_Param,Farend_Param,pLocal->Cursor);
		
		if(Farend_Param == (pLocal->Cursor)){
			Statusx = EFI_SUCCESS;
		}else{
			Statusx = EFI_ABORTED;
			goto __Exitx;
		}
	}

	//
	// Cursor
	//
	if (pLocal->Cmd == CMD_Cursor){
		Farend_Param = MmioBitFieldRead8(BaseAddr+((pMode3Reg+LaneNum)->Reg[FAREND_TX_PRE_CURSOR].Offset), 
					((pMode3Reg+LaneNum)->Reg[FAREND_TX_PRE_CURSOR].start), 
					((pMode3Reg+LaneNum)->Reg[FAREND_TX_PRE_CURSOR].end));
		Print(L"[FAREND] Tx PreCursor = 0x%02x(%d)  - expect:0x%02x\n",Farend_Param,Farend_Param,pLocal->PresetPreCursor);
		
		if(Farend_Param == (pLocal->PresetPreCursor)){
			Statusx = EFI_SUCCESS;
		}else{
			Statusx = EFI_ABORTED;
			goto __Exitx;
		}


		Farend_Param = MmioBitFieldRead8(BaseAddr+((pMode3Reg+LaneNum)->Reg[FAREND_TX_CURSOR].Offset), 
					((pMode3Reg+LaneNum)->Reg[FAREND_TX_CURSOR].start), 
					((pMode3Reg+LaneNum)->Reg[FAREND_TX_CURSOR].end));
		Print(L"[FAREND] Tx Cursor = 0x%02x(%d)  - expect:0x%02x\n",Farend_Param,Farend_Param,pLocal->Cursor);
		
		if(Farend_Param == (pLocal->Cursor)){
			Statusx = EFI_SUCCESS;
		}else{
			Statusx = EFI_ABORTED;
			goto __Exitx;
		}


		Farend_Param = MmioBitFieldRead8(BaseAddr+((pMode3Reg+LaneNum)->Reg[FAREND_TX_POST_CURSOR].Offset), 
					((pMode3Reg+LaneNum)->Reg[FAREND_TX_POST_CURSOR].start), 
					((pMode3Reg+LaneNum)->Reg[FAREND_TX_POST_CURSOR].end));
		Print(L"[FAREND] Tx PostCursor = 0x%02x(%d)  - expect:0x%02x\n",Farend_Param,Farend_Param,pLocal->PostCursor);
		
		if(Farend_Param == (pLocal->PostCursor)){
			Statusx = EFI_SUCCESS;
		}else{
			Statusx = EFI_ABORTED;
			goto __Exitx;
		}

		
	}

	Print(L"[FAREND] Meet Expection!\n\n");
	return Statusx;

__Exitx:
	Print(L"[FAREND] NOT Meet Expection!\n\n");
	return Statusx;
	
}



















//////////////////////////////////////////////////////////////////////////////
//Those code was for CHX002 only

EFI_STATUS
EQ_PcieEyeMonitorDbg(VOID)
{
	UINT64 PemcuDSRAMBase;
	UINT16 ix,ErrorCounter = 0;
	UINT16 EmTimeS[8] = {0};
	UINT16 EmTimeE[8] = {0};
	UINT16 PortDetailOffset ;
	UINT8 CursorDetail[8][4]={0};
	UINT16 iy;

	UINT8 secbus;

	
	Print(L"[CJW_PCIE_EYE_DEBUG] START\n");
	
	PemcuDSRAMBase = AsiaMemoryRead32(EPHYBASE+PCIEPHYCFG_BASE_ADR_OF_PEMCU_FW_FOR_DATA_SPACE);
	PemcuDSRAMBase = PemcuDSRAMBase<<16;
	PCIeDumpMem((UINT32)PemcuDSRAMBase+0x1000,0x100);
	PCIeDumpMem((UINT32)PemcuDSRAMBase+0x1100,0x300);
	PCIeDumpMem((UINT32)PemcuDSRAMBase+0x1400,0x800);
	Print(L"[CJW_PCIE_EYE_DEBUG] PemcuDSRAMBase: 0x%08x\n",PemcuDSRAMBase);

	PCIeDumpCfg(0xE0000000,0,3,0,0x80);
	PCIeDumpCfg(0xE0000000,0,3,1,0x80);
	PCIeDumpCfg(0xE0000000,0,3,2,0x80);
	PCIeDumpCfg(0xE0000000,0,3,3,0x80);
	PCIeDumpCfg(0xE0000000,0,4,0,0x80);
	PCIeDumpCfg(0xE0000000,0,4,1,0x80);
	PCIeDumpCfg(0xE0000000,0,5,0,0x80);
	PCIeDumpCfg(0xE0000000,0,5,1,0x80);
	
	//DEBUG((EFI_D_ERROR, "[CJW_PCIE_EYE_DEBUG] 1\n"));


	//Error Code check (offset 1200)
	for(ix = 0 ; ix<0x100; ix++){
		if(AsiaMemoryRead8(PemcuDSRAMBase+0x1200+ix)){
			ErrorCounter++;
		}
	}
	//DEBUG((EFI_D_ERROR, "[CJW_PCIE_EYE_DEBUG] 2\n"));

	for(ix= 0; ix<8;ix++){
		EmTimeS[ix] = (AsiaMemoryRead8(PemcuDSRAMBase+0x1300+0x20+ix)<<8)+(AsiaMemoryRead8(PemcuDSRAMBase+0x1300+0x30+ix));
		EmTimeE[ix] = (AsiaMemoryRead8(PemcuDSRAMBase+0x1300+0x40+ix)<<8)+(AsiaMemoryRead8(PemcuDSRAMBase+0x1300+0x50+ix));
	}
	//DEBUG((EFI_D_ERROR, "[CJW_PCIE_EYE_DEBUG] 3\n"));

	for(ix = 0; ix<8; ix++){
		PortDetailOffset = 0x1400+(0x100*ix);
		//DEBUG((EFI_D_ERROR, "[CJW_PCIE_EYE_DEBUG] 3-%x\n",PortDetailOffset));
		for(iy = 0x20; iy<0x100;iy+=0x10){
			//DEBUG((EFI_D_ERROR, "[CJW_PCIE_EYE_DEBUG] 3-%x\n",PemcuDSRAMBase+PortDetailOffset+iy));
			if(0xA5 == AsiaMemoryRead8(PemcuDSRAMBase+PortDetailOffset+iy)){
				CursorDetail[ix][0] = AsiaMemoryRead8(PemcuDSRAMBase+PortDetailOffset+iy+1);
				CursorDetail[ix][1] = AsiaMemoryRead8(PemcuDSRAMBase+PortDetailOffset+iy+2);
				CursorDetail[ix][2] = AsiaMemoryRead8(PemcuDSRAMBase+PortDetailOffset+iy+3);	
				CursorDetail[ix][3] = AsiaMemoryRead8(PemcuDSRAMBase+PortDetailOffset+iy+5);
				break;
			}
		}
		if(CursorDetail[ix][1] == 0){//not found
			CursorDetail[ix][0] = 0x0;
			CursorDetail[ix][1] = 0x0;
			CursorDetail[ix][2] = 0x0;
			CursorDetail[ix][3] = 0x0;
		}		
	}




	Print(L"[CJW_RXEQ_SEARCH Flag] %c Error:%c%d%c ",TAB,TAB,ErrorCounter,TAB);

	Print(L"VIDDID:");
	Print(L"%c",TAB);

	secbus = AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,0,0x19));
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(secbus,0,0,0x00)),TAB   );
	secbus = AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,1,0x19));
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(secbus,0,0,0x00)),TAB   );
	secbus = AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,2,0x19));
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(secbus,0,0,0x00)),TAB   );
	secbus = AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,3,0x19));
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(secbus,0,0,0x00)),TAB   );
	secbus = AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,4,0,0x19));
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(secbus,0,0,0x00)),TAB   );
	secbus = AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,4,1,0x19));
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(secbus,0,0,0x00)),TAB   );
	secbus = AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,5,0,0x19));
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(secbus,0,0,0x00)),TAB   );
	secbus = AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,5,1,0x19));
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(secbus,0,0,0x00)),TAB   );
	


	Print(L"Rx290(DOEQPhase2/3?):");
	Print(L"%c",TAB);
	Print(L"%02x-",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,0,0x290))  );
	Print(L"%02x-",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,1,0x290))  );
	Print(L"%02x-",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,2,0x290))  );
	Print(L"%02x-",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,3,0x290))  );
	Print(L"%02x-",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,4,0,0x290))  );
	Print(L"%02x-",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,4,1,0x290))  );
	Print(L"%02x-",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,5,0,0x290))  );
	Print(L"%02x-",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,5,1,0x290))  );
	Print(L"%c",TAB);
	

	Print(L"EQPhase3TimeComsume:");
	Print(L"%c",TAB);
	for(ix=0; ix<8;ix++){
		Print(L"%d",EmTimeE[ix]-EmTimeS[ix]);
		Print(L"%c",TAB);
	}


	Print(L"Rx52(LinkWidth_LinkSpeed):");
	Print(L"%c",TAB);
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,0,0x52)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,1,0x52)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,2,0x52)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,3,0x52)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,4,0,0x52)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,4,1,0x52)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,5,0,0x52)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,5,1,0x52)),TAB   );



	Print(L"AER_Correctable:");
	Print(L"%c",TAB);
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,3,0,0x110)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,3,1,0x110)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,3,2,0x110)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,3,3,0x110)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,4,0,0x110)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,4,1,0x110)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,5,0,0x110)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,5,1,0x110)),TAB   );

	Print(L"AER_Uncorrectable:");
	Print(L"%c",TAB);
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,3,0,0x104)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,3,1,0x104)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,3,2,0x104)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,3,3,0x104)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,4,0,0x104)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,4,1,0x104)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,5,0,0x104)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,5,1,0x104)),TAB   );


	Print(L"Rx72(EQPhase_RequestEQStatus):");
	Print(L"%c",TAB);
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,0,0x72)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,1,0x72)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,2,0x72)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,3,0x72)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,4,0,0x72)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,4,1,0x72)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,5,0,0x72)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,5,1,0x72)),TAB   );


	Print(L"\n");

		//clear AER
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,3,0,0x110),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,3,1,0x110),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,3,2,0x110),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,3,3,0x110),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,4,0,0x110),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,4,1,0x110),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,5,0,0x110),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,5,1,0x110),0xFFFFFFFF,0xFFFFFFFF);


		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,3,0,0x104),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,3,1,0x104),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,3,2,0x104),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,3,3,0x104),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,4,0,0x104),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,4,1,0x104),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,5,0,0x104),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,5,1,0x104),0xFFFFFFFF,0xFFFFFFFF);

			
	PCIeDumpMem(EPHYBASE+0x2900,0x200);
	PCIeDumpMem(EPHYBASE+0xA00,0x100);






	//clear All debug data in DSRAM
	for(ix = 0; ix<=16; ix++){
		for(iy = 0; iy<=0xFF; iy++){
			AsiaMemoryWrite8(PemcuDSRAMBase+0x1100+0x100*ix+iy,0);
		}
	}








	Print(L"[CJW] END\n");

	//AsiaIoWrite8(0xCF9,0x06);


	return EFI_SUCCESS;
}	









EFI_STATUS
EQ_RedoEQTest(UINT8 Bus,UINT8 Dev, UINT8 Func, UINT32 Delayus)
{
	UINT8 secbus, LTSSMx;



	if(0xFFFF == AsiaPcieRead16(0xE0000000,AsiaPciAddress(Bus, Dev, Func,0x0))){
		Print(L"[REPORT]SKIP THIS PORT: B%dD%dF%d\n",Bus, Dev,Func);
		return EFI_SUCCESS;
	}

	if(BIT6 & AsiaPcieRead8(0xE0000000,AsiaPciAddress(Bus, Dev, Func,0x5A))){
		Print(L"[REPORT]PRESENT[OK] B%dD%dF%d\n",Bus, Dev,Func);
	}else{
		Print(L"[REPORT]PRESENT[FAIL] B%dD%dF%d\n",Bus, Dev,Func);
		return EFI_SUCCESS;
	}

	Print(L"Wait LTSSM....");
	secbus = AsiaPcieRead8(0xE0000000,AsiaPciAddress(Bus, Dev, Func,0x19));
	AsiaPcieModify8(0xE0000000,AsiaPciAddress(Bus, Dev, Func,0x2A4),BIT0,BIT0);
	AsiaPcieModify8(0xE0000000,AsiaPciAddress(Bus, Dev, Func,0x50),BIT5,BIT5);//retrain link

	
	
	do{
		LTSSMx = AsiaPcieRead8(0xE0000000,AsiaPciAddress(Bus, Dev, Func,0x1C3));
	}while(LTSSMx != 0x8A);
	
	Print(L"done\n");
	if(Delayus ==	0){
		//rw test
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		
	}else{
		gBS->Stall(Delayus);
		//rw test
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(secbus,0,0,0x00),BIT0,BIT0);
		
	}


	


	return EFI_SUCCESS;
}


//this routine is a RxEQ result reporter
EFI_STATUS
RxEQ_Dbg(VOID)
{
	
	UINT64 PemcuDSRAMBase;
	UINT16 ix,ErrorCounter = 0;
	UINT16 EmTimeS[8] = {0};
	UINT16 EmTimeE[8] = {0};
	//UINT16 PortDetailOffset ;
	//UINT8 CursorDetail[8][4]={0};
	UINT16 iy;

	UINT8 EWDetail[0x10]; //each lane need one byte
	UINT16 EphyLaneOffset;

	UINT8 xDCOUT[0x10];
	UINT8 xHFOUT[0x10];
	UINT8 xOSOUT[0x10];
	UINT8 xW1OUT[0x10];
	UINT8 xW2OUT[0x10];
	UINT8 xRCV_HBW[0x10];
	UINT8 xFarendTxPreset[0x10];
	UINT8 xFarendPreCur[0x10] ;
	UINT8 xFarendCur[0x10];
	UINT8 xFarendPostCur[0x10];

	UINT8 secbus;

	
	Print(L"[CJW_PCIE_EYE_DEBUG] START\n");
	
	PemcuDSRAMBase = AsiaMemoryRead32(EPHYBASE+PCIEPHYCFG_BASE_ADR_OF_PEMCU_FW_FOR_DATA_SPACE);
	PemcuDSRAMBase = PemcuDSRAMBase<<16;
	PCIeDumpMem((UINT32)PemcuDSRAMBase+0x2000,0x500);
	Print(L"[CJW_PCIE_EYE_DEBUG] PemcuDSRAMBase: 0x%08x\n",PemcuDSRAMBase);
	Print(L"[RDT_PCIE_EYE_DEBUG] \n");
	PCIeDumpMem((UINT32)PemcuDSRAMBase+0x2600,0x500);

	PCIeDumpCfg(0xE0000000,0,3,0,0x80);
	PCIeDumpCfg(0xE0000000,0,3,1,0x80);
	PCIeDumpCfg(0xE0000000,0,3,2,0x80);
	PCIeDumpCfg(0xE0000000,0,3,3,0x80);
	PCIeDumpCfg(0xE0000000,0,4,0,0x80);
	PCIeDumpCfg(0xE0000000,0,4,1,0x80);
	PCIeDumpCfg(0xE0000000,0,5,0,0x80);
	PCIeDumpCfg(0xE0000000,0,5,1,0x80);
	
	//DEBUG((EFI_D_ERROR, "[CJW_PCIE_EYE_DEBUG] 1\n"));


	//Error Code check (offset 1200)
	for(ix = 0 ; ix<0x100; ix++){
		if(AsiaMemoryRead8(PemcuDSRAMBase+0x1200+ix)){
			ErrorCounter++;
		}
	}
	//DEBUG((EFI_D_ERROR, "[CJW_PCIE_EYE_DEBUG] 2\n"));

	for(ix= 0; ix<8;ix++){
		EmTimeS[ix] = (AsiaMemoryRead8(PemcuDSRAMBase+0x1300+0x20+ix)<<8)+(AsiaMemoryRead8(PemcuDSRAMBase+0x1300+0x30+ix));
		EmTimeE[ix] = (AsiaMemoryRead8(PemcuDSRAMBase+0x1300+0x40+ix)<<8)+(AsiaMemoryRead8(PemcuDSRAMBase+0x1300+0x50+ix));
	}
	//DEBUG((EFI_D_ERROR, "[CJW_PCIE_EYE_DEBUG] 3\n"));

	for(ix=0; ix<0x10;ix++){
		EphyLaneOffset = 0x2200+(0x10*ix);

		EWDetail[ix] =  AsiaMemoryRead8(PemcuDSRAMBase+EphyLaneOffset+0x08); //8 is the MemLog offset location about eye width result for each lane 
		
	}


	//read  DCOUT  HFOUT  OSOUT  W1OUT	W2OUT LPn_RCV_HBW
	//       FAREND_TX_PRESET   FAREND_PRE_CURSOR  FAREND_CURSOR   FAREND_POST_CURSOR
	for(ix=0; ix<0x10;ix++){
		EphyLaneOffset = 0x2400+(0x10*ix);

		xDCOUT[ix] = AsiaMemoryRead8(PemcuDSRAMBase+EphyLaneOffset+0x00);
		xHFOUT[ix] = AsiaMemoryRead8(PemcuDSRAMBase+EphyLaneOffset+0x01);
		xOSOUT[ix] = AsiaMemoryRead8(PemcuDSRAMBase+EphyLaneOffset+0x02);
		xW1OUT[ix] = AsiaMemoryRead8(PemcuDSRAMBase+EphyLaneOffset+0x03);
		xW2OUT[ix] = AsiaMemoryRead8(PemcuDSRAMBase+EphyLaneOffset+0x04);
		xRCV_HBW[ix] = AsiaMemoryRead8(PemcuDSRAMBase+EphyLaneOffset+0x05);
		xFarendTxPreset[ix] = AsiaMemoryRead8(PemcuDSRAMBase+EphyLaneOffset+0x06);
		xFarendPreCur[ix] = AsiaMemoryRead8(PemcuDSRAMBase+EphyLaneOffset+0x07);
		xFarendCur[ix] = AsiaMemoryRead8(PemcuDSRAMBase+EphyLaneOffset+0x08);
		xFarendPostCur[ix] = AsiaMemoryRead8(PemcuDSRAMBase+EphyLaneOffset+0x09);
		
	}







	Print(L"[CJW_RXEQ_SEARCH Flag] %c Error:%c%d%c ",TAB,TAB,ErrorCounter,TAB);

	Print(L"VIDDID:");
	Print(L"%c",TAB);

	secbus = AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,0,0x19));
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(secbus,0,0,0x00)),TAB   );
	secbus = AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,1,0x19));
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(secbus,0,0,0x00)),TAB   );
	secbus = AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,2,0x19));
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(secbus,0,0,0x00)),TAB   );
	secbus = AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,3,0x19));
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(secbus,0,0,0x00)),TAB   );
	secbus = AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,4,0,0x19));
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(secbus,0,0,0x00)),TAB   );
	secbus = AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,4,1,0x19));
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(secbus,0,0,0x00)),TAB   );
	secbus = AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,5,0,0x19));
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(secbus,0,0,0x00)),TAB   );
	secbus = AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,5,1,0x19));
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(secbus,0,0,0x00)),TAB   );
	


	Print(L"Rx290(DOEQPhase2/3?):");
	Print(L"%c",TAB);
	Print(L"%02x-",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,0,0x290))  );
	Print(L"%02x-",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,1,0x290))  );
	Print(L"%02x-",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,2,0x290))  );
	Print(L"%02x-",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,3,0x290))  );
	Print(L"%02x-",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,4,0,0x290))  );
	Print(L"%02x-",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,4,1,0x290))  );
	Print(L"%02x-",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,5,0,0x290))  );
	Print(L"%02x-",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,5,1,0x290))  );
	Print(L"%c",TAB);
	

	Print(L"EQPhase3TimeComsume:");
	Print(L"%c",TAB);
	for(ix=0; ix<8;ix++){
		Print(L"%d",EmTimeE[ix]-EmTimeS[ix]);
		Print(L"%c",TAB);
	}


	Print(L"EyeWidthDetail:");
	Print(L"%c",TAB);
	for(ix=0; ix<0x10;ix++){
		Print(L"%d",EWDetail[ix]);
		Print(L"%c",TAB);
	}


	Print(L"Rx52(LinkWidth_LinkSpeed):");
	Print(L"%c",TAB);
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,0,0x52)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,1,0x52)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,2,0x52)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,3,0x52)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,4,0,0x52)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,4,1,0x52)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,5,0,0x52)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,5,1,0x52)),TAB   );



	Print(L"AER_Correctable:");
	Print(L"%c",TAB);
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,3,0,0x110)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,3,1,0x110)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,3,2,0x110)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,3,3,0x110)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,4,0,0x110)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,4,1,0x110)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,5,0,0x110)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,5,1,0x110)),TAB   );

	Print(L"AER_Uncorrectable:");
	Print(L"%c",TAB);
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,3,0,0x104)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,3,1,0x104)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,3,2,0x104)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,3,3,0x104)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,4,0,0x104)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,4,1,0x104)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,5,0,0x104)),TAB   );
	Print(L"%08x%c",AsiaPcieRead32(0xE0000000,AsiaPciAddress(0,5,1,0x104)),TAB   );


	Print(L"Rx72(EQPhase_RequestEQStatus):");
	Print(L"%c",TAB);
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,0,0x72)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,1,0x72)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,2,0x72)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,3,3,0x72)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,4,0,0x72)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,4,1,0x72)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,5,0,0x72)),TAB   );
	Print(L"%02x%c",AsiaPcieRead8(0xE0000000,AsiaPciAddress(0,5,1,0x72)),TAB   );


		
		//report RXEQ result
		Print(L"DCOUT:");
		Print(L"%c",TAB);
		for(ix=0; ix<0x10;ix++){
			Print(L"%d",xDCOUT[ix]);
			Print(L"%c",TAB);
		}

		Print(L"HFOUT:");
		Print(L"%c",TAB);
		for(ix=0; ix<0x10;ix++){
			Print(L"%d",xHFOUT[ix]);
			Print(L"%c",TAB);
		}

		Print(L"OSOUT:");
		Print(L"%c",TAB);
		for(ix=0; ix<0x10;ix++){
			Print(L"%d",xOSOUT[ix]);
			Print(L"%c",TAB);
		}

		Print(L"W1OUT:");
		Print(L"%c",TAB);
		for(ix=0; ix<0x10;ix++){
			Print(L"%d",xW1OUT[ix]);
			Print(L"%c",TAB);
		}

		Print(L"W2OUT:");
		Print(L"%c",TAB);
		for(ix=0; ix<0x10;ix++){
			Print(L"%d",xW2OUT[ix]);
			Print(L"%c",TAB);
		}
 
		Print(L"RCV_HBW:");
		Print(L"%c",TAB);
		for(ix=0; ix<0x10;ix++){
			Print(L"%02x",xRCV_HBW[ix]);
			Print(L"%c",TAB);
		}

		Print(L"FarendTxPreset:");
		Print(L"%c",TAB);
		for(ix=0; ix<0x10;ix++){
			Print(L"%02x",xFarendTxPreset[ix]);
			Print(L"%c",TAB);
		}

		Print(L"FarendPreCursor:");
		Print(L"%c",TAB);
		for(ix=0; ix<0x10;ix++){
			Print(L"%d",xFarendPreCur[ix]);
			Print(L"%c",TAB);
		}

		Print(L"FarendCursor:");
		Print(L"%c",TAB);
		for(ix=0; ix<0x10;ix++){
			Print(L"%d",xFarendCur[ix]);
			Print(L"%c",TAB);
		}

		Print(L"FarendPostCursor:");
		Print(L"%c",TAB);
		for(ix=0; ix<0x10;ix++){
			Print(L"%d",xFarendPostCur[ix]);
			Print(L"%c",TAB);
		}

		Print(L"\n");



		//clear AER
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,3,0,0x110),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,3,1,0x110),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,3,2,0x110),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,3,3,0x110),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,4,0,0x110),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,4,1,0x110),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,5,0,0x110),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,5,1,0x110),0xFFFFFFFF,0xFFFFFFFF);


		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,3,0,0x104),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,3,1,0x104),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,3,2,0x104),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,3,3,0x104),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,4,0,0x104),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,4,1,0x104),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,5,0,0x104),0xFFFFFFFF,0xFFFFFFFF);
		AsiaPcieModify32(0xE0000000,AsiaPciAddress(0,5,1,0x104),0xFFFFFFFF,0xFFFFFFFF);

			
	PCIeDumpMem(EPHYBASE+0x2900,0x200);
	PCIeDumpMem(EPHYBASE+0xA00,0x100);






	//clear All debug data in DSRAM
	for(ix = 0; ix<=0x20; ix++){
		for(iy = 0; iy<=0xFF; iy++){
			AsiaMemoryWrite8(PemcuDSRAMBase+0x1100+0x100*ix+iy,0);
		}
	}







	Print(L"[CJW] END\n");

	//AsiaIoWrite8(0xCF9,0x06);


	return EFI_SUCCESS;






}





/////////////////////////////////////////////////////////////////////////////////
// EQ scan 
/////////////////////////////////////////////////////////////////////////////////


extern UINT8 Point_PH0_neg;
extern UINT8 Point_PH0_pos;

#define REPEAT_LOOP 4
//
//	Before run this test, pls change the BIOS setup and change DOEQ method as fixed EQ
//		Post Cursor increase first
//
VOID
EQ_ScanCoeff_For_Specific_Lane(UINT32 EphyBase,UINT8 LaneNum)
{
	UINT8 Farend_FS = 0,Farend_LF = 0;
	FTPC Coefficient;	
	EFI_STATUS Statusx=EFI_SUCCESS;

	UINT8 Bus=0, Dev=0, Func=0;

	UINT8 cn=0, c=0, cp=0; //cn(c-1)  c(cursor)  cp(c+1)

	UINT16 LinkSpeedWidth=0;

	UINT8 ix = 0;

	UINT8 EyeWidth[REPEAT_LOOP];
	UINT8 EyeWidthAverage;
	//
	// 
	//
	
	//Get Port BDF
	Bus = 0;
	Dev = (0xF0 & EQ_LaneMaptoPort(EphyBase, LaneNum))>>4;
	Func =(0x0F & EQ_LaneMaptoPort(EphyBase, LaneNum)); 
	Print(L"Target Port: [%d|%d|%d]\n",Bus, Dev,Func );

	//Auto Speed negotiation = Disable (ZX chipset specific)
	//	this setting avoid some request for EQ
	AsiaPcieModify8(0xE000000,AsiaPciAddress(Bus,  Dev,  Func,  0x1C1), BIT5, 0);



	//
	//
	//
	
	//Get FS
	Farend_FS = EQ_GetFs_SpecificLane(EphyBase, LaneNum);
	Print(L"Farend_FS:  0x%08x(%d)\n", Farend_FS,Farend_FS);
	//Get LF
	Farend_LF = EQ_GetLf_SpecificLane(EphyBase, LaneNum);
	Print(L"Farend_LF:  0x%08x(%d)\n", Farend_FS,Farend_FS);


	//cn <=Floor(FS/4)
	//cn+c+cp = FS
	//c - cp - cn >=LF
	for(cn = 0,c = Farend_FS; (cn<=(Farend_FS/4));cn++){
		for(cp = 0; ((cp<Farend_FS)&&(cp<=(c - cn - Farend_LF ))); cp++){
			c = Farend_FS - cn - cp;

			Print(L"Flagx:%c current cursor group is %c %d %c %d %c %d %c \n",9,9,cn,9,c,9,cp,9);



			
			Coefficient.Cmd = CMD_Cursor;
			Coefficient.Cursor = c;
			Coefficient.PostCursor = cp;
			Coefficient.PresetPreCursor = cn;

			//trans command to PEMCU, to set a group of specific Cursors
			Statusx = EQ_SendCmdToAdjustTxParam(EphyBase, &Coefficient);
			if(Statusx == EFI_SUCCESS){

				//Retrain link to init redo-EQ(Register: Perform EQ = 1b)
				EQ_RedoEQTest(Bus,  Dev,  Func, 0);

				Statusx = EQ_VerifyFarendParam(EphyBase, LaneNum, &Coefficient);
				if(Statusx == EFI_SUCCESS){

					//Get CurrentLinkSpeed & LinkWidth
					LinkSpeedWidth = 0x1FF & AsiaPcieRead16(0xE0000000, AsiaPciAddress(Bus,  Dev,  Func,  0x52));


					for(ix=0;ix<REPEAT_LOOP;ix++){
						
						//Gen3, ScanTime=10, Threshold=0, Fast Mode
						EyeMode3Scan(2,EphyBase,3, (UINT32)LaneNum, 10, 0,1);
						//EyeMcuCommand(2, (UINT16)Str2Num(Argv[3]),(UINT8)Str2Num(Argv[4]),(UINT8)Str2Num(Argv[5]));

						EyeWidth[ix] = Point_PH0_neg+Point_PH0_pos+1;
					}
					EyeWidthAverage = (EyeWidth[0]+EyeWidth[1]+EyeWidth[2]+EyeWidth[3])>>2;   //(x/4)


					Print(L"Flag:%c current cursor group is %c %d %c %d %c %d %c ",9,9,cn,9,c,9,cp,9);
					
					Print(L"EyeWidth:%c %d %c %02x",
									9,
									EyeWidthAverage,
									9,
									EyeWidthAverage);
					Print(L"%cLaneNum:%c%d",9,9,LaneNum);	
					Print(L"%cRx52:%c0x%02x \n",9,9,LinkSpeedWidth);
					
				}else{
					Print(L"Flag:%cEYE_ERROR: Verify Failed! - The Farend coefficient NOT meet expect! shoud be (%d,%d,%d)!\n",
							9,
							Coefficient.PresetPreCursor,
							Coefficient.Cursor,
							Coefficient.PostCursor);
				}

			}


			
		}

	}




}



//
//	Before run this test, pls change the BIOS setup and change DOEQ method as fixed EQ
//		PreCursor incrase first
//
VOID
EQ_ScanCoeff_For_Specific_Lane_2(UINT32 EphyBase,UINT8 LaneNum)
{
	UINT8 Farend_FS = 0,Farend_LF = 0;
	FTPC Coefficient;	
	EFI_STATUS Statusx=EFI_SUCCESS;

	UINT8 Bus=0, Dev=0, Func=0;

	UINT8 cn=0, c=0, cp=0; //cn(c-1)  c(cursor)  cp(c+1)

	UINT16 LinkSpeedWidth=0;

	UINT8 ix = 0;

	UINT8 EyeWidth[REPEAT_LOOP];
	UINT8 EyeWidthAverage;
	//
	// 
	//
	
	//Get Port BDF
	Bus = 0;
	Dev = (0xF0 & EQ_LaneMaptoPort(EphyBase, LaneNum))>>4;
	Func =(0x0F & EQ_LaneMaptoPort(EphyBase, LaneNum)); 
	Print(L"Target Port: [%d|%d|%d]\n",Bus, Dev,Func );

	//Auto Speed negotiation = Disable (ZX chipset specific)
	//	this setting avoid some request for EQ
	AsiaPcieModify8(0xE000000,AsiaPciAddress(Bus,  Dev,  Func,  0x1C1), BIT5, 0);



	//
	//
	//
	
	//Get FS
	Farend_FS = EQ_GetFs_SpecificLane(EphyBase, LaneNum);
	Print(L"Farend_FS:  0x%08x(%d)\n", Farend_FS,Farend_FS);
	//Get LF
	Farend_LF = EQ_GetLf_SpecificLane(EphyBase, LaneNum);
	Print(L"Farend_LF:  0x%08x(%d)\n", Farend_FS,Farend_FS);


	//cn <=Floor(FS/4)
	//cn+c+cp = FS
	//c - cp - cn >=LF
	for(cp = 0,c = Farend_FS; (cp<Farend_FS); cp++){
		for(cn = 0; ((cn<=(Farend_FS/4))&&( cn<=(c-cp-Farend_LF)));cn++){
			c = Farend_FS - cn - cp;

			Print(L"Flagx:%c current cursor group is %c %d %c %d %c %d %c \n",9,9,cn,9,c,9,cp,9);



			
			Coefficient.Cmd = CMD_Cursor;
			Coefficient.Cursor = c;
			Coefficient.PostCursor = cp;
			Coefficient.PresetPreCursor = cn;

			//trans command to PEMCU, to set a group of specific Cursors
			Statusx = EQ_SendCmdToAdjustTxParam(EphyBase, &Coefficient);
			if(Statusx == EFI_SUCCESS){

				//Retrain link to init redo-EQ(Register: Perform EQ = 1b)
				EQ_RedoEQTest(Bus,  Dev,  Func, 0);

				Statusx = EQ_VerifyFarendParam(EphyBase, LaneNum, &Coefficient);
				if(Statusx == EFI_SUCCESS){

					//Get CurrentLinkSpeed & LinkWidth
					LinkSpeedWidth = 0x1FF & AsiaPcieRead16(0xE0000000, AsiaPciAddress(Bus,  Dev,  Func,  0x52));


					for(ix=0;ix<REPEAT_LOOP;ix++){
						
						//Gen3, ScanTime=10, Threshold=0, Fast Mode
						EyeMode3Scan(2,EphyBase,3, (UINT32)LaneNum, 10, 0,1);
						//EyeMcuCommand(2, (UINT16)Str2Num(Argv[3]),(UINT8)Str2Num(Argv[4]),(UINT8)Str2Num(Argv[5]));

						EyeWidth[ix] = Point_PH0_neg+Point_PH0_pos+1;
					}
					EyeWidthAverage = (EyeWidth[0]+EyeWidth[1]+EyeWidth[2]+EyeWidth[3])>>2;   //(x/4)


					Print(L"Flag:%c current cursor group is %c %d %c %d %c %d %c ",9,9,cn,9,c,9,cp,9);
					
					Print(L"EyeWidth:%c %d %c %02x",
									9,
									EyeWidthAverage,
									9,
									EyeWidthAverage);
					Print(L"%cLaneNum:%c%d",9,9,LaneNum);	
					Print(L"%cRx52:%c0x%02x \n",9,9,LinkSpeedWidth);
					
				}else{
					Print(L"Flag:%cEYE_ERROR: Verify Failed! - The Farend coefficient NOT meet expect! shoud be (%d,%d,%d)!\n",
							9,
							Coefficient.PresetPreCursor,
							Coefficient.Cursor,
							Coefficient.PostCursor);
				}

			}


			
		}

	}




}













//
//	Before run this test, pls change the BIOS setup and change DOEQ method as fixed EQ
//
VOID
EQ_ScanCoeff_For_Specific_Lane_CheckSingleCursor(
	UINT32 EphyBase,UINT8 LaneNum,
	UINT8 PreCursor,
	UINT8 Cursor,
	UINT8 PostCursor)
{
	UINT8 Farend_FS = 0,Farend_LF = 0;
	FTPC Coefficient;	
	EFI_STATUS Statusx=EFI_SUCCESS;

	UINT8 Bus=0, Dev=0, Func=0;

	UINT8 cn=0, c=0, cp=0; //cn(c-1)  c(cursor)  cp(c+1)

	UINT16 LinkSpeedWidth=0;

	UINT8 ix = 0;

	UINT8 EyeWidth[REPEAT_LOOP];
	UINT8 EyeWidthAverage;
	//
	// 
	//
	
	//Get Port BDF
	Bus = 0;
	Dev = (0xF0 & EQ_LaneMaptoPort(EphyBase, LaneNum))>>4;
	Func =(0x0F & EQ_LaneMaptoPort(EphyBase, LaneNum)); 
	Print(L"Target Port: [%d|%d|%d]\n",Bus, Dev,Func );

	//Auto Speed negotiation = Disable (ZX chipset specific)
	//	this setting avoid some request for EQ
	AsiaPcieModify8(0xE000000,AsiaPciAddress(Bus,  Dev,  Func,  0x1C1), BIT5, 0);


	//
	//
	//
	
	//Get FS
	Farend_FS = EQ_GetFs_SpecificLane(EphyBase, LaneNum);
	Print(L"Farend_FS:  0x%08x(%d)\n", Farend_FS,Farend_FS);
	//Get LF
	Farend_LF = EQ_GetLf_SpecificLane(EphyBase, LaneNum);
	Print(L"Farend_LF:  0x%08x(%d)\n", Farend_FS,Farend_FS);


	//Check Rules:
	//	cn <=Floor(FS/4)
	//	cn+c+cp = FS
	//	c - cp - cn >=LF
	if((Cursor+PostCursor+PreCursor) != Farend_FS){
		Print(L"ERROR! PreCur+Cur+PostCur must equal to FS!  Device's FS = 0x%02x(%d)\n",Farend_FS,Farend_FS);
		return;
	}
	if(PreCursor > (Farend_FS/4)){
		Print(L"ERROR! PreCur must equal or less than FS/4!  Device's FS = 0x%02x(%d)\n",Farend_FS,Farend_FS);
		return;
	}

	if(Cursor - PreCursor - PostCursor < Farend_LF){
		Print(L"ERROR! Cursor-PreCursor-PostCursor must equal or great than LF!   Device's LF = 0x%02x(%d)\n",Farend_LF,Farend_LF);
		return;
	}


	Coefficient.Cmd = CMD_Cursor;
	Coefficient.Cursor = Cursor;
	Coefficient.PostCursor = PostCursor;
	Coefficient.PresetPreCursor = PreCursor;

	
	c = Cursor;
	cn = PreCursor;
	cp = PostCursor;
	Print(L"Flagx:%c current cursor group is %c %d %c %d %c %d %c \n",9,9,cn,9,c,9,cp,9);


	//trans command to PEMCU, to set a group of specific Cursors
	Statusx = EQ_SendCmdToAdjustTxParam(EphyBase, &Coefficient);
	if(Statusx == EFI_SUCCESS){

		//Retrain link to init redo-EQ(Register: Perform EQ = 1b)
		EQ_RedoEQTest(Bus,  Dev,  Func, 0);

		Statusx = EQ_VerifyFarendParam(EphyBase, LaneNum, &Coefficient);
		if(Statusx == EFI_SUCCESS){

			//Get CurrentLinkSpeed & LinkWidth
			LinkSpeedWidth = 0x1FF & AsiaPcieRead16(0xE0000000, AsiaPciAddress(Bus,  Dev,  Func,  0x52));


			for(ix=0;ix<REPEAT_LOOP;ix++){
						
				//Gen3, ScanTime=10, Threshold=0, Fast Mode
				EyeMode3Scan(2,EphyBase,3, (UINT32)LaneNum, 10, 0,1);
				//EyeMcuCommand(2, (UINT16)Str2Num(Argv[3]),(UINT8)Str2Num(Argv[4]),(UINT8)Str2Num(Argv[5]));

				EyeWidth[ix] = Point_PH0_neg+Point_PH0_pos+1;
			}
			EyeWidthAverage = (EyeWidth[0]+EyeWidth[1]+EyeWidth[2]+EyeWidth[3])>>2;   //(x/4)
					

			Print(L"Flag:%c current cursor group is %c %d %c %d %c %d %c ",9,9,cn,9,c,9,cp,9);
					
			Print(L"EyeWidth:%c %d %c %02x",
									9,
									EyeWidthAverage,
									9,
									EyeWidthAverage);
			Print(L"%cLaneNum:%c%d",9,9,LaneNum);
			Print(L"%cRx52:%c0x%02x \n",9,9,LinkSpeedWidth);

					
		}else{
			Print(L"Flag:%cEYE_ERROR: Verify Failed! - The Farend coefficient NOT meet expect! shoud be (%d,%d,%d)!\n",
							9,
							Coefficient.PresetPreCursor,
							Coefficient.Cursor,
							Coefficient.PostCursor);
		}

	}

}

































