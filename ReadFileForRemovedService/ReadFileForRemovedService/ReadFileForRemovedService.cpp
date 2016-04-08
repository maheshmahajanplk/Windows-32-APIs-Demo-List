// ReadFileForRemovedService.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<windows.h>
#include<stdio.h>
void GetDateFormatYYYYMMDD(char * szDate)
{
	int		iDateSize		=	0;
	int		iDateTimeSize	=	0;
	char	szAMPM[11]		=	{0};	

	SYSTEMTIME		sysTime;

	__try
	{
		__try
		{
			if((NULL == szDate))
			{
				return;
			}

			memset(&sysTime,NULL,sizeof(sysTime));	
			::GetLocalTime(&sysTime);

			sprintf(szDate,
					"%04d%02d%02d",
					sysTime.wYear,
					sysTime.wMonth,
					sysTime.wDay);

		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			return;
		}
	}

	__finally
	{



	}
	return;

}
void GetDateFormatYYYYMM(char * szDate)
{
	int		iDateSize		=	0;
	int		iDateTimeSize	=	0;
	char	szAMPM[11]		=	{0};	

	SYSTEMTIME		sysTime;

	__try
	{
		__try
		{
			if((NULL == szDate))
			{
				return;
			}

			memset(&sysTime,NULL,sizeof(sysTime));	
			::GetLocalTime(&sysTime);

			sprintf(szDate,
					"%04d%02d",
					sysTime.wYear,
					sysTime.wMonth);
		}

		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			return;
		}
	}

	__finally
	{

	}
	return;
}
void GetLogDateTime(char* szDateTime)
{
	SYSTEMTIME		sysTime;

	__try
	{
		__try
		{
			if(NULL == szDateTime)
			{
				return;
			}

			memset(&sysTime,NULL,sizeof(sysTime));	


			::GetLocalTime(&sysTime);

			//YYYY-MM-DD HH:MM:SS 

			sprintf(szDateTime,
					"%04d-%02d-%02d %02d:%02d:%02d",
					sysTime.wYear,
					sysTime.wMonth,
					sysTime.wDay,
					sysTime.wHour,
					sysTime.wMinute,
					sysTime.wSecond);
		}

		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			return;
		}
	}

	__finally
	{



	}
}
DWORD	CreateLogDirectory(char* szDirectoryName)
{
	int	iLen	=	0;
	int iCtr	=	0;	
	try
	{
		::SetFileAttributes(szDirectoryName,FILE_ATTRIBUTE_NORMAL);
		iLen = lstrlen(szDirectoryName);

		for(iCtr = 0; iCtr < iLen; ++iCtr)
		{
			if('\\' == szDirectoryName[iCtr])
			{
				szDirectoryName[iCtr] = NULL;
				::CreateDirectory(szDirectoryName,NULL);
				szDirectoryName[iCtr] = '\\';
			}
		}
		::CreateDirectory(szDirectoryName,NULL);
	}
	catch(...)
	{
	}
	return 0;
}
DWORD	LogDataToDisk(TCHAR *szModuleFilePath,
							TCHAR*	szData,
							BOOL bAlertData)
{
	TCHAR		szFullPathDirectory[MAX_PATH + 100]				=	{0};
	TCHAR		szFileName[MAX_PATH + 100]						=	{0};	
	TCHAR		szLogPath[MAX_PATH + 100]						=	{0};
	TCHAR		szTempLogPath[MAX_PATH + 100]					=	{0};
	TCHAR		szFullLogPath[MAX_PATH + 100]					=	{0};
	TCHAR		szMutexName[MAX_PATH + 100]						=	{0};
	TCHAR		szLogFileMapName[MAX_PATH + 100]				=	{0};
	TCHAR*		lpTraverser									=	NULL;
	TCHAR*		szFindChar									=	NULL;
	TCHAR*		pszString									=	NULL;
	TCHAR*		pszFinalError								=	NULL;
	TCHAR		chString									=	NULL;

	HANDLE		hFile										=	NULL;
	HANDLE		hMutex										=	NULL;

	DWORD		nResult										=	0;
	DWORD		dwNumOfBytesWritten							=	0;

	long		lRet										=	0;
	int			iCtr										=	0;
	int			iSubCtr										=	0;
	int			iLen										=	0;	

	BOOL		bFirstCreate								=	FALSE;

	__try
	{
		__try
		{
			if((NULL == szData)	||
				(NULL == szModuleFilePath))
			{
				return 1;
			}


			iLen = _tcslen(szModuleFilePath);
			if(0 == iLen)
			{
				return 1;
			}
			_tcscat(szLogFileMapName,_T("FILE_MAP_"));
			_tcscat(szMutexName,_T("FILE_MUTEX_"));

			for(iCtr = 0; iCtr < iLen; ++iCtr)
			{
				pszString = _tcsninc(szModuleFilePath,iCtr);	
				chString = _tcsnextc(pszString);
				if(_T('\\') == chString)
				{
					_tcscat(szLogFileMapName,_T("-"));
					_tcscat(szMutexName,_T("-"));
				}
				else
				{
					_tcsncat(szLogFileMapName,pszString,1);
					_tcsncat(szMutexName,pszString,1);
				}
			}


			//Create the Mutex and wait
			hMutex = CreateMutex(NULL,FALSE,szMutexName);
			if(NULL == hMutex)
			{
				return 1;
			}

			nResult = WaitForSingleObject(hMutex,(300 * 1000));
			if(WAIT_OBJECT_0 != nResult)
			{
				::ReleaseMutex(hMutex);
				CloseHandle(hMutex);
				hMutex = NULL;
				return 1;
			}
			

			_tcscpy(szFullLogPath,szModuleFilePath);
			//Remove the file name
			szFindChar = strrchr(szFullLogPath,'\\');
			if(NULL != szFindChar)
			{
				_tcscpy(szFileName,szFindChar);
				*szFindChar = NULL;
				//++szFindChar;
			}


			_tcscpy(szFullPathDirectory,szFullLogPath);
			_tcscat(szFullPathDirectory,szFileName);

			if(0 == ::SetFileAttributes(szFullPathDirectory,FILE_ATTRIBUTE_NORMAL))
			{
				CreateLogDirectory(szFullLogPath);
				bFirstCreate = TRUE;
			}
		
			//::SetFileAttributes(szFullPathDirectory,FILE_ATTRIBUTE_NORMAL);

			//Create or opens the existing log file
			hFile = CreateFile(szFullPathDirectory,
								GENERIC_READ | GENERIC_WRITE,
								FILE_SHARE_READ,
								NULL,
								OPEN_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
					
			if (hFile == INVALID_HANDLE_VALUE)
			{
				return 1;
			}


			
			if(TRUE == bFirstCreate)
			{
				if(FALSE == bAlertData)
				{
					::WriteFile(hFile,
								"DateTime\tApplication\tApp_Status\tMonitor\tCounter\tCounterStatus\r\n",
								lstrlen("DateTime\tApplication\tApp_Status\tMonitor\tCounter\tCounterStatus\r\n"),
								&dwNumOfBytesWritten,
								NULL);

					dwNumOfBytesWritten = 0;
				}
			}
			
			
			SetFilePointer(hFile, 0, NULL, FILE_END);

			::WriteFile(hFile,szData,lstrlen(szData),&dwNumOfBytesWritten,NULL);

			CloseHandle(hFile);
			hFile = NULL;


			::ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			hMutex = NULL;



			return 0;
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			//Its better to flush all the buffers
			//FlushFileBuffers(hFile);

			//Close the file handle
			if(NULL != hFile)
			{
				CloseHandle(hFile);
				hFile = NULL;
			}


			if(NULL != hMutex)
			{
				::ReleaseMutex(hMutex);
				CloseHandle(hMutex);
				hMutex = NULL;
			}

			return 1;
		}
	}

	__finally
	{
		//Its better to flush all the buffers
		//FlushFileBuffers(hFile);

		//Close the file handle
		if(NULL != hFile)
		{
			CloseHandle(hFile);
			hFile = NULL;
		}

		if(NULL != hMutex)
		{
			::ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			hMutex = NULL;
		}

	}
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//Get the file path first :D:\Mahesh Mahajan\Assignments\JIRA-1\zDeskApp\Debug\DataDirectory\RMM-PC-341\201501\APP\APPYYYYMMDD
	
	char*		pszReadLog						    =	NULL;
	char*		pszDataFile							=	NULL;
	char		szPort[25]							=	{0};

	char		szFinalLogFile[MAX_PATH + 100]		=	{0};
	char		szYYYYMM[20]						=	{0};
	char		szYYYYMMDD[20]						=	{0};
	char		szLogDateTime[51]					=	{0};

	__try
	{
		__try
		{
			//Put the data in the data directory
			GetDateFormatYYYYMM(szYYYYMM);
			GetDateFormatYYYYMMDD(szYYYYMMDD);
			GetLogDateTime(szLogDateTime);

			lstrcpy(szFinalLogFile,g_szModulePath);
			lstrcat(szFinalLogFile,"\\DataDirectory\\");
			lstrcat(szFinalLogFile,pszMachineName);
			lstrcat(szFinalLogFile,"\\");
			lstrcat(szFinalLogFile,szYYYYMM);
			lstrcat(szFinalLogFile,"\\APP\\APP");
			lstrcat(szFinalLogFile,szYYYYMMDD);
			lstrcat(szFinalLogFile,".log");
			
			pszReadLog = new char[1024];
		
			if(NULL == pszReadLog)
			{
				return;
			}

			memset(pszReadLog,NULL,1024);

			hFile = ::CreateFile(szFinalLogFile,
									GENERIC_READ,
									FILE_SHARE_READ,
									NULL,
									OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL,
									NULL);

			if((hFile == INVALID_HANDLE_VALUE)|| (hFile == NULL))
			{
				//Unable to open file, return from her
				hFile = NULL;
				LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								::GetLastError(),
								0,
								NULL,
								"ERR:   Open DataDirectory\\LogFile failed ",
								pszXMLFile,
								NULL);
				return 0;
			}

			dwFileSize = ::GetFileSize(hFile,NULL);

			if(0 == dwFileSize)
			{
				::CloseHandle(hFile);
				hFile = NULL;

				LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								0,
								0,
								NULL,
								"ERR:   XML file is corrupt",
								pszXMLFile,
								NULL);

				return -1;
			}

			pszBuffer	=	new char[(dwFileSize * 2) + 100];
			if(NULL == pszBuffer)
			{
				return 0;
			}
			memset(pszBuffer,NULL,(dwFileSize * 2) + 100);

			::ReadFile(hFile,pszBuffer,dwFileSize,&dwBytesRead,NULL);

			::CloseHandle(hFile);
			hFile = NULL;


		}__except(EXCEPTION_EXECUTE_HANDLER)
		{


		}
	}
	__finally
	{
		
		if(NULL != pszReadLog)
		{
			delete [] pszReadLog;
			pszReadLog = NULL;
		}
	}


	return 0;
}


