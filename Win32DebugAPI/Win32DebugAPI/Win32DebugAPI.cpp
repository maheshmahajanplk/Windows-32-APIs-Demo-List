#include <Windows.h>
#include <Dbghelp.h>
#include <iostream>
#include <tchar.h>
#include <strsafe.h>
#include <shlobj.h>

#pragma comment(lib, "DbgHelp")
#define MAX_BUFF_SIZE 1024

using namespace std;
DWORD WINAPI func();

void make_minidump(EXCEPTION_POINTERS* e)
{
	TCHAR tszFileName[MAX_BUFF_SIZE] = {0};
	TCHAR tszPath[MAX_BUFF_SIZE] = {0};
	SYSTEMTIME stTime = {0};
	GetSystemTime(&stTime);
	SHGetSpecialFolderPath(NULL,tszPath, CSIDL_APPDATA, FALSE);
	StringCbPrintf(tszFileName, 
			       _countof(tszFileName), 
				   _T("%s\\%s__%4d%02d%02d_%02d%02d%02d.dmp"), 
				   tszPath, _T("CrashDump"),  
				   stTime.wYear, 
				   stTime.wMonth, 
				   stTime.wDay, 
				   stTime.wHour, 
				   stTime.wMinute, 
				   stTime.wSecond);

    HANDLE hFile = CreateFile(tszFileName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if(hFile == INVALID_HANDLE_VALUE)
        return;

    MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
    exceptionInfo.ThreadId = GetCurrentThreadId();
    exceptionInfo.ExceptionPointers = e;
    exceptionInfo.ClientPointers = FALSE;

    MiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        hFile,
        MINIDUMP_TYPE(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory | MiniDumpWithFullMemory),
        e ? &exceptionInfo : NULL,
        NULL,
        NULL);

	if(hFile)
	{
		CloseHandle(hFile);
		hFile = NULL;
	}
    return; 
}

LONG CALLBACK unhandled_handler(EXCEPTION_POINTERS* e)
{
    make_minidump(e);
    return EXCEPTION_CONTINUE_SEARCH;
}

int main()
{
    SetUnhandledExceptionFilter( unhandled_handler);
	DWORD dwThreadID = 0;
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, NULL, 0, &dwThreadID);
	if(hThread == NULL)
	{
		return 1;
	}
	WaitForSingleObject(hThread, INFINITE);
	return 0;
    
}

DWORD WINAPI func()
{
	int *p = NULL;
	*p = 10;
	return 0;
}
