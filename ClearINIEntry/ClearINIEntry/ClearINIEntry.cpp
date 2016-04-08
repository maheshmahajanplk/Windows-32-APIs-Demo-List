// ClearINIEntry.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<Windows.h>


int _tmain(int argc, _TCHAR* argv[])
{

	WritePrivateProfileString("1",NULL, NULL, "C:\\Program Files (x86)\\SAAZOD\\Configuration\\LastRunTimestamp.ini");

	return 0;
}

