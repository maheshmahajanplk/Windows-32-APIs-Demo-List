// zExVssWRedirection.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <strsafe.h>

/*int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}*/

// Constant
#define BUFSIZE 4096
 
// Global variables for standard child read/write handle
HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;
HANDLE g_hInputFile = NULL;
 
// Function prototypes, needed for C++
void CreateChildProcess(void);
void WriteToPipe(void);
void ReadFromPipe(void);
void ErrorExit(PTSTR);
 
// wmain() is the main process...
int wmain(int argc, WCHAR *argv[])
{
		SECURITY_ATTRIBUTES saAttr;
 
		// Just some info
		wprintf(L"Start of parent execution.\n");
		wprintf(L"Parent process ID %u\n", GetCurrentProcessId());
		wprintf(L"Parent thread ID %u\n", GetCurrentThreadId());
 
		// Set the bInheritHandle flag so pipe handles are inherited
		saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
		saAttr.bInheritHandle = TRUE;
		// Just a dummy (point to NULL), we do not implement it
		saAttr.lpSecurityDescriptor = NULL;
 
		// Create a pipe for the child process's STDOUT
		if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
		ErrorExit(L"CreatePipe() - pipe for child process\'s STDOUT failed");
		else
		wprintf(L"CreatePipe() - pipe for child process\'s STDOUT pipe was created!\n");
 
		// Ensure the read handle to the pipe for STDOUT is not inherited
		if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
		ErrorExit(L"SetHandleInformation() - pipe STDOUT read handle failed for inheritance");
		else
		wprintf(L"SetHandleInformation() - pipe STDOUT read handle is not inherited!\n");
 
		// Create a pipe for the child process's STDIN
		if (! CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
		ErrorExit(L"CreatePipe() - pipe for child\'s STDIN failed");
		else
		wprintf(L"CreatePipe() - pipe for child process\'s STDIN was created!\n");
 
		// Ensure the write handle to the pipe for STDIN is not inherited
		if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
		ErrorExit(L"SetHandleInformation() - pipe STDIN write handle failed for inheritance");
		else
		wprintf(L"Stdin SetHandleInformation() - pipe STDIN read handle is not inherited!\n");
 
		// Create the child process
		wprintf(L"Verify: argv[1] = %s\n", argv[1]);
		wprintf(L"Creating the child process...\n");
		CreateChildProcess();
 
		// Get a handle to an input file for the parent. This example assumes a plain text file and uses string output to verify data flow
		if(argc == 1)
		{
		ErrorExit(L"Please specify an input file.\n");
		wprintf(L"%s [sample_text_file.txt]\n", argv[0]);
		}
 
		g_hInputFile = CreateFile(
		argv[1],
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_READONLY,
		NULL);
 
		if(g_hInputFile == INVALID_HANDLE_VALUE)
		ErrorExit(L"CreateFile()");
		else
		wprintf(L"CreateFile() - %s was successfully opened!\n", argv[1]);
 
		// Write to the pipe that is the standard input for a child process
		// Data is written to the pipe's buffers, so it is not necessary to wait until the child process is running before writing data
		WriteToPipe();
		wprintf(L" Contents of %s written to child STDIN pipe.\n", argv[1]);
 
		// Read from pipe that is the standard output for child process
		wprintf(L" Contents of %s child process STDOUT:\n", argv[1]);
		ReadFromPipe();
		wprintf(L" End of parent execution.\n");
 
		// The remaining open handles are cleaned up when this process terminates
		// To avoid resource leaks in a larger application, close handles explicitly
		return 0;
}
 
// Create a child process that uses the previously created pipes for STDIN and STDOUT
void CreateChildProcess()
{
		// The following should be the child executable, see the next program example
		// Change the path accordingly...
		WCHAR szCmdline[]=L"\\\\?\\C:\\Users\\mahesh.mahajan\\Documents\\visual studio 2010\\Projects\\ChildProcess\\Debug\\ChildProcess.exe"; //  "\\\\?\\C:\\amad\\ChildProcess\\Debug\\ChildProcess.exe"
		PROCESS_INFORMATION piProcInfo;
		STARTUPINFO siStartInfo;
		BOOL bSuccess = FALSE;
 
		// Set up members of the PROCESS_INFORMATION structure
		ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
 
		// Set up members of the STARTUPINFO structure. This structure specifies the STDIN and STDOUT handles for redirection
		ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
		siStartInfo.cb = sizeof(STARTUPINFO);
		siStartInfo.hStdError = g_hChildStd_OUT_Wr;
		siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
		siStartInfo.hStdInput = g_hChildStd_IN_Rd;
		siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
 
		// Create the child process
		bSuccess = CreateProcess(NULL, // Use szCmdLine
		szCmdline, // command line
		NULL, // process security attributes
		NULL, // primary thread security attributes
		TRUE, // handles are inherited
		0, // creation flags
		NULL, // use parent's environment
		NULL, // use parent's current directory
		&siStartInfo, // STARTUPINFO pointer
		&piProcInfo); // receives PROCESS_INFORMATION
 
		// If an error occurs, exit the application
		if (!bSuccess)
			ErrorExit(L"CreateProcess() - child");
		else
		{
			wprintf(L"\nChild process ID is: %u\n", GetCurrentProcessId());
			wprintf(L"Child thread ID is: %u\n", GetCurrentThreadId());
 
		// Close handles to the child process and its primary thread. Some applications might keep these handles to monitor the status of the child process, for example
			if(CloseHandle(piProcInfo.hProcess) != 0)
				wprintf(L"piProcInfo.hProcess handle was closed!\n");
			else
				ErrorExit(L"CloseHandle(piProcInfo.hProcess)");
 
			if(CloseHandle(piProcInfo.hThread) != 0)
				wprintf(L"piProcInfo.hThread handle was closed!\n");	
			else
				ErrorExit(L"CloseHandle(piProcInfo.hThread)");
		}
}
 
// Read from a file and write its contents to the pipe for the child's STDIN. Stop when there is no more data
void WriteToPipe(void)
{
		DWORD dwRead, dwWritten;
		CHAR chBuf[BUFSIZE];
		BOOL bSuccess = FALSE;
 
		for(;;)
		{
			bSuccess = ReadFile(g_hInputFile, chBuf, BUFSIZE, &dwRead, NULL);
			if (! bSuccess || dwRead == 0)
			{
				// Cannot use ErrorExit() lol
				wprintf(L"\nReadFile() - Failed to read file! Error %u\n", GetLastError());
				break;
			}
			else
				wprintf(L"\nReadFile() - Reading from a file...\n");
 
			bSuccess = WriteFile(g_hChildStd_IN_Wr, chBuf, dwRead, &dwWritten, NULL);
			if (!bSuccess)
			{	
				wprintf(L"\nWriteFile() - Failed to write to pipe for child\'s STDIN! Error %u\n", GetLastError());
				break;
			}
			else
				wprintf(L"\nWriteFile() - writing to the pipe for the child\'s STDIN...\n");
		}
 
		// Close the pipe handle so the child process stops reading
		if (!CloseHandle(g_hChildStd_IN_Wr))
			ErrorExit(L"CloseHandle()");
		else
			wprintf(L"Closing the pipe handle...\n");
}
 
// Read output from the child process's pipe for STDOUT and write to the parent process's pipe for STDOUT. Stop when there is no more data.
void ReadFromPipe(void)
{
		DWORD dwRead, dwWritten;
		WCHAR chBuf[BUFSIZE];
		BOOL bSuccess = FALSE;
		HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
 
		// Close the write end of the pipe before reading from the read end of the pipe, to control child process execution.
		// The pipe is assumed to have enough buffer space to hold the data the child process has already written to it
		if (!CloseHandle(g_hChildStd_OUT_Wr))
			ErrorExit(L"CloseHandle() - ReadFromPipe()");
		for(;;)
		{
			bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
			if(!bSuccess || dwRead == 0)
			{
				wprintf(L"\nReadFile() from child's standard output failed! Error %u\n", GetLastError());
				break;
			}
			else
			{
				wprintf(L"\nReadFile() from child's standard output is OK!\n");
			}
 
			bSuccess = WriteFile(hParentStdOut, chBuf, dwRead, &dwWritten, NULL);
			if(!bSuccess)
			{
				wprintf(L"\nWriteFile() to parent's standard output failed! Error %u\n", GetLastError());
				break;
			}
			else
			{
				wprintf(L"\nWriteFile() to parent's standard output is OK!\n");
			}
		}
}
 
// Format a readable error message, display a message box, and exit from the application.
void ErrorExit(PTSTR lpszFunction)
{
		LPVOID lpMsgBuf;
		LPVOID lpDisplayBuf;
		DWORD dw = GetLastError();
 
		FormatMessage(
						FORMAT_MESSAGE_ALLOCATE_BUFFER |
						FORMAT_MESSAGE_FROM_SYSTEM |
						FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						dw,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						(LPTSTR) &lpMsgBuf,
						0, NULL );
 
		lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(WCHAR));
		StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(WCHAR), L"%s failed with error %d: %s", lpszFunction, dw, lpMsgBuf);
		MessageBox(NULL, (LPCTSTR)lpDisplayBuf, L"Error", MB_OK);
		LocalFree(lpMsgBuf);
		LocalFree(lpDisplayBuf);
		ExitProcess(1);
}

