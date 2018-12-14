//**********************************************************************
//**********************************************************************
//**                                                                                                              **
//**     Copyright (c) 2015 Shanghai Zhaoxin Semiconductor Co., Ltd.                     **
//**                                                                                                              **
//**     Author: Chris J.Wang  --  chrisjwang@zhaoxin.com                                  **
//**********************************************************************
//**********************************************************************


#include "ZXEyeMonitorMode3Test.h"

#include "Top.h"

//
//this test just test on EPHY lane1
//


extern M3StFEL Mode3Def[];



VOID
RxEqAutoModeOrgEQTNVTH(UINTN BaseAddr,UINT8 LaneNum,UINT16 Valuex)
{
	M3StFEL *pMode3Reg = Mode3Def;

	MmioBitFieldWrite16(BaseAddr + ((pMode3Reg+LaneNum)->Reg[LPn_EQTNVTH].Offset), 
						((pMode3Reg+LaneNum)->Reg[LPn_EQTNVTH].start),
						((pMode3Reg+LaneNum)->Reg[LPn_EQTNVTH].end), 
						Valuex); 

	
}




//
//auto to set DC HF  OS W1 W2SETEN = 0
//
VOID
RxEqAutoMode(UINTN BaseAddr,UINT8 LaneNum,UINT8 DC, UINT8 HF, UINT8 OS, UINT8 W1, UINT8 W2)
{

	M3StFEL *pMode3Reg = Mode3Def;


	
	MmioBitFieldWrite16(BaseAddr + ((pMode3Reg+LaneNum)->Reg[LPn_DCSETEN].Offset), 
						((pMode3Reg+LaneNum)->Reg[LPn_DCSETEN].start),
						((pMode3Reg+LaneNum)->Reg[LPn_DCSETEN].end), 
						0); 
	MmioBitFieldWrite16(BaseAddr + ((pMode3Reg+LaneNum)->Reg[LPn_HFSETEN].Offset), 
						((pMode3Reg+LaneNum)->Reg[LPn_HFSETEN].start),
						((pMode3Reg+LaneNum)->Reg[LPn_HFSETEN].end), 
						0); 
	MmioBitFieldWrite16(BaseAddr + ((pMode3Reg+LaneNum)->Reg[LPn_OSSETEN].Offset), 
						((pMode3Reg+LaneNum)->Reg[LPn_OSSETEN].start),
						((pMode3Reg+LaneNum)->Reg[LPn_OSSETEN].end), 
						0); 
	MmioBitFieldWrite16(BaseAddr + ((pMode3Reg+LaneNum)->Reg[LPn_W1SETEN].Offset), 
						((pMode3Reg+LaneNum)->Reg[LPn_W1SETEN].start),
						((pMode3Reg+LaneNum)->Reg[LPn_W1SETEN].end), 
						0); 
	MmioBitFieldWrite16(BaseAddr + ((pMode3Reg+LaneNum)->Reg[LPn_W2SETEN].Offset), 
						((pMode3Reg+LaneNum)->Reg[LPn_W2SETEN].start),
						((pMode3Reg+LaneNum)->Reg[LPn_W2SETEN].end), 
						0); 


}

VOID
RxEqAutoModeParam(UINTN BaseAddr,UINT8 LaneNum,UINT16 P1,UINT16 P2,UINT16 P3,UINT16 P4,UINT16 P5 )
{
	M3StFEL *pMode3Reg = Mode3Def;
	MmioBitFieldWrite16(BaseAddr + ((pMode3Reg+LaneNum)->Reg[LPn_RCV_HBW].Offset), 
						((pMode3Reg+LaneNum)->Reg[LPn_RCV_HBW].start),
						((pMode3Reg+LaneNum)->Reg[LPn_RCV_HBW].end), 
						P1); 
	MmioBitFieldWrite16(BaseAddr + ((pMode3Reg+LaneNum)->Reg[FHPn_CDR_UGB_HBW].Offset), 
						((pMode3Reg+LaneNum)->Reg[FHPn_CDR_UGB_HBW].start),
						((pMode3Reg+LaneNum)->Reg[FHPn_CDR_UGB_HBW].end), 
						P2); 
	MmioBitFieldWrite16(BaseAddr + ((pMode3Reg+LaneNum)->Reg[LPn_KIPRSE].Offset), 
						((pMode3Reg+LaneNum)->Reg[LPn_KIPRSE].start),
						((pMode3Reg+LaneNum)->Reg[LPn_KIPRSE].end), 
						P3); 
	MmioBitFieldWrite16(BaseAddr + ((pMode3Reg+LaneNum)->Reg[LPn_KIPCSEL].Offset), 
						((pMode3Reg+LaneNum)->Reg[LPn_KIPCSEL].start),
						((pMode3Reg+LaneNum)->Reg[LPn_KIPCSEL].end), 
						P4); 
	MmioBitFieldWrite16(BaseAddr + ((pMode3Reg+LaneNum)->Reg[FHPn_CKBUFSEL_CDR].Offset), 
						((pMode3Reg+LaneNum)->Reg[FHPn_CKBUFSEL_CDR].start),
						((pMode3Reg+LaneNum)->Reg[FHPn_CKBUFSEL_CDR].end), 
						P5); 

}












//
//this function  fixed as half-rate mode for Eyemonitor
//
VOID
DebugMode3_SinglePresetScan(UINT32 EphyBase, UINT8 LaneNum,UINT8 Presetx)
{
	UINT8 Farend_FS = 0,Farend_LF = 0;
	UINT8 ipre = 0, ipost = 0;
	FTPC Coefficient;
	EFI_STATUS Statusx=EFI_SUCCESS;


	//
	// EPHY RxE0[6] = 1
	//
	AsiaMemoryModify8(EphyBase+0xE0,BIT6,BIT6);

	
	//
	// Get FS
	//
	Print(L"==================================================\n");
	Farend_FS = EQ_GetFs_SpecificLane(EphyBase, LaneNum);
	Print(L"Farend_FS:  0x%08x(%d)\n", Farend_FS,Farend_FS);
	
	Farend_LF = EQ_GetLf_SpecificLane(EphyBase, LaneNum);
	Print(L"Farend_LF:  0x%08x(%d)\n", Farend_FS,Farend_FS);


	

	Print(L"_________________________________________________________________(Pre:%d/24, Post:%d/24)\n",ipre,ipost);
	Print(L"Lane Number: %d\n",LaneNum);
	Print(L"Preset Scan: P%d\n",Presetx);
		
	Coefficient.Cmd = CMD_TxPreset;
	Coefficient.Cursor = Presetx;
			
	Statusx = EQ_SendCmdToAdjustTxParam(EphyBase, &Coefficient);
	if(Statusx == EFI_SUCCESS){

		Statusx = EQ_HotResetGen3Confirm(EphyBase, LaneNum);
		if(Statusx == EFI_SUCCESS){
					
			Statusx = EQ_VerifyFarendParam(EphyBase, LaneNum, &Coefficient);
			if(Statusx == EFI_SUCCESS){
				Print(L"Success! Next, DO Eye Monitor!\n");

				//Gen3, ScanTime=10, Threshold=0, Fast Mode
				EyeMode3Scan(2,EphyBase,3, (UINT32)LaneNum, 10, 0,1);
						
			}else{
				Print(L"EYE_ERROR: Verify Failed!\n");
			}
		}else{
			Print(L"EYE_ERROR: After HotReset,Not Gen3!\n");
		}
	}else{
		Print(L"EYE_ERROR: PEMCU cmd Failed!\n");
	}

	


}




//
//this function  fixed as half-rate mode for Eyemonitor
//
VOID
DebugMode3_SingleCursorScan(UINT32 EphyBase, UINT8 LaneNum,UINT8 Pre,UINT8 Cur,UINT8 Pos)
{
	UINT8 Farend_FS = 0,Farend_LF = 0;
	UINT8 PreCursor = 0, Cursor = 0, PostCursor= 0;
	UINT8 ipre = 0, ipost = 0;
	FTPC Coefficient;
	EFI_STATUS Statusx=EFI_SUCCESS;


	//
	// EPHY RxE0[6] = 1
	//
	AsiaMemoryModify8(EphyBase+0xE0,BIT6,BIT6);

	
	//
	// Get FS
	//
	Print(L"==================================================\n");
	Farend_FS = EQ_GetFs_SpecificLane(EphyBase, LaneNum);
	Print(L"Farend_FS:  0x%08x(%d)\n", Farend_FS,Farend_FS);
	
	Farend_LF = EQ_GetLf_SpecificLane(EphyBase, LaneNum);
	Print(L"Farend_LF:  0x%08x(%d)\n", Farend_FS,Farend_FS);

	if((Pre+Cur+Pos)!=Farend_FS){
		Print(L"ERROR: Bad Coefficient!  Pre+Cur+Pos != FS\n");
		return;
	}
	
	
	PreCursor = Pre;
	PostCursor = Pos;
	Cursor = Cur;
	Print(L"_________________________________________________________________(Pre:%d/24, Post:%d/24)\n",ipre,ipost);
	Print(L"Lane Number: %d\n",LaneNum);
	Print(L"Cursor Scan: C-1=%d,  C=%d,  C+1=%d\n",PreCursor, Cursor, PostCursor);
		
	Coefficient.Cmd = CMD_Cursor;
	Coefficient.Cursor = Cursor;
	Coefficient.PostCursor = PostCursor;
	Coefficient.PresetPreCursor = PreCursor;
			
	Statusx = EQ_SendCmdToAdjustTxParam(EphyBase, &Coefficient);
	if(Statusx == EFI_SUCCESS){

		Statusx = EQ_HotResetGen3Confirm(EphyBase, LaneNum);
		if(Statusx == EFI_SUCCESS){
					
			Statusx = EQ_VerifyFarendParam(EphyBase, LaneNum, &Coefficient);
			if(Statusx == EFI_SUCCESS){
				Print(L"Success! Next, DO Eye Monitor!\n");

				//Gen3, ScanTime=10, Threshold=0, Fast Mode
				EyeMode3Scan(2,EphyBase,3, (UINT32)LaneNum, 10, 0,1);
						
			}else{
				Print(L"EYE_ERROR: Verify Failed!\n");
			}
		}else{
			Print(L"EYE_ERROR: After HotReset,Not Gen3!\n");
		}
	}else{
		Print(L"EYE_ERROR: PEMCU cmd Failed!\n");
	}

	


}





//
//--------------------------------------------------------
//    Post    0/24  1/24  2/24  3/24  4/24  5/24  6/24  7/24  8/24
//Pre
//0/24         	+       +       +      +      +       +      +       +       #
//1/24	      	+       +       +      +      +       +      +       #       
//2/24 		+       +       +      +      +       +      #      
//3/24		+       +       +      +      +       #      
//4/24		+       +       +      +      #       
//5/24		+       +       +      #      
//6/24		+       +       #   

//
//this function  fixed as half-rate mode for Eyemonitor
//
VOID
DebugMode3_AllCursorScan(UINT32 EphyBase, UINT8 LaneNum)
{
	UINT8 Farend_FS = 0,Farend_LF = 0;
	UINT8 PreCursor = 0, Cursor = 0, PostCursor= 0;
	UINT8 ipre = 0, ipost = 0,ipost_limit= 0;
	FTPC Coefficient;
	EFI_STATUS Statusx=EFI_SUCCESS;


	//
	// EPHY RxE0[6] = 1
	//
	AsiaMemoryModify8(EphyBase+0xE0,BIT6,BIT6);


#if AUTO_MODE_DEBUG
	//Rx EQ Auto mode enable 
	RxEqAutoMode(EphyBase,LaneNum,0,0,0,0,0);//CJW_DEBUG those code is debug code
#endif



	
	//
	// Get FS
	//
	Print(L"==================================================\n");
	Farend_FS = EQ_GetFs_SpecificLane(EphyBase, LaneNum);
	Print(L"Farend_FS:  0x%08x(%d)\n", Farend_FS,Farend_FS);
	
	Farend_LF = EQ_GetLf_SpecificLane(EphyBase, LaneNum);
	Print(L"Farend_LF:  0x%08x(%d)\n", Farend_FS,Farend_FS);

	ipost_limit = 7;
	for(ipre = 0;ipre<=6;ipre++){
		for(ipost = 0; ipost<=ipost_limit; ipost++){

#if AUTO_MODE_DEBUG

			//
			//Firstly , set param    --from Keith
			//
			RxEqAutoModeOrgEQTNVTH(EphyBase,LaneNum,0x11);   //CJW_DEBUG those code is debug code
			RxEqAutoModeParam(EphyBase,LaneNum,5,1,2,1,2);//CJW_DEBUG those code is debug code
#endif		



			//
			//
			//
			PreCursor = (Farend_FS*ipre) /24;
			PostCursor = (Farend_FS*ipost) /24;
			Cursor = Farend_FS - PreCursor - PostCursor;
			Print(L"_________________________________________________________________(Pre:%d/24, Post:%d/24)\n",ipre,ipost);
			Print(L"Lane Number: %d\n",LaneNum);
			Print(L"Cursor Scan: C-1=%d,  C=%d,  C+1=%d\n",PreCursor, Cursor, PostCursor);
		
			Coefficient.Cmd = CMD_Cursor;
			Coefficient.Cursor = Cursor;
			Coefficient.PostCursor = PostCursor;
			Coefficient.PresetPreCursor = PreCursor;
			
			Statusx = EQ_SendCmdToAdjustTxParam(EphyBase, &Coefficient);
			if(Statusx == EFI_SUCCESS){

				Statusx = EQ_HotResetGen3Confirm(EphyBase, LaneNum);
				if(Statusx == EFI_SUCCESS){
					
					Statusx = EQ_VerifyFarendParam(EphyBase, LaneNum, &Coefficient);
					if(Statusx == EFI_SUCCESS){
						Print(L"Success! Next, DO Eye Monitor!\n");

						//Gen3, ScanTime=10, Threshold=0, Fast Mode
						EyeMode3Scan(2,EphyBase,3, (UINT32)LaneNum, 10, 0,1);
						
					}else{
						Print(L"EYE_ERROR: Verify Failed!\n");
					}
				}else{
					Print(L"EYE_ERROR: After HotReset,Not Gen3!\n");
				}
			}else{
				Print(L"EYE_ERROR: PEMCU cmd Failed!\n");
			}

			
		}
		ipost_limit--;
	}
	


}





//for Lane1
VOID
DebugMode3_SetRxEqAutoMode1(IN CHAR16 **param)
{
	//
	//Enable CPU to RW some EPHY regsiters
	//
	AsiaMemoryModify8(0xFEB14000+0xE0,BIT6,BIT6);

		if(!StrCmp(param[2], L"Auto")){


			//
			//Rx EQ  Auto Mode Enable
			//
			AsiaMemoryModify8(0xFEB14000+0x2D3,BIT7,0);
			AsiaMemoryModify8(0xFEB14000+0x2D4,BIT7,0);
			AsiaMemoryModify8(0xFEB14000+0x2D5,BIT7,0);
			AsiaMemoryModify8(0xFEB14000+0x2D6,BIT7,0);
			AsiaMemoryModify8(0xFEB14000+0x2D7,BIT7,0);


			//
			//Set EQTNVTH 
			//
			Print(L"EQTNVTH<4:0> = %d\n",(UINT8)Str2Num(param[3]));
			AsiaMemoryModify8(0xFEB14000+0x2D2,0x3F,(UINT8)Str2Num(param[3]));

			//
			//dump all related registers
			//
			PCIeDumpMem(0xFEB14000+0x200,0x100);
			PCIeDumpMem(0xFEB14000+0x2D0,0x10);
	
			//
			//set Request EQ - we need to execute a EQ process to make those 'Auto Rx EQ' effect
			//
			AsiaPcieModify8(0xE0000000,AsiaPciAddress(0,5,1,0x2A4),BIT0,BIT0);

#if 1
			//
			//DO HotReset
			//
			AsiaPcieModify8(0xE0000000,AsiaPciAddress(0,5,1,0x3E),BIT6,BIT6);
			gBS->Stall(1000000);
			AsiaPcieModify8(0xE0000000,AsiaPciAddress(0,5,1,0x3E),BIT6,0);
			gBS->Stall(100000);

			//
			//Dump the registers we need
			//
			PCIeDumpCfg(0xE0000000,0,5,1,0x100);
			PCIeDumpMem(0xFEB14000+0xA10,0x10);
#endif	
			
		}
		if(!StrCmp(param[2], L"RST")){

			//
			// dump those register after EyeMonitor
			// Note: actually this 
			//
			PCIeDumpMem(0xFEB14000+0xA10,0x10);
	
		}

		if(!StrCmp(param[2], L"More1")){

			Print(L"%d,%d,%d,%d,%d\n",(UINT8)Str2Num(param[3]), 
										(UINT8)Str2Num(param[4]),
										(UINT8)Str2Num(param[5]),
										(UINT8)Str2Num(param[6]),
										(UINT8)Str2Num(param[7]) );
			
			AsiaMemoryModify8(0xFEB14000+0x2C8,BIT4|BIT3|BIT2,((UINT8)Str2Num(param[3]))<<2);

			AsiaMemoryModify8(0xFEB14000+0x281,BIT2,((UINT8)Str2Num(param[4]))<<2);

			AsiaMemoryModify8(0xFEB14000+0x2CC,BIT3|BIT2,((UINT8)Str2Num(param[5]))<<2);

			AsiaMemoryModify8(0xFEB14000+0x2CC,BIT7|BIT6,((UINT8)Str2Num(param[6]))<<6);

			AsiaMemoryModify8(0xFEB14000+0x2C0,BIT5|BIT4,((UINT8)Str2Num(param[7]))<<4);

			PCIeDumpMem(0xFEB14000+0x280,0x10);
			PCIeDumpMem(0xFEB14000+0x2C0,0x10);
			
			
		}


		if(!StrCmp(param[2], L"PEMCU")){
			Print(L"0 : %d\n",Eye_GetEyeWidth(0,96,31,32));
			Print(L"1 : %d\n",Eye_GetEyeWidth(1,96,31,32));
		}






}





//for lane2
VOID
DebugMode3_SetRxEqAutoMode(IN CHAR16 **param)
{
	//
	//Enable CPU to RW some EPHY regsiters
	//
	AsiaMemoryModify8(0xFEB14000+0xE0,BIT6,BIT6);

		if(!StrCmp(param[2], L"Auto")){


			//
			//Rx EQ  Auto Mode Enable
			//
			AsiaMemoryModify8(0xFEB14000+0x353,BIT7,0);
			AsiaMemoryModify8(0xFEB14000+0x354,BIT7,0);
			AsiaMemoryModify8(0xFEB14000+0x355,BIT7,0);
			AsiaMemoryModify8(0xFEB14000+0x356,BIT7,0);
			AsiaMemoryModify8(0xFEB14000+0x357,BIT7,0);


			//
			//Set EQTNVTH 
			//
			Print(L"EQTNVTH<4:0> = %d\n",(UINT8)Str2Num(param[3]));
			AsiaMemoryModify8(0xFEB14000+0x352,0x3F,(UINT8)Str2Num(param[3]));

			//
			//dump all related registers
			//
			PCIeDumpMem(0xFEB14000+0x300,0x100);
			PCIeDumpMem(0xFEB14000+0x350,0x10);
	
			//
			//set Request EQ - we need to execute a EQ process to make those 'Auto Rx EQ' effect
			//
			AsiaPcieModify8(0xE0000000,AsiaPciAddress(0,4,0,0x2A4),BIT0,BIT0);

#if 1
			//
			//DO HotReset
			//
			AsiaPcieModify8(0xE0000000,AsiaPciAddress(0,4,0,0x3E),BIT6,BIT6);
			gBS->Stall(1000000);
			AsiaPcieModify8(0xE0000000,AsiaPciAddress(0,4,0,0x3E),BIT6,0);
			gBS->Stall(100000);

			//
			//Dump the registers we need
			//
			PCIeDumpCfg(0xE0000000,0,4,0,0x100);
			PCIeDumpMem(0xFEB14000+0xA10,0x10);
#endif	
			
		}
		if(!StrCmp(param[2], L"RST")){

			//
			// dump those register after EyeMonitor
			// Note: actually this 
			//
			PCIeDumpMem(0xFEB14000+0xA10,0x10);
	
		}

		if(!StrCmp(param[2], L"More1")){

			Print(L"%d,%d,%d,%d,%d\n",(UINT8)Str2Num(param[3]), 
										(UINT8)Str2Num(param[4]),
										(UINT8)Str2Num(param[5]),
										(UINT8)Str2Num(param[6]),
										(UINT8)Str2Num(param[7]) );
			
			AsiaMemoryModify8(0xFEB14000+0x348,BIT4|BIT3|BIT2,((UINT8)Str2Num(param[3]))<<2);

			AsiaMemoryModify8(0xFEB14000+0x301,BIT2,((UINT8)Str2Num(param[4]))<<2);

			AsiaMemoryModify8(0xFEB14000+0x34C,BIT3|BIT2,((UINT8)Str2Num(param[5]))<<2);

			AsiaMemoryModify8(0xFEB14000+0x34C,BIT7|BIT6,((UINT8)Str2Num(param[6]))<<6);

			AsiaMemoryModify8(0xFEB14000+0x340,BIT5|BIT4,((UINT8)Str2Num(param[7]))<<4);

			PCIeDumpMem(0xFEB14000+0x300,0x10);
			PCIeDumpMem(0xFEB14000+0x340,0x10);
			
			
		}


}














