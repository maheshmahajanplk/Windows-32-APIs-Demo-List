// Win32WinVersion.cpp : Defines the entry point for the application.
//




#include "stdafx.h"
#include "Win32WinVersion.h"
#include <windows.h>
#include <stdio.h>
#include <fstream>
///#include <VersionHelpers.h>

#pragma comment(lib, "User32.lib")


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	OSVERSIONINFOEX osvi;
	int version=0;
	std::ofstream logFile;
	
	logFile.open ("Window_Version.log");

	Sleep(30*1000);

	 ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	 osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	 GetVersionEx((OSVERSIONINFO *)&osvi);

	 version = (osvi.dwMajorVersion)*10 + osvi.dwMinorVersion;

	 switch(version)
	 {
	 case 100:	if(osvi.wProductType != VER_NT_WORKSTATION)
					logFile <<"Windows Server Technical preview";
				else
					logFile <<"Not Server : Windows 10 Insider Preview ";
				break;
	 case 63:  
				if(osvi.wProductType != VER_NT_WORKSTATION)
					logFile <<"Windows Server 2012 R2";
				else
					logFile <<"Not Server : Windows 8.1";
		    	break; 
	case 62:
				if(osvi.wProductType != VER_NT_WORKSTATION)
					logFile <<"Windows Server 2012";
				else
					logFile <<"Not Server : Windows 8";
		    	break;
	case 61:
				if(osvi.wProductType != VER_NT_WORKSTATION)
					logFile <<"Windows Server 2008 R2";
				else
					logFile <<"Not Server : Windows 7 ";
		    	break;
				
	 case 60:
				if(osvi.wProductType != VER_NT_WORKSTATION)
					logFile <<"Windows Server 2008";
				else
					logFile <<"Not Server : Windows Vista";
		    	break;
		 
	 case 52:
				if(GetSystemMetrics(SM_SERVERR2)!=0)
					logFile <<"Windows Server 2003 R2";
				else if(GetSystemMetrics(SM_SERVERR2)==0)
					logFile <<"Windows Server 2003";
				else if(osvi.wSuiteMask & VER_SUITE_WH_SERVER)
					logFile <<"Windows Home Server";
				else
					logFile <<"Version got 5.2:Not Matched";
		    	break;
		 
	 default: logFile <<"Version is not matched";
	 
	 }

	 logFile <<"\nVersion we got is:"<<version;
	
	 logFile.close();

	 return 0;
}

