//**********************************************************************
//**********************************************************************
//**                                                                                                              **
//**     Copyright (c) 2015 Shanghai Zhaoxin Semiconductor Co., Ltd.                     **
//**                                                                                                              **
//**     Author: Chris J.Wang  --  chrisjwang@zhaoxin.com                                  **
//**********************************************************************
//**********************************************************************

#include "ZXLibFile.h"
#include  "Top.h"

#define DEBUG_LIBFILE 0

/*
CHAR16 strin16[200] = {0};
CHAR16 strin16p[200] = {0};
*/
CHAR16 strin16[200] ;
CHAR16 strin16p[200] ;


#if 0
UINT8
DebugMode(void)
{
	EFI_STATUS Status;
	SHELL_FILE_HANDLE FileHandle;
	UINTN size = 200;
	BOOLEAN filetype;
	UINTN CmdlineNum;

	
	Print(L"[Line count] %d\n",GetTotalLine(L"DebugMode.cfg"));
	
	ReadLine(L"aaa.eye", 2, strin16);
	Print(L"[String in line] %s\n", strin16);


	GetColumn(strin16, strin16p, 3);
	Print(L"[String in column] %s\n", strin16p);
	Print(L"[size of] %d\n", sizeof(strin16p)/sizeof(CHAR16));

	CmdlineNum= GetParamLineNumber(L"aaa.eye", L"eye_code");
	Print(L"[CmdlineNum] %d\n", CmdlineNum);


	
	Status = ShellOpenFileByName(L"Debug.cfg", &FileHandle, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(Status)) {	
		Print(L"Open File failed!\n");
    }

	



	
	ShellFileHandleReadLine(
		FileHandle, 
		strin16, 
		&size, 
		TRUE, 
		&filetype);
	Print(L"[%d]%s\n", size, strin16);
	Print(L"%x\n", strin16[0]);

	
	ShellFileHandleReadLine(
		FileHandle, 
		strin16, 
		&size, 
		TRUE, 
		&filetype);
	Print(L"[%d]%s\n", size, strin16);
	Print(L"%x\n", strin16[0]);

	ShellFileHandleReadLine(
		FileHandle, 
		strin16, 
		&size, 
		TRUE, 
		&filetype);
	Print(L"[%d]%s\n", size, strin16);
	Print(L"%x\n", strin16[0]);
	

	ShellFileHandleReadLine(
		FileHandle, 
		strin16, 
		&size, 
		TRUE, 
		&filetype);
	Print(L"[%d]%s\n", size, strin16);
	Print(L"%x\n", strin16[0]);
	

	ShellFileHandleReadLine(
		FileHandle, 
		strin16, 
		&size, 
		TRUE, 
		&filetype);
	Print(L"[%d]%s\n", size, strin16);
	Print(L"%x\n", strin16[0]);	
	
	
	
	
	

	ShellCloseFile(&FileHandle);

}
#endif



///////////////////////////////////////////////////////////////////////////
//basic code
///////////////////////////////////////////////////////////////////////////
/**
	Read the content of specific line
	@Param:
		FileName:target filename
		Line:line number 
		Buff:the buffer to place the string we get in specific line
**/
UINT8
ReadLine(IN CHAR16 *FileName, IN UINT8 Line, IN OUT CHAR16* Buff)
{
	UINT8 LineCount;
	EFI_STATUS Status;
	SHELL_FILE_HANDLE FileHandle;
	UINTN size = 200;
	BOOLEAN filetype;

	ShellInitialize();
	Status = ShellOpenFileByName(FileName, &FileHandle, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(Status)) {	
		Print(L"<ReadLine> Open File failed!\n");
    }

	for(LineCount = 0; LineCount <= Line ; LineCount++){
		ShellFileHandleReadLine(
			FileHandle, 
			Buff, 
			&size, 
			TRUE, 
			&filetype);
		//Print(L"[%d]%s\n", size, strin16);
		//Print(L"%x\n", strin16[0]);	
//		if(Buff[0] == 0){
//			break;
//		}
	}


	ShellCloseFile(&FileHandle);

	
	
	return 0;
}

/**
	@Param:
		FileName: the filename of target file
	@Return:
		count of line in this file(From 0 to N-1)
	@Note:
		this routine will search the first No-String line, 
		and flag it as the end line of this file,  so if there is a empty line(just a \n) 
		following with a string line, then this string line will not be counted
	@Exp: the following will be calculated as 1, and this routing will return 1
		xxx  --- 0
		yyy  --- 1

		zzz  
**/
UINT8 
GetTotalLine(CHAR16 *FileName)
{
	UINT8 LineCount;
	EFI_STATUS Status;
	SHELL_FILE_HANDLE FileHandle;
	UINTN size = 200;
	BOOLEAN filetype;
//	Print(L"<GetTotalLine> xxxx~\n");
	ShellInitialize();
	Status = ShellOpenFileByName(FileName, &FileHandle, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(Status)) {	
		Print(L"<GetTotalLine> Open File failed!\n");
    }else{
		//Print(L"<GetTotalLine> Open File Success - %a!\n",FileName);
	}


	for(LineCount = 0; LineCount< 0xFE ; LineCount++){
		ShellFileHandleReadLine(
			FileHandle, 
			strin16, 
			&size, 
			TRUE, 
			&filetype);
#if 0 //DEBUG_LIBFILE == 1		
		Print(L"[%d]%s\n", size, strin16);
		Print(L"%x\n", strin16[0]);	
		Print(L"Ascii (TRUE) or UCS2 (FALSE) --->%d \n", filetype?1:0);	
#endif		
		if(strin16[0] == 0){
			break;
		}
	}
#if 0 //DEBUG_LIBFILE == 1
	Print(L"<GetTotalLine> out of ShellFileHandleReadLine\n");
#endif

	ShellCloseFile(&FileHandle);
		
	return LineCount;
}

/**
this is a parser to get the text in the specfic Colume from the input string
	@param:
		input: input string, to be parsed
		output: the buffer to put the text you wanted
		Column: specific column number (from 1 to N)
	@Exp: in following text, Comlumn 3 is 'yyy'
		'xxx   =    yyy'
		
**/
VOID
GetColumn(IN CHAR16* input, IN OUT CHAR16* output, IN UINT8 Column)
{
	UINT8 Cn = 1;
	UINT8 ix = 0;
	
	UINTN offset_s = 0;
	UINTN offset_d = 0;
	
	for(Cn = 1; Cn < 0xFE; Cn++){

		offset_d = 0;
		for(ix = 0 ; ix < 0xFE; ix++){
			*(output+offset_d) = *(input + offset_s);
			
			if(*(input + offset_s) == 0x20){  // found space?
				while(*(input + (++offset_s)) == 0x20);  //skip all space!
				
				// found a 
				break;
			}
			offset_d++;
			offset_s++; 
		}

		if(Cn == Column){	
			//offset_d--; // go back to cancel 'space' char...
			*(output+offset_d) = 0x0;  // replace the 'space' and change to '\0' for this string in the end
			break;
		}
	}
	

}





///////////////////////////////////////////////////////////////////


/**
when we need a command param, calling this routine to get 
the line number from the corresponding config file
	@param: 
		FileName:  the file name of the config file
		Target: the CMD string we need to find
	@return:
		The lane number corresponding to the CMD string
**/
UINTN
GetParamLineNumber(CHAR16* FileName, CHAR16 *Target)
{
	CHAR16 *strintmp;
	CHAR16 *strintmp1;
	UINT8 total, ix;


	strintmp = (CHAR16 *)AllocateZeroPool(512);
	strintmp1 = (CHAR16 *)AllocateZeroPool(512);
	total = GetTotalLine(FileName);
	
	for(ix = 0; ix<= total; ix++){
		ReadLine(FileName, ix, strintmp);
		GetColumn(strintmp, strintmp1, 1);

#if DEBUG_LIBFILE	
		Print(L"<GetParamLineNumber> strintmp = %s\n",strintmp);
		Print(L"<GetParamLineNumber> strintmp1 = %s\n",strintmp1);
#endif
		if(!StrCmp(strintmp1, Target)){
			break;
		}
	}
	if(ix>total){
		return NOT_FOUND_STRING;
	}


	FreePool(strintmp);
	FreePool(strintmp1);
	return ix;
}

/**
this routine provide the 'Data' string corresponding to the Cmd
	@param:
		FileName:the file name of the target file
		Cmd: the cmd string we want to search
		Data: the Data buffer pointer, the data corresponding to the Cmd will be put in 
**/
UINTN
GetParamData(CHAR16 *FileName, IN CHAR16 *Cmd, IN OUT CHAR16* Data)
{
	UINTN line;
	CHAR16 *strintmp;
	//CHAR16 strintmp1[200];


	strintmp = (CHAR16 *)AllocateZeroPool(512);
	

	line = GetParamLineNumber(FileName, Cmd);
	if(line == NOT_FOUND_STRING){
		return NOT_FOUND_STRING;
	}
	ReadLine(FileName, (UINT8)line, strintmp);
	GetColumn(strintmp, Data, 3);

	FreePool(strintmp);
	
	return 0;
}


/**
this routine provide the specific column of 'Data' string corresponding to the Cmd
	@param:
		FileName:the file name of the target file
		Cmd: the cmd string we want to search
		Data: the Data buffer pointer, the data corresponding to the Cmd will be put in 
**/

UINTN
GetParamDataX(CHAR16 *FileName, IN CHAR16 *Cmd, UINT8 Column,IN OUT CHAR16* Data)
{
	UINTN line;
	CHAR16 *strintmp;

	strintmp = (CHAR16 *)AllocateZeroPool(512);
	//CHAR16 strintmp1[200];
	line = GetParamLineNumber(FileName, Cmd);
	ReadLine(FileName, (UINT8)line, strintmp);
	GetColumn(strintmp, Data, Column);

	FreePool(strintmp);
	
	return 0;
}


/**
	Write 'StrWr' into 'FileName', start from offset 0 in this file
	@Param:
		FileName: target file
		StrWr: the content to be written into the file
**/
EFI_STATUS
WriteII(CHAR16 *FileName, CHAR16 *StrWr)
{
	EFI_STATUS Status = EFI_SUCCESS;
	SHELL_FILE_HANDLE FileHandle;
	//UINTN size = 200;
	//BOOLEAN filetype;
	//UINTN CmdlineNum;
	//UINT32 data;
	UINTN WrBufSize;
//	UINT64 FilePos;
	UINT8 *Str8;

	Str8 = (UINT8 *)AllocateZeroPool(1024);

	
	ShellInitialize();


	Status = ShellOpenFileByName(FileName, &FileHandle, 
				EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE|EFI_FILE_MODE_CREATE, 0);
    if (EFI_ERROR(Status)) {	
		Print(L"<WriteII> Open File failed -2!\n");
    }else{
#if DEBUG_LIBFILE      
		Print(L"<WriteII> Open File success - 2!\n");
#endif
	}



//	ShellSetFilePosition(FileHandle, 2);
//	ShellGetFilePosition(FileHandle, &FilePos);
//	Print(L"<WriteII> FilePos = %d\n", FilePos);

#if 0
	WrBufSize = 4;
	Status = ShellReadFile(FileHandle, &WrBufSize, strin16);
	Print(L"ReadFile  %s\n", strin16);	
	Print(L"ReadStatus = %d\n",Status);
	if(Status == EFI_NO_MEDIA){
		Print(L"2-1\n");
	}
	if(Status == EFI_DEVICE_ERROR){
		Print(L"2-2\n");
	}
	if(Status == EFI_VOLUME_CORRUPTED){
		Print(L"2-3\n");
	}
	if(Status == EFI_VOLUME_CORRUPTED){
		Print(L"2-4\n");
	}
	if(Status == EFI_BUFFER_TOO_SMALL){
		Print(L"2-5\n");
	}
#endif
	WrBufSize = StrLen(StrWr); 
#if DEBUG_LIBFILE  
	Print(L"<WriteII> Strlen=%d(char8)\n", WrBufSize);
#endif
	
	Char16Char8(StrWr, Str8);
	Status = ShellWriteFile(FileHandle, &WrBufSize, Str8);
	//Status = ShellWriteFile(FileHandle, &WrBufSize, "aqwertyui");


#if DEBUG_LIBFILE  	
	Print(L"<WriteII> Write Status = %d\n",Status);
	if(Status == EFI_UNSUPPORTED){
		Print(L"<WriteII> 1-1 EFI_UNSUPPORTED\n");
	}
	if(Status == EFI_NO_MEDIA){
		Print(L"<WriteII> 1-2 EFI_NO_MEDIA\n");
	}
	if(Status == EFI_DEVICE_ERROR){
		Print(L"<WriteII> 1-3 EFI_DEVICE_ERROR\n");
	}
	if(Status == EFI_VOLUME_CORRUPTED){
		Print(L"<WriteII> 1-4 EFI_DEVICE_ERROR\n");
	}
	if(Status == EFI_WRITE_PROTECTED){
		Print(L"<WriteII> 1-5 EFI_WRITE_PROTECTED\n");
	}
	if(Status == EFI_ACCESS_DENIED){
		Print(L"<WriteII> 1-6 EFI_ACCESS_DENIED\n");
	}
	if(Status == EFI_VOLUME_FULL){
		Print(L"<WriteII> 1-7 EFI_VOLUME_FULL\n");
	}
#endif

	ShellCloseFile(&FileHandle);
FreePool(Str8);
	return Status;
}

/**
	Write 'StrWr' into 'FileName', the insert position is 'pos'
	@Param:
		FileName: target file
		StrWr: the content to be written into the file
		pos: the start writing offset in file 
**/
EFI_STATUS
WriteIIPos(CHAR16 *FileName, CHAR16 *StrWr, UINT64 pos)
{
	EFI_STATUS Status = EFI_SUCCESS;
	SHELL_FILE_HANDLE FileHandle;
	//UINTN size = 200;
	//BOOLEAN filetype;
	//UINTN CmdlineNum;
	//UINT32 data;
	UINTN WrBufSize;
//	UINT64 FilePos;
	UINT8 *Str8;
	UINT64 offset = pos;
	
	Str8 = (UINT8 *)AllocateZeroPool(1024);	
	ShellInitialize();

	Status = ShellOpenFileByName(FileName, &FileHandle, 
				EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE|EFI_FILE_MODE_CREATE, 0);
    if (EFI_ERROR(Status)) {	
		Print(L"<WriteIIPos> Open File failed -2!\n");
    }else{
#if DEBUG_LIBFILE    
		Print(L"<WriteIIPos> Open File success - 2!\n");
#endif
	}



	ShellSetFilePosition(FileHandle, offset);
	ShellGetFilePosition(FileHandle, &offset);
#if DEBUG_LIBFILE  	
	Print(L"<WriteIIPos> FilePos = %d\n", offset);
#endif

	WrBufSize = StrLen(StrWr); 
#if DEBUG_LIBFILE  
	Print(L"<WriteIIPos> Strlen=%d(char8)\n", WrBufSize);
#endif
	
	Char16Char8(StrWr, Str8);
	Status = ShellWriteFile(FileHandle, &WrBufSize, Str8);
	//Status = ShellWriteFile(FileHandle, &WrBufSize, "aqwertyui");

#if DEBUG_LIBFILE  	
	Print(L"<WriteIIPos> Write Status = %d\n",Status);
	if(Status == EFI_UNSUPPORTED){
		Print(L"<WriteIIPos> 1-1 EFI_UNSUPPORTED\n");
	}
	if(Status == EFI_NO_MEDIA){
		Print(L"<WriteIIPos> 1-2 EFI_NO_MEDIA\n");
	}
	if(Status == EFI_DEVICE_ERROR){
		Print(L"<WriteIIPos> 1-3 EFI_DEVICE_ERROR\n");
	}
	if(Status == EFI_VOLUME_CORRUPTED){
		Print(L"<WriteIIPos> 1-4 EFI_VOLUME_CORRUPTED\n");
	}
	if(Status == EFI_WRITE_PROTECTED){
		Print(L"<WriteIIPos> 1-5 EFI_WRITE_PROTECTED\n");
	}
	if(Status == EFI_ACCESS_DENIED){
		Print(L"<WriteIIPos> 1-6 EFI_ACCESS_DENIED\n");
	}
	if(Status == EFI_VOLUME_FULL){
		Print(L"<WriteIIPos> 1-7 EFI_VOLUME_FULL\n");
	}
#endif

	ShellCloseFile(&FileHandle);
	FreePool(Str8);

	return Status;
}




EFI_STATUS
TestOpenFile(CHAR16 *FileName)
{
	EFI_STATUS Status = EFI_SUCCESS;
	SHELL_FILE_HANDLE FileHandle;

	ShellInitialize();

	Status = ShellOpenFileByName(FileName, &FileHandle, EFI_FILE_MODE_READ, 0);

	if (EFI_ERROR(Status)) {	
		Print(L"<TestOpenFile> Open File failed!\n");
		return Status;
    }else{
		Status = EFI_SUCCESS;
	}

	ShellCloseFile(&FileHandle);
	return Status;
}







/*
EDK 2:
#include <Library/ShellLib.h>
 
Then you can call the shell libraries for file manipulations such as reading and writing a file.
Shell File manipulation functions: 
CreateFile, DeleteFile, ReadFile, WriteFile, DeleteFileByName, CloseFile, FindFiles, FindFilesInDir, GetFilePosition, SetFilePosition, GetFileInfo, SetFileInfo, FreeFileList, OpenFileByName, OpenFileList, OpenRoot, OpenRootByHandle, GetFileSize, RemoveDupInFileList
*/


VOID
FileTest(CHAR16 * param)
{
	EFI_STATUS Status;
	SHELL_FILE_HANDLE FileHandle;
	UINTN size = 200;
	BOOLEAN filetype;
	UINTN CmdlineNum;
	UINT32 data;
	UINTN WrBufSize;
	UINT64 FilePos;
	CHAR16 stx[] = L"asdfgh";
		
	Print(L"<FileTest> [Line count] %d\n",GetTotalLine(L"aaa.eye"));
	
	ReadLine(L"aaa.eye", 2, strin16);
	Print(L"<FileTest> [String in line] %s\n", strin16);


	GetColumn(strin16, strin16p, 3);
	Print(L"<FileTest> [String in column] %s\n", strin16p);
	Print(L"<FileTest> [size of] %d\n", sizeof(strin16p)/sizeof(CHAR16));

	CmdlineNum= GetParamLineNumber(L"aaa.eye", L"eye_code");
	Print(L"<FileTest> [CmdlineNum] %d\n", CmdlineNum);

	GetParamData(L"aaa.eye", L"eye_code", strin16);
	Print(L"<FileTest> [Cmd's Data] %s\n", strin16);

	data = my_strhex2dec_16(strin16);
	Print(L"<FileTest> [Data] %x\n", data);

	data = my_strdec2dec_16(strin16);
	Print(L"<FileTest> [Data2] %x\n", data);

	data = Str2Num(strin16);
	Print(L"<FileTest> [Data3] %x\n", data);

	
if(0)
{
	ShellInitialize();

	Status = ShellOpenFileByName(L"aaa.eye", &FileHandle, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(Status)) {	
		Print(L"<FileTest> Open File failed!\n");
    }


	
	ShellFileHandleReadLine(
		FileHandle, 
		strin16, 
		&size, 
		TRUE, 
		&filetype);
	Print(L"<FileTest> [%d]%s\n", size, strin16);
	Print(L"<FileTest> %x\n", strin16[0]);

	
	ShellFileHandleReadLine(
		FileHandle, 
		strin16, 
		&size, 
		TRUE, 
		&filetype);
	Print(L"<FileTest> [%d]%s\n", size, strin16);
	Print(L"<FileTest> %x\n", strin16[0]);
	
	ShellFileHandleReadLine(
		FileHandle, 
		strin16, 
		&size, 
		TRUE, 
		&filetype);
	Print(L"<FileTest> [%d]%s\n", size, strin16);
	Print(L"<FileTest> %x\n", strin16[0]);


	

	ShellFileHandleReadLine(
		FileHandle, 
		strin16, 
		&size, 
		TRUE, 
		&filetype);
	Print(L"<FileTest> [%d]%s\n", size, strin16);
	Print(L"<FileTest> %x\n", strin16[0]);
	

	ShellFileHandleReadLine(
		FileHandle, 
		strin16, 
		&size, 
		TRUE, 
		&filetype);
	Print(L"<FileTest> [%d]%s\n", size, strin16);
	Print(L"<FileTest> %x\n", strin16[0]);	
	
//	WrBufSize = sizeof(L"qwerty\n")/sizeof(CHAR16);
//	ShellWriteFile(FileHandle, &WrBufSize, L"qwerty\n");
	
	

	ShellCloseFile(&FileHandle);
}
//////////////////////////////////////////////////////


	ShellInitialize();

	Status = ShellOpenFileByName(L"bbb.eye", &FileHandle, EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE|EFI_FILE_MODE_CREATE, 0);
    if (EFI_ERROR(Status)) {	
		Print(L"<FileTest> Open File failed -2!\n");
    }else{
		Print(L"<FileTest> Open File success - 2!\n");
	}


	
	ShellSetFilePosition(FileHandle, 2);
	ShellGetFilePosition(FileHandle, &FilePos);
	Print(L"<FileTest> FilePos = %d\n", FilePos);


	WrBufSize = 4;
	Status = ShellReadFile(FileHandle, &WrBufSize, strin16);
	Print(L"<FileTest> ReadFile  %s\n", strin16);	
	Print(L"<FileTest> ReadStatus = %d\n",Status);
	if(Status == EFI_NO_MEDIA){
		Print(L"<FileTest> 2-1\n");
	}
	if(Status == EFI_DEVICE_ERROR){
		Print(L"<FileTest> 2-2\n");
	}
	if(Status == EFI_VOLUME_CORRUPTED){
		Print(L"<FileTest> 2-3\n");
	}
	if(Status == EFI_VOLUME_CORRUPTED){
		Print(L"<FileTest> 2-4\n");
	}
	if(Status == EFI_BUFFER_TOO_SMALL){
		Print(L"<FileTest> 2-5\n");
	}

	
	WrBufSize = sizeof(stx)/sizeof(CHAR16);
	Print(L"<FileTest> WrBufSize = %d\n", WrBufSize);
	Status = ShellWriteFile(FileHandle, &WrBufSize, stx);
	Print(L"<FileTest> WrBufSize = %d\n",Status);
	if(Status == EFI_UNSUPPORTED){
		Print(L"<FileTest> 1-1\n");
	}
	if(Status == EFI_NO_MEDIA){
		Print(L"<FileTest> 1-2\n");
	}
	if(Status == EFI_DEVICE_ERROR){
		Print(L"<FileTest> 1-3\n");
	}
	if(Status == EFI_VOLUME_CORRUPTED){
		Print(L"<FileTest> 1-4\n");
	}
	if(Status == EFI_WRITE_PROTECTED){
		Print(L"<FileTest> 1-5\n");
	}
	if(Status == EFI_ACCESS_DENIED){
		Print(L"<FileTest> 1-6\n");
	}
	if(Status == EFI_VOLUME_FULL){
		Print(L"<FileTest> 1-7\n");
	}


	ShellCloseFile(&FileHandle);

/////////////////////////////////////////////////////

	Print(L"<FileTest> --> %d\n", StrLen(L"xyz123"));  //6


	WriteII(L"ccc.eye", L"xyz123"); ///write content from offset 0
	//WriteII(L"ccc.eye", "xyz123");
	
	WriteIIPos(L"ccc.eye", L"zxc456", 2);  //write content from offset 2

	WriteIIPos(L"ccc.eye", L"\n", 8);  //change line
	
	WriteIIPos(L"ccc.eye", L"zxc456", 9);  //write content zxc456 into the sencond line

/////////////////////////////////////////////////////

	if(param != NULL){
		Print(L"<FileTest> param = %s\n", param);
		WriteII(param, L"xyz123");
		WriteIIPos(param, L"zxc456", 2);
		WriteIIPos(param, L"\n", 8);
		WriteIIPos(param, L"zxc456", 9);
	}

	
	

	
}

/**
Log:


Argc = 2
Argc = FS0:\ShellTestKit.efi
[Line count] 9
[String in line] eye_cmd = Mode1     // GetStr("eye_cmd") 
[String in column] Mode1
[size of] 200
strintmp = eye_base = 31       // GetDec("eye_base")   -- GetData() - ¡Á??¡¥¨º?¡Àe 
strintmp1 = eye_base
strintmp = eye_code = Rx2a4F    // GetData("eye_code") 
strintmp1 = eye_code
[CmdlineNum] 1
strintmp = eye_base = 31       // GetDec("eye_base")   -- GetData() - ¡Á??¡¥¨º?¡Àe 
strintmp1 = eye_base
strintmp = eye_code = Rx2a4F    // GetData("eye_code") 
strintmp1 = eye_code
[Cmd's Data] Rx2a4F
<my_strhex2dec_16> str = Rx2a4F
<my_strhex2dec_16> len = 6
[Data] 2A4F
[Data2] 3EF8F2
<Str2Num> local = Rx
<my_strhex2dec_16> str = Rx2a4F
<my_strhex2dec_16> len = 6
[Data3] 2A4F
Open File success - 2!
FilePos = 2
ReadFile  ªQã¸2a4F
ReadStatus = 0
WrBufSize = 7
WrBufSize = 0
--> 6
<WriteII> Open File success - 2!
<WriteII> Strlen=6(char8)
<WriteII> Write Status = 0
<WriteIIPos> Open File success - 2!
<WriteIIPos> FilePos = 2
<WriteIIPos> Strlen=6(char8)
<WriteIIPos> Write Status = 0
<WriteIIPos> Open File success - 2!
<WriteIIPos> FilePos = 8
<WriteIIPos> Strlen=1(char8)
<WriteIIPos> Write Status = 0
<WriteIIPos> Open File success - 2!
<WriteIIPos> FilePos = 9
<WriteIIPos> Strlen=6(char8)
<WriteIIPos> Write Status = 0
**/







/*

EFI_STATUS
ZXFileOpInit(VOID)
{
	//
	// initialize the shell lib (we must be in non-auto-init...)
	//
	Status = ShellInitialize();
	ASSERT_EFI_ERROR(Status);
	
	Status = CommandInit();
	ASSERT_EFI_ERROR(Status);

}
*/










