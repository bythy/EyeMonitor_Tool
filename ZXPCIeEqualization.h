//**********************************************************************
//**********************************************************************
//**                                                                                                              **
//**     Copyright (c) 2015 Shanghai Zhaoxin Semiconductor Co., Ltd.                     **
//**                                                                                                              **
//**     Author: Chris J.Wang  --  chrisjwang@zhaoxin.com                                  **
//**********************************************************************
//**********************************************************************
#ifndef __ZX_PCIE_EQ_H__
#define __ZX_PCIE_EQ_H__


#include "Top.h"


//those define located in PEMCU Test Spec
#define CMD_READ  1
#define CMD_WRITE  2
#define CMD_TxPreset  3
#define CMD_Cursor  4
#define CMD_EyeMonitor  5



#define DOORBELL_C2P  0
#define SCRATCH_REG_C2P  1
#define FS_FAREND  2 
#define LF_FAREND  3
#define SCRATCH_REG_P2C 4
#define EPHYCFG_REG 5

#if 0
typedef union{
		UINT8 Preset;
		UINT8 PreCursor;
}FPax;
 #endif

typedef struct FarendTxParameterCmd{
	UINT8 Cmd;
	UINT8 PresetPreCursor;
	UINT8 Cursor;
	UINT8 PostCursor;
}FTPC;


//[7:4] = Dev Num
//[3:0] = Func Num
//Bus number = 0
#define PE0 0x30  //PE0 30 : D3F0
#define PE1 0x31
#define PE2 0x32
#define PE3 0x33

#define PE4 0x40 
#define PE5 0x41  //PE5 41: D4F1

#define PE6 0x50
#define PE7 0x51

#define VPI 0x70
#define PENA 0xFF




#define NUM_EPHYCFG 12  //number of EPHY config of this chip


typedef struct PCIE_EPHYCFG{
	UINT8 Ecfg;
	UINT8 LanePortMap[TOTAL_LANE];
}EPHYCFG;


#define EPHYBASE 0xFEB14000



//
// input: lane number
// output : corresponding Port
//
UINT8 
EQ_LaneMaptoPort(UINT32 BaseAddr, UINT8 LaneNum);


EFI_STATUS 
EQ_HotResetGen3Confirm(UINT32 BaseAddr, UINT8 LaneNum);


//
//the normal sequence is:
//Read FarendFS/LF(this function)  --> Set Param to PEMCU--> HotReset to Redo EQ --> Verify Farend Param
//
UINT8
EQ_GetFs_SpecificLane(UINT32 BaseAddr,UINT8 LaneNum);


//
//the normal sequence is:
//Read FarendFS/LF(this function)  --> Set Param to PEMCU--> HotReset to Redo EQ --> Verify Farend Param
//
UINT8
EQ_GetLf_SpecificLane(UINT32 BaseAddr,UINT8 LaneNum);


//
//the normal sequence is:
//Read FarendFS/LF --> Set Param to PEMCU(this function) --> HotReset to Redo EQ --> Verify Farend Param
//
EFI_STATUS
EQ_SendCmdToAdjustTxParam(UINT32 EphyBase, FTPC *pFtpc);

//
//the normal sequence is:
//Read FarendFS/LF --> Set Param to PEMCU --> HotReset to Redo EQ --> Verify Farend Param(this function)
//
EFI_STATUS
EQ_VerifyFarendParam(UINT32 BaseAddr,UINT8 LaneNum, FTPC *pFtpc);




EFI_STATUS
EQ_RedoEQTest(UINT8 Bus,UINT8 Dev, UINT8 Func, UINT32 Delayus);

EFI_STATUS
EQ_PcieEyeMonitorDbg(VOID);


EFI_STATUS
RxEQ_Dbg(VOID);

//
//	Before run this test, pls change the BIOS setup and change DOEQ method as fixed EQ
//
VOID
EQ_ScanCoeff_For_Specific_Lane(UINT32 EphyBase,UINT8 LaneNum);

//
//	Before run this test, pls change the BIOS setup and change DOEQ method as fixed EQ
//		PreCursor incrase first
//
VOID
EQ_ScanCoeff_For_Specific_Lane_2(UINT32 EphyBase,UINT8 LaneNum);

//
//	Before run this test, pls change the BIOS setup and change DOEQ method as fixed EQ
//
VOID
EQ_ScanCoeff_For_Specific_Lane_CheckSingleCursor(
	UINT32 EphyBase,UINT8 LaneNum,
	UINT8 PreCursor,
	UINT8 Cursor,
	UINT8 PostCursor);


#endif
