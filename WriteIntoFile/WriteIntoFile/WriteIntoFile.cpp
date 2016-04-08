// WriteIntoFile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<windows.h>
#include<iostream>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hFile; 
	char DataBuffer[] = {" <app><applications></app></applications>" };
    DWORD dwBytesToWrite = (DWORD)strlen(DataBuffer);
    DWORD dwBytesWritten = 0;
    BOOL bErrorFlag = FALSE;
	 
	hFile = CreateFile(TEXT("D:\\Mahesh Mahajan\\Assignments\\C++\\WriteIntoFile\\RMM-PC-341_Config.xml"),                // name of the write
                       GENERIC_WRITE,          // open for writing
                       0,                      // do not share
                       NULL,                   // default security
                       OPEN_EXISTING,             // create new file only
                       FILE_ATTRIBUTE_NORMAL,  // normal file
                       NULL);                  // no attr. template

    if (hFile == INVALID_HANDLE_VALUE) 
    { 
        _tprintf(TEXT("Terminal failure: Unable to open file \"config.xml\" for write.\n"));
        return 1;
    }

	bErrorFlag = WriteFile( 
                    hFile,           // open file handle
                    DataBuffer,      // start of data to write
                    dwBytesToWrite,  // number of bytes to write
                    &dwBytesWritten, // number of bytes that were written
                    NULL);            // no overlapped structure

    if (FALSE == bErrorFlag)
    {
        printf("Terminal failure: Unable to write to file.\n");
    }
    else
    {
        if (dwBytesWritten != dwBytesToWrite)
        {
            // This is an error because a synchronous write that results in
            // success (WriteFile returns TRUE) should write all data as
            // requested. This would not necessarily be the case for
            // asynchronous writes.
            printf("Error: dwBytesWritten != dwBytesToWrite\n");
        }
        else
        {
            _tprintf(TEXT("Wrote %d bytes to %s successfully.\n"), dwBytesWritten);
        }
    }

    CloseHandle(hFile);

	return 0;
}



