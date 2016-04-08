// ChildProcess.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
/*
int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}

*/

// For WinXp as a target, change accordingly
#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <stdio.h>
 
void main(void)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
 
	// Start the child process.
	if (!CreateProcess(L"C:\\WINDOWS\\system32\\cmd.exe", // module name.
						NULL, // Command line.
						NULL, // Process handle not inheritable.
						NULL, // Thread handle not inheritable.
						FALSE, // Set handle inheritance to FALSE.
						0, // No creation flags.
						NULL, // Use parents environment block.
						NULL, // Use parents starting directory.
						&si, // Pointer to STARTUPINFO structure.
						&pi) // Pointer to PROCESS_INFORMATION structure.
		)
		printf("\nSorry! CreateProcess() failed.\n\n");
		else
		printf("\nWell, CreateProcess() looks OK.\n\n");
 
	// Wait until child process exits (in milliseconds). If INFINITE, the functions time-out interval never elapses except with user or other intervention.
	WaitForSingleObject(pi.hProcess, INFINITE);
	printf("\n");
	// Close process and thread handles.
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}
