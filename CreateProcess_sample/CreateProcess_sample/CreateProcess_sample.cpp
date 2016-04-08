// CreateProcess_sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


/*int _tmain(int argc, _TCHAR* argv[])
{

	return 0;
}*/

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

void _tmain( int argc, TCHAR *argv[] )
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
	char szErr[MAX_PATH]= {0};
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
	si.lpDesktop = "winsta0\\default";
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
    ZeroMemory( &pi, sizeof(pi) );

	char str[MAX_PATH + 100]={0};
	//"C:\Windows\system32\cmd.exe" /C "gpotool.exe /gpo:{31B2F340-016D-11D2-945F-00C04FB984F9}" > "C:\Agent Setup Installed Test\SAAZOD\zcmon\ApplicationLog\ZADMon\AdmonTemp_GpoTool_Default_Domain_Policy.txt"

	//strcpy(str,"\"C:\\Windows\\SysWOW64\\cmd.exe\"");
	//  C:\Agent Setup Installed Test\SAAZOD\zSCC
	strcpy(str,"C:\\Agent Setup Installed Test\\SAAZOD\\zSCC\\zscccon.exe ipconfig.ps1");
	//strcat(str," ");
	//strcat(str,"> \"D:\\Mahesh Mahajan\\Assignments\\C++\\CreateProcess_sample\\Release\\NetDom.txt\"");

	OutputDebugString(str);
	//strcat(str,"> \"C:\\Agent Setup Installed Test\\SAAZOD\\ZcMon\\NetDom.txt");

  /*  if( argc != 2 )
    {
        printf("Usage: %s [cmdline]\n", argv[0]);
        return;
    }*/
	char message[MAX_PATH] = {0};
    // Start the child process. 
    if( !CreateProcess(str,   // No module name (use command line)
        NULL,        // Command line  =>  "C:\\Program Files (x86)\\SAAZOD\\zSmart.exe 1"",
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        TRUE,          // Set handle inheritance to FALSE
        ABOVE_NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        sprintf(message,"%s%d","CreateProcess failed",GetLastError() );
		OutputDebugString(message);
		OutputDebugString("CreateProcess Failed");
        return;
    }
	DWORD err = GetLastError();
	itoa(err,szErr,10);
	
	OutputDebugString("Error Code=>");
	OutputDebugString(szErr);
    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread ); 
	OutputDebugString("CreateProcess Success");

	
   
}



