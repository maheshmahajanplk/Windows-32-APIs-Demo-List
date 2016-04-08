// LogLastError.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "LogLastError.h"
#include "stdio.h"
#include "stdlib.h"

/********************Globals******************************************/

const	char					g_szRecDelimiter[]			=	"\r\n";
const	char					g_szFieldDelimiter[]		=	"\t";

/*--------------------------------------------------------------------*/

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

DWORD	LogErrorToDisk(TCHAR *szModuleFilePath,
							TCHAR* szErrorString)
{
	TCHAR		szFullPathDirectory[MAX_PATH + 100]			=	{0};
	TCHAR		szFileName[MAX_PATH + 100]					=	{0};	
	TCHAR		szLogPath[MAX_PATH + 100]					=	{0};
	TCHAR		szTempLogPath[MAX_PATH + 100]				=	{0};
	TCHAR		szFullLogPath[MAX_PATH + 100]				=	{0};
	TCHAR		szMutexName[MAX_PATH + 100]					=	{0};
	TCHAR		szBuffer[51]								=	{0};
	TCHAR*		szFindChar									=	NULL;
	TCHAR*		pszString									=	NULL;
	TCHAR		chString									=	NULL;


	HANDLE		hFile										=	NULL;
	HANDLE		hMutex										=	NULL;
	HANDLE		hWaitHandle[2]								=	{0};					
	
	DWORD		nResult										=	0;
	DWORD		dwFileSizeLow								=	0;
	DWORD		dwFileSizeHigh								=	0;
	DWORD		dwBufferSize								=	0;
	DWORD		dwSize										=	0;
	DWORD		dwNumOfBytesWritten							=	0;

	long		lRet										=	0;
	int			iCtr										=	0;
	int			iLen										=	0;	

	BOOL		bln											=	FALSE;
	BOOL		bMutexFail									=	FALSE;

	__try
	{
		__try
		{
			//First check for the ErrorString and the Module Path
			if((NULL == szErrorString)	||
				(NULL == szModuleFilePath))
			{
				return 1;
			}


			//Get the length of the module path
			iLen = lstrlen(szModuleFilePath);
			if(0 == iLen)
			{
				return 1;
			}

			//Replace the Slashes with the '-' for the mutex, since
			//its the restriction placed by the Mutex
			for(iCtr = 0; iCtr < iLen; ++iCtr)
			{
				if('\\' == szModuleFilePath[iCtr])
				{
					szMutexName[iCtr] = '-';
				}
				else
				{
					szMutexName[iCtr] = szModuleFilePath[iCtr];
				}
			}


			//Create the Mutex and wait
			hMutex = CreateMutex(NULL,FALSE,szMutexName);
			if(NULL == hMutex)
			{
				return 1;
			}
			
			//Wait for the mutex atleast 5 Minutes
			nResult = WaitForSingleObject(hMutex,(300 * 1000));
			if(WAIT_OBJECT_0 != nResult)
			{
				//Unable to get Mutex, return from here
				::ReleaseMutex(hMutex);
				CloseHandle(hMutex);
				hMutex = NULL;
				return 1;
			}
			

			lstrcpy(szFullLogPath,szModuleFilePath);

			//Remove the file name
			szFindChar = strrchr(szFullLogPath,'\\');
			if(NULL != szFindChar)
			{
				lstrcpy(szFileName,szFindChar);
				*szFindChar = NULL;
			}

			//Create the full path here
			lstrcpy(szFullPathDirectory,szFullLogPath);
			lstrcat(szFullPathDirectory,szFileName);

			if(0 == ::SetFileAttributes(szFullPathDirectory,FILE_ATTRIBUTE_NORMAL))
			{
				CreateLogDirectory(szFullLogPath);
			}

			//Create respective directories and sub directories
			//CreateRecursiveDirectory(szFullLogPath);

			//Get the length of the error string
			dwBufferSize = lstrlen(szErrorString);


			//Set the file attributes to normal
			//::SetFileAttributes(szFullPathDirectory,FILE_ATTRIBUTE_NORMAL);

			//Create or opens the existing log file
			hFile = CreateFile(szFullPathDirectory,
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
			
			//Get the log file size
			dwFileSizeLow = GetFileSize(hFile,&dwFileSizeHigh);

			if(dwFileSizeLow > (1024 * 1024))
			{
				//If the log file size is more than 1 MB, need to delete the log file

				//Close all the previous file handles
				::CloseHandle(hFile);
				hFile = NULL;

				//Set the file attributes to normal
				::SetFileAttributes(szFullPathDirectory,FILE_ATTRIBUTE_NORMAL);

				//Delete the log file
				::DeleteFile(szFullPathDirectory);


				//Create or opens the existing log file
				hFile = CreateFile(szFullPathDirectory,
									GENERIC_READ | GENERIC_WRITE,
									FILE_SHARE_READ,
									NULL,
									OPEN_ALWAYS,
									FILE_ATTRIBUTE_NORMAL,
									NULL);
						
				if((hFile == INVALID_HANDLE_VALUE)		||
					(hFile == NULL))
				{
					//Unable to open file for writting, return from here
					hFile = NULL;
					return 1;
				}
				
				//Get the log file size
				dwNumOfBytesWritten =	0;
				dwFileSizeLow		=	0;	
			}

			if(0 != dwFileSizeLow)
			{
				//Set the file pointer to the last offset
				SetFilePointer(hFile, 0, NULL, FILE_END);
			}

			dwNumOfBytesWritten = 0;
			//Write the data to the file
			::WriteFile(hFile,szErrorString,dwBufferSize,&dwNumOfBytesWritten,NULL);

			//Close the file handle
			CloseHandle(hFile);
			hFile = NULL;


			//Release the mutex now
			::ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			hMutex = NULL;



			return 0;
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			//Close the file handle
			if(NULL != hFile)
			{
				CloseHandle(hFile);
				hFile = NULL;
			}


			//Release the mutex now
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
		//Close the file handle
		if(NULL != hFile)
		{
			CloseHandle(hFile);
			hFile = NULL;
		}

		//Release the nutex now
		if(NULL != hMutex)
		{
			::ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			hMutex = NULL;
		}
	}
	return 0;
}
DWORD	GetDateTime(char*	szDate, char* szDateTime)
{
	int		iDateSize		=	0;
	int		iDateTimeSize	=	0;
	char	szAMPM[11]		=	{0};	

	SYSTEMTIME		sysTime;

	__try
	{
		__try
		{
			if((NULL == szDate)	||
				(NULL == szDateTime))
			{
				return 1;
			}

			memset(&sysTime,NULL,sizeof(sysTime));	


			::GetLocalTime(&sysTime);



			sprintf(szDateTime,
					"%04d/%02d/%02d %02d:%02d:%02d",
					sysTime.wYear,
					sysTime.wMonth,
					sysTime.wDay,
					sysTime.wHour,
					sysTime.wMinute,
					sysTime.wSecond);

			sprintf(szDate,
					"%04d%02d%02d",
					sysTime.wYear,
					sysTime.wMonth,
					sysTime.wDay);

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

int	LogLastError(char*	szFilePath,
				 long	llastLineCode,
				 char*	szModuleCode,
				 long	lSystemCode,
				 long	lMiscCode,
				 char*	pszModuleErrorString,
				 char*	pAddInfo1,
				 char*	pAddInfo2,
				 char*	pAddInfo3)
{
	char*	pszFinalErrorString							=	NULL;
	LPVOID	pMsgBuf										=	NULL;

	char*	pchBackChar									=	NULL;	
	char*	pszFindChar									=	NULL;

	char	szFinalErrLogFile[MAX_PATH + 100]			=	{0};
	char	szDateTime[101]								=	{0};
	char	szDate[101]									=	{0};

	char	szLineCode[25]								=	{0};
	char	szSystemCode[25]							=	{0};
	char	szMiscCode[25]								=	{0};

	int		iReqMemAllocated							=	0;
	__try
	{
		__try
		{
			//prepare the error file
			lstrcpy(szFinalErrLogFile,szFilePath);

			memset(szDate,NULL,25);
			memset(szDateTime,NULL,25);

			GetDateTime(szDate,szDateTime);

			ltoa(llastLineCode,szLineCode,10);
			ltoa(lSystemCode,szSystemCode,10);
			ltoa(lMiscCode,szMiscCode,10);



			//calculate the memory requirements
			//szDateTime
			iReqMemAllocated	+=	30;
			iReqMemAllocated	+=	5;	
			//llastLineCode   
			iReqMemAllocated	+=	30;
			iReqMemAllocated	+=	5;	
			//lModuleCode
			iReqMemAllocated	+=	30;
			iReqMemAllocated	+=	5;	
			//lSystemCode
			iReqMemAllocated	+=	30;
			iReqMemAllocated	+=	5;	
			//lMiscCode
			iReqMemAllocated	+=	30;
			iReqMemAllocated	+=	5;	



			if(NULL != pszModuleErrorString)
			{
				iReqMemAllocated	+= lstrlen(pszModuleErrorString);
				iReqMemAllocated	+=	5;	
			}

			if(NULL != pAddInfo1)
			{
				iReqMemAllocated	+= lstrlen(pAddInfo1);
				iReqMemAllocated	+=	5;	
			}

			if(NULL != pAddInfo2)
			{
				iReqMemAllocated	+= lstrlen(pAddInfo2);
				iReqMemAllocated	+=	5;	
			}

			if(NULL != pAddInfo3)
			{
				iReqMemAllocated	+= lstrlen(pAddInfo3);
				iReqMemAllocated	+=	5;	
			}

			if(0 != lSystemCode)
			{
				//Chek out for any last error memory req
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER	| 
								FORMAT_MESSAGE_FROM_SYSTEM		| 
								FORMAT_MESSAGE_IGNORE_INSERTS,
								NULL,
								lSystemCode,
								MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
								(LPTSTR) &pMsgBuf,
								0,
								NULL);

				if(NULL != pMsgBuf)
				{
					iReqMemAllocated	+= lstrlen((char*)pMsgBuf);
					iReqMemAllocated	+=	5;	

					pchBackChar = strrchr((LPTSTR) pMsgBuf,'\n');
					if(NULL != pchBackChar)
					{
						*pchBackChar = NULL;
					}

					pchBackChar = strrchr((LPTSTR) pMsgBuf,'\r');
					if(NULL != pchBackChar)
					{
						*pchBackChar = NULL;
					}
				}
			}

			iReqMemAllocated	+=	151;

			pszFinalErrorString	=	new char[iReqMemAllocated];
			if(NULL == pszFinalErrorString)
			{
				return 1;
			}

			memset(pszFinalErrorString,NULL,iReqMemAllocated);
			//Copy the whole strings
			lstrcat(pszFinalErrorString,szDateTime);
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);
			lstrcat(pszFinalErrorString,szLineCode);
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);
			if(NULL != szModuleCode)
			{
				lstrcat(pszFinalErrorString,szModuleCode);
			}
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);
			lstrcat(pszFinalErrorString,szSystemCode);
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);
			lstrcat(pszFinalErrorString,szMiscCode);
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);

			if(NULL != pszModuleErrorString)
			{
				lstrcat(pszFinalErrorString,pszModuleErrorString);
			}
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);

			if(NULL != pMsgBuf)
			{
				lstrcat(pszFinalErrorString,(char*)pMsgBuf);
			}
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);
			

			if(NULL != pAddInfo1)
			{
				lstrcat(pszFinalErrorString,pAddInfo1);
			}
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);

			if(NULL != pAddInfo2)
			{
				lstrcat(pszFinalErrorString,pAddInfo2);
			}
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);

			if(NULL != pAddInfo3)
			{
				lstrcat(pszFinalErrorString,pAddInfo3);
			}
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);

			lstrcat(pszFinalErrorString,g_szRecDelimiter);


			//Log the Error

			LogErrorToDisk(szFinalErrLogFile,pszFinalErrorString);


		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			return 1;
		}
	}
	__finally
	{
		delete [] pszFinalErrorString;
		pszFinalErrorString = NULL;


		if(NULL != pMsgBuf)
		{
			LocalFree(pMsgBuf);
			pMsgBuf = NULL;
		}
	}
	return 0;	
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
		//strcpy(mmCurentDir,SAAZODSetUpPath);
		lstrcat(mmCurentDir ,"ApplicationLog\\");
		//strcat(mmCurentDir , "\\Log");

		GetLocalTime(&systime);

		sprintf(mdate  , "%04d-%02d-%02d" ,systime.wYear, systime.wDay , systime.wMonth );
		sprintf(mmdate , "%04d%02d%02d" ,systime.wYear, systime.wDay , systime.wMonth);
		//GetTimeFormat(LOCALE_SYSTEM_DEFAULT , NULL , &systime , "hh':'mm':'ss" , mtime , 255);
		sprintf(mtime , "%02d:%02d:%02d" , systime.wHour, systime.wMinute , systime.wSecond);
		sprintf(mmFileName , "%s\\%s.Log" , mmCurentDir , mappname);
		FILE *fp=NULL;

		if(IsDirExist(mmFileName) == false)
		{
			isFileExist = false;
			fp=fopen( mmFileName ,"w+"); //for wiping out the contents
			_fcloseall( );
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
			fp=fopen( mmFileName ,"w+"); //For wiping out the contents
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



int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	


	return 0;
}



