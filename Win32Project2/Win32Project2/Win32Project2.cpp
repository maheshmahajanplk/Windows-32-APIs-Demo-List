// Win32Project2.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Win32Project2.h"
#include <atlbase.h> 
#include <atlstr.h> 


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	CRegKey key;
	LPTSTR szBuffer = new TCHAR[50];
	ULONG cchBuffer = 257;
	try{
		key.Open(HKEY_LOCAL_MACHINE,L"SOFTWARE\\SAAZOD\0",KEY_READ);
		ULONG chars;
		CString str;

		if (key.QueryStringValue(L"REGID", 0, &chars) == ERROR_SUCCESS)
		{
		 key.QueryStringValue(L"REGID", str.GetBuffer(chars), &chars);
		 str.ReleaseBuffer();
		}
		key.Close();
	}
	catch(...){
	}
}


