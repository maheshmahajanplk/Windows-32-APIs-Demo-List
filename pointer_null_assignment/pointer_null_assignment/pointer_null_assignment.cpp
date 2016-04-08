// pointer_null_assignment.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	char *p=NULL;

	if(p==NULL)
		printf("Pointer is NULL");
	else
		printf("Pointer is non NULL");

	p = new char[20];

	if(p==NULL)
		printf("Pointer after allocation is matched with NULL");
	else
		printf("Pointer after allocation is found non null");

	delete[] p;

	return 0;
}

