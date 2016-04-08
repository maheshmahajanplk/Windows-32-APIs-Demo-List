// DownloadFile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include <Urlmon.h>

int _tmain(int argc, _TCHAR* argv[])
{

    printf("URLDownloadToFile test function.\n");

    TCHAR url[] = TEXT("http://10.2.27.11:82/Webpost-Master files_SYNCAPP1/webpost.itsupport-MASTER/tfr_wpappmonconfigxml.asp?mmid=1&sid=1&rid=1&rty=x"); /*http://google.com*/

    printf("Url: %S\n", url);

    TCHAR path[MAX_PATH];

    GetCurrentDirectory(MAX_PATH, path);

    wsprintf(path, TEXT("%s\\index.html"), path);

    printf("Path: %S\n",path);

    HRESULT res = URLDownloadToFile(NULL, url, path, 0, NULL);

    if(res == S_OK) {
        printf("Ok\n");
    } else if(res == E_OUTOFMEMORY) {
        printf("Buffer length invalid, or insufficient memory\n");
    } else if(res == INET_E_DOWNLOAD_FAILURE) {
        printf("URL is invalid\n");
    } else {
        printf("Other error: %d\n", res);
    }

    return 0;
}

