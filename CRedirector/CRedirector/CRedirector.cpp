// CRedirector.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Redir.h"
#include<Windows.h>



int _tmain(int argc, _TCHAR* argv[])
{

	char WindowsDir[MAX_PATH+ 100]={0};
	char VssAdminExe[MAX_PATH+ 100]={0};

	CRedirector obj;
	GetWindowsDirectory(WindowsDir,MAX_PATH+ 100);
	sprintf(VssAdminExe,"cmd.exe /c %s\\system32\\vssadmin.exe list writers",WindowsDir);
	
	obj.Open(VssAdminExe);



	return 0;
}

