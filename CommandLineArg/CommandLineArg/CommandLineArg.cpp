// CommandLineArg.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	printf("Argc is = %d  =>",argc);
	for(int i=0;i<argc;i++)
	{
		printf("i is = %d  =>",i);
		puts(argv[i]);
		puts("\n");
	}
	return 0;
}

