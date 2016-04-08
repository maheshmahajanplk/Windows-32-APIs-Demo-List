// IsNetworkDirExist_SYSTEM_acnt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<Windows.h>
#include<Shlwapi.h>

int _tmain(int argc, _TCHAR* argv[])
{
	
	char szNetworkPathShared[MAX_PATH + 100] = {"\\\\10.2.17.29\\Shift Details\\Engineering Payroll"};

	if(PathIsNetworkPath(szNetworkPathShared))
		printf("Found");
	else
		printf("Not Found");


	return 0;
}

