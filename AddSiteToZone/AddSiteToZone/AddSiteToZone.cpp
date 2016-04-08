// AddSiteToZone.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "urlmon.h"
#using <mscorlib.dll>
#include <atldef.h>
#include <atlconv.h>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

char *psites[50] = {0};

int _tmain(int argc, _TCHAR* argv[])
{
	const int SZM_CREATE  = 0;
	const int SZM_DELETE  = 0x1;
	
	HRESULT hr;
	IInternetSecurityManager *pSecurityMgr;

	/*if( RegOpenKeyEx( HKEY_CURRENT_USER,TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ZoneMap\\Domains"),0,KEY_READ,&hTestKey) == ERROR_SUCCESS  )
	{
		QueryKey(hTestKey,psites);
	}
	RegCloseKey(hTestKey);*/
	
	LPCWSTR sites = SysAllocString(L"https://www.mahesh.com");
	CoInitialize(NULL);
		
	hr = CoCreateInstance(CLSID_InternetSecurityManager, NULL, CLSCTX_INPROC_SERVER, IID_IInternetSecurityManager,(void**)&pSecurityMgr);

	pSecurityMgr->SetZoneMapping(URLZONE_TRUSTED,sites,SZM_CREATE); //adding
	//pSecurityMgr->SetZoneMapping(URLZONE_TRUSTED,sites,SZM_DELETE); //adding
	pSecurityMgr->Release();

	::CoUninitialize();
	
	return 0;
}

void QueryKey(HKEY hKey,char **psites) 
{ 
    TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
    DWORD    cbName;                   // size of name string 
    TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys=0;               // number of subkeys 
    DWORD    cbMaxSubKey;              // longest subkey size 
    DWORD    cchMaxClass;              // longest class string 
    DWORD    cValues;              // number of values for key 
    DWORD    cchMaxValue;          // longest value name 
    DWORD    cbMaxValueData;       // longest value data 
    DWORD    cbSecurityDescriptor; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 
 
    DWORD i, retCode; 
 
    //TCHAR  achValue[MAX_VALUE_NAME]; 
    DWORD cchValue = MAX_VALUE_NAME; 
 
    // Get the class name and the value count. 
    retCode = RegQueryInfoKey(
        hKey,                    // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        &cbMaxSubKey,            // longest subkey size 
        &cchMaxClass,            // longest class string 
        &cValues,                // number of values for this key 
        &cchMaxValue,            // longest value name 
        &cbMaxValueData,         // longest value data 
        &cbSecurityDescriptor,   // security descriptor 
        &ftLastWriteTime);       // last write time 
 
    // Enumerate the subkeys, until RegEnumKeyEx fails.
    
    if (cSubKeys)
    {
        printf( "\nNumber of subkeys: %d\n", cSubKeys);

        for (i=0; i<cSubKeys; i++) 
        { 
            cbName = MAX_KEY_LENGTH;
            retCode = RegEnumKeyEx(hKey, i,
                     achKey, 
                     &cbName, 
                     NULL, 
                     NULL, 
                     NULL, 
                     &ftLastWriteTime); 
            if (retCode == ERROR_SUCCESS) 
            {
                //_tprintf(TEXT("%s\n"),achKey);
				char *str = new char[50];
				 strcpy(str,achKey);
				 *(psites+i) = str;
            }
        }
    } 
	else{
		*psites = NULL; //There is no sites in trusted zone
	}
 
    /* Enumerate the key values. 

    if (cValues) 
    {
        for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
        { 
            cchValue = MAX_VALUE_NAME; 
            achValue[0] = '\0'; 
            retCode = RegEnumValue(hKey, i, 
                achValue, 
                &cchValue, 
                NULL, 
                NULL,
                NULL,
                NULL);
 
            if (retCode == ERROR_SUCCESS ) 
            { 
                _tprintf(TEXT("%s\n"),achValue); 
            } 
        }
    }*/
}


