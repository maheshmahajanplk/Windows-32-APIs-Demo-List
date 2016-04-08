// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <string.h>
#include <aclapi.h>
#include<conio.h>
#include<atlbase.h>
#include<stdlib.h>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#pragma warning(disable: 4996)

DWORD LogDataToDisk(TCHAR *szModuleFilePath, TCHAR *szData, BOOL bAlertData);
DWORD CreateLogDirectory(char* szDirectoryName);

int _tmain(int argc, _TCHAR* argv[])
{
 TCHAR filepath[256];
 TCHAR *content = "This is log content";
 strcpy(filepath,"getlost\\autoreboot.log");
 //strcpy(a, "whatsapp");
 printf("hello world");
 LogDataToDisk(filepath, content, TRUE);

 return 0;
}


DWORD LogDataToDisk(TCHAR *szModuleFilePath, TCHAR* szData,  BOOL bAlertData)
{
 TCHAR  szFullPathDirectory[MAX_PATH + 100] = { 0 };
 TCHAR  szFileName[MAX_PATH + 100] = { 0 };
 TCHAR  szLogPath[MAX_PATH + 100] = { 0 };
 TCHAR  szTempLogPath[MAX_PATH + 100] = { 0 };
 TCHAR  szFullLogPath[MAX_PATH + 100] = { 0 };
 TCHAR  szMutexName[MAX_PATH + 100] = { 0 };
 TCHAR  szLogFileMapName[MAX_PATH + 100] = { 0 };
 TCHAR*  lpTraverser = NULL;
 TCHAR*  szFindChar = NULL;
 TCHAR*  pszString = NULL;
 TCHAR*  pszFinalError = NULL;
 TCHAR  chString = NULL;

 HANDLE  hFile = NULL;
 HANDLE  hMutex = NULL;

 DWORD  nResult = 0;
 DWORD  dwNumOfBytesWritten = 0;

 long  lRet = 0;
 int   iCtr = 0;
 int   iSubCtr = 0;
 int   iLen = 0;

 BOOL  bFirstCreate = FALSE;

 __try
 {
  __try
  {
   if ((NULL == szData) ||
    (NULL == szModuleFilePath))
   {
    return 1;
   }


   iLen = _tcslen(szModuleFilePath);
   if (0 == iLen)
   {
    return 1;
   }
   _tcscat(szLogFileMapName, _T("FILE_MAP_"));
   _tcscat(szMutexName, _T("FILE_MUTEX_"));

   for (iCtr = 0; iCtr < iLen; ++iCtr)
   {
    pszString = _tcsninc(szModuleFilePath, iCtr);
    chString = _tcsnextc(pszString);
    if (_T('\\') == chString)
    {
     _tcscat(szLogFileMapName, _T("-"));
     _tcscat(szMutexName, _T("-"));
    }
    else
    {
     _tcsncat(szLogFileMapName, pszString, 1);
     _tcsncat(szMutexName, pszString, 1);
    }
   }


   //Create the Mutex and wait
   hMutex = CreateMutex(NULL, FALSE, szMutexName);
   if (NULL == hMutex)
   {
    return 1;
   }

   nResult = WaitForSingleObject(hMutex, (300 * 1000));
   if (WAIT_OBJECT_0 != nResult)
   {
    ::ReleaseMutex(hMutex);
    CloseHandle(hMutex);
    hMutex = NULL;
    return 1;
   }


   _tcscpy(szFullLogPath, szModuleFilePath);
   //Remove the file name
   szFindChar = strrchr(szFullLogPath, '\\');
   if (NULL != szFindChar)
   {
    _tcscpy(szFileName, szFindChar);
    *szFindChar = NULL;
    //++szFindChar;
   }


   _tcscpy(szFullPathDirectory, szFullLogPath);
   _tcscat(szFullPathDirectory, szFileName);

   if (0 == ::SetFileAttributes(szFullPathDirectory, FILE_ATTRIBUTE_NORMAL))
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



   if (TRUE == bFirstCreate)
   {
    if (FALSE == bAlertData)
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

   ::WriteFile(hFile, szData, lstrlen(szData), &dwNumOfBytesWritten, NULL);

   CloseHandle(hFile);
   hFile = NULL;


   ::ReleaseMutex(hMutex);
   CloseHandle(hMutex);
   hMutex = NULL;



   return 0;
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
   //Its better to flush all the buffers
   //FlushFileBuffers(hFile);

   //Close the file handle
   if (NULL != hFile)
   {
    CloseHandle(hFile);
    hFile = NULL;
   }


   if (NULL != hMutex)
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
  if (NULL != hFile)
  {
   CloseHandle(hFile);
   hFile = NULL;
  }

  if (NULL != hMutex)
  {
   ::ReleaseMutex(hMutex);
   CloseHandle(hMutex);
   hMutex = NULL;
  }

 }
 return 0;
}

DWORD CreateLogDirectory(char* szDirectoryName)
{
 int iLen = 0;
 int iCtr = 0;
 try
 {
  ::SetFileAttributes(szDirectoryName, FILE_ATTRIBUTE_NORMAL);
  iLen = lstrlen(szDirectoryName);

  for (iCtr = 0; iCtr < iLen; ++iCtr)
  {
   if ('\\' == szDirectoryName[iCtr])
   {
    szDirectoryName[iCtr] = NULL;
    ::CreateDirectory(szDirectoryName, NULL);
    szDirectoryName[iCtr] = '\\';
   }
  }
  ::CreateDirectory(szDirectoryName, NULL);
 }
 catch (...)
 {
 }
 return 0;
}