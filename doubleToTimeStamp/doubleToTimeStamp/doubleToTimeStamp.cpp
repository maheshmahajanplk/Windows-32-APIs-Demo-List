// doubleToTimeStamp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include"ATLComTime.h"


int _tmain(int argc, _TCHAR* argv[])
{
	SYSTEMTIME stime;
	struct tm * timeinfo = new struct tm;
	if(NULL == timeinfo)
	{
		return 0;
	}

	DATE JobStartTime=42297.083333;

	COleDateTime odt1(JobStartTime);

	CString time = odt1.Format(); //10/20/2015 02:00:00

	 int y  = odt1.GetDayOfYear();
	 int hr = odt1.GetHour();

	//odt1.DateFromDouble(JobStartTime);

	 timeinfo->tm_year = odt1.GetYear() - 1900;//= convertJobStartTime.wYear - 1900;
	 timeinfo->tm_mon = odt1.GetMonth() - 1;//convertJobStartTime.wMonth - 1;
	 timeinfo->tm_mday = odt1.GetDay();//convertJobStartTime.wDay;
	 timeinfo->tm_hour = odt1.GetHour();//convertJobStartTime.wHour;
	 timeinfo->tm_min = odt1.GetMinute();//convertJobStartTime.wMinute;
	 timeinfo->tm_sec = odt1.GetSecond();//convertJobStartTime.wSecond;

	 mktime(timeinfo);

	

	return 0;
}

