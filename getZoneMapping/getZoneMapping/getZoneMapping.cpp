// getZoneMapping.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include"windows.h"
#include "urlmon.h"
#include "iostream"

int _tmain(int argc, _TCHAR* argv[])
{

	IInternetSecurityManager *pSecurityManager = NULL;
    HRESULT hResult = S_OK;
	IEnumString * penum = NULL;
		
    ::CoInitialize(NULL);
    hResult=CoCreateInstance( CLSID_InternetSecurityManager, 
                              NULL, 
                              CLSCTX_INPROC_SERVER,
                              IID_IInternetSecurityManager,
                              (void **)&pSecurityManager );
    
    if (SUCCEEDED(hResult))
    {
		  hResult=pSecurityManager->GetZoneMappings(URLZONE_ESC_FLAG|URLZONE_TRUSTED,
												   &penum,
												   0);
		  ULONG p;
		  wchar_t *psz = NULL;//new wchar_t[50];
		  //memset(psz,0,sizeof(wchar_t)*50);
		  //hResult = penum->Next(1,&psz,NULL);
		 while (S_OK == penum->Next(1,&psz,&p))
		 {
			//Do something with the string in psz and free it
			 if(p==1){
					std::wcout<<"string :"<<(wchar_t *)psz<<std::endl;
			 }
		 }
				
	}
	penum->Release();
	pSecurityManager->Release();

   ::CoUninitialize();

	return 0;
}

