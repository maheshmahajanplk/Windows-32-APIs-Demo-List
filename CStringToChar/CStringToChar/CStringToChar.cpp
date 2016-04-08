// CStringToChar.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include"ATLComTime.h"


int _tmain(int argc, _TCHAR* argv[])
{
	DATE JobStartTime=42297.083333;

	COleDateTime objStartTime(JobStartTime);
	CString time = objStartTime.Format();

	char szStartTime[MAX_PATH] = {0};

	sprintf(szStartTime,"Error code tokenize successfully!");//,JobNextRunTimecharecters);

	return 0;
}

