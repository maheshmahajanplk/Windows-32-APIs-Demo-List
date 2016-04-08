// ReferenceCpp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int m=1;
	int & x = m+1;
	int & y = m;

	return 0;
}

