
#include "stdafx.h"
#include "windows.h"
#include "urlmon.h"
#using <mscorlib.dll>
#include <atldef.h>
#include <atlconv.h>
#include <cwchar>  // or wchar.h


using namespace System;
using namespace System::Runtime::InteropServices;
#define MAX_LOADSTRING 100

 
int _tmain(int argc, _TCHAR* argv[])
{
	// constants from urlmon.h
	const int URLZONE_LOCAL_MACHINE = 0;
	const int URLZONE_INTRANET = URLZONE_LOCAL_MACHINE + 1;
	const int URLZONE_TRUSTED = URLZONE_INTRANET + 1;
	const int URLZONE_INTERNET = URLZONE_TRUSTED + 1;
	const int URLZONE_UNTRUSTED = URLZONE_INTERNET + 1;
	//const int URLZONE_ESC_FLAG = 0x100;
	const int SZM_CREATE  = 0;
	const int SZM_DELETE  = 0x1;
	
	HRESULT hr;
	IInternetSecurityManager *pSecurityMgr;
	
	LPCWSTR sites = SysAllocString(L"https://msdn.microsoft.com");
	CoInitialize(NULL);
		
	hr = CoCreateInstance(CLSID_InternetSecurityManager, NULL, CLSCTX_INPROC_SERVER, IID_IInternetSecurityManager,(void**)&pSecurityMgr);

	pSecurityMgr->SetZoneMapping(URLZONE_TRUSTED,sites,SZM_CREATE); //adding
	pSecurityMgr->Release();

	::CoUninitialize();
	//pSecurityMgr->SetZoneMapping(URLZONE_TRUSTED, sites, SZM_DELETE); //deleting
	

	/*IInternetSecurityManager *pSecurityManager = NULL;
    HRESULT hResult = S_OK;

	LPCWSTR sites = SysAllocString(L"https://www.aaa.com"); //<----very imp step
    ::CoInitialize(NULL);
    hResult=CoCreateInstance( CLSID_InternetSecurityManager, 
                              NULL, 
                              CLSCTX_INPROC_SERVER,
                              IID_IInternetSecurityManager,
                              (void **)&pSecurityManager );
    
    if (SUCCEEDED(hResult))
    {
      hResult=pSecurityManager->SetZoneMapping(URLZONE_ESC_FLAG|URLZONE_TRUSTED,
                                               L"http://www.aaa.com",
                                               SZM_CREATE );
    
      pSecurityManager->Release();
    }
    ::CoUninitialize(); */

	
	return 0;

}




/*class TrustedZoneSites
{
	private const string CLSID_InternetSecurityManager = "7b8a2d94-0ac9-11d1-896c-00c04fb6bfc4";
 	private const int E_FAIL            = ((int)0x80004005);
	private const int ERROR_FILE_EXISTS = ((int)0x80070050);
 
	private const uint SZM_CREATE = 0;
	private const uint SZM_DELETE = 1;

	public const uint ZoneLocalMachine = 0;
	public const uint ZoneIntranet     = 1;
	public const uint ZoneTrusted      = 2;
	public const uint ZoneInternet     = 3;
	public const uint ZoneUntrusted    = 4;

	private static IInternetSecurityManager CreateInternetSecurityManager()
	{
		Type iismType = Type.GetTypeFromCLSID(new Guid(CLSID_InternetSecurityManager));
		return (IInternetSecurityManager)Activator.CreateInstance(iismType);
	}
 
	public static void AddSiteToZone(uint zone, String pattern)
	{
		try
		{
			IInternetSecurityManager ism = CreateInternetSecurityManager();
			ism.SetZoneMapping(zone, pattern, SZM_CREATE);
		}
		catch (COMException e)
		{
			if (e.ErrorCode == E_FAIL || e.ErrorCode == ERROR_FILE_EXISTS)
				throw new InvalidOperationException("URL has already been added to a zone", e);
			else
				throw;
		}
		catch (UnauthorizedAccessException e)
		{
			throw new InvalidOperationException("Can't add non-SSL site to zone that requires SSL", e);
		}
	}

	public static void RemoveSiteFromZone(String pattern)
	{
		uint  currentZone;
		IInternetSecurityManager ism = CreateInternetSecurityManager();
 
		ism.MapUrlToZone(pattern, out currentZone, 0);
		ism.SetZoneMapping(currentZone, pattern, SZM_DELETE);
	}
}
 */

