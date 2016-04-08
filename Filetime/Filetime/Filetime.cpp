// Filetime.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include "time.h"

// SetFileToCurrentTime - sets last write time to current system time
// Return value - TRUE if successful, FALSE otherwise
// hFile  - must be a valid file handle

BOOL SetFileToCurrentTime(HANDLE hFile)
{
    FILETIME ft;
    SYSTEMTIME st;
    BOOL f;

    GetSystemTime(&st);              // Gets the current system time
    SystemTimeToFileTime(&st, &ft);  // Converts the current system time to file time format
    f = SetFileTime(hFile,           // Sets last-write time of the file 
        (LPFILETIME) NULL,           // to the converted current system time 
        (LPFILETIME) NULL, 
        &ft);    

    return f;
}
int _tmain(int argc, _TCHAR* argv[])
{
	/*FILETIME f = {0};
	SYSTEMTIME convertJobStartTime;
	unsigned long long timeTaken;

	struct tm * timeinfo = new struct tm;
				if(NULL == timeinfo)
				{
					return 0;
				}

	SystemTimeToFileTime( &convertJobStartTime, &f );
	ULARGE_INTEGER u  ; 
	memcpy( &u  , &f , sizeof( u ) );
	const double c_dSecondsPer100nsInterval = 100. * 1.E-9;
	u.QuadPart += timeTaken / c_dSecondsPer100nsInterval; 
	memcpy( &f, &u, sizeof( f ) );
	FileTimeToSystemTime( &f, &convertJobStartTime );

	//timeinfo = localtime(&now);		//Current local time..
	memset(timeinfo,NULL,sizeof(struct tm));

	timeinfo->tm_year = convertJobStartTime.wYear- 1900;
	timeinfo->tm_mon = convertJobStartTime.wMonth- 1;
	timeinfo->tm_mday = convertJobStartTime.wDay;
	timeinfo->tm_hour = convertJobStartTime.wHour;
	timeinfo->tm_min = convertJobStartTime.wMinute;
	timeinfo->tm_sec = convertJobStartTime.wSecond;*/

	/* call mktime: timeinfo->tm_wday will be set */
	/*mktime (timeinfo);
	char BufferJobEnd[100] = {0};
	//if(JobStartTime != 0)
	//{
		strftime (BufferJobEnd,100,"%Y-%m-%d %H:%M:%S",timeinfo);
	//}	else
	//{
		/*
		if(LogFilenotGenerated)	//if there no log file...
		{
			timeinfo = TokenTwoStringAndMakeTimeObject(EndDateErrorDate,EndDateErrorTime);
			strftime (BufferJobEnd,100,"%Y-%m-%d %H:%M:%S",timeinfo);
		}
		else
		{
			BufferJobEnd[0] = '\0';
		}
		*/
	  //}*/


	return 0;
}





