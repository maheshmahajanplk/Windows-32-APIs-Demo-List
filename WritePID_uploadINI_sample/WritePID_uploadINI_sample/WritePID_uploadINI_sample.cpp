// WritePID_uploadINI_sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<windows.h>

char g_szModulePath[MAX_PATH]       = {""};
char g_szSAAZMobilePath[MAX_PATH]   = {""};
char g_szInternalStatusINI[MAX_PATH]= {""};

int	 WritePID(char* szMachineName, char* szRegId, char* szDescription);

int _tmain(int argc, _TCHAR* argv[])
{
	::GetModuleFileName(NULL,g_szModulePath,MAX_PATH);

	lstrcpy(g_szSAAZMobilePath,g_szModulePath);
	lstrcat(g_szSAAZMobilePath,"\\SAAZMobile.ini");

	lstrcpy(g_szInternalStatusINI,g_szModulePath);
	lstrcat(g_szInternalStatusINI,"\\Configuration\\App\\InternalStatus.ini");

	WritePID("RMM-PC-194","2831259","Description");

	return 0;
}

DWORD	GetAlertDateTime(char* szDateTime)
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
				strcpy( am_pm, "PM\0" );
			if(sysTime.wHour == 12)
			{				
				//WORD nSeconds = sysTime.wSecond;
				//if( nSeconds > 0 || sysTime.wMinute > 0)
					strcpy( am_pm, "PM\0" );
			}
			if( sysTime.wHour > 12 )        // Convert from 24-hour to 12-hour clock.
				sysTime.wHour -= 12;		
			if( sysTime.wHour == 0 )        // Set hour to 12 if midnight.
				sysTime.wHour = 12;

			sprintf(szDateTime,
					"%02d/%02d/%04d %02d:%02d:%02d %s\0",
					sysTime.wMonth,
					sysTime.wDay,
					sysTime.wYear,
					sysTime.wHour,
					sysTime.wMinute,
					sysTime.wSecond,
					am_pm);
		}

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

int		WritePID(char* szMachineName, char* szRegId, char* szDescription)
{
	char	szUploadPath[MAX_PATH + 100]		=	{0};
	char	szUploadDir[MAX_PATH + 100]			=	{0};
	char	szPath[MAX_PATH + 100]				=	{0};
	char	szINIFileName[MAX_PATH + 100]		=	{0};
	char	szLastRunDateTime[MAX_PATH + 100]	=	{0};
	char	szDef[10]							=	{0};
	char*	pszPIDDesc							=	NULL;


	DWORD	dwBufferSize						=	0;
	DWORD	dwNumOfBytesWritten					=	0;

	HANDLE	hFile								=	NULL;

	__try
	{
		__try
		{
			if(0 == ::GetPrivateProfileInt(szMachineName,
											"Status",
											1,
											g_szInternalStatusINI))
			{
				return 0;
			}
			else
			{
				::WritePrivateProfileString(szMachineName,
											"Status",
											"0",
											g_szInternalStatusINI);
			}

			::GetPrivateProfileString("SAAZFolders",
										"UploadFolder",
										szDef,
										szPath,
										(MAX_PATH + 99),
										g_szSAAZMobilePath);

			if(NULL == szPath[0])
			{
				lstrcpy(szPath,"\\PMSTATUS");
			}

			lstrcpy(szUploadPath,g_szModulePath);
			lstrcat(szUploadPath,szPath);
			GetTempFileName(szINIFileName);
			
			lstrcat(szUploadPath,"\\");
			lstrcat(szUploadPath,szINIFileName);
			lstrcat(szUploadPath,".ini");

			//Write the last run date time
			GetAlertDateTime(szLastRunDateTime);

			pszPIDDesc = new char[1024];
			if(NULL == pszPIDDesc)
			{
				return 0;
			}

			memset(pszPIDDesc,NULL,1024);

			sprintf(pszPIDDesc,
					"[STATUS_INFO]\r\nREGID=%s\r\nDesc=%s\r\nPID=Application Monitor\r\nSysID=\r\nLastRunDateTime=%s\r\nNextRunDateTime=\r\nLastRunStatusCode=%s\r\n",
					szRegId,
					szDescription,
					szLastRunDateTime,
					"FAILED");

			dwBufferSize = lstrlen(pszPIDDesc);
			

			hFile = CreateFile(szUploadPath,
								GENERIC_READ | GENERIC_WRITE,
								FILE_SHARE_READ,
								NULL,
								OPEN_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
					
			if((hFile == INVALID_HANDLE_VALUE)		||
				(hFile == NULL))
			{
				//Unable to open file, return from her
				hFile = NULL;

				lstrcpy(szUploadDir,g_szModulePath);
				lstrcat(szUploadDir,szPath);
				::CreateDirectory(szUploadDir,NULL);


				hFile = CreateFile(szUploadPath,
									GENERIC_READ | GENERIC_WRITE,
									FILE_SHARE_READ,
									NULL,
									OPEN_ALWAYS,
									FILE_ATTRIBUTE_NORMAL,
									NULL);
						
				if((hFile == INVALID_HANDLE_VALUE)		||
					(hFile == NULL))
				{
					//Unable to open file, return from her
					hFile = NULL;
					return 1;
				}
			}

			
			::WriteFile(hFile,pszPIDDesc,dwBufferSize,&dwNumOfBytesWritten,NULL);

			::CloseHandle(hFile);
			hFile = NULL;

			delete [] pszPIDDesc;
			pszPIDDesc = NULL;

			return 0;
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{

		}
	}

	__finally
	{
		if(NULL != pszPIDDesc)
		{
			delete [] pszPIDDesc;
			pszPIDDesc = NULL;
		}

		if(NULL != hFile)
		{
			::CloseHandle(hFile);
			hFile = NULL;
		}
	}
	return 0;
}
