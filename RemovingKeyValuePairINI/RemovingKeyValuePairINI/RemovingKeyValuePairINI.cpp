// RemovingKeyValuePairINI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<windows.h>
#include<iostream>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	
	WritePrivateProfileString("MAHI","Key1","Value1","C:\\MAHI1.ini");
	WritePrivateProfileString("MAHI","Key2","Value2","C:\\MAHI1.ini");
	WritePrivateProfileString("MAHI","Key3",NULL,"C:\\MAHI1.ini");

	return 0;
}

