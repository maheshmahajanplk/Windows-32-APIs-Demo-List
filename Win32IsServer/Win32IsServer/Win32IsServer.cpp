// Win32IsServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Win32IsServer.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <fstream>
#include <exception>

#define MAX_LOADSTRING 100

bool IsServer()
{
	  OSVERSIONINFOEX osvi;
	  BOOL bOsVersionInfoEx;
	  ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	  std::ofstream exFile;
	  bool bServer = false;
	 

	try{
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

			if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
				return 0; //write log here...


			if ( VER_PLATFORM_WIN32_NT==osvi.dwPlatformId && osvi.dwMajorVersion > 4 )
				if( (osvi.wProductType!=VER_NT_WORKSTATION)
					&& (GetSystemMetrics(SM_SERVERR2)==0 || GetSystemMetrics(SM_SERVERR2) !=0 )
				  ) 
				{
					bServer = true;
					return bServer;
				}
				else
					return bServer;
			
	}
	catch(std::exception& e){

		 exFile.open ("Window_Version_exception.log");
		//cout << e.what() << '\n';
		 exFile <<e.what();
		 exFile.close();
		 
	}
	return 0;
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	
	std::ofstream logFile;
	logFile.open ("Window_Version.log");
	
	if(IsServer())
	{
		logFile << "Windows server version found";
	}
	else
		logFile << "Windows is not a server";

	logFile.close();

	return 0;
}




