// Crash_Sample2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	char*	chTempBuffer = NULL;
	while(NULL != *chTempBuffer) //<---cpp check prev was while(NULL != *chTempBuffer)
	{
		printf(" Its crash safe position\t");
	}
	return 0;
}

