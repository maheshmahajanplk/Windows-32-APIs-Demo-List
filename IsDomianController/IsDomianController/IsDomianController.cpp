// IsDomianController.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "activeds.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//int main(int argc, char* argv[])
	//{
    HRESULT hr;
    IADsContainer *pCont;
    IDispatch *pDisp=NULL;
    IADs *pUser;

     // Initialize COM before calling any ADSI functions or interfaces.
     CoInitialize(NULL);

    hr = ADsGetObject( L"LDAP://CN=users,DC=fabrikam,DC=com", 
                               IID_IADsContainer, 
                               (void**) &pCont );
    
    if ( !SUCCEEDED(hr) )
    {
        return 0;
    }

    //-----------------
    // Create a user
    //-----------------
    
    hr = pCont->Create(CComBSTR("user"), CComBSTR("cn=jeffsmith"), &pDisp );
    
    // Release the container object.    
    pCont->Release();
    
    if ( !SUCCEEDED(hr) )
    {
        return 0;
    }

    hr = pDisp->QueryInterface( IID_IADs, (void**) &pUser );

    // Release the dispatch interface.
    pDisp->Release();

    if ( !SUCCEEDED(hr) )
    {    
        return 0;
    }

    // Commit the object data to the directory.
    pUser->SetInfo();

    // Release the object.
    pUser->Release();

    CoUninitialize();
}


	return 0;
}

