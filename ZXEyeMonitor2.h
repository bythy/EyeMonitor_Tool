//**********************************************************************
//**********************************************************************
//**                                                                                                              **
//**     Copyright (c) 2015 Shanghai Zhaoxin Semiconductor Co., Ltd.                     **
//**                                                                                                              **
//**     Author: Chris J.Wang  --  chrisjwang@zhaoxin.com                                  **
//**********************************************************************
//**********************************************************************


#ifndef __ZX_EYE_MONITOR_2_H__
#define __ZX_EYE_MONITOR_2_H__

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/SortLib.h>

#define EYE_CMD_PLUS 1  //this default enable for CHX002
#define EYE2_DEBUG 1   //use for debug message 

//#define SLAVE_SOCKET 1


typedef struct CmdCycle{
	UINT8 Cmd:4;
	UINT8 reserved0:4;
	UINT8 LaneNumber;
	UINT16 reverved1;
}CmdC;

typedef struct DataCycle{
	UINT8 Mode;
	UINT8 Threshold;
	UINT8 Phy;
	UINT8 PortNumber;
}DataC;



typedef struct DataCycle1{
	UINT8 x1;
	UINT8 y1;
	UINT8 x2;
	UINT8 y2;
}DataC1;
typedef struct DataCycle2{
	UINT8 ScanTime;
	UINT8 reserved1;
	UINT8 reserved2;
	UINT8 reserved3;
}DataC2;



#define CMD_DATA_SCRATCH 0x9D8
#define PEMCU_SENT_SCRATCH 0x9D0 //result
#define BIOS2MCU_DOORBELL 0x9CC
#define MCU2BIOS_DOORBELL 0x9C8
#define DATA_EXP1_SCRATCH 0xF8
#define DATA_EXP2_SCRATCH 0xFC


VOID
EyeCreateCfgFile(VOID);

EFI_STATUS
EyeMcuCommand(
	UINT8 Mode, 
	UINT16 Port,
	UINT16 Lane,
	UINT8 Threshold);

EFI_STATUS
EyeHorFConfig(UINT8 HorF,UINT32 EphyBase);





unsigned short
Eye_GetEyeWidth(
	unsigned char WidthonWhichLine,
	unsigned char Xstart,
	unsigned char Xend,
	unsigned char YLength
);



#endif
