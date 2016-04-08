// Win32getHostname.cpp : Defines the entry point for the application.
#include "stdafx.h"
#include "Win32getHostname.h"
#include<stdio.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	
	char szPath[128] = "";

    WSADATA wsaData;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    gethostname(szPath, sizeof(szPath));

    printf("%s", szPath);

    WSACleanup();

    return 0;

}



