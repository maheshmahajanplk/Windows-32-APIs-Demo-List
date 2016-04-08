// VariantTimeToSystemTime.cpp : Defines the entry point for the console application.
//
#include "WinBase.h"
#include "stdafx.h"
#include<Windows.h>
#include<iostream>
using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
	DATE JobStartTime;	
	char BufferJobStart[100] = {0};
	SYSTEMTIME convertJobStartTime = {0};	

	/*struct tm * timeinfo = new struct tm;
	if(NULL == timeinfo)
	{
		return 0;
	}*/

	JobStartTime = 42297.750000  ; //42297.083333
	VariantTimeToSystemTime(JobStartTime, &convertJobStartTime);	//42293.708333333336
		 
	 /*DateTime MyDate = new DateTime(1904, 12, 12, 1,4,1);
     double MyDouble = MyDate.FromOADate(JobStartTime);

	 printf("MyDouble[Job start time is:]=>");
	 std::cout<<MyDouble<<endl;*/
	 
	 /*timeinfo->tm_year = convertJobStartTime.wYear - 1900;
					
	timeinfo->tm_mon = convertJobStartTime.wMonth - 1;
	timeinfo->tm_mday = convertJobStartTime.wDay;
	timeinfo->tm_hour = convertJobStartTime.wHour;
	timeinfo->tm_min = convertJobStartTime.wMinute;
	timeinfo->tm_sec = convertJobStartTime.wSecond;

	mktime (timeinfo);*/




	return 0;
}

