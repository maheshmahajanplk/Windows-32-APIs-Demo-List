// ParsingINIFile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<Windows.h>


int _tmain(int argc, _TCHAR* argv[])
{
	 WritePrivateProfileString("1182", "Employee Name", "",".\\mahi.ini"); //creating file.ini
     WritePrivateProfileString("1182", "Skills", "",".\\mahi.ini");
	 return 0;
}

