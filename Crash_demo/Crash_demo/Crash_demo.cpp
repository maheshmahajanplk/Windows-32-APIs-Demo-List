// Crash_demo.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Crash_demo.h"
#include "stdio.h"



int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	int *p = NULL;
	printf("This is Start");
	*p = 10;
	printf("This is End");
	return 0;
}




