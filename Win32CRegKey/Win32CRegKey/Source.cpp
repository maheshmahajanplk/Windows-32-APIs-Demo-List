

#include <atlbase.h>
#define _AFXDLL
#include <tchar.h>
#include <afx.h>

/*
#ifdef __windows__
#undef __windows__
#endif
*/

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
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
