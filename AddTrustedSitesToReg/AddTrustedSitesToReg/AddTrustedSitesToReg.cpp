// AddTrustedSitesToReg.cpp : Reading Registry and printing sites added to Trusted Zone of Internet Explorer

// QueryKey - Enumerates the subkeys of key and its associated values.
//     hKey - Key whose subkeys and values are to be enumerated.

#include "stdafx.h"
#include "windows.h"
#include "stdio.h"
#include "string.h"
#include <tchar.h>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

void QueryKey(HKEY hKey,char **psites);
 
void __cdecl _tmain(void)
{
   	HKEY hTestKey;
	char *psites[50]={0};
	char ch;

	// constants from urlmon.h
	const int URLZONE_LOCAL_MACHINE = 0;
	const int URLZONE_INTRANET = URLZONE_LOCAL_MACHINE + 1;
	const int URLZONE_TRUSTED = URLZONE_INTRANET + 1;
	const int URLZONE_INTERNET = URLZONE_TRUSTED + 1;
	const int URLZONE_UNTRUSTED = URLZONE_INTERNET + 1;
	//const int URLZONE_ESC_FLAG = 0x100;
	const int SZM_CREATE  = 0;
	const int SZM_DELETE  = 0x1;
	
	int choice,flag=0;
    char sitename[50];
    wchar_t wsitename[50];
	size_t v_size;

	HRESULT hr;
	IInternetSecurityManager *pSecurityMgr;

   do{
	   printf("\n***************Menu To Add Site to Trusted Zone*****************\n");
	   printf("1.Add Site \n");
	   printf("2.Remove Site\n");
	   printf("3.List Site\n");
	   printf("4.Exit");
	   printf("\nEnter Your Choice: ");
	   scanf_s("%d",&choice);

		switch(choice)
		{
			case 1:/*************************Add Site******************************/
					if( RegOpenKeyEx( HKEY_CURRENT_USER,TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ZoneMap\\Domains"),0,KEY_READ,&hTestKey) == ERROR_SUCCESS  )
					{
						QueryKey(hTestKey,psites);
					}
					RegCloseKey(hTestKey);

					printf("\nEnter the site that you want to add:");
					fflush(stdin);
					gets_s(sitename);
					for(int i=0;psites[i];i++)
						if(strcmp(sitename,psites[i]) == 0)
						{
							flag = 1;
							break;
						}
					if(flag){
						printf("\n  Site Already added...\n");break;
					}
					//free heap
					for(int i=0;psites[i];i++)
					{
						free(psites[i]);
					}
					//Add site code..
					//LPCWSTR sites = SysAllocString(L"https://msdn.microsoft.com");
					v_size = mbstowcs(wsitename,sitename,50);
					CoInitialize(NULL);
					hr = CoCreateInstance(CLSID_InternetSecurityManager, NULL, CLSCTX_INPROC_SERVER, IID_IInternetSecurityManager,(void**)&pSecurityMgr);

					pSecurityMgr->SetZoneMapping(URLZONE_TRUSTED, wsitename, SZM_CREATE); //adding
					pSecurityMgr->Release();
					//pSecurityMgr->SetZoneMapping(URLZONE_TRUSTED, sites, SZM_DELETE); //deleting
					break;

			case 2:/*************************Remove Site******************************/

					if( RegOpenKeyEx( HKEY_CURRENT_USER,TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ZoneMap\\Domains"),0,KEY_READ,&hTestKey) == ERROR_SUCCESS  )
					{
						QueryKey(hTestKey,psites);
					}
					RegCloseKey(hTestKey);
					if(psites) /*able to remove and if site is already there, show message 'site is already added' */  
					{
						printf("\nEnter the site that you want to remove: ");
						fflush(stdin);
						gets_s(sitename);
						mbstowcs(wsitename,sitename,50);
						//Remove site code
						CoInitialize(NULL);
						hr = CoCreateInstance(CLSID_InternetSecurityManager, NULL, CLSCTX_INPROC_SERVER, IID_IInternetSecurityManager,(void**)&pSecurityMgr);

						pSecurityMgr->SetZoneMapping(URLZONE_TRUSTED,wsitename,SZM_DELETE); //adding
						pSecurityMgr->Release();
					}
					else
						printf("\n No sites in Trusted zone");

					for(int i=0;psites[i];i++)
					{
						free(psites[i]);
					}
					break;

			case 3: /*************************List Site******************************/
					
					if( RegOpenKeyEx( HKEY_CURRENT_USER,TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ZoneMap\\Domains"),0,KEY_READ,&hTestKey) == ERROR_SUCCESS  )
					{
						QueryKey(hTestKey,psites);
					}
					RegCloseKey(hTestKey);

					printf("Sites are: ");

				    for(int i=0;psites[i];i++)
					{
						printf("           %s\n",psites[i]);
						free(psites[i]);
					}

					break;
			case 4: 
					exit(0);
				    break;

			default: printf("\nInvalid choice...\n");

		}
		printf("\nDo you want to continue [y/n] :");
		scanf("%c",&ch);
   }while(ch!='n');

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
 
    TCHAR  achValue[MAX_VALUE_NAME]; 
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
 
    // Enumerate the key values. 

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
    }
}


	