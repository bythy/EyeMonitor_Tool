//**********************************************************************
//**********************************************************************
//**                                                                                                              **
//**     Copyright (c) 2015 Shanghai Zhaoxin Semiconductor Co., Ltd.                     **
//**                                                                                                              **
//**     Author: Chris J.Wang  --  chrisjwang@zhaoxin.com                                  **
//**********************************************************************
//**********************************************************************


#ifndef __TOP_H__
#define __TOP_H__

#include "Configure.h"

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/ShellCEntryLib.h>
//#include <Library/SortLib.h>

#include "ZXPciExpress.h"
//#include <Library/FileHandleLib.h>

#if NT32_FILE_ACCESS
#include "ZXLibFile.h"
#else
#include "ZXLibFile2.h"
#endif


#include "ZXLibString.h"

#include <Library/BaseLib.h>
#include "ZXEyeMonitor.h"
#include "ZXEyeMonitor2.h"
#include "ZXEyeMonitorMode3Test.h"
#include "ZXPCIeEqualization.h"

//#include "ZXDebug.h"


#include <Protocol/PciIo.h>
#include <Library/PciCf8Lib.h>
#include <Library/PciExpressLib.h>
#include <Library/PciLib.h>   

#include <Library/IoLib.h> //for MMio operations


//#include <Include/unistd.h>  //for sleep()
#include <Library/UefiBootServicesTableLib.h>


#include <Library/MemoryAllocationLib.h>


#include "ZXLibRW.h"






#define AUTO_MODE_DEBUG  0   //just for temp use




#endif


