// Movefile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<Windows.h>


int _tmain(int argc, _TCHAR* argv[])
{
	char szFinalLogFile[MAX_PATH] = {""};
	char szFinalAlertLogFile[MAX_PATH] = {""};

	MoveFileEx(szFinalLogFile,szFinalAlertLogFile,MOVEFILE_REPLACE_EXISTING);

	return 0;
}

