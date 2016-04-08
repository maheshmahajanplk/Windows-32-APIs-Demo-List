// file_exist.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<Windows.h>
#include<stdio.h>
#include <tchar.h>
#include<stdlib.h>


int fileExists(TCHAR * file);

int _tmain(int argc, _TCHAR* argv[])
{
	int i=fileExists("D:\\Mahesh Mahajan");
	printf("%d",i);
	
	return 0;
}
int fileExists(TCHAR * file)
{
   WIN32_FIND_DATA FindFileData;
   HANDLE handle = FindFirstFile(file, &FindFileData) ;
   int found = handle != INVALID_HANDLE_VALUE;
   if(found) 
   {
       //FindClose(&handle); this will crash
       FindClose(handle);
   }
   return found;
}
