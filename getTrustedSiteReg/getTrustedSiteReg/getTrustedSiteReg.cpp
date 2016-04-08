// getTrustedSiteReg.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "windows.h"
#include "string.h"
#include <iostream>

using namespace std;

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

void QueryKey(HKEY hKey,TCHAR **buff);
void AddSite(TCHAR *sitename,TCHAR **trustedSites);
void RemoveSite(TCHAR *sitename,TCHAR **trustedSites);

int _tmain(int argc, _TCHAR* argv[])
{
		HKEY hTestKey;
		TCHAR reg1[100]=L"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ZoneMap\\EscDomains";
		TCHAR reg2[100];
		TCHAR *pre[50];
		TCHAR *domains[50];
		TCHAR *prefix[50];
		TCHAR *sites[50];

		memset(domains,0,50);
		memset(prefix,0,50);
		memset(sites,0,50);

	
	if( RegOpenKeyEx( HKEY_CURRENT_USER,reg1,0,KEY_READ,&hTestKey) == ERROR_SUCCESS  )
	{
		QueryKey(hTestKey,domains);
	}
	RegCloseKey(hTestKey);

	for(int i=0;domains[i];i++)
	{
		TCHAR dom[256];
		wcscpy(dom,reg1);
		wcscat_s(dom,L"\\");
		wcscat_s(dom,domains[i]);

		if( RegOpenKeyEx( HKEY_CURRENT_USER,dom,0,KEY_READ,&hTestKey) == ERROR_SUCCESS  )
		{
			QueryKey(hTestKey,pre);
			prefix[i]=pre[0];
		}
		RegCloseKey(hTestKey);
	}

	//Concatenate the corresponding prefix with domain to get full url
	for(int i=0;domains[i];i++)
	{
	  TCHAR *str = new TCHAR[50];
	  sites[i] = str;
	  wcscpy(sites[i],prefix[i]);
	  wcscat(sites[i],L".");
	  wcscat(sites[i],domains[i]);
	}



	printf("   Sites are: ");
	for(int i=0;sites[i];i++)
	{
		wcout<<L"\n "<<i+1<<") "<<sites[i]<<endl;
	}

	//AddSite(L"https://www.opera.com",sites);

	//free the heap
	for(int i=0;domains[i];i++)
	{
		free(domains[i]);
		free(prefix[i]);
		free(sites[i]);
	}

	return 0;
}

void QueryKey(HKEY hKey,TCHAR **buff) 
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
                     achKey, //sitename
                     &cbName, //site size 
                     NULL, 
                     NULL, 
                     NULL, 
                     &ftLastWriteTime); 

            if (retCode == ERROR_SUCCESS) 
            {
                _tprintf(TEXT("%s\n"),achKey);
				TCHAR *str = new TCHAR[50];
				wcscpy(str,achKey);

				 *(buff+i) = str;
            }
        }
    } 
	else{
		*buff = NULL; //There is no sites in trusted zone
	}
}

void AddSite(TCHAR *sitename,TCHAR **trustedSites)
{
	
	HRESULT hr;
	IInternetSecurityManager *pSecurityMgr;

	LPCWSTR sites = SysAllocString(sitename);
	CoInitialize(NULL);
		
	hr = CoCreateInstance(CLSID_InternetSecurityManager, NULL, CLSCTX_INPROC_SERVER, IID_IInternetSecurityManager,(void**)&pSecurityMgr);

	pSecurityMgr->SetZoneMapping(URLZONE_ESC_FLAG|URLZONE_TRUSTED,sites,SZM_CREATE); //adding
	//pSecurityMgr->SetZoneMapping(URLZONE_TRUSTED,sites,SZM_DELETE); //adding
	pSecurityMgr->Release();

	::CoUninitialize();
}

void RemoveSite(TCHAR *sitename,TCHAR **trustedSites)
{
	HRESULT hr;
	IInternetSecurityManager *pSecurityMgr;

	LPCWSTR sites = SysAllocString(sitename);
	CoInitialize(NULL);
		
	hr = CoCreateInstance(CLSID_InternetSecurityManager, NULL, CLSCTX_INPROC_SERVER, IID_IInternetSecurityManager,(void**)&pSecurityMgr);

	//pSecurityMgr->SetZoneMapping(URLZONE_TRUSTED,sites,SZM_CREATE); //adding
	pSecurityMgr->SetZoneMapping(URLZONE_TRUSTED,sites,SZM_DELETE); //deleting
	pSecurityMgr->Release();

	::CoUninitialize();
}
	