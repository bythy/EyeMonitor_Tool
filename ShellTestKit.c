//**********************************************************************
//**********************************************************************
//**                                                                                                              **
//**     Copyright (c) 2015 Shanghai Zhaoxin Semiconductor Co., Ltd.                     **
//**                                                                                                              **
//**     Author: Chris J.Wang  --  chrisjwang@zhaoxin.com                                  **
//**********************************************************************
//**********************************************************************




#include "Top.h"


char strin[20] = {0};







/**
  UEFI application entry point which has an interface similar to a
  standard C main function.

  The ShellCEntryLib library instance wrappers the actual UEFI application
  entry point and calls this ShellAppMain function.

  @param  Argc             Argument count
  @param  Argv             The parsed arguments

  @retval  0               The application exited normally.
  @retval  Other           An error occurred.

**/
INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{
UINT16 Basex;
  UINT8 arx;
  UINT8 LinkSpeed;
  UINT32 ix;
  UINT32 iy;
 	//CHAR16 *LocalFileName;
 UINT32 EphyBase;
 UINT8 X1 = 0,Y1 = 0,X2 = 0,Y2 = 0,ScanTime = 0;
 EphyBase = GetCfg(&X1,&Y1,&X2,&Y2,&ScanTime);
  strin[0] = 0;
//	LocalFileName = (CHAR16 *)AllocateZeroPool(128);

  Print(L"=========Version 20181204=========\n");


  Print(L"Argc = %d\n", Argc);
  for(arx = 0; arx < Argc; arx++){
	Print(L"Argv[%d] = %s\n",arx, Argv[arx]);
  }


  if(Argc == 1){
	goto __Help;
  }
  if(!StrCmp(Argv[1], L"?")){
  	goto __Help;
  }

  
///////////////////////////////////////////////////////////////////////////////////////
  //
  // test tool for eye monitor
  //
  if(!StrCmp(Argv[1], L"-eye")){

	if(Argc < 3){
		goto __Help;
	}

	if(!StrCmp(Argv[2],L"-v")){  //verify
		//StrCpy(LocalFileName, Argv[3]);
  		EyeTest(Argv[3]);
	}else if(!StrCmp(Argv[2],L"-c")){  //create confige file
		CreateCfgFile(Argv[3]);
	}else{
		Print(L"Input error!\n");
	}


	goto __ExitDirect;
  }
///////////////////////////////////////////////////////////////////////////////////////
	//
	// command tool 
	//
	if(!StrCmp(Argv[1], L"-cmd")){
		if(Argc < 3){
		 	goto __Help;
	  	}
		
		if(!StrCmp(Argv[2], L"port")){ //-cmd port  portnum   threshold  L"-cmd port [Port Num] [Threshold]"
			if(Argc < 5){
		 		goto __Help;
	  		}
			EyeMcuCommand(1, (UINT16)Str2Num(Argv[3]),0xFFFF,(UINT8)Str2Num(Argv[4]));
		}else if(!StrCmp(Argv[2], L"lane")){  //-cmd lane PortNum, Lanenum Threshold    L"-cmd lane [PortNum] [LaneNum] [Threshold]"
			if(Argc < 6){
		 		goto __Help;
	  		}
			EyeMcuCommand(2, (UINT16)Str2Num(Argv[3]),(UINT8)Str2Num(Argv[4]),(UINT8)Str2Num(Argv[5]));
		}else if(!StrCmp(Argv[2], L"createcfg")){
			EyeCreateCfgFile();
		}else if(!StrCmp(Argv[2], L"exp")){
			Print(L"^0\n");
			Print(L"|.\n");
			Print(L"|.\n");
			Print(L"|.\n");
			Print(L"|.\n");
			Print(L"|.\n");
			Print(L"|.\n");
			Print(L"|31\n");
			Print(L"|  96.......127-00.......31\n");
			Print(L"+--------------------------->\n");
			Print(L"when set the x1,y1,x2,y2 in cfg file, you can select:\n");
			Print(L"-->(x1,y1) from 'upper left'\n");
			Print(L"-->(x2,y2) from 'low right'\n");
		}else if(!StrCmp(Argv[2], L"justprint")){
			if(Argc < 7){
				goto __Help;
			}
			OutPutBerVal_3(Str2Num(Argv[3]),Str2Num(Argv[4]), Str2Num(Argv[5]),Str2Num(Argv[6]),EphyBase);
			
		}else if(!StrCmp(Argv[2], L"half")){
		
			Print(L"half EphyBase:\n",EphyBase);
			EyeHorFConfig(2,EphyBase);  //set all eye monitor mode as half-rate (for each lanes)	

		}else if(!StrCmp(Argv[2], L"full")){

			EyeHorFConfig(3,EphyBase);//set all eye monitor mode as full-rate (for each lanes)
			Print(L"full EphyBase:\n",EphyBase);


		}else if(!StrCmp(Argv[2], L"fix")){
			
			if(!StrCmp(Argv[3], L"preset")){
				
				if(Argc<7){
					goto __Help;
				}
				
				//app.efi  -cmd fix preset 9
				// TODO: to be implement

			}else if(!StrCmp(Argv[3], L"cursor")){
				//app.efi  -cmd fix curosr 4  20  4 
				//app.efi  -cmd fix curosr all
				if(!StrCmp(Argv[4], L"allscan")){
					
					if(Argc<7){
						goto __Help;
					}
					
					EQ_ScanCoeff_For_Specific_Lane(Str2Num(Argv[5]), (UINT8)Str2Num(Argv[6]));

				}else if(!StrCmp(Argv[4], L"allscan2")){
					if(Argc<7){
						goto __Help;
					}

					EQ_ScanCoeff_For_Specific_Lane_2(Str2Num(Argv[5]), (UINT8)Str2Num(Argv[6]));

				}else{
				
					if(Argc<9){
						goto __Help;
					}
					EQ_ScanCoeff_For_Specific_Lane_CheckSingleCursor(Str2Num(Argv[7]), 
															(UINT8)Str2Num(Argv[8]),
															(UINT8)Str2Num(Argv[4]),
															(UINT8)Str2Num(Argv[5]),
															(UINT8)Str2Num(Argv[6]));
				}
				
				
			}else{
				Print(L"Invalid parameter! [preset/cursor]\n");
				goto __Help;
			}

		}else{
			Print(L"Input error!\n");
			goto __Help;
		}



	  goto __ExitDirect;
	}






///////////////////////////////////////////////////////////////////////////////////////
	//
	// Mode3 scan tool - 
	//
	if(!StrCmp(Argv[1], L"-mode3a")){
		if(Argc < 8){
			goto __Help;
		}
		if(!StrCmp(Argv[7], L"slow")){
			EyeMode3Scan(2,Str2Num(Argv[2]),Str2Num(Argv[3]), Str2Num(Argv[4]), Str2Num(Argv[5]), Str2Num(Argv[6]),0);
			goto __ExitDirect;
		}else if(!StrCmp(Argv[7], L"fast")){
			EyeMode3Scan(2,Str2Num(Argv[2]),Str2Num(Argv[3]), Str2Num(Argv[4]), Str2Num(Argv[5]), Str2Num(Argv[6]),1);	
			goto __ExitDirect;
		}else{
			goto __Help;
		}
		
		
	}
	
	if(!StrCmp(Argv[1], L"-mode3b")){
		if(Argc < 8){
			goto __Help;
		}
		if(!StrCmp(Argv[7], L"slow")){
			EyeMode3Scan(3,Str2Num(Argv[2]),Str2Num(Argv[3]), Str2Num(Argv[4]), Str2Num(Argv[5]), Str2Num(Argv[6]),0);
			goto __ExitDirect;
		}else if(!StrCmp(Argv[7], L"fast")){
			EyeMode3Scan(3,Str2Num(Argv[2]),Str2Num(Argv[3]), Str2Num(Argv[4]), Str2Num(Argv[5]), Str2Num(Argv[6]),1);	
			goto __ExitDirect;
		}else{
			goto __Help;
		}
		
		
	}


	//scan cursor table by using Mode3(half rate)
	if(!StrCmp(Argv[1], L"-scan_mode3a")){
		if(Argc < 4){
			goto __Help;
		}
		DebugMode3_AllCursorScan(Str2Num(Argv[2]), (UINT8)Str2Num(Argv[3]));
		goto __ExitDirect;		
	}
	//only to scan one group of Cursor by using mode3(half rate)
	if(!StrCmp(Argv[1], L"-scansingle_mode3a")){
		if(Argc < 4){
			goto __Help;
		}
		DebugMode3_SingleCursorScan(Str2Num(Argv[2]), (UINT8)Str2Num(Argv[3]), 
									(UINT8)Str2Num(Argv[4]),(UINT8)Str2Num(Argv[5]),(UINT8)Str2Num(Argv[6]));
		goto __ExitDirect;		
	}

	//only to scan one group of Cursor by using mode3(half rate)
	if(!StrCmp(Argv[1], L"-scansinglep_mode3a")){
		if(Argc < 4){
			goto __Help;
		}
		DebugMode3_SinglePresetScan(Str2Num(Argv[2]), (UINT8)Str2Num(Argv[3]), 
									(UINT8)Str2Num(Argv[4]));
		goto __ExitDirect;		
	}



///////////////////////////////////////////////////////////////////////////////////////
  //
  // this just for debug
  //
  if(!StrCmp(Argv[1], L"-debug")){
  	//FileTest(Argv[2]);

	if(!StrCmp(Argv[2], L"Delay")){
		Print(L"start debug mode\n");
		gBS->Stall(3000000);
		Print(L"after 3 seconds\n");
	}

	if(!StrCmp(Argv[2], L"D1us")){
		if(Argc < 6){
			goto __Help;
		}

		
		//NanoSecondDelay(100);
		AsiaPcieWrite8(0xE0000000,AsiaPciAddress((UINT8)Str2Num(Argv[3]),
										(UINT8)Str2Num(Argv[4]),
										(UINT8)Str2Num(Argv[5]),0x02),0xA1);
		gBS->Stall(1);  //3.719us
		AsiaPcieWrite8(0xE0000000,AsiaPciAddress((UINT8)Str2Num(Argv[3]),
										(UINT8)Str2Num(Argv[4]),
										(UINT8)Str2Num(Argv[5]),0x02),0xA2);

		gBS->Stall(10);  //12.927us
		AsiaPcieWrite8(0xE0000000,AsiaPciAddress((UINT8)Str2Num(Argv[3]),
										(UINT8)Str2Num(Argv[4]),
										(UINT8)Str2Num(Argv[5]),0x02),0xA3);

		gBS->Stall(100); //102.668us
		AsiaPcieWrite8(0xE0000000,AsiaPciAddress((UINT8)Str2Num(Argv[3]),
										(UINT8)Str2Num(Argv[4]),
										(UINT8)Str2Num(Argv[5]),0x02),0xA4);

		gBS->Stall(1000);  //1.003ms
		AsiaPcieWrite8(0xE0000000,AsiaPciAddress((UINT8)Str2Num(Argv[3]),
										(UINT8)Str2Num(Argv[4]),
										(UINT8)Str2Num(Argv[5]),0x02),0xA5);




	}



	if(!StrCmp(Argv[2], L"PCIE")){
		PciScanX();
	}


	DebugMode3_SetRxEqAutoMode(Argv);
#if 0
	if(!StrCmp(Argv[2], L"Auto")){

		AsiaMemoryModify8(0xFEB14000+0xE0,BIT6,BIT6);
		
		AsiaMemoryModify8(0xFEB14000+0x2D3,BIT7,0);
		AsiaMemoryModify8(0xFEB14000+0x2D4,BIT7,0);
		AsiaMemoryModify8(0xFEB14000+0x2D5,BIT7,0);
		AsiaMemoryModify8(0xFEB14000+0x2D6,BIT7,0);
		AsiaMemoryModify8(0xFEB14000+0x2D7,BIT7,0);

		Print(L"EQTNVTH<4:0> = %d\n",(UINT8)Str2Num(Argv[3]));
		AsiaMemoryModify8(0xFEB14000+0x2D2,0x3F,(UINT8)Str2Num(Argv[3]));
		PCIeDumpMem(0xFEB14000+0x200,0x100);
		PCIeDumpMem(0xFEB14000+0x2D0,0x10);

		
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(0,5,1,0x2A4),BIT0,BIT0);
		
		//AsiaPcieModify8(0xE0000000,AsiaPciAddress(0,5,1,0x50),BIT5,BIT5);
		//DO HotReset
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(0,5,1,0x3E),BIT6,BIT6);
		gBS->Stall(1000000);
		AsiaPcieModify8(0xE0000000,AsiaPciAddress(0,5,1,0x3E),BIT6,0);
		gBS->Stall(100000);

		PCIeDumpCfg(0xE0000000,0,5,1,0x100);

		PCIeDumpMem(0xFEB14000+0xA10,0x10);

		
	}
	if(!StrCmp(Argv[2], L"RST")){
		
		PCIeDumpMem(0xFEB14000+0xA10,0x10);

	}
#endif
#if 0
	if(!StrCmp(Argv[2], L"String")){
		Print(L"xxx\n");
	{
		CHAR16 p[] = L"12345-lanexx";
		CHAR16 pp[20];

		memcpy(pp, p, sizeof(p));
		Print(L"P %s\n",p);
		
		Print(L"PP %s\n",pp);

		pp[10] = L'0';
		pp[11] = L'4';
		Print(L"PP2 %s\n",pp);
		Print(L"PP3 %04X\n",pp[10]);

		pp[10] = (0x30+2);
		Print(L"PP4 %s\n",pp);
		
	}

		
	}

#endif

	goto __ExitDirect;
  }



	if(!StrCmp(Argv[1], L"gen2test")){
		if(Argc < 6){
			goto __ExitDirect;
		}
		iy = (Str2Num(Argv[5]));
		for(ix=0;ix<iy;ix++){
			if(EFI_SUCCESS == PCIeSearchCapOffset((UINT8)Str2Num(Argv[2]),(UINT8)Str2Num(Argv[3]),(UINT8)Str2Num(Argv[4]),0,0x10, &Basex)){

					AsiaPcieModify8(0xE0000000,AsiaPciAddress((UINT8)Str2Num(Argv[2]),(UINT8)Str2Num(Argv[3]),(UINT8)Str2Num(Argv[4]),Basex+0x30),0xF,1);
					AsiaPcieModify8(0xE0000000,AsiaPciAddress((UINT8)Str2Num(Argv[2]),(UINT8)Str2Num(Argv[3]),(UINT8)Str2Num(Argv[4]),Basex+0x10),BIT5,BIT5);
					gBS->Stall(500000);
					LinkSpeed = AsiaPcieRead8(0xE0000000,AsiaPciAddress((UINT8)Str2Num(Argv[2]),(UINT8)Str2Num(Argv[3]),(UINT8)Str2Num(Argv[4]),Basex+0x12));
					if((LinkSpeed & 0xF) != 1){
						Print(L"Error! change Gen1 Fail! current:%d\n",LinkSpeed&0xF);
					}else{
						Print(L"current:%d\n",LinkSpeed&0xF);
					}
					AsiaPcieModify8(0xE0000000,AsiaPciAddress((UINT8)Str2Num(Argv[2]),(UINT8)Str2Num(Argv[3]),(UINT8)Str2Num(Argv[4]),Basex+0x30),0xF,2);
					AsiaPcieModify8(0xE0000000,AsiaPciAddress((UINT8)Str2Num(Argv[2]),(UINT8)Str2Num(Argv[3]),(UINT8)Str2Num(Argv[4]),Basex+0x10),BIT5,BIT5);
					gBS->Stall(500000);
					LinkSpeed = AsiaPcieRead8(0xE0000000,AsiaPciAddress((UINT8)Str2Num(Argv[2]),(UINT8)Str2Num(Argv[3]),(UINT8)Str2Num(Argv[4]),Basex+0x12));
					if((LinkSpeed & 0xF) != 2){
						Print(L"Error! change Gen2 Fail! current:%d\n",LinkSpeed&0xF);
					}else{
						Print(L"current:%d\n",LinkSpeed&0xF);
					}



						
			}else{
				

			}
		}	
		goto __ExitDirect;
	}




	//
	//redo-EQ and check AER
	//
	if(!StrCmp(Argv[1], L"-EMEQTEST")){
		if(Argc < 6){
			goto __ExitDirect;
		}
		Print(L"==========================================================================\n");
		Print(L"==========================================================================\n");
		Print(L"==============================EMEQ TEST===================================\n");
		Print(L"==========================================================================\n");
		Print(L"==========================================================================\n");
		if((Str2Num(Argv[2]) == 0)&&(Str2Num(Argv[3]) == 0)&&(Str2Num(Argv[4]) == 0)){
			EQ_RedoEQTest(0,3,0,Str2Num(Argv[5]));
			EQ_RedoEQTest(0,3,1,Str2Num(Argv[5]));
			EQ_RedoEQTest(0,3,2,Str2Num(Argv[5]));
			EQ_RedoEQTest(0,3,3,Str2Num(Argv[5]));
			EQ_RedoEQTest(0,4,0,Str2Num(Argv[5]));
			EQ_RedoEQTest(0,4,1,Str2Num(Argv[5]));
			EQ_RedoEQTest(0,5,0,Str2Num(Argv[5]));
			EQ_RedoEQTest(0,5,1,Str2Num(Argv[5]));
			EQ_PcieEyeMonitorDbg();
		}else{
			EQ_RedoEQTest((UINT8)Str2Num(Argv[2]), (UINT8)Str2Num(Argv[3]),(UINT8)Str2Num(Argv[4]),Str2Num(Argv[5]));
			EQ_PcieEyeMonitorDbg();
		}

		goto __ExitDirect;
	}


	if(!StrCmp(Argv[1], L"-RXEQTEST")){
		if(Argc < 6){
			goto __ExitDirect;
		}
		Print(L"==========================================================================\n");
		Print(L"==========================================================================\n");
		Print(L"==============================RXEQ TEST===================================\n");
		Print(L"==========================================================================\n");
		Print(L"==========================================================================\n");
		if((Str2Num(Argv[2]) == 0)&&(Str2Num(Argv[3]) == 0)&&(Str2Num(Argv[4]) == 0)){
			EQ_RedoEQTest(0,3,0,Str2Num(Argv[5]));
			EQ_RedoEQTest(0,3,1,Str2Num(Argv[5]));
			EQ_RedoEQTest(0,3,2,Str2Num(Argv[5]));
			EQ_RedoEQTest(0,3,3,Str2Num(Argv[5]));
			EQ_RedoEQTest(0,4,0,Str2Num(Argv[5]));
			EQ_RedoEQTest(0,4,1,Str2Num(Argv[5]));
			EQ_RedoEQTest(0,5,0,Str2Num(Argv[5]));
			EQ_RedoEQTest(0,5,1,Str2Num(Argv[5]));
			RxEQ_Dbg();
		}else{
			EQ_RedoEQTest((UINT8)Str2Num(Argv[2]), (UINT8)Str2Num(Argv[3]),(UINT8)Str2Num(Argv[4]),Str2Num(Argv[5]));

			RxEQ_Dbg();
		}

		goto __ExitDirect;

		
		

	}


	if(!StrCmp(Argv[1], L"-REDOEQ")){
		if(Argc < 6){
			goto __ExitDirect;
		}
		Print(L"============================Just Redo EQ START=================================\n");

		EQ_RedoEQTest(0,3,0,Str2Num(Argv[5]));
		EQ_RedoEQTest(0,3,1,Str2Num(Argv[5]));
		EQ_RedoEQTest(0,3,2,Str2Num(Argv[5]));
		EQ_RedoEQTest(0,3,3,Str2Num(Argv[5]));
		EQ_RedoEQTest(0,4,0,Str2Num(Argv[5]));
		EQ_RedoEQTest(0,4,1,Str2Num(Argv[5]));
		EQ_RedoEQTest(0,5,0,Str2Num(Argv[5]));
		EQ_RedoEQTest(0,5,1,Str2Num(Argv[5]));

		Print(L"============================Just Redo EQ END=================================\n");

		// TODO:  report some link status...

		goto __ExitDirect;	

	}


	if(!StrCmp(Argv[1], L"-beep")){
		Print(L"============================BEEP=================================\n");
		AsiaIoModify8(0x61,BIT0|BIT1,BIT0|BIT1);
		AsiaIoModify8(0x43,0xFF,0xB6);
		AsiaIoModify8(0x42,0xFF,0x11);
		AsiaIoModify8(0x42,0xFF,0x11);
		gBS->Stall(300000);
		AsiaIoModify8(0x61,BIT0,0);
		gBS->Stall(1000000);
		
		goto __ExitDirect;	
	}





///////////////////////////////////////////////////////////////////////////////////////

  
#if 0  // test code  --- for CHAR16 *
  {	
	char *a = "xxx";
	Char16Char8(Argv[1], strin);
	if(!strcmp(strin, "xxx")){
		Print(L"Good\n");  
	}else{
		Print(L"Argv[1] = %s\n", Argv[1]);
		Print(L"Argv[1] = %02x  %02x %02x %02x %02x %02x %02x %02x %02x %02x \n", 
				*(char *)(Argv[1]),
				*(((char *)(Argv[1]))+1),
				*(((char *)(Argv[1]))+2),
				*(((char *)(Argv[1]))+3),
				*(((char *)(Argv[1]))+4),
				*(((char *)(Argv[1]))+5),
				*(((char *)(Argv[1]))+6),
				*(((char *)(Argv[1]))+7),
				*(((char *)(Argv[1]))+8),
				*(((char *)(Argv[1]))+9)
				);

		Print(L"a = %02x  %02x %02x %02x %02x %02x %02x %02x %02x %02x \n", 
				*((char *)a),
				*(((char *)a)+1),
				*(((char *)a)+2),
				*(((char *)a)+3),
				*(((char *)a)+4),
				*(((char *)a)+5),
				*(((char *)a)+6),
				*(((char *)a)+7),
				*(((char *)a)+8),
				*(((char *)a)+9));
		
  
	}
  }
#endif
			
		
	
__Help:
#ifdef SLAVE_SOCKET
	Print(L"-this tool for slave socket eye mointor test!\n");
#else
    Print(L"-this tool for master socket eye monitor test!\n");
#endif
	Print(L"-eye -c [mode1|mode2|mode3|modex]    ---Create Eye monitor config file template\n");
	Print(L"-eye -v <CfgFile>    ---Eye monitor test tool\n");
	Print(L">>>For Mode1/2,need to check log file: Eye_Mode1_BER_VAL.log/Eye_Mode2_BER_VAL.log\n");
	Print(L"====================================================================\n");
	Print(L"-cmd [HorF]\n");
	Print(L"         [HorF]: half: enable half-rate eye monitor \n");
	Print(L"                 full: enable full-rate eye monitor \n");
	Print(L"-cmd port [PortNum] [Threshold]\n");
	Print(L"        >>before run this cmd, you need to run '-cmd half/full' firstly\n");
	Print(L"-cmd lane [PortNum] [LaneNum] [Threshold]\n");
	Print(L"        [PortNum]: 0(PE0) 1(PE1) 2(PE2) 3(PE3)  4(PE4) 5(PE5) 6(PE6) 7(PE7) 8(VPI)\n");
	Print(L"        [LaneNum]: 0~7\n");
	Print(L"        [Threshold]: 0~0xF\n");	
	Print(L"        >>before run this cmd, you need to run '-cmd half/full' firstly\n");
	Print(L"-cmd createcfg   ---create the config file\n");
	Print(L"-cmd exp   ---eye diagram example\n");
	Print(L"-cmd justprint   ---print EPHY BERVAL[2048:0] as a eye diagram directly\n");
	Print(L"-cmd fix cursor allscan/allscan2  [0xEphyBase] [LaneNum]  ---redo EQ to scan all cursor groups and get the eye diagram<Excel>\n");
	Print(L"        >>before do this test, change BIOS setup as DOEQ+FixedEQ\n");
	Print(L"        >>before do this test, need to run ShellTestKit.efi -cmd half/full\n");
	Print(L"        >>Param allscan is PostCur increase first, allscan2 is PreCur increase first\n");
	Print(L"-cmd fix cursor PreCur Cur PostCur [0xEphyBase] [LaneNum]  ---redo EQ to specific cursor groups and get the eye diagram<Excel>\n");
	Print(L"        >>before do this test, change BIOS setup as DOEQ+FixedEQ\n");
	Print(L"        >>before do this test, need to run ShellTestKit.efi -cmd half/full\n");
	Print(L"====================================================================\n");
	Print(L"-mode3a/mode3b [PhyMmioBase] [GenSpeed] [LaneSel] [ScanTime] [PassThreshold] [MODE]\n");
	Print(L"        PhyMmioBase: MMIO base addres of EPHY,  eg.0xFEB14000 \n");
	Print(L"        GenSpeed: target test gen speed  eg. 1/2/3\n");
	Print(L"        LaneSel: 	0,1,2,3....15\n");
	Print(L"        ScanTime:  us\n");
	Print(L"        PassThreshold:  can be 0~1023\n");
	Print(L"        MODE:  slow/fast\n");
	Print(L"          >>fast mode: no log file generated\n");
	Print(L"          >>slow mode: log file is EyeMode3Scan-Lanexx_A.log and EyeMode3Scan-Lanexx_B.log\n");
	Print(L"-scan_mode3a [PhyMmioBase] [LaneSel]\n");
	Print(L"        >>auto scan cursor table (need BIOS DO_EQ)\n");
	Print(L"-scansingle_mode3a [PhyMmioBase] [LaneSel] [PreCursor] [Cursor] [PostCursor]\n");
	Print(L"        >>scan single group of cursor (need BIOS DO_EQ)\n");
	Print(L"-scansinglep_mode3a [PhyMmioBase] [LaneSel] [Preset]\n");	
	Print(L"        >>scan single preset (need BIOS DO_EQ)\n");
	Print(L" >>mode3a: half eye  /  mode3b:whole eye\n");
	Print(L"====================================================================\n");
	Print(L"-EMEQTEST [Bus] [Dev] [Func] [DelayTime] - redo EMEQ test, to test EMEQ Phase3\n");
	Print(L"  >>if your Bus,Dev,Func = 0,0,0,  then the tool will redoEQ for all ports\n");
	Print(L"  >>DelayTime: us\n");
	Print(L"  >>Note: this tool only for Tx EMEQ fine tune, BIOS need load PEMCU FW and FW must support TxEMEQ(Use lastest version FW)\n");
	Print(L"====================================================================\n");
	Print(L"-RXEQTEST [Bus] [Dev] [Func] [DelayTime] - redo RxEQ, to test RxEQ in Phase3\n");
	Print(L"  >>if your Bus,Dev,Func = 0,0,0,  then the tool will redoEQ for all ports\n");
	Print(L"  >>DelayTime: us\n");
	Print(L"  >>Note: this tool for RxEQ, BIOS need to load PEMCU FW and FW must support RxEQ(use lastest version FW)\n");
	Print(L"====================================================================\n");	
	Print(L"-debug\n");	
	Print(L"-beep  beep 300ms\n");	


///////////////////////////////////////////////////////////////////////////////////////////
#if 0
{
	UINT8 BaseBit = 0;
	UINT32 FieldMask= 0;
	Str2NumBitField(L"Rx1234[31:14]",&FieldMask,&BaseBit);
	Print(L"0x%08x, %d\n",FieldMask,BaseBit);	
}	
#endif

#if 0
{
	CHAR16 Data[255];
	GetParamDataX(L"RP_PIC.cfg",L"PE0",2,Data);

	Print(L"<CJW> %s\n",Data);

}
#endif
///////////////////////////////////////////////////////////////////////////////////////////




	return 0;
	
__ExitDirect:	

  return 0;
}
