// String_find.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include<string>

int CheckOSSupport(char *osName);

int _tmain(int argc, _TCHAR* argv[])
{
	std::string obj1("Windows Server (R) 2008 Standard");
	std::string obj2("Windows Small Business Server 2011");

	int pos1=obj1.find("2011");
	int pos2=obj2.find("2011");

	printf("\n\t pos1:%d\t pos2:%d\n",pos1,pos2);

	CheckOSSupport((char *)obj2.c_str());

	return 0;
}

int CheckOSSupport(char *osName)
{
	std::string str=osName;
	int pos=str.find("2003");
	if(pos!=-1)
	{
		printf("zExVssW 5.0.0.6 CheckOSSupport Info OS Name 2003 Matched");
		return 1;
	}
	else if((pos=str.find("2008"))!=-1)
	{
		printf("zExVssW 5.0.0.6 CheckOSSupport Info OS Name 2008 Matched");
		return 1;
	}
	else if((pos=str.find("2010"))!=-1)
	{
		printf("zExVssW 5.0.0.6 CheckOSSupport Info OS Name 2010 Matched");
		return 1;
	}
	else if((pos=str.find("2011"))!=-1)
	{
		printf("zExVssW 5.0.0.6 CheckOSSupport Info OS Name 2011 Matched");
		return 1;
	}
	else if((pos=str.find("2012"))!=-1)
	{
		printf("zExVssW 5.0.0.6 CheckOSSupport Info OS Name 2012 Matched");
		return 1;
	}
	else
	{
		printf("zExVssW 5.0.0.6 CheckOSSupport Info OS Name Not Matched");
		return 0;
	}
	
}