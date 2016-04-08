// ServiceStartupType.cpp : Defines the entry point for the application.

/**********************************SERVICE_STATUS structure 2ND fIELD : dwCurrentState->
 0x00000005 ---->SERVICE_CONTINUE_PENDING
 0x00000006 ---->SERVICE_PAUSE_PENDING
 0x00000007 ---->SERVICE_PAUSED
 0x00000004 ---->SERVICE_RUNNING
 0x00000002 ---->SERVICE_START_PENDING
 0x00000003 ---->SERVICE_STOP_PENDING
 0x00000001 ---->SERVICE_STOPPED

 *********************************/

#include "stdafx.h"
#include "ServiceStartupType.h"
#include "stdio.h"

void	printServiceStartType();
VOID __stdcall DoQuerySvc(char *);

struct	stServiceInfo
{
	char	szAppId[51];
	char	szServiceName[256];
	int		iStatus;
	int		iWinExitCode;
	int		iServiceExitCode;
	int		iEventErrorCode;
};


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{ 
	TCHAR srvName[300] = {0};

	try{
		printServiceStartType();
	
	}catch(...){

	}
	
	return 0;
}


//
// Purpose: 
//   Retrieves and displays the current service configuration.
//
// Parameters:
//   None
// 
// Return value:
//   None
//
VOID __stdcall DoQuerySvc(char * szSvcName)
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    LPQUERY_SERVICE_CONFIG lpsc; 
    LPSERVICE_DESCRIPTION lpsd;
    DWORD dwBytesNeeded, cbBufSize, dwError; 
	

    // Get a handle to the SCM database. 
 
    schSCManager = OpenSCManager( 
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 
 
    if (NULL == schSCManager) 
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    // Get a handle to the service.

    schService = OpenService( 
        schSCManager,          // SCM database 
        szSvcName,             // name of service 
        SERVICE_QUERY_CONFIG); // need query config access 
 
    if (schService == NULL)
    { 
        printf("OpenService failed (%d)\n", GetLastError()); 
        CloseServiceHandle(schSCManager);
        return;
    }

    // Get the configuration information.
 
    if( !QueryServiceConfig( 
        schService, 
        NULL, 
        0, 
        &dwBytesNeeded))
    {
        dwError = GetLastError();
        if( ERROR_INSUFFICIENT_BUFFER == dwError )
        {
            cbBufSize = dwBytesNeeded;
            lpsc = (LPQUERY_SERVICE_CONFIG) LocalAlloc(LMEM_FIXED, cbBufSize);
        }
        else
        {
            printf("QueryServiceConfig failed (%d)", dwError);
            goto cleanup; 
        }
    }
  
    if( !QueryServiceConfig( 
        schService, 
        lpsc, 
        cbBufSize, 
        &dwBytesNeeded) ) 
    {
        printf("QueryServiceConfig failed (%d)", GetLastError());
        goto cleanup;
    }

    if( !QueryServiceConfig2( 
        schService, 
        SERVICE_CONFIG_DESCRIPTION,
        NULL, 
        0, 
        &dwBytesNeeded))
    {
        dwError = GetLastError();
        if( ERROR_INSUFFICIENT_BUFFER == dwError )
        {
            cbBufSize = dwBytesNeeded;
            lpsd = (LPSERVICE_DESCRIPTION) LocalAlloc(LMEM_FIXED, cbBufSize);
        }
        else
        {
            printf("QueryServiceConfig2 failed (%d)", dwError);
            goto cleanup; 
        }
    }
 
    if (! QueryServiceConfig2( 
        schService, 
        SERVICE_CONFIG_DESCRIPTION,
        (LPBYTE) lpsd, 
        cbBufSize, 
        &dwBytesNeeded) ) 
    {
        printf("QueryServiceConfig2 failed (%d)", GetLastError());
        goto cleanup;
    }
 
    // Print the configuration information.
 
    _tprintf(TEXT("%s configuration: \n"), szSvcName);
    _tprintf(TEXT("  Type: 0x%x\n"), lpsc->dwServiceType);
    _tprintf(TEXT("  Start Type: 0x%x\n"), lpsc->dwStartType);
    _tprintf(TEXT("  Error Control: 0x%x\n"), lpsc->dwErrorControl);
    _tprintf(TEXT("  Binary path: %s\n"), lpsc->lpBinaryPathName);
    _tprintf(TEXT("  Account: %s\n"), lpsc->lpServiceStartName);

    if (lpsd->lpDescription != NULL && lstrcmp(lpsd->lpDescription, TEXT("")) != 0)
        _tprintf(TEXT("  Description: %s\n"), lpsd->lpDescription);
    if (lpsc->lpLoadOrderGroup != NULL && lstrcmp(lpsc->lpLoadOrderGroup, TEXT("")) != 0)
        _tprintf(TEXT("  Load order group: %s\n"), lpsc->lpLoadOrderGroup);
    if (lpsc->dwTagId != 0)
        _tprintf(TEXT("  Tag ID: %d\n"), lpsc->dwTagId);
    if (lpsc->lpDependencies != NULL && lstrcmp(lpsc->lpDependencies, TEXT("")) != 0)
        _tprintf(TEXT("  Dependencies: %s\n"), lpsc->lpDependencies);
 
    LocalFree(lpsc); 
    LocalFree(lpsd);

cleanup:
    CloseServiceHandle(schService); 
    CloseServiceHandle(schSCManager);
}

void	printServiceStartType()
{
	SC_HANDLE				scHandle			=	NULL;
	ENUM_SERVICE_STATUS		enumServiceStatus	=	{0};
	ENUM_SERVICE_STATUS*	penumServiceStatus	=	NULL;
	
	DWORD					dwBytesNeeded		=	0;
	DWORD					dwServicesReturned	=	0;
	DWORD					dwResumeHandle		=	0;
	DWORD					dwBytesAllocated	=	0;
	DWORD					dwCtr				=	0;
	BOOL					bRet				=	TRUE;

	int						iCtr				=	0;
	int						iCheckNumServices	=	0;
	int						iEventId			=	0;

	char					szTimeINIFile[MAX_PATH + 100]		=	{0};
	char					szDef[10]							=	{0};
	char					szUpTime[101]						=	{0};

	__try
	{
		__try
		{
			//Open the SCM manager
			scHandle	=	::OpenSCManager(NULL,
											SERVICES_ACTIVE_DATABASE,
											SC_MANAGER_ENUMERATE_SERVICE);
			if(NULL == scHandle)
			{
				/*LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								::GetLastError(),
								0,
								NULL,
								"ERR:   Open SC Manager failed",
								NULL,
								NULL);*/
				return;
			}

			bRet	=	::EnumServicesStatus(scHandle,
												SERVICE_WIN32,      //<----Service Type
												SERVICE_STATE_ALL,  //<----Service State
												&enumServiceStatus, //<----buf storing srvnm,srvst
												sizeof(enumServiceStatus),//size of structure
												&dwBytesNeeded,        //
												&dwServicesReturned,
												&dwResumeHandle);

			if(dwBytesNeeded > 0)
			{
				dwBytesAllocated = dwBytesNeeded + sizeof(ENUM_SERVICE_STATUS);
				penumServiceStatus = new ENUM_SERVICE_STATUS[dwBytesAllocated];

				memset(penumServiceStatus,NULL,dwBytesAllocated);
			}
			else
			{
				/*LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								::GetLastError(),
								0,
								NULL,
								"ERR:   Enum Service Status failed",
								NULL,
								NULL);*/
				return;
			}

			dwBytesNeeded		=	0;
			dwServicesReturned	=	0;
			dwResumeHandle		=	0;


			bRet	=	::EnumServicesStatus(scHandle,
											 SERVICE_WIN32,
											 SERVICE_STATE_ALL,
											 penumServiceStatus,
											 dwBytesAllocated,
											 &dwBytesNeeded,
											 &dwServicesReturned,
											 &dwResumeHandle);
			if(0 == bRet)
			{
				/*LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								::GetLastError(),
								0,
								NULL,
								"ERR:   Enum Service Status failed",
								NULL,
								NULL);*/
				return;
			}

			::CloseServiceHandle(scHandle);
			scHandle = NULL;
			
			for(dwCtr = 0; dwCtr < dwServicesReturned; ++dwCtr)
			{
					//Retrieve sERVICE nAME
					DoQuerySvc(penumServiceStatus[dwCtr].lpServiceName);
					_tprintf(TEXT("  Service Start Type: %s\n"), penumServiceStatus[dwCtr].ServiceStatus.dwCurrentState);
			}
			
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{


		}
	}
	__finally
	{
		if(NULL != scHandle)
		{
			::CloseServiceHandle(scHandle);
			scHandle = NULL;
		}
	}
}
