// WideCharToMultiByte.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "WideCharToMultiByte.h"
#include "SysInfo.h"

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	wchar_t ioDescW[200]= L"CONTINUUM MANAGED SERVICES TRANSFORMED";
	char ioDesc[200] = {0};

	WideCharToMultiByte(CP_ACP,
						0,
						ioDescW,
						lstrlenW(ioDescW)+1,
						ioDesc,
						200,
						NULL,
						NULL);
						

	return 0;
}



