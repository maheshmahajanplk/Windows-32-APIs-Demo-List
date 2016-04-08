// zSmartExecute.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tchar.h>
#include <malloc.h>
#include <atlbase.h>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <exception>

#define TIMESTAMPBUF 100
#pragma comment(lib, "user32.lib")
#define  g_dwExtraBuffer	 256
#include "SysInfo.h"
#define HEADER_APP  "DateTime\tModule\tStatus\tDescription\r\n"
#define FILE_SIZE	300
#define Virtual_Machine			"Virtual Machine"
#define VMware_Virtual_Platform	"VMware Virtual Platform"
#define VirtualBox				"VirtualBox"
#define PID_TEXT "[STATUS_INFO]\r\nREGID=%s\r\nDesc=%s\r\nPID=%s\r\nSysID=\r\nLastRunDateTime=%s\r\nNextRunDateTime=\r\nLastRunStatusCode=%s\r\n"

char SAAZODSetUpPath[MAX_PATH] = {0};

/*WMI Query include files*/
#define _WIN32_DCOM
#include <wbemidl.h>
#include <comdef.h>
# pragma comment(lib, "wbemuuid.lib")

/*****************************/

DWORD	GetCurrentDateTime(char* szDateTime)
{
	int		iDateSize		=	0;
	int		iDateTimeSize	=	0;	
	char am_pm[] = "AM\0";


	SYSTEMTIME		sysTime;

	__try
	{
		__try
		{
			if((NULL == szDateTime))
			{
				return 1;
			}

			memset(&sysTime,NULL,sizeof(sysTime));	


			::GetLocalTime(&sysTime);

			if( sysTime.wHour > 12)
				strcpy_s( am_pm, "PM\0" );
			if(sysTime.wHour == 12)
			{				
				//WORD nSeconds = sysTime.wSecond;
				//if( nSeconds > 0)
					strcpy_s( am_pm, "PM\0" );
			}
			if( sysTime.wHour > 12 )        // Convert from 24-hour to 12-hour clock.
				sysTime.wHour -= 12;		
			if( sysTime.wHour == 0 )        // Set hour to 12 if midnight.
				sysTime.wHour = 12;

			sprintf_s(szDateTime,TIMESTAMPBUF,
					"%02d/%02d/%04d %02d:%02d:%02d %s",
					sysTime.wMonth,
					sysTime.wDay,
					sysTime.wYear,
					sysTime.wHour,
					sysTime.wMinute,
					sysTime.wSecond,
					am_pm);
		}
		/*int sprintf_s(char *buffer,
						size_t sizeOfBuffer,
						const char *format [,
						argument] ... 
						);
		*/
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			return 1;
		}
	}


	__finally
	{
	}
	return 0;
}

/*********************************************************/
char* GetStringRegKey(HKEY hKey, LPCSTR strValueName, char* strValue, char* strDefaultValue,ULONG& nError)
{
    char szBuffer[260] = {0};
    DWORD dwBufferSize = sizeof(szBuffer);
	nError = RegQueryValueEx(hKey, strValueName, 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
    if (ERROR_SUCCESS == nError)
    {
		lstrcpy(strValue, szBuffer);
    }
	else
	{
		lstrcpy(strValue, "");
	}
	return strValue;
}
/**********************************************************/

/***************************************/
DWORD IsFile2MB(char *mmFileName)
{
	DWORD dwFileSize=0;
	try
	{
		HANDLE hFile=NULL;
		hFile = CreateFile(mmFileName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			OPEN_ALWAYS,
			0,
			NULL);
		
		if (hFile == INVALID_HANDLE_VALUE)
		{
			return false;
		}
		
		dwFileSize = GetFileSize(hFile,NULL);

		CloseHandle(hFile);
	}
	catch(...)
	{}

	return dwFileSize;
}
bool IsDirExist(char *mdirectory)
{
	bool mflag=false;
	try
	{
		WIN32_FIND_DATA mwfd;
		HANDLE mhandle;
		mhandle = FindFirstFile(mdirectory , &mwfd);
		if(mhandle == INVALID_HANDLE_VALUE)
		{
			// create folder
			//CreateDirectory(mDebugDir ,  NULL);
			FindClose(mhandle); 

			mflag = false;
		}
		else
		{
			FindClose(mhandle); 
			mflag = true;
		}
	}
	catch(...)
	{}
	return mflag;
}

void WriteApplicationFile(char *mappname,char* MessageString)
{
	try
	{
		char mmCurentDir[512];
		SYSTEMTIME systime;

		char mdate[255]={0};
		char mmdate[255]={0};
		char mtime[255]={0};
		char mmFileName[512]={0};
		bool isFileExist;
		char mlogtype[1024]={0};
		bool isPerDeleted=false;
		
		//lstrcpy(mmCurentDir , mAPPLOG);
		strcpy_s(mmCurentDir,SAAZODSetUpPath);
		lstrcat(mmCurentDir ,"ApplicationLog\\");
		//strcat(mmCurentDir , "\\Log");

		GetLocalTime(&systime);

		sprintf_s(mdate,255,"%04d-%02d-%02d" ,systime.wYear, systime.wDay , systime.wMonth );
		sprintf_s(mmdate ,255, "%04d%02d%02d" ,systime.wYear, systime.wDay , systime.wMonth);
		//GetTimeFormat(LOCALE_SYSTEM_DEFAULT , NULL , &systime , "hh':'mm':'ss" , mtime , 255);
		sprintf_s(mtime ,255, "%02d:%02d:%02d" , systime.wHour, systime.wMinute , systime.wSecond);
		sprintf_s(mmFileName ,512,"%s\\%s.Log" , mmCurentDir , mappname);
		
		FILE *fp=NULL;
		errno_t err;

		if(IsDirExist(mmFileName) == false)
		{
			isFileExist = false;
			err=fopen_s( &fp,mmFileName ,"w+");
			/*if( err == 0 )
			{
				printf( "The file %s was opened\n",mmFileName );
			}
			else
			{
				printf( "The file 'crt_fopen_s.c' was not opened\n" );
			}*/
			_fcloseall( );
		}
		else
		{
			isFileExist = true;
		}

		DWORD dwFileSize = IsFile2MB(mmFileName);// Check for 2 MB (2097152 Bytes)
		 errno_t err1;
		if(dwFileSize >= 2097152)
		{
			isPerDeleted = true;
			isFileExist = false;
			err1 =fopen_s( &fp,mmFileName ,"w+");
			_fcloseall( );
		}
		
		fp = fopen( mmFileName , "a");
		if(!(fp == NULL))
		{
			if (isFileExist == false)
			{
				fprintf(fp , "#Software: %s\n#Version: 1.0 \n#Date: %s %s\n# \n" , "zSmart.exe", mdate , mtime);
				fprintf(fp , "----------------------------------------------------------------------\n" );
				//fprintf(fp , "#Fields:\tdtime\tSession Id\tSession User Name\tmodule\n");
				if(isPerDeleted == true)
				{
					fprintf(fp , "Previous content is deleted. Due to the size limit of 2 MB\n" );
					fprintf(fp , "----------------------------------------------------------------------\n" );
				}
			}

				
			fprintf(fp , "%s %s\t%s \n" , mdate , mtime , MessageString);
		
		}
		else
		{ 

		}
		_fcloseall( );
	}
	catch(...)
	{
	}
}
/*****************************************/

void writePIDFile(char* MessageString,char* szRegId,char* szExeName)
{
	try
	{
		char mmCurentDir[512];
		SYSTEMTIME systime;

		char mmFileName[512]={0};
		char RenameFileName[512]={0};
		bool isFileExist;
		char mlogtype[1024]={0};
		bool isPerDeleted=false;
		

		sprintf(mmFileName,"%s\\TEMP\\%s_%s.ini\0",SAAZODSetUpPath,szRegId,szExeName);
		sprintf(RenameFileName,"%s\\PMSTATUS\\%s_%s.ini\0",SAAZODSetUpPath,szRegId,szExeName);
		
		GetLocalTime(&systime);

		FILE *fp=NULL;

		if(IsDirExist(mmFileName) == false)
		{
			isFileExist = false;
			fp=fopen( mmFileName ,"w+");
			_fcloseall();
		}
		else
		{
			isFileExist = true;
		}

		DWORD dwFileSize = IsFile2MB(mmFileName);// Check for 2 MB (2097152 Bytes)

		if(dwFileSize >= 2097152)
		{
			isPerDeleted = true;
			isFileExist = false;
			fp=fopen( mmFileName ,"w+");
			_fcloseall( );
		}
		fp = fopen( mmFileName , "a");
		if(!(fp == NULL))
		{
			fprintf(fp , "%s \n" , MessageString);		
		}
		else
		{ 

		}
		_fcloseall( );
		CopyFile(mmFileName,RenameFileName,FALSE);
		DeleteFile(mmFileName);
	}
	catch(...)
	{
	}
}

HRESULT WriteToBuffer(LPTSTR	&lpszBuffer,	//[in,out]Destination pointer where the buffer is located
						LPCTSTR lpszString,		//[in]String to be copied to the buffer
						BOOL bRefresh)			//[in]Initialize the buffer if it is TRUE, default is FALSE
{
	DWORD dwSize = 0;
	try
	{
		if(NULL == lpszString)
		{
			return E_POINTER;
		}

		//Get the actual size of the string
		dwSize = _tcslen(lpszString);

		if(NULL == lpszBuffer)
		{
			//allocate the required initial memory
			DWORD dwMemoryReq = g_dwExtraBuffer + dwSize;

			lpszBuffer = new TCHAR [dwMemoryReq + 1];
			if(NULL == lpszBuffer)
			{
				//allocation failed
				return ERROR_NOT_ENOUGH_MEMORY;
			}

			//allocation successful, set the memory
			//_tcsnset(lpszBuffer,_T('\0'),dwMemoryReq);

			memset(lpszBuffer,_T('\0'),dwMemoryReq);
			//copy the source string to the destination
			_tcscpy(lpszBuffer,lpszString);
		}
		else
		{
			//Check for the validity of the lpszBuffer pointer
			BOOL bRet1 = IsBadWritePtr(lpszBuffer,1); //cALLING PROC HAS WRITE ACCESS RET 0 OW NON0
			BOOL bRet2 = IsBadReadPtr(lpszBuffer,1); 
			if(0 != bRet1 ||
				0 != bRet2)
			{
				//bad memory pointer, allocate a new memory
				DWORD dwMemoryReq = g_dwExtraBuffer + dwSize;
				lpszBuffer = new TCHAR [dwMemoryReq + 1];
				if(NULL == lpszBuffer)
				{
					return ERROR_NOT_ENOUGH_MEMORY;
				}
				//allocation successful, set the memory
				//_tcsnset(lpszBuffer,_T('\0'),dwMemoryReq);
				memset(lpszBuffer,_T('\0'),dwMemoryReq);
			}

			//see whether the current memory is sufficient to add the string to the buffer
			DWORD dwActualSize = _msize(lpszBuffer); //Returns the size of a memory block allocated in the heap.
			--dwActualSize;
			DWORD dwSizeUsed = _tcslen(lpszBuffer);

			//Initialize the buffer if set to TRUE
			if(TRUE == bRefresh)
			{
				//_tcsnset(lpszBuffer,_T('\0'),dwActualSize);
				memset(lpszBuffer,_T('\0'),dwActualSize);
				dwSizeUsed = 0;
			}

			//calculate the required size for the store string & the string to be stored
			DWORD dwReqSize = dwSizeUsed + dwSize + 1;
			if(dwReqSize > dwActualSize)
			{
				//Memory is insufficient allocate a new block
				DWORD dwMemoryReq = g_dwExtraBuffer + dwReqSize;

				LPTSTR lpszTemp = new TCHAR[dwMemoryReq + 1];
				if(NULL == lpszTemp)
				{
					//allocation failed
					return ERROR_NOT_ENOUGH_MEMORY;
				}
				//allocation found successful, initialize the mem block
				//_tcsnset(lpszTemp,_T('\0'),dwMemoryReq);
				memset(lpszTemp,_T('\0'),dwMemoryReq);

				//copy the old string back to the newly allocated block
				_tcscpy(lpszTemp,lpszBuffer);

				//delete the old allocated block
				try
				{
					delete [] lpszBuffer;
				}
				catch(...)
				{
					//Something has gone wrong while deleting, it might be Corrupt Memory
					lpszBuffer = NULL;
					//Store the newly allocated memory
					lpszBuffer = lpszTemp;

					//copy the final string
					_tcscpy(lpszBuffer,lpszString);
					return S_OK;
				}
				lpszBuffer = NULL;
				lpszBuffer = lpszTemp;

				//copy the final string
				_tcscat(lpszBuffer,lpszString);
			}
			else
			{
				//Memory is sufficient copy the string
				_tcscat(lpszBuffer,lpszString);
			}
		}
	}
	catch(...)
	{
		//some thing has gone wrong ???????
		return E_FAIL;
	}
	return S_OK;
}

/************************************************/
bool WriteDummyWebPost(char* szLRegId,char* szCurrDateTime,char* szSmartDataFile,bool IsVM,bool IsMart)
{
	char* pszDataBuffer;
	pszDataBuffer = NULL;
	//WriteToBuffer(pszDataBuffer,"<wpsmartdisknew>\r\n",FALSE);
	//modified by Kedar
	WriteToBuffer(pszDataBuffer,"<wpsmartdisknew>\r\n",TRUE);
	WriteToBuffer(pszDataBuffer,"<regid>",FALSE);
	WriteToBuffer(pszDataBuffer,szLRegId,FALSE);
	WriteToBuffer(pszDataBuffer,"</regid>\r\n",FALSE);
	WriteToBuffer(pszDataBuffer,"<scandatetime>",FALSE);
	WriteToBuffer(pszDataBuffer,szCurrDateTime,FALSE);
	WriteToBuffer(pszDataBuffer,"</scandatetime>\r\n",FALSE);	
	WriteToBuffer(pszDataBuffer,"<isvm>",FALSE);
	if(IsVM == true)
		WriteToBuffer(pszDataBuffer,"1",FALSE);
	else
		WriteToBuffer(pszDataBuffer,"0",FALSE);
	WriteToBuffer(pszDataBuffer,"</isvm>\r\n",FALSE);
	WriteToBuffer(pszDataBuffer,"<diskdetail>\r\n",FALSE);	
	WriteToBuffer(pszDataBuffer,"<issmartenabled>",FALSE);
	if(IsMart == true)
		WriteToBuffer(pszDataBuffer,"1",FALSE);
	else
		WriteToBuffer(pszDataBuffer,"0",FALSE);
	WriteToBuffer(pszDataBuffer,"</issmartenabled>\r\n",FALSE);

	WriteToBuffer(pszDataBuffer,"<diskid>",FALSE);
	WriteToBuffer(pszDataBuffer,"",FALSE);
	WriteToBuffer(pszDataBuffer,"</diskid>\r\n",FALSE);

	WriteToBuffer(pszDataBuffer,"<id>",FALSE);
	WriteToBuffer(pszDataBuffer,"",FALSE);
	WriteToBuffer(pszDataBuffer,"</id>\r\n",FALSE);
				
			
	WriteToBuffer(pszDataBuffer,"<description>",FALSE);
	WriteToBuffer(pszDataBuffer,"",FALSE);
	WriteToBuffer(pszDataBuffer,"</description>\r\n",FALSE);

	WriteToBuffer(pszDataBuffer,"<rawvalue>",FALSE);
	WriteToBuffer(pszDataBuffer,"",FALSE);
	WriteToBuffer(pszDataBuffer,"</rawvalue>\r\n",FALSE);

	WriteToBuffer(pszDataBuffer,"<status>",FALSE);
	WriteToBuffer(pszDataBuffer,"",FALSE);
	WriteToBuffer(pszDataBuffer,"</status>\r\n",FALSE);

	WriteToBuffer(pszDataBuffer,"<value>",FALSE);
	WriteToBuffer(pszDataBuffer,"",FALSE);
	WriteToBuffer(pszDataBuffer,"</value>\r\n",FALSE);
			
	WriteToBuffer(pszDataBuffer,"<worst>",FALSE);
	WriteToBuffer(pszDataBuffer,"",FALSE);
	WriteToBuffer(pszDataBuffer,"</worst>\r\n",FALSE);
			
	WriteToBuffer(pszDataBuffer,"<threshold>",FALSE);
	WriteToBuffer(pszDataBuffer,"",FALSE);
	WriteToBuffer(pszDataBuffer,"</threshold>\r\n",FALSE);
			
	WriteToBuffer(pszDataBuffer,"<tec>",FALSE);
	WriteToBuffer(pszDataBuffer,"",FALSE);
	WriteToBuffer(pszDataBuffer,"</tec>\r\n",FALSE);
	WriteToBuffer(pszDataBuffer,"</diskdetail>\r\n",FALSE);

	WriteToBuffer(pszDataBuffer,"</wpsmartdisknew>\r\n",FALSE);
	if((pszDataBuffer) && strlen(pszDataBuffer) > 0)
	{
		HANDLE hFile;
		hFile = CreateFile(szSmartDataFile,                // name of the write
					GENERIC_WRITE,          // open for writing
					0,                      // do not share
					NULL,                   // default security
					CREATE_NEW,             // create new file only
					FILE_ATTRIBUTE_NORMAL,  // normal file
					NULL);                  // no attr. template

		if (hFile == INVALID_HANDLE_VALUE) 
		{ 
			DeleteFile(szSmartDataFile);
			hFile = CreateFile(szSmartDataFile,                // name of the write
							GENERIC_WRITE,          // open for writing
							0,                      // do not share
							NULL,                   // default security
							CREATE_NEW,             // create new file only
							FILE_ATTRIBUTE_NORMAL,  // normal file
							NULL);                  // no attr. template
			if (hFile == INVALID_HANDLE_VALUE) 
			{ 
				delete[] pszDataBuffer;
				return false;
			}
		}
		DWORD dwBytesToWrite = (DWORD)strlen(pszDataBuffer);
		DWORD dwBytesWritten = 0;
		BOOL bErrorFlag = FALSE;
		bErrorFlag = WriteFile(hFile,           // open file handle
						pszDataBuffer,      // start of data to write
						dwBytesToWrite,  // number of bytes to write
						&dwBytesWritten, // number of bytes that were written
						NULL);
		if(bErrorFlag == false)
		{
			delete[] pszDataBuffer;
			return false;
		}
		CloseHandle(hFile);
		delete[] pszDataBuffer;
	}
	return true;
}
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	char* pszDataBuffer;
	pszDataBuffer = NULL;
	char szCurrDateTime[TIMESTAMPBUF]	={0};
	char szDateTime[TIMESTAMPBUF] = {0};
	//SAAZOD reading and verification..
	//Sleep(30*1000);
	HKEY hKey;

	LONG lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\SAAZOD", 0, KEY_READ, &hKey);
	if(lRes != ERROR_SUCCESS)
	{
		return 0;
	}
	if(lRes == ERROR_FILE_NOT_FOUND)
	{
		return 0;
	}

	//Getting installation path..
	ULONG nError;
	nError = 0;
	GetStringRegKey(hKey,"InstallationPath", SAAZODSetUpPath,"bad",nError);
	if (ERROR_SUCCESS != nError)
    {
		return 0;
    }
	/*
	if(strcmp(SAAZODSetUpPath,"") == 0)
	{
		return 0;
	}
	*/
	//Modified by Kedar

	if(NULL == SAAZODSetUpPath[0])
	{
		return 0;
	}


	//Gettting REG ID..
	char szLRegId[50] = {0};
	
	nError = 0;
	GetStringRegKey(hKey,"regid", szLRegId,"bad",nError);
	if (ERROR_SUCCESS != nError)
    {
		return 0;
    }
	/*
	if(strcmp(szLRegId,"") == 0)
	{
		return 0;
	}
	*/

	if(NULL == szLRegId[0])
	{
		return 0;
	}
	

	///Header
	char szMessage[300] = {0};
	sprintf(szMessage,"**************************************************************");
	WriteApplicationFile("SMART",szMessage);

	//Blank line
	memset(szMessage,0,300);
	sprintf(szMessage,"");
	WriteApplicationFile("SMART",szMessage);

	memset(szMessage,0,300);
	sprintf(szMessage,"**************************************************************");
	WriteApplicationFile("SMART",szMessage);
	if(_strcmpi(lpCmdLine,"SmartAll") == 0)
	{
		lpCmdLine = "1";
	}
	else if(_strcmpi(lpCmdLine,"SmartCheck") == 0)
	{
		lpCmdLine = "2";
	}	
	else if(_strcmpi(lpCmdLine,"SmartCal") == 0)
	{
		lpCmdLine = "3";
	}

	//Getting name of executable by its parameter.
	char ExeName[MAX_PATH] = {0};
	if(strcmp(lpCmdLine,"1") == 0)
	{
		strcpy(ExeName,"SmartAll");
	}
	else if(strcmp(lpCmdLine,"2") == 0)
	{
		strcpy(ExeName,"SmartCheck");
	}
	else if(strcmp(lpCmdLine,"3") == 0)
	{
		strcpy(ExeName,"SmartCal");
	}
	else
	{
		strcpy(ExeName,"SmartExecute");
	}

	if(strcmp(lpCmdLine,"")==0)
	{
		memset(szMessage,0,300);
		sprintf(szMessage,"ERR\t%s\tNo parameter is passed to executable.",ExeName);
		WriteApplicationFile("SMART",szMessage);
		return 0;
	}

	try
	{
		memset(szMessage,0,300);
		sprintf(szMessage,"INF\t%s\tStart Execution.",ExeName);
		WriteApplicationFile("SMART",szMessage);

		SYSTEMTIME sysTime;
		//memset(&sysTime,NULL,300);	
		GetLocalTime(&sysTime);
		GetCurrentDateTime(szCurrDateTime);		

		//Application log folder creation
		char szDirName[MAX_PATH] = {0};
		memset(szDirName,0,MAX_PATH);
		strcpy(szDirName,SAAZODSetUpPath);
		strcat(szDirName,"\\ApplicationLog\0");
		CreateDirectory(szDirName,NULL);

		//TEMP folder creation
		//cant remove folder temp folder because this folder contain smart check schedule file
		memset(szDirName,0,MAX_PATH);
		strcpy(szDirName,SAAZODSetUpPath);
		strcat(szDirName,"\\TEMP\0");
		CreateDirectory(szDirName,NULL);

		//PMSTATUS folder creation
		memset(szDirName,0,MAX_PATH);
		strcpy(szDirName,SAAZODSetUpPath);
		strcat(szDirName,"\\PMSTATUS\0");
		CreateDirectory(szDirName,NULL);
		
		//Configuration folder.
		memset(szDirName,0,MAX_PATH);
		strcpy(szDirName,SAAZODSetUpPath);
		strcat(szDirName,"\\Configuration\0");
		CreateDirectory(szDirName,NULL);

		//Configuration folder.
		memset(szDirName,0,MAX_PATH);
		strcpy(szDirName,SAAZODSetUpPath);
		strcat(szDirName,"\\AlertCD\0");
		CreateDirectory(szDirName,NULL);
		

		char szHostName[MAX_PATH];
		memset(szHostName,0,MAX_PATH);
		WSADATA wsData;
		WSAStartup(MAKEWORD( 2, 2 ),&wsData);
		gethostname(szHostName,sizeof(szHostName));
		WSACleanup();
		
		memset(szMessage,0,300);
		sprintf(szMessage,"INF\t%s\tGot host name.Name is %s",ExeName,szHostName);
		WriteApplicationFile("SMART",szMessage);
		
		//For date allocation
		if(strlen(szHostName) == 0)
		{
			memset(szMessage,0,300);
			sprintf(szMessage,"ERR\t%s\tHost name is not found",ExeName);
			WriteApplicationFile("SMART",szMessage);


			memset(szDateTime,0,51);
			GetCurrentDateTime(szDateTime);
			memset(szMessage,0,300);
			sprintf(szMessage,PID_TEXT,szLRegId,"Cannot Find Local Host Name.","SmartExecute",szDateTime,"Failed");
			//WriteToLog(szPIDFile,szTmpBuf);
			writePIDFile(szMessage,szLRegId,"SmartExecute");

		}

		if((strcmp(lpCmdLine,"1") !=0)&&
			(strcmp(lpCmdLine,"2") !=0)&&
			(strcmp(lpCmdLine,"3") !=0))
		{
			memset(szMessage,0,300);
			sprintf(szMessage,"ERR\t%s\tInvalid parameter",ExeName);
			WriteApplicationFile("SMART",szMessage);

			memset(szDateTime,0,51);
			GetCurrentDateTime(szDateTime);
			memset(szMessage,0,300);
			sprintf(szMessage,PID_TEXT,szLRegId,"Invalid parameter.","SmartExecute",szDateTime,"Failed");
			writePIDFile(szMessage,szLRegId,"SmartExecute");
			return 0;
		}

		int ParameterValue = atoi(lpCmdLine);
		switch(ParameterValue)
		{
		case 1:
			//Calling Smart all
			{
				///No condition execute it at all condition
				memset(szMessage,0,300);
				sprintf(szMessage,"INF\t%s\tExecuting smartAll.",ExeName);
				WriteApplicationFile("SMART",szMessage);
			}
			break;
		case 2:
			//calling smart check
			{
				memset(szMessage,0,300);
				sprintf(szMessage,"INF\t%s\tExecuting smartCheck.",ExeName);
				WriteApplicationFile("SMART",szMessage);

				char szTimeStampFile[MAX_PATH] = {0};
				char szTimeStamp[50] = {0};
				char szCurrTimeStamp[50] = {0};


				sprintf(szTimeStampFile,"%s\\TEMP\\LastRunTimestamp.ini\0",SAAZODSetUpPath);

				::GetPrivateProfileString("LastRunTimeStampSmartCheck","LastRunTime",NULL,szTimeStamp,sizeof(szTimeStamp),szTimeStampFile);

				time_t long_time = 0;
				time( &long_time );		

				if(szTimeStamp == NULL || strlen(szTimeStamp) == 0 || strcmp(szTimeStamp,"0") == 0)
				{
					//_ltoa(long_time,szTimeStamp,10);
					_i64toa(long_time,szTimeStamp,10);
					WritePrivateProfileString("LastRunTimeStampSmartCheck","LastRunTime",szTimeStamp,szTimeStampFile);
				}
				else
				{
					__int64 lFiletime = 0,lDiffTime = 0,lFourHrs = 0;
					lFourHrs = (60 * 60 * 4);

					lFiletime = atol(szTimeStamp);
			
					lDiffTime = long_time - lFiletime;

					long lConvTimeVal = 0;

					if(lDiffTime < 0)
					{
						//lFourHrs = lFiletime - lFourHrs;
						struct tm *newtime = NULL;
						char am_pm[] = "AM\0";
				
						newtime = ::localtime(&long_time);
				
						if( newtime->tm_hour > 12 )
								strcpy( am_pm, "PM\0" );

						if(newtime->tm_hour == 12)
						{				
							strcpy( am_pm, "PM\0" );
						}

						if( newtime->tm_hour > 12 )
								newtime->tm_hour -= 12;
						if( newtime->tm_hour == 0 )
								newtime->tm_hour = 12;

						sprintf(szCurrTimeStamp,
						"%02d/%02d/%4d %02d:%02d:%02d %s\0",
						newtime->tm_mon + 1,
						newtime->tm_mday,
						(newtime->tm_year + 2000) - 100,
						newtime->tm_hour,
						newtime->tm_min,
						newtime->tm_sec,
						am_pm);

						WritePrivateProfileString("LastRunTimeStampSmartCheck","LastRun",szCurrTimeStamp,szTimeStampFile);

						memset(szTimeStamp,0,50);
						_i64toa(long_time,szTimeStamp,10);
						WritePrivateProfileString("LastRunTimeStampSmartCheck","LastRunTime",szTimeStamp,szTimeStampFile);

						memset(szMessage,0,300);
						sprintf(szMessage,"ERR\t%s\tTime spam is not completed.",ExeName);
						WriteApplicationFile("SMART",szMessage);
						return 0;
					}
					else if(lDiffTime >= 0)
					{
						lFourHrs = lFiletime + lFourHrs;

						if(long_time < lFourHrs)
						{
							memset(szMessage,0,300);
							sprintf(szMessage,"INF\t%s\tSmartCheck Could not retrieve data because of long_time < lFourHrs.",ExeName);
							WriteApplicationFile("SMART",szMessage);
							return 0;
						}

						struct tm *newtime = NULL;
						char am_pm[] = "AM\0";
				
						newtime = ::localtime(&long_time);
				
						if( newtime->tm_hour > 12 )
								strcpy( am_pm, "PM\0" );

						if(newtime->tm_hour == 12)
						{				
							strcpy( am_pm, "PM\0" );
						}

						if( newtime->tm_hour > 12 )
								newtime->tm_hour -= 12;
						if( newtime->tm_hour == 0 )
								newtime->tm_hour = 12;

						sprintf(szCurrTimeStamp,
						"%02d/%02d/%4d %02d:%02d:%02d %s\0",
						newtime->tm_mon + 1,
						newtime->tm_mday,
						(newtime->tm_year + 2000) - 100,
						newtime->tm_hour,
						newtime->tm_min,
						newtime->tm_sec,
						am_pm);

						WritePrivateProfileString("LastRunTimeStampSmartCheck","LastRun",szCurrTimeStamp,szTimeStampFile);

						memset(szTimeStamp,0,50);
						_i64toa(long_time,szTimeStamp,10);
						WritePrivateProfileString("LastRunTimeStampSmartCheck","LastRunTime",szTimeStamp,szTimeStampFile);

						memset(szMessage,0,300);
						sprintf(szMessage,"ERR\t%s\tTime span is not completed.",ExeName);
						WriteApplicationFile("SMART",szMessage);
					}
				}

			}
			break;
		case 3:
			//calling smart cal
			{
				memset(szMessage,0,300);
				sprintf(szMessage,"ERR\t%s\tExecuting smart cal.",ExeName);
				WriteApplicationFile("SMART",szMessage);
				return 0;
			}
			break;
		}

		//Smart executable working.

		char szScriptDNSFile[MAX_PATH] = {0};
		char wpsmartdisknewReturn[50] = {0};
		
 		sprintf(szScriptDNSFile,"%s\\Configuration\\zScriptDNS.ini\0",SAAZODSetUpPath);

		::GetPrivateProfileString("wpsmartdisknew","URL",NULL,wpsmartdisknewReturn,sizeof(szScriptDNSFile),szScriptDNSFile);

		if(strcmp(wpsmartdisknewReturn,"") == 0)
		{
			WritePrivateProfileString("wpsmartdisknew","URL","https://webpost2.itsupport247.net",szScriptDNSFile);
		}

		char szAlrtDCSmartDataFile[MAX_PATH] = {0};
		char szSmartDataFile[MAX_PATH] = {0};


		sprintf(szSmartDataFile,"%s\\wpsmartdisknew_N_%s_%04d%02d%02d%02d%02d%02d.txt\0",SAAZODSetUpPath,szLRegId,sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute,sysTime.wSecond);
		sprintf(szAlrtDCSmartDataFile,"%s\\AlertCD\\wpsmartdisknew_N_%s_%04d%02d%02d%02d%02d%02d.txt\0",SAAZODSetUpPath,szLRegId,sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute,sysTime.wSecond);
		

		//VM machine checking..
		memset(szMessage,0,300);
		sprintf(szMessage,"INF\t%s\tReading VMware registry.",ExeName);
		WriteApplicationFile("SMART",szMessage);
		//To check weather machine is VM or not
		bool VmMachine;
		VmMachine = false;
		
		HKEY VmWareKey;
		LONG lRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"hardware\\DESCRIPTION\\SYSTEM\\BIOS", 0, KEY_READ, &VmWareKey);
		if(lRes != ERROR_SUCCESS)
		{
			VmMachine = false;
		}
		if(lRes == ERROR_FILE_NOT_FOUND)
		{
			VmMachine = false;
		}

		char  VmMachineName[100] = {0};
		nError = 0;
		GetStringRegKey(VmWareKey,"SystemProductName", VmMachineName,"bad",nError);
		if (ERROR_SUCCESS != nError)
		{
			VmMachine = false;
		}

		if(strcmp(VmMachineName,"") == 0)
		{
			VmMachine = false;
		}
		if(StrCmp(VmMachineName,"") == 0)
		{
			memset(szMessage,0,300);
			sprintf(szMessage,"ERR\t%s\tFailed to read SystemProductName for VM from BIOS!!",ExeName);
			WriteApplicationFile("SMART",szMessage);
			//Read it using WMI
			HRESULT hres;  
			// Step 1: --------------------------------------------------  
			// Initialize COM. ------------------------------------------    
			hres =  CoInitializeEx(0, COINIT_MULTITHREADED);   
			if (FAILED(hres))  
			{  
				memset(szMessage,0,300);
				sprintf(szMessage,"ERR\t%s\tFailed to initialize WMI COM Library!!",ExeName);
				WriteApplicationFile("SMART",szMessage);
				return 1;                  // Program has failed.  
			}  
  
			// Step 2: --------------------------------------------------  
			// Set general COM security levels --------------------------  
			// Note: If you are using Windows 2000, you need to specify -  
			// the default authentication credentials for a user by using  
			// a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----  
			// parameter of CoInitializeSecurity ------------------------  
  
			hres =  CoInitializeSecurity(  
				NULL,   
				-1,                          // COM authentication  
				NULL,                        // Authentication services  
				NULL,                        // Reserved  
				RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication   
				RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation    
				NULL,                        // Authentication info  
				EOAC_NONE,                   // Additional capabilities   
				NULL                         // Reserved  
				);  
  
                        
			if (FAILED(hres)) 
			{  
				memset(szMessage,0,300);
				sprintf(szMessage,"ERR\t%s\tFailed to initialize WMI security!!",ExeName);
				WriteApplicationFile("SMART",szMessage);
				CoUninitialize();  
				return 1;                    // Program has failed.  
			}  
      
			// Step 3: ---------------------------------------------------  
			// Obtain the initial locator to WMI -------------------------  
  
			IWbemLocator *pLoc = NULL;  
  
			hres = CoCreateInstance(  
				CLSID_WbemLocator,               
				0,   
				CLSCTX_INPROC_SERVER,   
				IID_IWbemLocator, (LPVOID *) &pLoc);  
   
			if (FAILED(hres))  
			{  
				memset(szMessage,0,300);
				sprintf(szMessage,"ERR\t%s\tFailed to create IWbemLocator object!!",ExeName);
				WriteApplicationFile("SMART",szMessage);
				CoUninitialize();  
				return 1;                 // Program has failed.  
			}  
  
			// Step 4: -----------------------------------------------------  
			// Connect to WMI through the IWbemLocator::ConnectServer method  
  
			IWbemServices *pSvc = NULL;  
      
			// Connect to the root\cimv2 namespace with  
			// the current user and obtain pointer pSvc  
			// to make IWbemServices calls.  
			hres = pLoc->ConnectServer(  
				 _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace  
				 NULL,                    // User name. NULL = current user  
				 NULL,                    // User password. NULL = current  
				 0,                       // Locale. NULL indicates current  
				 NULL,                    // Security flags.  
				 0,                       // Authority (e.g. Kerberos)  
				 0,                       // Context object   
				 &pSvc                    // pointer to IWbemServices proxy  
				 );  
      
			if (FAILED(hres))  
			{  
				memset(szMessage,0,300);
				sprintf(szMessage,"ERR\t%s\tCould not connect. ROOT\\CIMV2!!",ExeName);
				WriteApplicationFile("SMART",szMessage);
				pLoc->Release();       
				CoUninitialize();  
				return 1;                // Program has failed.  
			}  
  
			memset(szMessage,0,300);
			sprintf(szMessage,"ERR\t%s\tConnected to ROOT\\CIMV2 WMI namespace",ExeName);
			WriteApplicationFile("SMART",szMessage);  
  
			// Step 5: --------------------------------------------------  
			// Set security levels on the proxy -------------------------  
  
			hres = CoSetProxyBlanket(  
			   pSvc,                        // Indicates the proxy to set  
			   RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx  
			   RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx  
			   NULL,                        // Server principal name   
			   RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx   
			   RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx  
			   NULL,                        // client identity  
			   EOAC_NONE                    // proxy capabilities   
			);  

			if (FAILED(hres))  
			{  
				memset(szMessage,0,300);
				sprintf(szMessage,"ERR\t%s\tCould not set proxy blanket for WMI!",ExeName);
				WriteApplicationFile("SMART",szMessage); 
				pSvc->Release();  
				pLoc->Release();       
				CoUninitialize();  
				return 1;               // Program has failed.  
			}  
			memset(szMessage,0,300);
			sprintf(szMessage,"ERR\t%s\tproxy blanket set successfully!",ExeName);
			WriteApplicationFile("SMART",szMessage); 
			  
      
			// Step 6: --------------------------------------------------  
			// Use the IWbemServices pointer to make requests of WMI ----  
  
			// For example, get the name of the operating system  
			IEnumWbemClassObject* pEnumerator = NULL;  
			hres = pSvc->ExecQuery(  
				bstr_t("WQL"),   
				bstr_t("SELECT * FROM Win32_ComputerSystem"),  
				WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,   
				NULL,  
				&pEnumerator);  

			memset(szMessage,0,300);
			sprintf(szMessage,"ERR\t%s\tWQL \"SELECT * FROM Win32_ComputerSystem\"! executed!!",ExeName);
			WriteApplicationFile("SMART",szMessage); 
			if (FAILED(hres))  
			{  
				memset(szMessage,0,300);
				sprintf(szMessage,"ERR\t%s\tQuery for operating system name failed!!",ExeName);
				WriteApplicationFile("SMART",szMessage);  
				pSvc->Release();  
				pLoc->Release();  
				CoUninitialize();  
				return 1;               // Program has failed.  
			}  
  

			// Step 7: -------------------------------------------------  
			// Get the data from the query in step 6 -------------------  
   
			IWbemClassObject *pclsObj;  
			ULONG uReturn = 0;  
     
			while (pEnumerator)  
			{  
				HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);  
  
				if (FAILED(hr))  
				{  
					memset(szMessage,0,300);
					sprintf(szMessage,"ERR\t%s\tpEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);  failed!!",ExeName);
					WriteApplicationFile("SMART",szMessage);  
					pSvc->Release();  
					pLoc->Release();  
					CoUninitialize();  
					return 1;               // Program has failed.  
				}  
				if(0 == uReturn)  
				{  
					break;  
				}  
  
				VARIANT vtProp; 
				// Get the value of the Name property  
  
				hr = pclsObj->Get(L"Model", 0, &vtProp , 0, 0);  
				if (FAILED(hr))  
				{
					memset(szMessage,0,300);
					sprintf(szMessage,"ERR\t%s\tFailed to retrive Model from Win32_ComputerSystem!!",ExeName);
					WriteApplicationFile("SMART",szMessage);  
					pSvc->Release();  
					pLoc->Release();  
					CoUninitialize();  
					return 1;               // Program has failed.  
				}
				USES_CONVERSION;
				strcpy(VmMachineName , W2A(vtProp.bstrVal));
				IEnumWbemClassObject* pEnumerator2 = NULL;  
				VariantClear(&vtProp);  
			}  
		}
		if(strcmp(VmMachineName,"") == 0)
		{
			WriteApplicationFile("SMART","Failed to retrive VM informartion!!");  
			return 1;
		}
		if(strcmp(VmMachineName,Virtual_Machine) == 0)
		{
			VmMachine = true;
		}
		if(strcmp(VmMachineName,VMware_Virtual_Platform) == 0)
		{
			VmMachine = true;
		}
		if(strcmp(VmMachineName,VirtualBox) == 0)
		{
			VmMachine = true;
		}
		if(VmMachine)
		{//If machine is VM
			if(ParameterValue == 2)
			{//And if it is smart check then no data will send to datacentre
				memset(szMessage,0,300);
				GetCurrentDateTime(szDateTime);
				sprintf(szMessage,PID_TEXT,szLRegId,"VMaware with SMARTCheck.\0","SmartExecute",szDateTime,"Failed");
				writePIDFile(szMessage,szLRegId,"SmartExecute");

				memset(szMessage,0,300);
				sprintf(szMessage,"ERR\t%s\tVMaware with SMARTCheck.",ExeName);
				WriteApplicationFile("SMART",szMessage);
				return 0;
			}

			if(!WriteDummyWebPost(szLRegId,szCurrDateTime,szSmartDataFile,VmMachine,0))
			{
				memset(szMessage,0,300);
				sprintf(szMessage,"ERR\t%s\tFailed to create XML file.",ExeName);
				WriteApplicationFile("SMART",szMessage);
				return 0;
			}

			rename(szSmartDataFile, szAlrtDCSmartDataFile);
			DeleteFile(szSmartDataFile);

			memset(szMessage,0,300);
			sprintf(szMessage,"INF\t%s\tMachine is virtual.Default data for virtual machine is send.",ExeName);
			WriteApplicationFile("SMART",szMessage);


			memset(szMessage,0,300);
			GetCurrentDateTime(szDateTime);
			sprintf(szMessage,PID_TEXT,szLRegId,"Machine is virtual.Machine is virtual.\0","SmartExecute",szDateTime,"Failed");
			//WriteToLog(szPIDFile,szTmpBuf);
			writePIDFile(szMessage,szLRegId,"SmartExecute");
			return 0;
		}

		memset(szMessage,0,300);
		sprintf(szMessage,"INF\t%s\tBefore Intialize Library",ExeName);
		WriteApplicationFile("SMART",szMessage);
		try
		{
			if(SysInfoDll_Smart_Init(false,NULL,true) != SMART_STATUS_SUCCESS)
			{

				memset(szMessage,0,300);
				GetCurrentDateTime(szDateTime);
				sprintf(szMessage,PID_TEXT,szLRegId,"Call to SysInfoDll_Smart_Init() get Failed..","SmartExecute",szDateTime,"Failed");
				//WriteToLog(szPIDFile,szTmpBuf);
				writePIDFile(szMessage,szLRegId,"SmartExecute");

				memset(szMessage,0,300);
				sprintf(szMessage,"ERR\t%s\tCall to SysInfoDll_Smart_Init() get Failed.",ExeName);
				WriteApplicationFile("SMART",szMessage);
				return 0;
			}
		}
		catch(...)
		{
			memset(szMessage,0,300);
			GetCurrentDateTime(szDateTime);
			sprintf(szMessage,PID_TEXT,szLRegId,"Call to SysInfoDll_Smart_Init() get Failed..","SmartExecute",szDateTime,"Failed");
			//WriteToLog(szPIDFile,szTmpBuf);
			writePIDFile(szMessage,szLRegId,"SmartExecute");

			memset(szMessage,0,300);
			sprintf(szMessage,"ERR\t%s\tCall to SysInfoDll_Smart_Init() get Failed.",ExeName);
			WriteApplicationFile("SMART",szMessage);
		}

		memset(szMessage,0,300);
		sprintf(szMessage,"INF\t%s\tAfter Initialise Smart Library.",ExeName);
		WriteApplicationFile("SMART",szMessage);


		int nSmartDevices = 0;
		nSmartDevices = SysInfoDll_Smart_GetNumSmartDevices();


		memset(szMessage,0,300);
		sprintf(szMessage,"INF\t%s\tNumber of Smart Devices: %d",ExeName,nSmartDevices);
		WriteApplicationFile("SMART",szMessage);


		if(nSmartDevices <= 0)
		{//No smart device found
			if(ParameterValue == 2)
			{
				memset(szMessage,0,300);
				GetCurrentDateTime(szDateTime);
				sprintf(szMessage,PID_TEXT,szLRegId,"No device with SmartCheck.\0","SmartExecute",szDateTime,"Failed");
				writePIDFile(szMessage,szLRegId,"SmartExecute");

				memset(szMessage,0,300);
				sprintf(szMessage,"ERR\t%s\tNo device with SmartCheck.",ExeName);
				WriteApplicationFile("SMART",szMessage);
				return 0;
			}
			//Write Dummy data for web post.
			if(!WriteDummyWebPost(szLRegId,szCurrDateTime,szSmartDataFile,VmMachine,0))
			{
				memset(szMessage,0,300);
				sprintf(szMessage,"ERR\t%s\tFailed to create XML file.",ExeName);
				WriteApplicationFile("SMART",szMessage);
				return 0;
			}

			//move and Delete created file
			rename(szSmartDataFile, szAlrtDCSmartDataFile);
			DeleteFile(szSmartDataFile);

			memset(szMessage,0,300);
			sprintf(szMessage,"INF\t%s\tNo smart device is enabled.Default data for SMART is send.",ExeName);
			WriteApplicationFile("SMART",szMessage);

			memset(szMessage,0,300);
			GetCurrentDateTime(szDateTime);
			sprintf(szMessage,PID_TEXT,szLRegId,"No SMART Device Found.\0","SmartExecute",szDateTime,"Failed");
			writePIDFile(szMessage,szLRegId,"SmartExecute");
			return 0;
		}


		int nDeviceID = 0;
		nDeviceID = SysInfoDll_Smart_GetSmartDeviceIDFirst ();//returns device id of smart device detected

		if(nDeviceID == SMART_ERR_SMART_DEVICE_ID_EMPTY)
		{
			//printf("\n S.M.A.R.T. DeviceID list is Empty\n");
			memset(szMessage,0,300);
			GetCurrentDateTime(szDateTime);
			sprintf(szMessage,PID_TEXT,szLRegId,"Could not found SMART Device.\0","SmartExecute",szDateTime,"Failed");
			//WriteToLog(szPIDFile,szTmpBuf);
			writePIDFile(szMessage,szLRegId,"SmartExecute");

			memset(szMessage,0,300);
			sprintf(szMessage,"ERR\t%s\tCould not found SMART Device.",ExeName);
			WriteApplicationFile("SMART",szMessage);
			return 0;
		}
		
		bool FirstTimewriting;
		FirstTimewriting = true;

		bool WriteEndTag;
		WriteEndTag = false;

		for(int nLocalCount= 0;nLocalCount < nSmartDevices;nLocalCount++)
		{
			memset(szMessage,0,300);
			sprintf(szMessage,"INF\t%s\t=========Iteration for Device: %d.",ExeName,nLocalCount);
			WriteApplicationFile("SMART",szMessage);

			char ioBuf[100]			=	{0};
			wchar_t ioBufW[100]		=	{0};
			
			if(nDeviceID < 0)
			{
				nDeviceID = SysInfoDll_Smart_GetSmartDeviceIDNext();//returns dev -id of next SMART dev detected
				if(nDeviceID == SMART_ERR_SMART_DEVICE_ID_EOL)
					break;
				continue;
			}

			memset(szMessage,0,300);
			sprintf(szMessage,"INF\t%s\tGeting device serial number.",ExeName);
			WriteApplicationFile("SMART",szMessage);

			int len=0;
			if(SysInfoDll_Smart_GetDeviceSerialNumber(ioBufW,&len, nDeviceID) == SMART_STATUS_SUCCESS)
			{
				WideCharToMultiByte(CP_ACP, //Default ANSI Code Page
									0,      //  For the code page 65001 (UTF-8) or the code page 54936 (GB18030, Windows Vista and later), dwFlags must be set to either 0 
									ioBufW, // buffW
									lstrlenW(ioBufW)+1, //returns size in chars + 1 for null
									ioBuf, //dest
									100,   //size of ioBuf
									NULL,  //use a system default value
									NULL); //pointer to the flag default character in the conversion

				char szSerialFile[MAX_PATH] = {0};
				memset(szSerialFile,0,MAX_PATH);
				lstrcpy(szSerialFile,SAAZODSetUpPath);
				lstrcat(szSerialFile,"\\");
				lstrcat(szSerialFile,ioBuf);
				lstrcat(szSerialFile,".bin\0");
				WIN32_FILE_ATTRIBUTE_DATA attr = {0};
				BOOL res = GetFileAttributesEx(szSerialFile,GetFileExInfoStandard,&attr);
				if( GetFileAttributesEx(szSerialFile,GetFileExInfoStandard,&attr) != 0)
				{
					long lMBVal = 0;
					lMBVal = (1024 * 1024);
					if(attr.nFileSizeLow > (DWORD)lMBVal)
					{
						DeleteFile(szSerialFile);
					}
				}				
			}

			memset(szMessage,0,300);
			sprintf(szMessage,"INF\t%s\tIs smart device is valid checking.",ExeName);
			WriteApplicationFile("SMART",szMessage);
			int nIsValid = SysInfoDll_Smart_IsValidSmartDevice(nDeviceID);


			if(nIsValid != 0)//disc is not smart then dump dummy data and continue..
			{
				if(FirstTimewriting == true)
				{
					//WriteToBuffer(pszDataBuffer,"<wpsmartdisknew>\r\n",FALSE);
					//Modified by Kedar
					WriteToBuffer(pszDataBuffer,"<wpsmartdisknew>\r\n",TRUE);
					WriteToBuffer(pszDataBuffer,"<regid>",FALSE);
					WriteToBuffer(pszDataBuffer,szLRegId,FALSE);
					WriteToBuffer(pszDataBuffer,"</regid>\r\n",FALSE);
					WriteToBuffer(pszDataBuffer,"<scandatetime>",FALSE);
					WriteToBuffer(pszDataBuffer,szCurrDateTime,FALSE);
					WriteToBuffer(pszDataBuffer,"</scandatetime>\r\n",FALSE);
					WriteToBuffer(pszDataBuffer,"<isvm>",FALSE);
					if(VmMachine)		//if vmware is there
						WriteToBuffer(pszDataBuffer,"1",FALSE);
					else				//no vmware support
						WriteToBuffer(pszDataBuffer,"0",FALSE);
					WriteToBuffer(pszDataBuffer,"</isvm>\r\n",FALSE);
					FirstTimewriting = false;
				}
				WriteToBuffer(pszDataBuffer,"<diskdetail>\r\n",FALSE);	
				WriteToBuffer(pszDataBuffer,"<issmartenabled>",FALSE);
				WriteToBuffer(pszDataBuffer,"0",FALSE);
				WriteToBuffer(pszDataBuffer,"</issmartenabled>\r\n",FALSE);

				WriteToBuffer(pszDataBuffer,"<diskid>",FALSE);
				WriteToBuffer(pszDataBuffer,"",FALSE);
				WriteToBuffer(pszDataBuffer,"</diskid>\r\n",FALSE);

				WriteToBuffer(pszDataBuffer,"<id>",FALSE);
				WriteToBuffer(pszDataBuffer,"",FALSE);
				WriteToBuffer(pszDataBuffer,"</id>\r\n",FALSE);
				
			
				WriteToBuffer(pszDataBuffer,"<description>",FALSE);
				WriteToBuffer(pszDataBuffer,"",FALSE);
				WriteToBuffer(pszDataBuffer,"</description>\r\n",FALSE);

				WriteToBuffer(pszDataBuffer,"<rawvalue>",FALSE);
				WriteToBuffer(pszDataBuffer,"",FALSE);
				WriteToBuffer(pszDataBuffer,"</rawvalue>\r\n",FALSE);

				WriteToBuffer(pszDataBuffer,"<status>",FALSE);
				WriteToBuffer(pszDataBuffer,"",FALSE);
				WriteToBuffer(pszDataBuffer,"</status>\r\n",FALSE);

				WriteToBuffer(pszDataBuffer,"<value>",FALSE);
				WriteToBuffer(pszDataBuffer,"",FALSE);
				WriteToBuffer(pszDataBuffer,"</value>\r\n",FALSE);
			
				WriteToBuffer(pszDataBuffer,"<worst>",FALSE);
				WriteToBuffer(pszDataBuffer,"",FALSE);
				WriteToBuffer(pszDataBuffer,"</worst>\r\n",FALSE);
			
				WriteToBuffer(pszDataBuffer,"<threshold>",FALSE);
				WriteToBuffer(pszDataBuffer,"",FALSE);
				WriteToBuffer(pszDataBuffer,"</threshold>\r\n",FALSE);
			
				WriteToBuffer(pszDataBuffer,"<tec>",FALSE);
				WriteToBuffer(pszDataBuffer,"",FALSE);
				WriteToBuffer(pszDataBuffer,"</tec>\r\n",FALSE);
				WriteToBuffer(pszDataBuffer,"</diskdetail>\r\n",FALSE);


				memset(szMessage,0,300);
				sprintf(szMessage,"INF\t%s\t%d device is not smart enabled.",ExeName,nLocalCount);
				WriteApplicationFile("SMART",szMessage);
				
				
				memset(szMessage,0,300);
				GetCurrentDateTime(szDateTime);
				sprintf(szMessage,PID_TEXT,szLRegId,"device is not smart enabled.\0","SmartExecute",szDateTime,"Failed");
				writePIDFile(szMessage,szLRegId,"SmartExecute");
				nDeviceID = SysInfoDll_Smart_GetSmartDeviceIDNext();

				if(nDeviceID == SMART_ERR_SMART_DEVICE_ID_EOL)
					break;
				continue;
			}
			else//smart is enabled.
			{

				memset(szMessage,0,300);
				sprintf(szMessage,"INF\t%s\t%d device is smart enabled.",ExeName,nLocalCount);
				WriteApplicationFile("SMART",szMessage);


				int iStatus = 0;
				int nAttribID = SysInfoDll_Smart_GetSmartAttributeIDFirst(nDeviceID);
				memset(szMessage,0,300);
				sprintf(szMessage,"INF\t%s\tGetting Device attribute ID.Device atribute ID is %d",ExeName,nAttribID);
				WriteApplicationFile("SMART",szMessage);


				long nAttrCount = 0;
				do
				{
					nAttrCount++;
					if(nAttrCount > 255)
					{
						break;
					}

					if(nAttribID != 0)
					{
						
						memset(szMessage,0,300);
						sprintf(szMessage,"INF\t%s\tStart to get the attributes value for AttributeID:%d",ExeName,nAttribID);
						WriteApplicationFile("SMART",szMessage);					

						int ioStatus = 1;					//For io status
						int ioThreshold = 1;				//For Threashold
						int ioWorstVal = 1;					//for worst value
						int ioNormVal = 1;					//For normal value
						__int64 ioRawVal = 1;				//for raw value
						char ioDesc[200] = {0};					//Actual Description
						wchar_t ioDescW[200]	=	{0};	//SMART API return unicode description
						
						char szStatus[20];
						char szThreshold[20];
						char szWorstVal[20];
						char szRawVal[20];
						char szNormVal[20];
						char szDeviceID[30];
						char szAttribID[30];

						
						memset(ioDesc,0,sizeof(char)*200);
						memset(ioDescW,0,sizeof(wchar_t)*200);

						int ioLen = 20;
						if(SysInfoDll_Smart_GetSmartAttributeByID(&ioStatus, &ioLen, nAttribID,ATTR_TYPE_INT_STATUS, nDeviceID) == SMART_STATUS_SUCCESS)
						{
							memset(szMessage,0,300);
							sprintf(szMessage,"INF\t%s\tGetting status of disc",ExeName);
							WriteApplicationFile("SMART",szMessage);
							if(ioStatus == 0)
							{
								memset(szMessage,0,300);
								sprintf(szMessage,"INF\t%s\tStatus is OK",ExeName);
								WriteApplicationFile("SMART",szMessage);

								memset(szStatus,0,20);
								strcpy(szStatus,"OK\0");
								if(ParameterValue == 2)
									continue;
							}
							else if(ioStatus == 1)
							{
								memset(szMessage,0,300);
								sprintf(szMessage,"INF\t%s\tStatus is NOT OK",ExeName);
								WriteApplicationFile("SMART",szMessage);

								memset(szStatus,0,20);
								strcpy(szStatus,"Not OK\0");
							}
						}
						ioThreshold = 0;
						ioLen = 20;
						memset(szThreshold,0,20);
						memset(szMessage,0,300);
						sprintf(szMessage,"INF\t%s\tStart to get treashhold value",ExeName);
						WriteApplicationFile("SMART",szMessage);
						if(SysInfoDll_Smart_GetSmartAttributeByID(&ioThreshold, &ioLen, nAttribID,ATTR_TYPE_INT_THRESHOLD, nDeviceID) == SMART_STATUS_SUCCESS)
						{							
							//itoa(ioThreshold,szThreshold,10);
							memset(szMessage,0,300);
							sprintf(szMessage,"INF\t%s\tTreashold retrive successfully.Treashold is %d",ExeName,ioThreshold);
							WriteApplicationFile("SMART",szMessage);
							sprintf(szThreshold,"%d\0",ioThreshold);
						}
						else
						{
							memset(szMessage,0,300);
							sprintf(szMessage,"INF\t%s\tFailed to retrive threashold value.",ExeName);
							WriteApplicationFile("SMART",szMessage);
						}
						memset(szMessage,0,300);
						sprintf(szMessage,"INF\t%s\tStart to get Description.",ExeName);
						WriteApplicationFile("SMART",szMessage);


						memset(ioDesc,0,200);
						ioLen = 200;
						memset(ioDescW,0,sizeof(wchar_t)*200);
						if(SysInfoDll_Smart_GetSmartAttributeByID(ioDescW, &ioLen, nAttribID,ATTR_TYPE_STR_DESC, nDeviceID) == SMART_STATUS_SUCCESS)
						{
							
							WideCharToMultiByte(CP_ACP,
											0,
											ioDescW,
											lstrlenW(ioDescW)+1,
											ioDesc,
											200,
											NULL,
											NULL);
							memset(szMessage,0,300);
							sprintf(szMessage,"INF\t%s\tSuccessfully to retrive Description.Description is %s",ExeName,ioDesc);
							WriteApplicationFile("SMART",szMessage);
						}
						else
						{
							memset(szMessage,0,300);
							sprintf(szMessage,"INF\t%s\tFailed to retrive Description.",ExeName);
							WriteApplicationFile("SMART",szMessage);
							StrCpy(ioDesc,"");
						}

						memset(szMessage,0,300);
						sprintf(szMessage,"INF\t%s\tStart to get Worst value.",ExeName);
						WriteApplicationFile("SMART",szMessage);
						
						ioWorstVal = 0;
						ioLen = 20;
						memset(szWorstVal,0,20);
						if(SysInfoDll_Smart_GetSmartAttributeByID (&ioWorstVal, &ioLen, nAttribID,ATTR_TYPE_INT_WORST, nDeviceID) == SMART_STATUS_SUCCESS)
						{
							memset(szMessage,0,300);
							sprintf(szMessage,"INF\t%s\tWorst value retrive successfully.Worst value is %d",ExeName,ioWorstVal);
							WriteApplicationFile("SMART",szMessage);
							sprintf(szWorstVal,"%d\0",ioWorstVal);
						}
						else
						{
							memset(szMessage,0,300);
							sprintf(szMessage,"INF\t%s\tFailed to get worst value.",ExeName);
							WriteApplicationFile("SMART",szMessage);
							sprintf(szWorstVal,"%d\0",ioWorstVal);
						}
						sprintf(szWorstVal,"%d\0",ioWorstVal);

						memset(szMessage,0,300);
						sprintf(szMessage,"INF\t%s\tStart to get raw value.",ExeName);
						WriteApplicationFile("SMART",szMessage);
						ioRawVal = 0;
						ioLen = 20;
						memset(szRawVal,0,20);
						if(SysInfoDll_Smart_GetSmartAttributeByID (&ioRawVal, &ioLen, nAttribID,ATTR_TYPE_ULONGLONG_RAW, nDeviceID) == SMART_STATUS_SUCCESS)
						{							
							//itoa(ioRawVal,szRawVal,10);
							memset(szMessage,0,300);
							sprintf(szMessage,"INF\t%s\tRaw value retrive successfully.Raw value is %I64d",ExeName,ioRawVal);
							WriteApplicationFile("SMART",szMessage);
							sprintf(szRawVal,"%I64d\0",ioRawVal);
						}
						else
						{
							memset(szMessage,0,300);
							sprintf(szMessage,"INF\t%s\tFailed to retrive raw value.",ExeName);
							WriteApplicationFile("SMART",szMessage);
						}

						memset(szMessage,0,300);
						sprintf(szMessage,"INF\t%s\tStart to get normal value.",ExeName);
						WriteApplicationFile("SMART",szMessage);

						ioNormVal = 0;
						ioLen = 20;
						memset(szNormVal,0,20);
						if(SysInfoDll_Smart_GetSmartAttributeByID (&ioNormVal, &ioLen, nAttribID,ATTR_TYPE_INT_VALUE, nDeviceID) == SMART_STATUS_SUCCESS)
						{							
							//itoa(ioNormVal,szNormVal,10);
							memset(szMessage,0,300);
							sprintf(szMessage,"INF\t%s\tNormal value retrive successfully.Normal value is %d",ExeName,ioNormVal);
							WriteApplicationFile("SMART",szMessage);
							sprintf(szNormVal,"%d\0",ioNormVal);
						}
						else
						{
							memset(szMessage,0,300);
							sprintf(szMessage,"INF\t%s\tFailed to retrive normal value.",ExeName);
							WriteApplicationFile("SMART",szMessage);
						}

						memset(szMessage,0,300);
						sprintf(szMessage,"INF\t%s\tStart to get TECByAttribute IS.",ExeName);
						WriteApplicationFile("SMART",szMessage);

						memset(szAttribID,0,30);
						sprintf(szAttribID,"%d\0",nAttribID);
						memset(szDeviceID,0,30);
						sprintf(szDeviceID,"%d\0",nDeviceID);

						time_t ioEarliest = NULL;
						time_t ioLatest = NULL;					
						
						char szEarliestFail[70] = {0};
						ioLen = 70;
						try
						{
							int iErrorCode = SysInfoDll_Smart_EstimateTECByAttributeID (&ioEarliest, &ioLatest, nAttribID, nDeviceID);
							if(iErrorCode == SMART_STATUS_SUCCESS)
							{
								memset(szMessage,0,300);
								sprintf(szMessage,"INF\t%s\tSmart TEC retrive successfully",ExeName);
								WriteApplicationFile("SMART",szMessage);
								struct tm *newtime = NULL;
								char am_pm[] = "AM\0";
								time_t val = ioEarliest;
								newtime = localtime(&val);
							
								if( newtime->tm_hour > 12 )
										strcpy( am_pm, "PM\0" );

								if(newtime->tm_hour == 12)
								{				
									WORD nSeconds = 0;
									nSeconds = newtime->tm_sec;
									if( nSeconds > 0)
										strcpy( am_pm, "PM\0" );
								}

								if( newtime->tm_hour > 12 )
										newtime->tm_hour -= 12;
								if( newtime->tm_hour == 0 )
										newtime->tm_hour = 12;


								sprintf(szEarliestFail,
								"%02d/%02d/%4d %02d:%02d:%02d %s\0",
								newtime->tm_mon + 1,
								newtime->tm_mday,
								(newtime->tm_year + 2000) - 100,
								newtime->tm_hour,
								newtime->tm_min,
								newtime->tm_sec,
								am_pm);
							}
							else
							{
								strcpy(szEarliestFail,"N.A.\0");
								memset(szMessage,0,300);
								sprintf(szMessage,"ERR\t%s\tFailed to execute function \"SysInfoDll_Smart_EstimateTECByAttributeID()\" for Attribute ID %s and Error code is %d",ExeName,szAttribID,iErrorCode);
								WriteApplicationFile("SMART",szMessage);
							}
						}
						catch(...)
						{

							memset(szMessage,0,300);
							sprintf(szMessage,"ERR\t%s\tException Occur in SysInfoDll_Smart_EstimateTECByAttributeID.",ExeName);
							WriteApplicationFile("SMART",szMessage);
			
							memset(szMessage,0,300);
							GetCurrentDateTime(szDateTime);
							sprintf(szMessage,PID_TEXT,szLRegId,"Exception Occur in SysInfoDll_Smart_EstimateTECByAttributeID.\r\n","SmartExecute",szDateTime,"Failed");
							writePIDFile(szMessage,szLRegId,"SmartExecute");
						}
						memset(szMessage,0,300);
						sprintf(szMessage,"INF\t%s\tWriting data for smart enable device.",ExeName);
						WriteApplicationFile("SMART",szMessage);

						if(FirstTimewriting == true)
						{
							WriteEndTag = true;
							//WriteToBuffer(pszDataBuffer,"<wpsmartdisknew>\r\n",FALSE);
							//Modified by Kedar
							WriteToBuffer(pszDataBuffer,"<wpsmartdisknew>\r\n",TRUE);
							WriteToBuffer(pszDataBuffer,"<regid>",FALSE);
							WriteToBuffer(pszDataBuffer,szLRegId,FALSE);
							WriteToBuffer(pszDataBuffer,"</regid>\r\n",FALSE);
							WriteToBuffer(pszDataBuffer,"<scandatetime>",FALSE);
							WriteToBuffer(pszDataBuffer,szCurrDateTime,FALSE);
							WriteToBuffer(pszDataBuffer,"</scandatetime>\r\n",FALSE);
							WriteToBuffer(pszDataBuffer,"<isvm>",FALSE);
							if(VmMachine)		//if vmware is there
							{
								WriteToBuffer(pszDataBuffer,"1",FALSE);
							}
							else				//no vmware support.
							{
								WriteToBuffer(pszDataBuffer,"0",FALSE);
							}
							WriteToBuffer(pszDataBuffer,"</isvm>\r\n",FALSE);
							FirstTimewriting = false;
						}

						WriteToBuffer(pszDataBuffer,"<diskdetail>\r\n",FALSE);	
						WriteToBuffer(pszDataBuffer,"<issmartenabled>",FALSE);
						WriteToBuffer(pszDataBuffer,"1",FALSE);
						WriteToBuffer(pszDataBuffer,"</issmartenabled>\r\n",FALSE);

						WriteToBuffer(pszDataBuffer,"<diskid>",FALSE);
						WriteToBuffer(pszDataBuffer,szDeviceID,FALSE);
						WriteToBuffer(pszDataBuffer,"</diskid>\r\n",FALSE);

						WriteToBuffer(pszDataBuffer,"<id>",FALSE);
						WriteToBuffer(pszDataBuffer,szAttribID,FALSE);
						WriteToBuffer(pszDataBuffer,"</id>\r\n",FALSE);
				
			
						WriteToBuffer(pszDataBuffer,"<description>",FALSE);
						WriteToBuffer(pszDataBuffer,ioDesc,FALSE);
						WriteToBuffer(pszDataBuffer,"</description>\r\n",FALSE);

						WriteToBuffer(pszDataBuffer,"<rawvalue>",FALSE);
						WriteToBuffer(pszDataBuffer,szRawVal,FALSE);
						WriteToBuffer(pszDataBuffer,"</rawvalue>\r\n",FALSE);

						WriteToBuffer(pszDataBuffer,"<status>",FALSE);
						WriteToBuffer(pszDataBuffer,szStatus,FALSE);
						WriteToBuffer(pszDataBuffer,"</status>\r\n",FALSE);

						WriteToBuffer(pszDataBuffer,"<value>",FALSE);
						WriteToBuffer(pszDataBuffer,szNormVal,FALSE);
						WriteToBuffer(pszDataBuffer,"</value>\r\n",FALSE);
			
						WriteToBuffer(pszDataBuffer,"<worst>",FALSE);
						WriteToBuffer(pszDataBuffer,szWorstVal,FALSE);
						WriteToBuffer(pszDataBuffer,"</worst>\r\n",FALSE);
			
						WriteToBuffer(pszDataBuffer,"<threshold>",FALSE);
						WriteToBuffer(pszDataBuffer,szThreshold,FALSE);
						WriteToBuffer(pszDataBuffer,"</threshold>\r\n",FALSE);
			
						WriteToBuffer(pszDataBuffer,"<tec>",FALSE);
						WriteToBuffer(pszDataBuffer,szEarliestFail,FALSE);
						WriteToBuffer(pszDataBuffer,"</tec>\r\n",FALSE);
						WriteToBuffer(pszDataBuffer,"</diskdetail>\r\n",FALSE);
					}

				}while((nAttribID = SysInfoDll_Smart_GetSmartAttributeIDNext(nDeviceID)) != SMART_ERR_SMART_ATTR_ID_EOL);

				nDeviceID = SysInfoDll_Smart_GetSmartDeviceIDNext();
				if(nDeviceID == SMART_ERR_SMART_DEVICE_ID_EOL)
					break;
			}
		}		

		if(WriteEndTag == true)
		{
			strcat(pszDataBuffer,"</wpsmartdisknew>\r\n");
			memset(szMessage,0,300);
			sprintf(szMessage,"INF\t%s\tWriting datacomleted.",ExeName);
			WriteApplicationFile("SMART",szMessage);
		}
		if((pszDataBuffer) && strlen(pszDataBuffer) > 0)
		{
			HANDLE hFile;
			hFile = CreateFile(szSmartDataFile,                // name of the write
						GENERIC_WRITE,          // open for writing
						0,                      // do not share
						NULL,                   // default security
						CREATE_NEW,             // create new file only
						FILE_ATTRIBUTE_NORMAL,  // normal file
						NULL);                  // no attr. template

			if (hFile == INVALID_HANDLE_VALUE) 
			{ 
				DeleteFile(szSmartDataFile);
				hFile = CreateFile(szSmartDataFile,                // name of the write
								GENERIC_WRITE,          // open for writing
								0,                      // do not share
								NULL,                   // default security
								CREATE_NEW,             // create new file only
								FILE_ATTRIBUTE_NORMAL,  // normal file
								NULL);                  // no attr. template
				if (hFile == INVALID_HANDLE_VALUE) 
				{ 
					memset(szMessage,0,300);
					sprintf(szMessage,"ERR\t%s\tFailed to create XML file.",ExeName);
					WriteApplicationFile("SMART",szMessage);
					return 0;
				}
			}
			DWORD dwBytesToWrite = (DWORD)strlen(pszDataBuffer);
			DWORD dwBytesWritten = 0;
			BOOL bErrorFlag = FALSE;
			bErrorFlag = WriteFile(hFile,           // open file handle
							pszDataBuffer,      // start of data to write
							dwBytesToWrite,  // number of bytes to write
							&dwBytesWritten, // number of bytes that were written
							NULL);  

			CloseHandle(hFile);
			int result;
			result = rename(szSmartDataFile, szAlrtDCSmartDataFile);
			DeleteFile(szSmartDataFile);

			if(pszDataBuffer!= NULL)
			{
				delete[] pszDataBuffer;
				pszDataBuffer = NULL;			
			}

			memset(szMessage,0,300);
			sprintf(szMessage,"INF\t%s\tProcess complete.",ExeName);
			WriteApplicationFile("SMART",szMessage);

			memset(szMessage,0,300);
			GetCurrentDateTime(szDateTime);
			sprintf(szMessage,PID_TEXT,szLRegId,"SUCCESS.\r\n","SmartExecute",szDateTime,"Failed");
			writePIDFile(szMessage,szLRegId,"SmartExecute");

		}
		else
		{
			memset(szMessage,0,300);
			sprintf(szMessage,"ERR\t%s\tFound no attribute below threshold value.",ExeName);
			WriteApplicationFile("SMART",szMessage);

			memset(szMessage,0,300);
			GetCurrentDateTime(szDateTime);
			sprintf(szMessage,PID_TEXT,szLRegId,"Found no attribute below threshold value.\r\n","SmartExecute",szDateTime,"SUCCESS");
			//WriteToLog(szPIDFile,szTmpBuf);
			writePIDFile(szMessage,szLRegId,"SmartExecute");
		}
	}
	catch(std::exception e)
	{
		try
		{

			memset(szMessage,0,300);
			sprintf(szMessage,"ERR\t%s\tException Occur in WinMain. %s",ExeName, e.what() );
			
			WriteApplicationFile("SMART",szMessage);

			memset(szMessage,0,300);
			GetCurrentDateTime(szDateTime);
			sprintf(szMessage,PID_TEXT,szLRegId,"Exception Occur in WinMain.\r\n","SmartExecute",szDateTime,"FAILED");
			//WriteToLog(szPIDFile,szTmpBuf);
			writePIDFile(szMessage,szLRegId,"SmartExecute");
		}
		catch(...)
		{

		}
	}
	return 0;
}

