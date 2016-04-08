// try1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<string.h>
#include<Windows.h>


int _tmain(int argc, _TCHAR* argv[])
{

	int ch= '#';
	char *StringPtr = NULL;
	char *cTemp = NULL;
	char szFileName[300] = "Filename1#Filename2#Filename3#";
	StringPtr = szFileName;
	cTemp = strchr(StringPtr, ch); //format like this: filename1#filename2#filename3#
	
	/*if (NULL == cTemp)
		printf("\nNo string has matched in the filename buffer\n");

	memset(szFileName, 0, sizeof(szFileName));
					//szInputDir is dir in which files r there to parse
	strcpy(szFileName, "C:\\Program Files");
	strncat(szFileName, StringPtr, cTemp - StringPtr);*/
	cTemp++;
	StringPtr = cTemp;

	return 0;
}

