//**********************************************************************
//**********************************************************************
//**                                                                                                              **
//**     Copyright (c) 2015 Shanghai Zhaoxin Semiconductor Co., Ltd.                     **
//**                                                                                                              **
//**     Author: Chris J.Wang  --  chrisjwang@zhaoxin.com                                  **
//**********************************************************************
//**********************************************************************


#include "ZXLibString.h"
#include "Top.h"

#define DEBUG_STR  0

/**
	This function will translate CHAR16 formate string to CHAR8 format string
**/
char Char16Char8(IN CHAR16 *in, OUT char *out)
{
	//31 00 32 00 33  00 00 00
	//31 32 33 00 00  00  00  00
	char ix = 0;
	for(ix = 0; 1 ; ix++){
		*(out + ix) = *(((char*)in)+ (ix<<1))	;
		if((*(((char*)in)+ (ix<<1)) == 0x00) && (*(((char*)in)+ (ix<<1) + 1) == 0x00)) {
			break;
		}
	}
	
	return 0;
}

char Char8Char16(IN char *in, OUT CHAR16 *out)
{
	
	return 0;
}

#if 0
// those functions have been defined.
char *strcpy(char *strDest, const char *strScr)
{
       char *address = strDest;
       assert((strDest != NULL) && (strScr != NULL));
       while(*strScr)
       {
              *strDest++ = *strScr++;
       }
       *strDest = '\0';
       return address;
}
 
int strcmp (const char *str1,const char *str2)
{           
       int len = 0;
       assert((str1 != NULL) && (str2 != NULL));
       while(*str1 && *str2 && (*str1 == *str2))
       {
              str1++;
              str2++;
       }
       return *str1-*str2;
}
#endif


#if 0
UINT8
StrHexByte(char *str)
{
	UINT8 hh,ll;
	hh = (*str);
//	Devx("-->%x\n", hh);
	if(hh>'9'){ //> 9
		if(hh >'a'){
			hh = hh-'a';
			hh += 10;
		}else{
			hh = hh-'A';
			hh += 10;
		}
	}else{
		hh = hh-'0';
	}
//	Devx("-->%x\n", hh);
	ll = *(str+1);
	if(ll>'9'){ //> 9
		if(*(str+1)>'a'){
			ll = ll-'a';
			ll += 10;
		}else{
			ll = ll-'A';
			ll += 10;
		}
	}else{
		ll = ll-'0';
	}
		
	return (hh<<4)+ll;
}

UINT32 
Str2Hex(
	char *str
)
{
	//UINT32 temp=0;
	UINT8 m7,m6,m5,m4,m3,m2,m1,m0;

	/*
	printf("----%d\n",*str);
	printf("----%d\n",*(str+1));
	printf("----%d\n",*(str+2));
	printf("----%d\n",*(str+3));
	printf("----%d\n",*(str+4));
	printf("----%d\n",*(str+5));
	printf("----%d\n",*(str+6));
	printf("----%d\n",*(str+7));
*/
	//m7 = ((*str)<97)?(((UINT8)(*str - 0x30))<<28):(((UINT8)(*str - 0x57))<<28);

	

	m6 = StrHexByte(str)&0xFF;
	m4 = StrHexByte((str+2))&0xFF; 
	m2 = StrHexByte((str+4))&0xFF;
	m0 = StrHexByte((str+6))&0xFF;
	
	return (m6<<24)|(m4<<16)|(m2<<8)|(m0<<0);
	
}













//反转字符串  
char *reverse(char *s)  
{  
    char temp;  
    char *p = s;    //p指向s的头部  
    char *q = s;    //q指向s的尾部  
    while(*q)  
        ++q;  
    q--;  
  
    //交换移动指针，直到p和q交叉  
    while(q > p)  
    {  
        temp = *p;  
        *p++ = *q;  
        *q-- = temp;  
    }  
    return s;  
}  
  
/* 
 * 功能：整数转换为字符串 
 * char s[] 的作用是存储整数的每一位 
 */  
char *my_itoa(int n)  
{  
    int i = 0,isNegative = 0;  
    static char s[100];      //必须为static变量，或者是全局变量  
    if((isNegative = n) < 0) //如果是负数，先转为正数  
    {  
        n = -n;  
    }  
    do      //从各位开始变为字符，直到最高位，最后应该反转  
    {  
        s[i++] = n%10 + '0';  
        n = n/10;  
    }while(n > 0);  
  
    if(isNegative < 0)   //如果是负数，补上负号  
    {  
        s[i++] = '-';  
    }  
    s[i] = '\0';    //最后加上字符串结束符  
    return reverse(s);    
} 

int test_itoa(void)  
{  
    int m;  
//    Devx("请输入int型整数m：");  
 //   scanf("%d",&m);  
 //   Devx("整数=%d,字符串=%s\n",m,my_itoa(m));  
    return 0;  
} 

/*
//atoi 函数说明

函数说明
atoi()会扫描参数nptr字符串，跳过前面的空格字符，
直到遇上数字或正负符号才开始做转换，而再遇到非
数字或字符串结束时('/0')才结束转换，并将结果返回。

返回值
返回转换后的整型数。 
*/
int my_atoi(const char * str)
{
	int res=0,begin=0,i;
 	unsigned char minus=0;
 	while(*str != '\0'){
  		if(begin==0&&(('0'<*str&&*str<'9')|| *str=='-') ) {  //从第一个数字或者'-'号开始
  
  		 	begin=1;
   			if(*str == '-'){
    			minus=1;
    			str++;
   			}
  		}else if( begin==1&&(*str<'0'||*str>'9') )          //遇到第一个非数字，退出
   			break;
  		if(begin==1)
   			res=res*10+(*str-'0');                       	//计算
  			str++;
 	}
 	
	 return minus? -res : res;
}

int test_atoi()
{
	 int i=0;
	 char s1[]="-1245781";
	 char s2[]="35678";
	 char s3[]="ab568678";
	 char s4[]="bn-35878";
	 char s5[]="bh-836583mk";
	 i=my_atoi(s1) ;
//	 printf("i1 is %d\n",i);
	 i=my_atoi(s2) ;
//	 printf("i2 is %d\n",i);
	 i=my_atoi(s3) ;
//	 printf("i3 is %d\n",i);
	 i=my_atoi(s4) ;
//	 printf("i4 is %d\n",i);
	 i=my_atoi(s5) ;
//	 printf("i5 is %d\n",i);
	 return 0;
}
#endif

#if 0
int memcmp(char *str1,char *str2,int len)
{
//	assert(str1);
//	assert(str2);
	while(len--)
	{
	while(*str1==*str2)
	{
		if(*str1=='\0')
			return 0;
	
			str1++;
			str2++;
	
	}
	}
	if(*str1>*str2)
		return 1;
	if(*str1<*str2)
		return -1;
}
#endif


/**
	CHAR8 Function
	Hex Format string to Dec data 
**/
//'len' must be input by strlen(str);
unsigned int my_strhex2dec(unsigned char *str, unsigned char len)
{
	unsigned char buf[1024];
	unsigned int t;
	unsigned int sum = 0;
	unsigned char i;
	
	strcpy(buf, str);
//	Devx("[1]-->%s   len:%d\n",buf,len);
	
	for(i =2; i < len; i++){
		if(buf[i] > 'a'){
			t = buf[i] - 'a' +10;
		}else{
			t = buf[i] - '0';
		}
		sum = sum*16 + t;	
	}
	
	return sum;
} 


/**
	CHAR8 Function
	Dec Format string to Dec data
**/
//'len' must be input by strlen(str);
unsigned int my_strdec2dec(unsigned char *str, unsigned char len)
{
	unsigned char buf[1024];
	unsigned int t;
	unsigned int sum = 0;
	unsigned char i;
	
	strcpy(buf, str);
	//Devx("[3]-->%s   len:%d\n",buf,len);
	
	for(i =0; i < len; i++){

		t = buf[i] - '0';
	
		sum = sum*10 + t;	
	}
	
	return sum;
} 

/**
	CHAR8 Function
	@param:
		str: the string we get (CHAR8)
	@return:
		the number in this string
	@note:
		the formate of this string can be 
		1. Rx...(hex)
		2. 0x...(hex)
		3. ...(Dec)

**/
//'len' must be input by strlen(str);
unsigned int my_str2dec(unsigned char *str, unsigned char len)
{
	unsigned char buf[1024];
	unsigned int t;
	unsigned int sum = 0;
	unsigned char i;
	
	strcpy(buf, str);
//	Devx("[6]-->%s   len:%d\n",buf,len);
	
	if((memcmp(buf, "0x", 2) == 0)||(memcmp(buf, "Rx", 2) == 0)){
		for(i =2; i < len; i++){
			if(buf[i] >= 'a'){
				t = buf[i] - 'a' +10;
			}else if(buf[i] >= 'A'){
				t = buf[i] - 'A' +10;
			}else{
				t = buf[i] - '0';
			}

			sum = sum*16 + t;	
		}	
	}else{
		for(i =0; i < len; i++){
	
			t = buf[i] - '0';
		
			sum = sum*10 + t;	
		}			
	}

	return sum;
} 




/**
	CHAR16 function
	Hex format string to Dec data
**/
//'len' must be input by strlen(str);
UINT32 my_strhex2dec_16(CHAR16 *str)
{
	CHAR16 buf[1024];
	UINT32 t;
	UINT32 sum = 0;
	UINT8 i;
#if DEBUG_STR	
	Print(L"<my_strhex2dec_16> str = %s\n", str);
	Print(L"<my_strhex2dec_16> len = %d\n", StrLen(str));
#endif
	
	StrCpy(buf, str);
//	Devx("[1]-->%s   len:%d\n",buf,len);
	
	for(i =2; i < (UINT8)(StrLen(str)); i++){
		//Print(L"<my_strhex2dec_16> buf[i] = %c\n", buf[i]);
		if(buf[i] >= L'a'){
			
			t = buf[i] - L'a' +10;
		}else{
			
			if((buf[i] >= L'A') && (buf[i] <= L'F')){
				t = buf[i] - L'A' + 10;
			}else{
				t = buf[i] - L'0';
			}
		}
		sum = sum*16 + t;	
	}
	
	return sum;
} 

/**
	CHAR16 Function
	Dec format string to Dec data
**/
UINT32 my_strdec2dec_16(CHAR16 *str)
{
	CHAR16 buf[1024];
	UINT32 t;
	UINT32 sum = 0;
	UINT8 i;
	
	StrCpy(buf, str);
	//Devx("[3]-->%s   len:%d\n",buf,len);
	
	for(i =0; i < StrLen(str); i++){

		t = buf[i] - L'0';
	
		sum = sum*10 + t;	
	}
	
	return sum;
} 

/**
	String to Number
	@param:
		str: the string we get (CHAR16)
	@return:
		the number in this string
	@note:
		the formate of this string can be 
		1. Rx...(hex)
		2. 0x...(hex)
		3. ...(Dec)
**/
UINT32 
Str2Num(CHAR16 *str)
{
	CHAR16 head1[] = L"Rx";
	CHAR16 head2[] = L"0x";
	CHAR16 local[20]; // = {0};

	StrCpy(local, str);
	local[2]= 0x00;

#if DEBUG_STR		
	Print(L"<Str2Num> local = %s\n", local);
#endif	

	if((!StrCmp(head1, local))||(!StrCmp(head2, local))){
		return my_strhex2dec_16(str);
	}else{
		return my_strdec2dec_16(str);
	}

}


/**
this is a parser to get the text in the specfic Colume from the input string
the difference between this routine and Str2Num() is: this routine can auto
	analyse the format like Rx70[3:0]/0x70[3:1] and output the Mask for those bit fields
	@Param:
		
	@Return:
		EFI_NOT_FOUND: input format error
		EFI_SUCCESS;

Note:
	Rx70[15:10]
	local = Rx70
	p1 = 4   is  EndOfFirstPart
	p2 = 7 
	p3 = 10  is  EndOfSecondPart
**/

EFI_STATUS
Str2NumBitField(
	IN CHAR16 * input, 
	IN OUT UINT32 *FieldMask, 
	IN OUT UINT8 *BaseBit,
	IN OUT UINT8 *EndBit,
	IN OUT UINT32 *FirstPart)
{
	//CHAR16 head1[] = L"Rx";
	//CHAR16 head2[] = L"0x";
	CHAR16 local2[10];   //used for compare char 
	UINT8 EndOfFirstPart = 0;
	CHAR16 local[200];
	UINT8 p1 = 0;
	UINT8 p2 = 0;
	UINT8 p3 = 0;

	CHAR16 FirstPartx[200];   //save the first part
	CHAR16 EndField[200];
	CHAR16 StartField[200];
	UINT32 EndFieldNum = 0;
	UINT32 StartFieldNum = 0;
	UINT32 MaskFieldTmp = 0;
	UINT8 i;
	
	
	StrCpy(local, input);

#if DEBUG_STR		
			Print(L"<Str2NumBitField> local = %s\n", local);
#endif	
	//get the first part of this string(Exp: Rx70[3:1] --> Rx70 is the first part, [3:1] is the second part)
	local2[1] = 0x0; //end of string 
	StrCpy(FirstPartx, input);
	for(EndOfFirstPart = 0; EndOfFirstPart<StrLen(local);EndOfFirstPart++){
		local2[0] = local[EndOfFirstPart];
		if(!StrCmp(local2,L"[")){   //find '['
			FirstPartx[EndOfFirstPart] = 0;   //for now , the first part  saved to local[]
			p1 = EndOfFirstPart;
#if DEBUG_STR	
			Print(L"<Str2NumBitField> p1 = %d\n", p1);
			Print(L"<Str2NumBitField> FirstPartx = %s\n", FirstPartx);
#endif			
		}
		if(!StrCmp(local2,L":")){
			p2 = EndOfFirstPart;
#if DEBUG_STR		
			Print(L"<Str2NumBitField> p2 = %d\n", p2);
#endif				
		}
		if(!StrCmp(local2,L"]")){
			p3 = EndOfFirstPart;
#if DEBUG_STR		
			Print(L"<Str2NumBitField> p3 = %d\n", p3);
#endif				
		}
		
	}
	if(p3 != (StrLen(local)-1)){
		Print(L"<Str2NumBitField> Error -- format Error\n");		
		return EFI_NOT_FOUND;
	}
#if DEBUG_STR		
		Print(L"<Str2NumBitField> first part = %s\n", FirstPartx);
#endif	

	Print(L"<Debug> %d\n", p2 - p1 - 1);
	//get the EndField --- Exp: Rx70[13:11] --> 13
	for(i = 0; i < (p2 - p1 - 1); i++){
		EndField[i] = local[p1+1+i];
		Print(L"<Debug> local[EndOfFirstPart+1] = %c\n", local[p1+1]);
	}
	EndField[i] = 0;
	EndFieldNum = Str2Num(EndField);
#if DEBUG_STR		
	Print(L"<Str2NumBitField> EndField = %s\n", EndField);
	Print(L"<Str2NumBitField> EndFieldNum = %d\n", EndFieldNum);
#endif		


	//get the StartField --- Exp: Rx70[13:11] --> 11
	for(i = 0; i < (p3 - p2 -1); i++){
		StartField[i] = local[p2+1+i];
	}
	StartField[i] = 0;
	StartFieldNum = Str2Num(StartField);
#if DEBUG_STR	
	Print(L"<Str2NumBitField> StartField = %s\n", StartField);
	Print(L"<Str2NumBitField> StartFieldNum = %d\n", StartFieldNum);
#endif		
		


	//calculate the Mask 
	for(i = 0; i<32; i++){
		if((i <= EndFieldNum)&&(i>=StartFieldNum)){
			MaskFieldTmp|=(1<<i);
		} 
	}
	*FieldMask = MaskFieldTmp;	
#if DEBUG_STR		
	Print(L"<Str2NumBitField> FieldMask = 0x%08x\n", *FieldMask);
#endif		

	*BaseBit = (UINT8)StartFieldNum;
	*EndBit =  (UINT8)EndFieldNum;
	*FirstPart = Str2Num(FirstPartx);
	return EFI_SUCCESS;

}


#if 0
EFI_STATUS 
my_Hex2Str_16bit(UINT16 Dai,CHAR16 *Pax)
{
	UINT8 tra;

	tra = Dai&0xF
	
	


}

CHAR16 *Pax
#endif

#if 0
//反转字符串  
char *reverse(char *s)  
{  
    char temp;  
    char *p = s;    //p指向s的头部  
    char *q = s;    //q指向s的尾部  
    while(*q)  
        ++q;  
    q--;  
  
    //交换移动指针，直到p和q交叉  
    while(q > p)  
    {  
        temp = *p;  
        *p++ = *q;  
        *q-- = temp;  
    }  
    return s;  
}  
  
/* 
 * 功能：整数转换为字符串 
 * char s[] 的作用是存储整数的每一位 
 */  
char *my_itoa(int n)  
{  
    int i = 0,isNegative = 0;  
    static char s[100];      //必须为static变量，或者是全局变量  
    if((isNegative = n) < 0) //如果是负数，先转为正数  
    {  
        n = -n;  
    }  
    do      //从各位开始变为字符，直到最高位，最后应该反转  
    {  
        s[i++] = n%10 + '0';  
        n = n/10;  
    }while(n > 0);  
  
    if(isNegative < 0)   //如果是负数，补上负号  
    {  
        s[i++] = '-';  
    }  
    s[i] = '\0';    //最后加上字符串结束符  
    return reverse(s);    
} 
#endif

