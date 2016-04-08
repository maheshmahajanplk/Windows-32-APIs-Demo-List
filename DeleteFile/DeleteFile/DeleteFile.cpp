// DeleteFile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<Windows.h>

int ret;

int _tmain(int argc, _TCHAR* argv[])
{
		ret=DeleteFile("C:\\Program Files (x86)\\SAAZOD\\delete.txt");
		if(!ret)
		 printf("File isn't deleted....");
		return 0;
}

