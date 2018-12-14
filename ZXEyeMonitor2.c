//**********************************************************************
//**********************************************************************
//**                                                                                                              **
//**     Copyright (c) 2015 Shanghai Zhaoxin Semiconductor Co., Ltd.                     **
//**                                                                                                              **
//**     Author: Chris J.Wang  --  chrisjwang@zhaoxin.com                                  **
//**********************************************************************
//**********************************************************************

#include "ZXEyeMonitor.h"
#include "ZXEyeMonitor2.h"

#include "Top.h"



CmdC Cmdx = {0xF, 0x0, 0xFF, 0x0000};
DataC Datax = {0xFF, 0xFF,0xFF,0xFF};
#if EYE_CMD_PLUS	
DataC1 Datax1 = {0xFF, 0xFF,0xFF,0xFF};
DataC1 Datax2 = {0xFF, 0x00,0x00,0x00};
#endif


extern M3StFEL Mode3Def[];

UINT8 CurrentLinkGenSpeed = 0;

UINT8
CurrentLinkSpeed(UINT16 PortId)
{
	UINT8 Bus,Dev,Func;
	Bus = 0;
	
	switch(PortId){
	case 0: Dev = 3; Func = 0; break;
	case 1: Dev = 3; Func = 1; break;
	case 2: Dev = 3; Func = 2; break;
	case 3: Dev = 3; Func = 3; break;
	case 4: Dev = 4; Func = 0; break;
	case 5: Dev = 4; Func = 1; break;
	case 6: Dev = 5; Func = 0; break;
	case 7: Dev = 5; Func = 1; break;
	case 8: Dev = 7; Func = 0; break;
	default: Dev = 0xA5; Func = 0xA5;break;

	}
	return (0xF & AsiaPcieRead8(0xE0000000,AsiaPciAddress(Bus,Dev,Func,0x52)));
}


/**
Note:
some PEMCU FW will not see those parameters, but some FW will see this, 
but no matter PEMCU FW need it or not, we must provide a cfg file with those parameters!
**/
UINT32 
GetCfg(
	IN OUT UINT8 *pX1, 
	IN OUT UINT8 *pY1, 
	IN OUT UINT8 *pX2, 
	IN OUT UINT8 *pY2, 
	IN OUT UINT8 *pScanTime)
{
	CHAR16 *strinx;
	UINT32 DataTemp32 = NOT_FOUND_STRING; 
	UINT32 BaseAddr;


	strinx = (CHAR16 *)AllocateZeroPool(512);


	if(EFI_SUCCESS != TestOpenFile(L"EyeCmd.cfg")){
		FreePool(strinx);
		return NOT_FOUND_STRING;
	}

	DataTemp32 = (UINT32)GetParamData(L"EyeCmd.cfg", L"EPHYBASE", strinx);
	if(DataTemp32 != NOT_FOUND_STRING){
		BaseAddr = Str2Num(strinx);
		Print(L"<GetCfg> EPHYBASE = [0x%08x]\n", BaseAddr);

	}else{
		BaseAddr = NOT_FOUND_STRING;
		Print(L"-------  EPHYBASE: NULL \n");
		FreePool(strinx);
		return NOT_FOUND_STRING;
	}

#if EYE_CMD_PLUS	

	DataTemp32 = (UINT32)GetParamData(L"EyeCmd.cfg", L"PointX1", strinx);
	if(DataTemp32 != NOT_FOUND_STRING){
		*pX1 = (UINT8)Str2Num(strinx);
		Print(L"<GetCfg> PointX1 = %d\n", *pX1);
	}else{
		FreePool(strinx);
		return NOT_FOUND_STRING;
	}

	DataTemp32 = (UINT32)GetParamData(L"EyeCmd.cfg", L"PointY1", strinx);
	if(DataTemp32 != NOT_FOUND_STRING){
		*pY1 = (UINT8)Str2Num(strinx);
		Print(L"<GetCfg> PointY1 = %d\n", *pY1);
	}else{
		FreePool(strinx);
		return NOT_FOUND_STRING;
	}

	DataTemp32 = (UINT32)GetParamData(L"EyeCmd.cfg", L"PointX2", strinx);
	if(DataTemp32 != NOT_FOUND_STRING){
		*pX2 = (UINT8)Str2Num(strinx);
		Print(L"<GetCfg> PointX2 = %d\n", *pX2);
	}else{
		FreePool(strinx);
		return NOT_FOUND_STRING;
	}

	DataTemp32 = (UINT32)GetParamData(L"EyeCmd.cfg", L"PointY2", strinx);
	if(DataTemp32 != NOT_FOUND_STRING){
		*pY2 = (UINT8)Str2Num(strinx);
		Print(L"<GetCfg> PointY2 = %d\n", *pY2);
	}else{
		FreePool(strinx);
		return NOT_FOUND_STRING;
	}

	DataTemp32 = (UINT32)GetParamData(L"EyeCmd.cfg", L"ScanTime", strinx);
	if(DataTemp32 != NOT_FOUND_STRING){
		*pScanTime = (UINT8)Str2Num(strinx);
		Print(L"<GetCfg> ScanTime = %d\n", *pScanTime);
	}else{
		FreePool(strinx);
		return NOT_FOUND_STRING;
	}
#endif

	FreePool(strinx);
	return BaseAddr;
}



VOID
EyeCreateCfgFile(VOID)
{
#ifndef SLAVE_SOCKET
	WriteIIPos(L"EyeCmd.cfg",L"eye_MemBase = 0xFEB14000\n",0);
#if EYE_CMD_PLUS	
	WriteIIPos(L"EyeCmd.cfg",L"PointX1 = 96\n",25);
	WriteIIPos(L"EyeCmd.cfg",L"PointY1 = 0\n",38);
	WriteIIPos(L"EyeCmd.cfg",L"PointX2 = 31\n",50);
	WriteIIPos(L"EyeCmd.cfg",L"PointY2 = 31\n",63);
	WriteIIPos(L"EyeCmd.cfg",L"ScanTime = 0xFF\n",76);
	WriteIIPos(L"EyeCmd.cfg",L"EPHYBASE = 0xFEB14000\n",92);
#endif
#else
	WriteIIPos(L"EyeCmd.cfg",L"eye_MemBase = 0xFEB94000\n",0);
#if EYE_CMD_PLUS	
	WriteIIPos(L"EyeCmd.cfg",L"PointX1 = 96\n",25);
	WriteIIPos(L"EyeCmd.cfg",L"PointY1 = 0\n",38);
	WriteIIPos(L"EyeCmd.cfg",L"PointX2 = 31\n",50);
	WriteIIPos(L"EyeCmd.cfg",L"PointY2 = 31\n",63);
	WriteIIPos(L"EyeCmd.cfg",L"ScanTime = 0xFF\n",76);
	WriteIIPos(L"EyeCmd.cfg",L"EPHYBASE = 0xFEB94000\n",92);
#endif
#endif


}

//Note: this is a temp function
//HorF:  2:half    3:full-rate   others:reserved
EFI_STATUS
EyeHorFConfig(UINT8 HorF,UINT32 EphyBase)
{
	EFI_STATUS Status = EFI_SUCCESS;
	M3StFEL *pMode3Reg = Mode3Def;

	UINT8 loopx;

	
	//need to set EPHY RxE0[6]
	AsiaMemoryModify8(EphyBase+0xE0,BIT6,BIT6);
	
	
	for(loopx =0; loopx<TOTAL_LANE; loopx++){
		MmioBitFieldWrite16(EphyBase + ((pMode3Reg+loopx)->Reg[ABCSEL_CDR].Offset), 
								((pMode3Reg+loopx)->Reg[ABCSEL_CDR].start),
								((pMode3Reg+loopx)->Reg[ABCSEL_CDR].end), 
								HorF); 		
	}
	
	return Status;
}




EFI_STATUS
EyeMcuCommand(
	UINT8 Mode, 
	UINT16 Port,
	UINT16 Lane,
	UINT8 Threshold)
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINT32 Cmd,Data,Data1,Data2;

	UINT8 X1 = 0,Y1 = 0,X2 = 0,Y2 = 0,ScanTime = 0;
	UINT32 EphyBase = 0;
	UINT32 resultx;
	UINT8 Timeout;

	//get special data from cfg file
	EphyBase = GetCfg(&X1,&Y1,&X2,&Y2,&ScanTime);

	if(EphyBase == NOT_FOUND_STRING){
		Print(L"ERROR!! <EyeMcuCommand>: please use '-cmd createcfg' to create a configure file!\n");
		goto __GoOut;
	}  

	//transfer the data (in param or cfg file) to the write-in structure(CMD/DATA/DATA1/DATA2)
	Cmdx.Cmd = 5; //for eye monitor
	Cmdx.LaneNumber = (UINT8)Lane;
	Datax.Mode = Mode;
	Datax.Phy = 0; //reserved for CHX002
	Datax.PortNumber = (UINT8)Port;
	Datax.Threshold = Threshold;

	Cmd = *((UINT32 *)(&Cmdx));
	Data =*((UINT32 *)(&Datax)); 

#if EYE_CMD_PLUS	
	Datax1.x1 = X1;
	Datax1.y1 = Y1;
	Datax1.x2 = X2;
	Datax1.y2 = Y2;

	Datax2.x1 = ScanTime; // Datax2 [7:0] is scantime
	
	Data1 =*((UINT32 *)(&Datax1));  
	Data2 =*((UINT32 *)(&Datax2)); 
#endif

#if EYE2_DEBUG
	Print(L"[EYE2_Debug]-->0x%08x, 0x%08x, 0x%08x, 0x%08x\n",Cmd,Data,Data1,Data2);
#endif

#if 0	
	switch(Mode){
		case 1:
			AsiaMemoryWrite32(EphyBase+CMD_DATA_SCRATCH,Cmd);
			AsiaMemoryModify8(EphyBase+BIOS2MCU_DOORBELL,BIT0,BIT0);
			while(BIT0 & AsiaMemoryRead8(EphyBase+BIOS2MCU_DOORBELL)){
				gBS->Stall(1000000);//1s
				Print(L"Wait PEMCU to clear Doorbell! - CMD cycle\n")
			}
			Print(L"CMD cycle done!");
			

#if EYE_CMD_PLUS
			AsiaMemoryWrite32(EphyBase+DATA_EXP1_SCRATCH,Data1);
			AsiaMemoryWrite32(EphyBase+DATA_EXP2_SCRATCH,Data2);
#endif
			AsiaMemoryWrite32(EphyBase+CMD_DATA_SCRATCH,Data);
			AsiaMemoryModify8(EphyBase+BIOS2MCU_DOORBELL,BIT0,BIT0);
			while(BIT0 & AsiaMemoryRead8(EphyBase+BIOS2MCU_DOORBELL)){
				gBS->Stall(1000000);//1s
				Print(L"Wait PEMCU to clear Doorbell! - DATA cycle\n")
			}
			Print(L"DATA cycle done!");
			break;

		case 2:
			break;
			
		default:
			Status = EFI_NOT_FOUND;
			goto __GoOut;
	}
#endif

	//enable CPU RW for PEMCU specific registers? -- No need to do

	//Do CMD cycle
	AsiaMemoryWrite32(EphyBase+CMD_DATA_SCRATCH,Cmd);
	AsiaMemoryModify8(EphyBase+BIOS2MCU_DOORBELL,BIT0,BIT0);
	while(BIT0 & AsiaMemoryRead8(EphyBase+BIOS2MCU_DOORBELL)){
		gBS->Stall(1000000);//1s
		Print(L"Wait PEMCU to clear Doorbell! - CMD cycle\n");
	}
	Print(L"CMD cycle done!");


	//Do Data Cycle
#if EYE_CMD_PLUS
	//express message on DATA1/DATA2(which not in PEMCU CMD spec)
	AsiaMemoryWrite32(EphyBase+DATA_EXP1_SCRATCH,Data1);
	AsiaMemoryWrite32(EphyBase+DATA_EXP2_SCRATCH,Data2);
#endif
	AsiaMemoryWrite32(EphyBase+CMD_DATA_SCRATCH,Data);
	AsiaMemoryModify8(EphyBase+BIOS2MCU_DOORBELL,BIT0,BIT0);
	while(BIT0 & AsiaMemoryRead8(EphyBase+BIOS2MCU_DOORBELL)){
		gBS->Stall(1000000);//1s
		Print(L"Wait PEMCU to clear Doorbell! - DATA cycle\n");
	}
	Print(L"DATA cycle done!");


	//wait Result cycle
	Timeout = 0;
	do{
		resultx = AsiaMemoryRead32(EphyBase + PEMCU_SENT_SCRATCH);
#if QEMU_SIM
		resultx = 1;
#endif
		if(resultx == 1){ //success
			Print(L"Result-Success!\n");
 		    //Step 6: polling this register EYESCAN_DONE_PEnn (Rx2901h)	
            //JNY add for eye monitor -s
#if QEMU_SIM
			//do nothing
#else
            while((AsiaMemoryRead8(EphyBase +PCIEPHYCFG_EYE_MONITOR_2+1)&BIT6)!=BIT6){
				gBS->Stall(1000000);//1s
				Print(L"Waite Eye Scan Done!\n");
			}
#endif
			AsiaMemoryModify8(EphyBase +PCIEPHYCFG_EYE_MONITOR_2+1,BIT6,BIT6); // clear the EYESCAN_DONE_ 
			Print(L"Eyescan Done(clear status),PEMCU check result!\n");
			//JNY add for eye monitor -e
			//step 7 :dump the BERVAL 2048 point
			//dump the BERVAL 2048 point
			//OutPutBerVal_2(L"EyeCmd.cfg", X1,Y1, X2,Y2, L"Cmd2Pemcu.log");//save value in BERVAL
			CurrentLinkGenSpeed = CurrentLinkSpeed(Port);
			OutPutBerVal_3(X1,Y1, X2,Y2,EphyBase);

			break;
		}else if(resultx == 2){ //fail
			Print(L"Result-Fail!\n");
			Timeout++;
		}else{
			Print(L"Result-Unkonwn!\n");
			Timeout++;
		}
		if(Timeout > 10){
			Print(L"Timeout!\n");
			break;
		}
	}while(1);
	










__GoOut:

	return Status;
}



















#define XLen  64


unsigned short
Eye_GetEyeWidth(
	unsigned char WidthonWhichLine,
	unsigned char Xstart,
	unsigned char Xend,
	unsigned char YLength
)
{
	unsigned char mask = 0;
	unsigned char tempx = 0;
	unsigned short PointID = 0;  //0.1.2.3.4....
	unsigned short RegByte = 0;
	unsigned char LineID = 0;
	unsigned char PassPointOnThisLine = 0;  // this used to calculate the number of pass point on each line 
	unsigned char PointIdOnThisLine = 0;  //point ID for each line, when it come to a new line, this valie will be reset, point ID is : 0 1 2 3 4.... 
	unsigned char PHx_neg = 0xff, PHx_pos = 0xff;

	unsigned char RegReadCount = (((127-Xstart)+Xend)*YLength)>>3;  // (total point count )/ 8

	for(RegByte = 0; RegByte<= RegReadCount; RegByte++){
		tempx = AsiaMemoryRead8(0xFEB14000+0x2A00+RegByte);
		//tempx= XBYTE(MCU_PCIE_BASE + REG_OFFSET_BERVAL + RegByte);
		for(mask = 0 ; mask < 8; mask++){
			//Print(L"xx : %d,%d,%d,%d,%d\n",PointID,PointIdOnThisLine,LineID,PassPointOnThisLine,PointIdOnThisLine);


			if((1<<mask)&tempx){   //this point is a 'pass' point
				PassPointOnThisLine++; 
				if((LineID == WidthonWhichLine)&&(PassPointOnThisLine == 1)){   //found PHx_neg!!! record it
					PHx_neg = PointIdOnThisLine;
					//Print(L"yy : %d\n",PHx_neg);
				}
				if(LineID == WidthonWhichLine){
					PHx_pos = PointIdOnThisLine;
					//Print(L"zz : %d\n",PHx_pos);
				}
				
			}






			PointID++;
			PointIdOnThisLine++;
			if(PointID%XLen == 0){
				LineID++;
				PassPointOnThisLine = 0;  //reset the LinePassCounter
				PointIdOnThisLine=0; //reset the point ID for this line
			}
	
		}
	}

	if((PHx_neg == 0xff)||(PHx_pos == 0xff)){
		return 0xFF;
	}
	
	return  ((PHx_pos-PHx_neg)+1);
}





