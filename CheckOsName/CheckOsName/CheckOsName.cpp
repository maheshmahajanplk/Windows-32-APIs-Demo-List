// CheckOsName.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include<Windows.h>
#include<string.h>

BOOL CheckOSSupport(char *osName)
{
	std::string str =osName;
	int pos=str.find("2003");
	if(pos!=-1)
	{
		printf("zExVssW 5.0.0.7 CheckOSSupport Info OS Name 2003 Matched");
		return 1;
	}
	else if((pos=str.find("2008"))!=-1) 
	{
		//pos=str.find("2003");
		//if(pos!=-1)
		//{
			printf("zExVssW 5.0.0.7 CheckOSSupport Info OS Name 2008 Matched");
			return 1;
		//}
	}
	else if((pos=str.find("2012"))!=-1)
	{
			printf("zExVssW 5.0.0.7 CheckOSSupport Info OS Name 2012 Matched");
			return 1;
	}
	else{
		printf("zExVssW 5.0.0.7 CheckOSSupport Info OS Name Not Matched");
		return 0;
	}
	
}
int _tmain(int argc, _TCHAR* argv[])
{
	if(CheckOSSupport("Microsoft Windows Server 2012"))
		printf("2012 found");
	else
		printf("Other os version");
	return 0;
}

