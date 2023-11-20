#include "stdafx.h"
#include "SutterManip.h"
#include "math.h"
#include <string>

SutterManip::SutterManip()
{
	is_moving = false;
}

void SutterManip::Initialize(CString com_index)
{	
	hCom=CreateFile(com_index, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, NULL);
	if (hCom == INVALID_HANDLE_VALUE)
	{
		::MessageBox(NULL,"Can not open serial port, Please check Sutter-MP285", "Warning", MB_OK | MB_ICONEXCLAMATION);// Place all significant initialization in InitInstance
		::CloseHandle(hCom);
	}
	else
	{
		std::cout<<"Successfully connected to Sutter-MP285"<<std::endl;
	}

	DCB dcb1 = {0};
    dcb1.DCBlength = sizeof(dcb1);
    ::BuildCommDCB("9600,N,8,1", &dcb1);
 	if (!::SetCommState(hCom,&dcb1) )
	{
		::MessageBox(NULL,"Can not set serial port, Please check Sutter-MP285", "Warning", MB_OK | MB_ICONEXCLAMATION);// Place all significant initialization in InitInstance
		::CloseHandle(hCom);
	}
	else
	{
		std::cout<<"Successfully set the serial port for Sutter-MP285"<<std::endl;
	}
}

sManipPosition SutterManip::GetPosition()
{
	DWORD dwWritten1,dwWTrans1, dwReader1;
 	char Buf1[2];
 	char lpBuf1[40];
	unsigned long nRead1;

	long xValue,yValue,zValue;

	OVERLAPPED osWriteC1={0};
	SYSTEMTIME sTime1,sTime2;

	osWriteC1.hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
 	OVERLAPPED osReader1={0};
    osReader1.hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
 
	Buf1[0]='c';
	Buf1[1]='\r';

    //The PurgeComm function can discard all characters from the output or input buffer of a specified communications resource
    PurgeComm(hCom,PURGE_RXABORT | PURGE_RXCLEAR );//| PURGE_TXABORT | PURGE_TXCLEAR);
	//write buf contents to the instrument  
 	::WriteFile(hCom,Buf1,2,&dwWritten1, &osWriteC1);   
	//True: Wait forever until the writting process is done
	::GetOverlappedResult(hCom,&osWriteC1,&dwWTrans1,TRUE);

    //Read response from instrument to lpBuf
    ReadFile(hCom, lpBuf1, 13, &nRead1, &osReader1);      //not working 
    //to prevent launch of Mp285 windows program causing system hang-up when controller is off 
	GetSystemTime(&sTime1); 
    //Wait 6 seconds. If the read process still did not finish, abort the whole program.
    while (GetOverlappedResult(hCom,&osReader1,&dwReader1,FALSE)== 0)
    {
	GetSystemTime(&sTime2);
    if ((sTime2.wSecond-sTime1.wSecond)>6)
		{::MessageBox(NULL,"Can not communicate with MP285!", "Warning", MB_OK | MB_ICONEXCLAMATION);// Place all significant initialization in InitInstance
		::CloseHandle(hCom);
		::ExitProcess(-1);}
	}
	memcpy((char*)&xValue,lpBuf1,4);
	memcpy((char*)&yValue,lpBuf1+4,4);
	memcpy((char*)&zValue,lpBuf1+8,4);
	
	sManipPosition manipPosition;
	manipPosition.x=(float)(xValue*0.04+(xValue<0?-1:1)*0.5);
	manipPosition.y=(float)(yValue*0.04+(yValue<0?-1:1)*0.5);
	manipPosition.z=(float)(zValue*0.04+(zValue<0?-1:1)*0.5);
		
	return manipPosition;

}

/////////////////////////////////////////////////////////////
// Set velocity for MP285
void SutterManip::SetVelocity(unsigned short v)
{
	DWORD dwWritten,dwWTrans;
 	char Buf[4];
 	char lpBuf1[11];
	unsigned long nRead1;

	OVERLAPPED osWriteC={0};
	DWORD dwReader;
	SYSTEMTIME sTime1,sTime2;

	osWriteC.hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
 	OVERLAPPED osReader={0};
    osReader.hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);

	// 将速度值转换为字符数组
	char* pV = reinterpret_cast<char*>(&v);
	memcpy(Buf + 1, pV, 2);
	
	Buf[0]='V';
    //strcpy(Buf+1,(char*)&v);
	Buf[3]='\r';
    
	//The PurgeComm function can discard all characters from the output or input buffer 
	//of a specified communications resource
  	PurgeComm(hCom,PURGE_RXABORT | PURGE_RXCLEAR );//| PURGE_TXABORT | PURGE_TXCLEAR);
    //write buf contents to the instrument  
 	::WriteFile(hCom,Buf,4,&dwWritten, &osWriteC);   
    //True: Wait forever until the writting process is done
	::GetOverlappedResult(hCom,&osWriteC,&dwWTrans,TRUE); 

	//Read response from instrument to lpBuf
    ReadFile(hCom, lpBuf1, 1, &nRead1, &osReader );   
	//to prevent launch of Mp285 windows program causing system hang-up when controller is off
	GetSystemTime(&sTime1); 
    //Wait 6 seconds. If the read process still did not finish, abort the whole program.
    while (GetOverlappedResult(hCom,&osReader,&dwReader,FALSE)== 0)
    {
		GetSystemTime(&sTime2);
		if ((sTime2.wSecond-sTime1.wSecond)>6)
		{::MessageBox(NULL,"Can not communicate with MP285!", "Warning", MB_OK | MB_ICONEXCLAMATION);// Place all significant initialization in InitInstance
		::CloseHandle(hCom);
		}
	}
	CloseHandle(osWriteC.hEvent);
	CloseHandle(osReader.hEvent);
}

///////////////////////////////////////////////////////////////
// Check status of MP285
bool SutterManip::CheckStatus()
{
	DWORD dwWritten1,dwWTrans1, dwReader1;
 	char Buf1[2];
 	char lpBuf1[33];
	unsigned long nRead1;

	OVERLAPPED osWriteC1={0};
	SYSTEMTIME sTime1,sTime2;

	osWriteC1.hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
 	OVERLAPPED osReader1={0};
    osReader1.hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
 
	Buf1[0]='s';
	Buf1[1]='\r';

    //The PurgeComm function can discard all characters from the output or input buffer of a specified communications resource
    PurgeComm(hCom,PURGE_RXABORT | PURGE_RXCLEAR );//| PURGE_TXABORT | PURGE_TXCLEAR);
	//write buf contents to the instrument  
 	::WriteFile(hCom,Buf1,2,&dwWritten1, &osWriteC1);   
	//True: Wait forever until the writting process is done
	::GetOverlappedResult(hCom,&osWriteC1,&dwWTrans1,TRUE);

    //Read response from instrument to lpBuf
    ReadFile(hCom, lpBuf1, 13, &nRead1, &osReader1);      //not working 
    //to prevent launch of Mp285 windows program causing system hang-up when controller is off 
	GetSystemTime(&sTime1); 
    //Wait 6 seconds. If the read process still did not finish, abort the whole program.
    while (GetOverlappedResult(hCom,&osReader1,&dwReader1,FALSE)== 0)
    {
	GetSystemTime(&sTime2);
    if ((sTime2.wSecond-sTime1.wSecond)>6)
		{::MessageBox(NULL,"Can not communicate with MP285!", "Warning", MB_OK | MB_ICONEXCLAMATION);// Place all significant initialization in InitInstance
		::CloseHandle(hCom);
		::ExitProcess(-1);}
	}

	bool mp285_status_is_moving;
	char buf2;
	buf2 = lpBuf1[27];
	if (buf2 ==0)
	{
		mp285_status_is_moving = false;
		//::MessageBox(NULL,"stop moving", "Warning", MB_OK | MB_ICONEXCLAMATION);
	}
	else 
	{
	    mp285_status_is_moving = true;
		//::MessageBox(NULL,"moving", "Warning", MB_OK | MB_ICONEXCLAMATION);
	}

	return mp285_status_is_moving;
}
//////////////////////////////////////////////////////////////////
// Move Manip
void SutterManip::Move(float xValue, float yValue, float zValue)
{
	//The scale of xValue is micron. One micron = 25 steps. The input value is step  
	xValue*=25;
    yValue*=25;
    zValue*=25;

	long xValue285, yValue285, zValue285;
    //4 She 5 Ru
	xValue285 = floor(xValue+0.5);
	yValue285 = floor(yValue+0.5);
	zValue285 = floor(zValue+0.5);

    DWORD dwWritten;
	char Buf[14];
	char lpBuf[40];
	unsigned long nRead;

	Buf[0]='m';
	memcpy(Buf+1,(char*)&xValue285,4);
	memcpy(Buf+5,(char*)&yValue285,4);
	memcpy(Buf+9,(char*)&zValue285,4);
	Buf[13]='\r';
	
	is_moving=true;

    DWORD dwWTrans;
	OVERLAPPED osWrite={0};
	osWrite.hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
    //write buf contents to the instrument  
 	::WriteFile(hCom,Buf,14,&dwWritten, &osWrite);   
    //True: Wait forever until the writting process is done
	GetOverlappedResult(hCom,&osWrite,&dwWTrans,TRUE); 

	DWORD dwReader;
	OVERLAPPED osRead={0};
    osRead.hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
	ResetEvent(osRead.hEvent);
	//Read response from instrument to lpBuf
    ::ReadFile(hCom, lpBuf, 1, &nRead, &osRead);   
//	GetSystemTime(&sTime1); 
    //Wait untill the moving is finished.
    while (is_moving)
	{
	    //MP285_PeekandPump();
        if (GetOverlappedResult(hCom,&osRead,&dwReader,FALSE))
		{
			is_moving = false;
		}
	}
	CloseHandle(osWrite.hEvent);
	CloseHandle(osRead.hEvent);
}

