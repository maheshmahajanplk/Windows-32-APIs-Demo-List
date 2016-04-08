// Crash_Sample3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	/************Please note that:
	sizeof() would work for array of char instead pointer to char
	******/
	char * g_szCheckSum = NULL;
	char g_safe[20] = {""};

	strlen(NULL); //same for sizeof(NULL)
	memset(g_safe,0,sizeof(g_safe));
	printf(" sizeof result is %d <=> strlen result is %d  ",sizeof(g_safe),strlen(g_safe)); //for Array of char =>safe
	printf(" sizeof result is %d <=> strlen result is %d  ",sizeof(g_szCheckSum),strlen(g_szCheckSum)); //for pointer => leads to crash if got NULL ie empty
	memset(g_szCheckSum,0,sizeof(g_szCheckSum));
	
	return 0;
}

