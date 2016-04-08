// ZDeskAppRelease.cpp : Defines the entry point for the application.

#include "stdafx.h"
#include "ZDeskAppRelease.h"

///////////////////Header Content////////////////////////////////////////////////////////
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <tlhelp32.h>
#include "stdafx.h"
#include "zDeskAppRelease.h"
#include "md5.h"
#include "winhttpclient.h"
#include <iostream>
#include "atlbase.h"
#include "atlconv.h"
#include "comutil.h"
#include "winhttp.h"
#include "EventList.h"
#include "Socket.h"
#include "PerfData.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <tchar.h>
#include <psapi.h>



#include <list>
#include <ctime>
#pragma comment (lib, "Winhttp.lib")
#pragma comment(lib, "comsuppw.lib")
#pragma comment (lib, "psapi.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "webPost/ZWebPost.lib")
											/* #import "C:\Program Files\Common Files\System\ado
											 \msado15.dll" no_namespace   rename( "EOF", "adoEOF" ) */
#include "webpost/ZWebPost.h"

using namespace std;


#define BUFFER_SIZE 1024*64
#define	SYS_UPTIME_ERROR	1

#define SERVICE_ERROR		0
#define SERVICE_GOOD		1
#define SERVICE_REMOVED		2
#define SERVICE_NOTFOUND	3

struct	stAppInfo
{
	char	szAppId[51];
};

struct	stServiceInfo
{
	char	szAppId[51];
	char	szServiceName[256];
	int		iStatus;
	int		iWinExitCode;
	int		iServiceExitCode;
	int		iEventErrorCode;
};

struct	stProcessInfo
{
	char	szAppId[51];
	char	szProcessName[101];
	int		iStatus;
};

struct	stPortInfo
{
	char	szAppId[51];
	char	szIP[256];
	int		iPort;
	int		iStatus;	
};

int	TerminateProcess(char*	szProcess,
					 char*	szProcessExe);

bool CallWPForUpdateXML(char * pszMachineName);

bool CallWPForXMLMD5(char * pszMD5);

int GetUpdateConfigXML(char *pszMachineName);

DWORD	RemotePortCheck(char* pszMachineName);

DWORD	CheckRemotePort(stPortInfo* pstPortInfo);

DWORD WINAPI ClosePortGracefully(CSAMSocket* pSamSocket);

void	CheckServices(stServiceInfo* pstServiceInfo,int	iNumServices);

void	CheckProcess(stProcessInfo*	pstProcessInfo,int	iNumProcess);

void	CheckPorts(stPortInfo*	pstPortInfo,int	iNumPorts);

void	CheckPerfProcess(stProcessInfo*	pstProcessInfo,int	iNumProcess);
	

void	WriteCheckedData(char*	pszMachineName,
							stServiceInfo*&		pstServiceInfo,
							int&				iNumServices,		
							stProcessInfo*&		pstProcessInfo,
							int&				iNumProcess,
							stPortInfo*&		pstPortInfo,
							int&				iNumPorts);

int GetRandomHour();

bool CallWPForUpdate(char*	pszMachineName);

void GetCurrentTimeString(char * pszCurTime);

void	WriteAlertData(char*	pszMachineName,
						char*				pszRegId,
						stAppInfo*&			pstAppInfo,
						int&				iNumAppInfo,
						stServiceInfo*&		pstServiceInfo,
						int&				iNumServices,		
						stProcessInfo*&		pstProcessInfo,
						int&				iNumProcess,
						stPortInfo*&		pstPortInfo,
						int&				iNumPorts);

int		ReadXMLConfigFile(char*	pszXMLFile,
							stAppInfo*&			pstAppInfo,
							int&				iNumAppInfo,
							stServiceInfo*&		pstServiceInfo,
							int&				iNumServices,		
							stProcessInfo*&		pstProcessInfo,
							int&				iNumProcess,
							stPortInfo*&		pstPortInfo,
							int&				iNumPorts);

int		LogLastError(char*	szFilePath,
					 long	llastLineCode,
					 char*	szModuleCode,
					 long	lSystemCode,
					 long	lMiscCode,
					 char*	pszModuleErrorString,
					 char*	pAddInfo1,
					 char*	pAddInfo2,
					 char*	pAddInfo3);

DWORD	LogDataToDisk(TCHAR *szModuleFilePath,
							TCHAR*	szData,
							BOOL bAlertData = FALSE);

DWORD	LogErrorToDisk(TCHAR *szModuleFilePath,
							TCHAR* szErrorString);

DWORD	CreateLogDirectory(char* szDirectoryName);

void	GetLogDateTime(char* szDateTime);

DWORD	GetDateTime(char*	szDate, char* szDateTime);

void	GetDateFormatYYYYMM(char * szDate);

void	GetDateFormatYYYYMMDD(char * szDate);

void	GetTempFileName(char* pszDateTime);

DWORD	GetAlertDateTime(char* szDateTime);

int		CheckSchedule(char* szWeekDays, float fstartTime, float fendtime);

int		CheckTime(char* szStartTime, char* szEndTime);

BOOL	EnablePrivilege(HANDLE hToken, LPCTSTR szPrivName, BOOL fEnable);

int		WritePID(char* szMachineName, char* szRegId, char* szDescription);

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
LPFN_ISWOW64PROCESS fnIsWow64Process;

BOOL ZGetMD5(char *pszFilePath, char*	g_szCheckSum);

bool NeedToWebPost(char* szCurrentFile, char* szMachineName);

bool CallWPForCount(char* Count);

bool GetMachineNames(char* IniFilePath);

bool GetDNSFromINI(char* Section, char* RetVal);

list<string>	MachineNameList;
enum serviceFoundStatus {SERVICE_NOT_FOUND=0,SERVICE_FOUND};
	 serviceFoundStatus isServiceFound;

int  flag=0;
bool resultWP ;
int  serviceRemovedFlag=0;
int  serviceNotFoundFlag=0;

HRESULT WriteToBuffer(LPTSTR	&lpszBuffer,	//[in,out]Destination pointer where the buffer is located
						LPCTSTR lpszString,		//[in]String to be copied to the buffer
						BOOL bRefresh=FALSE);			//[in]Initialize the buffer if it is TRUE, default is FALSE

void	CaptureServiceEvents();

char		g_szModulePath[MAX_PATH + 100]			=	{0};
char		g_szSAAZMobilePath[MAX_PATH + 100]		=	{0};
char		g_szDeviceListINI[MAX_PATH + 100]		=	{0};
char		g_szErrorLogPath[MAX_PATH + 100]		=	{0};
char		g_szInternalStatusINI[MAX_PATH + 100]	=	{0};
char        g_szServicePrevStatusINI[MAX_PATH + 100]=	{0};
char		szCurrentTime[30]						=	{0};
int			updateConfigXMLHour  = 0;  //taking random hour time

const    DWORD    g_dwExtraBuffer	= 256;			//Extra Buffer required to be allocated by WriteToBuffer()
const	char					g_szRecDelimiter[]			=	"\r\n";
const	char					g_szFieldDelimiter[]		=	"\t";

stServiceEvent*					g_pServiceEventRoot			=	NULL;
DWORD							g_dwTickCount				=	0;


////////////////////////////////////////////////////////////////////////////////////////////



int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{

	//UNREFERENCED_PARAMETER(hPrevInstance);
	//UNREFERENCED_PARAMETER(lpCmdLine);


	char*		pszFindChar							=	NULL;
	char*		pszFinalData						=	NULL;
	char*		pszDataFile							=	NULL;
	char*		pszPrivateSectionINI				=	NULL;	
	char*		pszMachineName						=	NULL;
	char*		pszStartChar						=	NULL;
	char		szMachineName[201]					=	{0};
	char		szRegId[201]						=	{0};	
	char		szDef[10]							=	{0};
	char		szXMLFilePath[MAX_PATH + 100]		=	{0};
	char		szWeekDays[51]						=	{0};
	char		szFromTime[11]						=	{0};
	char		szToTime[11]						=	{0};
	char		szPIDInfo[356]						=	{0};
	char		szLocalMachine[101]					=	{0};
	
	stServiceInfo*		pstServiceInfo				=	NULL;
	stProcessInfo*		pstProcessInfo				=	NULL;
	stPortInfo*			pstPortInfo					=	NULL;
	stAppInfo*			pstAppInfo					=	NULL;
													
													
	int					iNumServices				=	0;
	int					iNumProcess					=	0;
	int					iNumPorts					=	0;
	int					iNumAppInfo					=	0;
	int					iNumofThreads				=	0;
	int					iCtr						=	0;
													
	float				fFromTime					=	0.0;
	float				fToTime						=	0.0;
													
													
	HANDLE				hToken						=	NULL;
	HANDLE				hTotalHandles[58]			=	{0};
													
	WORD				wVersionRequested			=	0;
	WSADATA				wsaData						=	{0};
													
	DWORD				dwThreadId					=	0;


 	__try
	{
		__try
		{
					g_dwTickCount = ::GetTickCount();
					::GetModuleFileName(NULL,g_szModulePath,MAX_PATH);

					pszFindChar = strrchr(g_szModulePath,'\\');
					if(NULL != pszFindChar)
					{
						memset(pszFindChar,NULL,15);
						*pszFindChar = NULL;
					}

					//adjust the token priviledge, so it could terminate the exe properly
					if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) 
					{
						//Enable the SE_DEBUG_NAME privildge
						if(!EnablePrivilege(hToken, SE_DEBUG_NAME, TRUE)) 
						{
						}
						//Close the token handle
						CloseHandle(hToken);
						hToken = NULL;
					}
					if(0 == TerminateProcess("zDeskApp","zDeskApp.EXE"))
					{
						return 0;
					}

					wVersionRequested = MAKEWORD( 2, 2 );
					WSAStartup(wVersionRequested, &wsaData);
					lstrcpy(g_szDeviceListINI,g_szModulePath);
					lstrcat(g_szDeviceListINI,"\\Configuration\\DeviceList.ini");
					lstrcpy(g_szSAAZMobilePath,g_szModulePath);
					lstrcat(g_szSAAZMobilePath,"\\SAAZMobile.ini");
					lstrcpy(g_szErrorLogPath,g_szModulePath);
					lstrcat(g_szErrorLogPath,"\\ApplicationLog\\App\\zDeskApp.log");
					lstrcpy(g_szInternalStatusINI,g_szModulePath);
					lstrcat(g_szInternalStatusINI,"\\Configuration\\App\\InternalStatus.ini");
					
					//first we are checking for the local machine
					GetPrivateProfileString("MACHINE_INFO",
											"MachineName",
											szDef,
											szMachineName,
											200,
											g_szSAAZMobilePath);

					if(NULL == szMachineName[0])
					{
						pszPrivateSectionINI	=	new char[1024 * 2];
						if(NULL == pszPrivateSectionINI)
						{
							return 0;
						}
						memset(pszPrivateSectionINI,NULL,(1024 * 2));
						GetPrivateProfileSectionNames(pszPrivateSectionINI,
														(1024 * 2),
														g_szDeviceListINI);
						pszFindChar = pszPrivateSectionINI;
						while(NULL != *pszFindChar)
						{
							memset(szLocalMachine,NULL,101);
							GetPrivateProfileString(pszFindChar,
													"LocalMachine",
													szDef,
													szLocalMachine,
													100,
													g_szDeviceListINI);

							if(0 == lstrcmpi("Y",szLocalMachine))
							{
								lstrcpy(szMachineName,pszFindChar);
								break;
							}
							pszFindChar = pszFindChar + lstrlen(pszFindChar) + 1;
						}
					}

					if(NULL == szMachineName[0])
					{
						LogLastError(g_szErrorLogPath,
										9999,
										NULL,
										0,
										0,
										NULL,
										"ERR:   Base Machine Name not available in SAAZMobile.ini or DeviceList.ini",
										NULL,
										NULL);
						return 0;
					}

					GetPrivateProfileString("MACHINE_INFO",
											"DCRegId",
											szDef,
											szRegId,
											200,
											g_szSAAZMobilePath);

					if(NULL == szRegId[0])
					{
						LogLastError(g_szErrorLogPath,
										9999,
										NULL,
										0,
										0,
										NULL,
										"ERR:   Base RegId not available in SAAZMobile.ini",
										szMachineName,
										NULL);
						if(NULL != szMachineName[0])
						{
							GetPrivateProfileString(szMachineName,
													"RegId",
													szDef,
													szRegId,
													200,
													g_szDeviceListINI);
						}

						if(NULL == szRegId[0])
						{
							LogLastError(g_szErrorLogPath,
											9999,
											NULL,
											0,
											0,
											NULL,
											"ERR:   Base RegId not available in DeviceList.ini",
											szMachineName,
											NULL);
							return 0;
						}
					}

					int UpdateStatus = GetUpdateConfigXML(szMachineName);

					//first check whether the monitoring is enable for the local server
					if((1 == ::GetPrivateProfileInt(szMachineName,"Enable",1,g_szDeviceListINI)))
					{
							GetPrivateProfileString(szMachineName,
													"DaysToMonitor",
													szDef,
													szWeekDays,
													50,
													g_szDeviceListINI);


							GetPrivateProfileString(szMachineName,
													"FromTime",
													szDef,
													szFromTime,
													10,
													g_szDeviceListINI);

							GetPrivateProfileString(szMachineName,
													"ToTime",
													szDef,
													szToTime,
													10,
													g_szDeviceListINI);

							if(1 == CheckTime(szFromTime, szToTime))
							{
								pszFindChar = strchr(szFromTime,':');
								if(NULL != pszFindChar)
								{
									*pszFindChar = '.';
								}

								pszFindChar = strchr(szToTime,':');
								if(NULL != pszFindChar)
								{
									*pszFindChar = '.';
								}
								sscanf(szFromTime,"%f",&fFromTime);
								sscanf(szToTime,"%f",&fToTime);


								if(1 == CheckSchedule(szWeekDays,fFromTime,fToTime))
								{
										lstrcpy(szXMLFilePath,g_szModulePath);
										lstrcat(szXMLFilePath,"\\Configuration\\App\\");
										lstrcat(szXMLFilePath,szMachineName);
										lstrcat(szXMLFilePath,"_Config.xml");


										ReadXMLConfigFile(szXMLFilePath,
															pstAppInfo,
															iNumAppInfo,
															pstServiceInfo,
															iNumServices,
															pstProcessInfo,
															iNumProcess,
															pstPortInfo,
															iNumPorts);

										if((0 == iNumServices)	&&
											(0 == iNumProcess)	&&
											(0 == iNumPorts))
										{
											LogLastError(g_szErrorLogPath,
															9999,
															NULL,
															0,
															0,
															NULL,
															"MSG:   Policies not found for base machine",
															szMachineName,
															NULL);

											sprintf(szPIDInfo,"Policies not found for base machine %s",szMachineName);
											WritePID(szMachineName,szRegId,szPIDInfo);
										} 
										else
										{
												::WritePrivateProfileString(szMachineName,
																			"Status",
																			"1",
																			g_szInternalStatusINI);

												if(iNumServices > 0)
												{
													CaptureServiceEvents();
													CheckServices(pstServiceInfo, iNumServices);
												}

												if(iNumProcess > 0)
												{
													//CheckProcess(pstProcessInfo, iNumProcess);
													if(FALSE == PerfDataInitialize())
													{
														LogLastError(g_szErrorLogPath,
																		9999,
																		NULL,
																		0,
																		0,
																		NULL,
																		"ERR:   Unable to initialize perf data",
																		szMachineName,
																		NULL);
														return 0;
													}
													PerfDataRefresh();
													CheckPerfProcess(pstProcessInfo, iNumProcess);
													PerfDataUninitialize();

													CheckProcess(pstProcessInfo, iNumProcess);
												}

												if(iNumPorts > 0)
												{
													CheckPorts(pstPortInfo, iNumPorts);
												}

								
												WriteCheckedData(szMachineName,
																	pstServiceInfo,
																	iNumServices,
																	pstProcessInfo,
																	iNumProcess,
																	pstPortInfo,
																	iNumPorts);

												WriteAlertData(szMachineName,
																szRegId,
																pstAppInfo,
																iNumAppInfo,
																pstServiceInfo,
																iNumServices,
																pstProcessInfo,
																iNumProcess,
																pstPortInfo,
																iNumPorts);
										} 


									if(NULL != pstServiceInfo)
									{
										delete [] pstServiceInfo;
										pstServiceInfo = NULL;
									}

									if(NULL != pstProcessInfo)
									{
										delete [] pstProcessInfo;
										pstProcessInfo = NULL;
									}

									if(NULL != pstPortInfo)
									{
										delete [] pstPortInfo;
										pstPortInfo = NULL;
									}

									if(NULL != pstAppInfo)
									{
										delete [] pstAppInfo;
										pstAppInfo = NULL;
									}
									iNumServices	=	0;
									iNumProcess		=	0;
									iNumPorts		=	0;
									iNumAppInfo		=	0;
								}//end of if checkSchedule
								else
								{
									LogLastError(g_szErrorLogPath,
												9999,
												NULL,
												0,
												0,
												NULL,
												"MSG:   Not in current schedule",
												szMachineName,
												NULL);

									sprintf(szPIDInfo,"Not in current schedule %s",szMachineName);
									WritePID(szMachineName,szRegId,szPIDInfo);
								}//end of else checkSchedule
							}//end of else checkTime
							else
							{
								LogLastError(g_szErrorLogPath,
									9999,
									NULL,
									0,
									0,
									NULL,
									"MSG:   Not in current schedule",
									szMachineName,
									NULL);

								sprintf(szPIDInfo,"Not in current schedule %s",szMachineName);
								WritePID(szMachineName,szRegId,szPIDInfo);
							}
					} //GetPrivateProfileInt local server monitoring enable if end
					else
					{
						LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								0,
								0,
								NULL,
								"MSG:   Base Machine is not enabled",
								szMachineName,
								NULL);

						sprintf(szPIDInfo,"Machine is not enabled %s",szMachineName);
						WritePID(szMachineName,szRegId,szPIDInfo);
					}
			//Now we need to get the data of all the remote servers for the ports
			
			return 0;
		} //end of second try
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			if(NULL != pstServiceInfo)
			{
				delete [] pstServiceInfo;
				pstServiceInfo = NULL;
			}

			if(NULL != pstProcessInfo)
			{
				delete [] pstProcessInfo;
				pstProcessInfo = NULL;
			}

			if(NULL != pstPortInfo)
			{
				delete [] pstPortInfo;
				pstPortInfo = NULL;
			}

			if(NULL != pstAppInfo)
			{
				delete [] pstAppInfo;
				pstAppInfo = NULL;
			}

		}
	} //end of first try
	__finally
	{

		if(NULL != pstServiceInfo)
		{
			delete [] pstServiceInfo;
			pstServiceInfo = NULL;
		}

		if(NULL != pstProcessInfo)
		{
			delete [] pstProcessInfo;
			pstProcessInfo = NULL;
		}

		if(NULL != pstPortInfo)
		{
			delete [] pstPortInfo;
			pstPortInfo = NULL;
		}

		if(NULL != pstAppInfo)
		{
			delete [] pstAppInfo;
			pstAppInfo = NULL;
		}

	}
		
	return 0;
} //end of main

	


/////////////////////////////////////////Function Definitions///////////////////////////////////////////////


void	CheckServices(stServiceInfo* pstServiceInfo,
					  int	iNumServices)
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
	char					szReturnedStatus[50]						=	{0};

	__try
	{
		__try
		{
			if(NULL == pstServiceInfo)
			{
				return;
			}

			//Open the SCM manager
			scHandle	=	::OpenSCManager(NULL,
											SERVICES_ACTIVE_DATABASE,
											SC_MANAGER_ENUMERATE_SERVICE);
			if(NULL == scHandle)
			{
				LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								::GetLastError(),
								0,
								NULL,
								"ERR:   Open SC Manager failed",
								NULL,
								NULL);
				return;
			}

			bRet	=	::EnumServicesStatus(scHandle,
												SERVICE_WIN32,
												SERVICE_STATE_ALL,
												&enumServiceStatus,
												sizeof(enumServiceStatus),
												&dwBytesNeeded,
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
				LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								::GetLastError(),
								0,
								NULL,
								"ERR:   Enum Service Status failed",
								NULL,
								NULL);
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
				LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								::GetLastError(),
								0,
								NULL,
								"ERR:   Enum Service Status failed",
								NULL,
								NULL);
				return;
			}

			::CloseServiceHandle(scHandle);
			scHandle = NULL;
			
			lstrcpy(szTimeINIFile,g_szModulePath);
			lstrcat(szTimeINIFile,"\\Configuration\\App\\ZAppEvent.ini");

			GetPrivateProfileString("System",
									"SysUpTime",
									szDef,
									szUpTime,
									100,
									szTimeINIFile);

			_strupr(szUpTime);

			isServiceFound = SERVICE_NOT_FOUND; 

			for(iCtr = 0; iCtr < iNumServices; ++iCtr)
			{
				for(dwCtr = 0; dwCtr < dwServicesReturned; ++dwCtr)
				{
					if(0 == lstrcmpi(pstServiceInfo[iCtr].szServiceName,penumServiceStatus[dwCtr].lpServiceName))
					{
						if(SERVICE_RUNNING	== penumServiceStatus[dwCtr].ServiceStatus.dwCurrentState)
						{
								pstServiceInfo[iCtr].iStatus			=	1;
								pstServiceInfo[iCtr].iServiceExitCode	=	0;
								pstServiceInfo[iCtr].iWinExitCode		=	0;	
						}
						/*else if(pstServiceInfo[iCtr].iStatus == SERVICE_REMOVED) //we have given a chance to the service to be in run state but it isnt
						{
							//suppose in previous cycle service status marked as removed and this is the time to mark it as NOTFOUND
								pstServiceInfo[iCtr].iStatus			=	SERVICE_NOTFOUND;
								pstServiceInfo[iCtr].iServiceExitCode	=	penumServiceStatus[dwCtr].ServiceStatus.dwServiceSpecificExitCode;
								pstServiceInfo[iCtr].iWinExitCode		=	penumServiceStatus[dwCtr].ServiceStatus.dwWin32ExitCode;
								pstServiceInfo[iCtr].iEventErrorCode	=	0;
						}*/
						else
						{
							
							pstServiceInfo[iCtr].iStatus			=	0;
							pstServiceInfo[iCtr].iServiceExitCode	=	penumServiceStatus[dwCtr].ServiceStatus.dwServiceSpecificExitCode;
							pstServiceInfo[iCtr].iWinExitCode		=	penumServiceStatus[dwCtr].ServiceStatus.dwWin32ExitCode;
							pstServiceInfo[iCtr].iEventErrorCode	=	0;

							iEventId = 0;
							SearchList(g_pServiceEventRoot,penumServiceStatus[dwCtr].lpDisplayName,iEventId);
							if(0 != iEventId)
							{
								pstServiceInfo[iCtr].iEventErrorCode	=	9900000 + iEventId;
							}
							else
							{
								if('N' != szUpTime[0])
								{
									if((0 == pstServiceInfo[iCtr].iServiceExitCode)	&&
										(0 == pstServiceInfo[iCtr].iWinExitCode))
									{
										if(g_dwTickCount <= ((10 * 60) * 1000))
										{
											pstServiceInfo[iCtr].iEventErrorCode	=	9900000 + SYS_UPTIME_ERROR;
										}
									}
								}
							}
							
						}
						isServiceFound=SERVICE_FOUND;
						break;
						
					} //end of cmpare if 
				
				} //End of inner for loop
				if(isServiceFound == SERVICE_NOT_FOUND)
				{
					lstrcpy(g_szServicePrevStatusINI,g_szModulePath);
					lstrcat(g_szServicePrevStatusINI,"\\Configuration\\App\\ServicePrevStatus.ini");
					
					::GetPrivateProfileString( TEXT("LastServiceStatus"),  //section name
											   pstServiceInfo[iCtr].szServiceName,//Service name/key name
											   NULL,     //Default value
											   szReturnedStatus,
											   50,					//size of returnedStatus
											   g_szServicePrevStatusINI);  //ini file path

					if(0 == lstrcmpi(szReturnedStatus,"REMOVED") ) //if service is not in SCM DB
					{
						::WritePrivateProfileString(TEXT("LastServiceStatus"),  //section name
														pstServiceInfo[iCtr].szServiceName,//Service name/key name
														"",					//value
														g_szServicePrevStatusINI);  //ini file path

							//suppose in previous cycle service status marked as removed, Now this time, service notfound in SCM-DB hence need to mark it as NOTFOUND
								pstServiceInfo[iCtr].iStatus			=	SERVICE_NOTFOUND;
								pstServiceInfo[iCtr].iServiceExitCode	=	0;
								pstServiceInfo[iCtr].iWinExitCode		=	0;	
								pstServiceInfo[iCtr].iEventErrorCode	=	0;
					}
					else{
							//create a separate ini file ServicePrevStatus.ini
							::WritePrivateProfileString(TEXT("LastServiceStatus"),  //section name
														pstServiceInfo[iCtr].szServiceName,//Service name/key name
														"REMOVED",					//value
														g_szServicePrevStatusINI);  //ini file path
					
						//update status into structure
						pstServiceInfo[iCtr].iStatus =	SERVICE_REMOVED;

					}
				}

			} //end of for loop number of services
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


void	CheckProcess(stProcessInfo*	pstProcessInfo,
					 int	iNumProcess)
{

	DWORD	dwResult				=	0;
	//DWORD	dwPID[5500]				=	{0};
	DWORD	dwProcessCount			=	0;
	char	szFileName[300]			=	{0};
	char	szShortFileName[300]	=	{0};
	char	szShortProcessExe[101]	=	{0};
	char	szShortProcess[101]		=	{0};
	char	szProcessExe[101]		=	{0};
	char	szProcess[101]			=	{0};
	char*	pszFindChar				=	NULL;
	char*	pszStartExe				=	NULL;

	int		iEnumCtr				=	0;
	int		iCtr					=	0;

	HANDLE	hProcess				=	NULL;
	HMODULE hMod					=	NULL;
	DWORD	cbNeeded				=	0;

	HANDLE hProcessSnap						=	NULL;
	PROCESSENTRY32 pe32						=	{0};

	__try
	{
		__try
		{

			// Take a snapshot of all processes in the system.
			hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
			if( hProcessSnap == INVALID_HANDLE_VALUE )
			{
				return;
			}

			// Set the size of the structure before using it.
			pe32.dwSize = sizeof( PROCESSENTRY32 );

			// Retrieve information about the first process,
			// and exit if unsuccessful
			if( !Process32First( hProcessSnap, &pe32 ) )
			{
				CloseHandle( hProcessSnap );     // Must clean up the snapshot object!
				return;
			}

			// Now walk the snapshot of processes, and
			// display information about each process in turn
			do
			{
				memset(szProcessExe,NULL,101);
				memset(szProcess,NULL,101);

				_strupr(pe32.szExeFile);
				pszFindChar = strstr(pe32.szExeFile,".EXE");
				if(NULL != pszFindChar)
				{
					*pszFindChar = NULL;
					lstrcpy(szProcess,pe32.szExeFile);
					*pszFindChar = '.';
					lstrcpy(szProcessExe,pe32.szExeFile);
				}
				else
				{
					lstrcpy(szProcess,pe32.szExeFile);
					lstrcpy(szProcessExe,pe32.szExeFile);
				}

				for(iCtr = 0; iCtr < iNumProcess; ++iCtr)
				{
					if((0 == lstrcmpi(szProcess,pstProcessInfo[iCtr].szProcessName))			||
						(0 == lstrcmpi(szProcessExe,pstProcessInfo[iCtr].szProcessName)))
					{
						pstProcessInfo[iCtr].iStatus	=	1;
						//break;
					}
				}

			} while( Process32Next( hProcessSnap, &pe32 ) );

			CloseHandle( hProcessSnap );
			hProcessSnap = NULL;

			/*
			if(0 != EnumProcesses(dwPID,5500,&dwProcessCount))
			{
				dwProcessCount = dwProcessCount/sizeof(DWORD);
				for(iEnumCtr = 0; iEnumCtr < dwProcessCount;++iEnumCtr)
				{
					hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE,dwPID[iEnumCtr]);
					if(NULL != hProcess)
					{
						memset(szFileName,NULL,300);
						memset(szShortFileName,NULL,300);
						memset(szProcessExe,NULL,101);
						memset(szProcess,NULL,101);
						memset(szShortProcessExe,NULL,101);
						memset(szShortProcess,NULL,101);


						hMod		=	NULL;
						cbNeeded	=	0;

						if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), 
							 &cbNeeded) )
						{
							GetModuleFileNameEx( hProcess, hMod, szFileName, 
											   sizeof(szFileName)/sizeof(TCHAR) );

							if(NULL == szFileName[0])
							{
								
								GetModuleBaseName( hProcess, hMod, szFileName, 
												   sizeof(szFileName)/sizeof(TCHAR) );
							}
						}

						::CloseHandle(hProcess);
						hProcess = NULL;
						
										
						pszFindChar = strrchr(szFileName,'\\');
						if(NULL != pszFindChar)
						{
							pszStartExe = (pszFindChar + 1);
							lstrcpy(szProcessExe,pszFindChar + 1);

							strupr(pszFindChar + 1);
							pszFindChar = strstr((pszFindChar + 1),".EXE");

							if(NULL != pszFindChar)
							{
								*pszFindChar = '\0';
								lstrcpy(szProcess,pszStartExe);
								*pszFindChar = '.';
							}	

							pszStartExe = strstr(szFileName,"\\??\\");
							if(NULL != pszStartExe)
							{
								pszStartExe += lstrlen("\\??\\");
								::GetShortPathName(pszStartExe,szShortFileName,299);
							}
							else
							{
								::GetShortPathName(szFileName,szShortFileName,299);
							}


							pszFindChar = strrchr(szShortFileName,'\\');
							if(NULL != pszFindChar)
							{
								lstrcpy(szShortProcessExe,pszFindChar + 1);

								pszStartExe = (pszFindChar + 1);

								strupr(pszFindChar + 1);
								pszFindChar = strstr((pszFindChar + 1),".EXE");

								if(NULL != pszFindChar)
								{
									*pszFindChar = '\0';
									lstrcpy(szShortProcess,pszStartExe);
									*pszFindChar = '.';
								}	

							}

							for(iCtr = 0; iCtr < iNumProcess; ++iCtr)
							{
								if((0 == lstrcmpi(szProcess,pstProcessInfo[iCtr].szProcessName))			||
									(0 == lstrcmpi(szProcessExe,pstProcessInfo[iCtr].szProcessName))		||
									(0 == lstrcmpi(szShortProcess,pstProcessInfo[iCtr].szProcessName))		||
									(0 == lstrcmpi(szShortProcessExe,pstProcessInfo[iCtr].szProcessName)))
								{
									pstProcessInfo[iCtr].iStatus	=	1;
									//break;
								}
							}
						}
						else
						{
							lstrcpy(szProcessExe,szFileName);

							strupr(szFileName);
							pszFindChar = strstr(szFileName,".EXE");

							if(NULL != pszFindChar)
							{
								*pszFindChar = '\0';
								lstrcpy(szProcess,szFileName);
								*pszFindChar = '.';
							}	

							for(iCtr = 0; iCtr < iNumProcess; ++iCtr)
							{
								if((0 == lstrcmpi(szProcess,pstProcessInfo[iCtr].szProcessName))			||
									(0 == lstrcmpi(szProcessExe,pstProcessInfo[iCtr].szProcessName)))
								{
									pstProcessInfo[iCtr].iStatus	=	1;
									//break;
								}
							}
						}
					}
				}
			}
			*/
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{


		}
	}
	__finally
	{



	}
}

void	CheckPorts(stPortInfo*	pstPortInfo,
				   int	iNumPorts)
{
	PMIB_TCPTABLE	tcpTable	=	NULL;
	PMIB_UDPTABLE	udpTable	=	NULL;
	DWORD			dwSize		=	0;
	DWORD			dwError		=	0;

	int				i			=	0;
	int				iCtr		=	0;

	__try
	{
		__try
		{
			dwSize	=	0;
			dwError	=	GetTcpTable(NULL,&dwSize,TRUE);
			if(ERROR_INSUFFICIENT_BUFFER != dwError)
			{
				return;
			}
			tcpTable	=	(PMIB_TCPTABLE) malloc(dwSize);
			if(NULL == tcpTable)
			{
				return;
			}
			memset(tcpTable,NULL,dwSize);
			dwError	=	GetTcpTable(tcpTable,&dwSize,TRUE);

			if(NO_ERROR != dwError)
			{
				LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								::GetLastError(),
								0,
								NULL,
								"ERR:   Get TCP Table failed",
								NULL,
								NULL);
				return;
			}
		
			//
			// Get the table of UDP endpoints
			//
			dwSize = 0;
			dwError = GetUdpTable( NULL, &dwSize, TRUE );
			if( dwError != ERROR_INSUFFICIENT_BUFFER ) 
			{
				return;
			}
			udpTable = (PMIB_UDPTABLE) malloc( dwSize );
			if(NULL == udpTable)
			{
				return;
			}
			memset(udpTable,NULL,dwSize);

			dwError = GetUdpTable( udpTable, &dwSize, TRUE );
			if(NO_ERROR != dwError)
			{
				LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								::GetLastError(),
								0,
								NULL,
								"ERR:   Get UDP Table failed",
								NULL,
								NULL);
				return;
			}			


			for( i = 0; i < tcpTable->dwNumEntries; i++ ) 
			{
				if(2 == tcpTable->table[i].dwState)
				{
					for(iCtr = 0; iCtr < iNumPorts; ++iCtr)
					{
						if(pstPortInfo[iCtr].iPort == htons(tcpTable->table[i].dwLocalPort))
						{
							pstPortInfo[iCtr].iStatus	=	1;
							//break;
						}
					}
				}
			}


			for( i = 0; i < udpTable->dwNumEntries; i++ ) 
			{
				for(iCtr = 0; iCtr < iNumPorts; ++iCtr)
				{
					if(pstPortInfo[iCtr].iPort == htons(udpTable->table[i].dwLocalPort))
					{
						pstPortInfo[iCtr].iStatus	=	1;
						//break;
					}
				}
			}

		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{


		}
	}
	__finally
	{
		if(NULL != tcpTable)
		{
			free(tcpTable);
			tcpTable = NULL;
		}

		if(NULL != udpTable)
		{
			free(udpTable);
			udpTable = NULL;
		}
	}
}


int		ReadXMLConfigFile(char*	pszXMLFile,
							stAppInfo*&			pstAppInfo,
							int&				iNumAppInfo,
							stServiceInfo*&		pstServiceInfo,
							int&				iNumServices,		
							stProcessInfo*&		pstProcessInfo,
							int&				iNumProcess,
							stPortInfo*&		pstPortInfo,
							int&				iNumPorts)
{
	DWORD			dwBytesNeeded		=	0;
	DWORD			dwBytesAllocated	=	0;
	DWORD			dwBytesRead			=	0;
	DWORD			dwFileSize			=	0;

	char*			pszBuffer			=	0;
	char*			pszFindChar			=	0;
	char*			pszStartAdd			=	0;
	char*			pszComma			=	0;


	HANDLE			hFile				=	NULL;

	char			szAppId[51]			=	{0};
	char			szTempBuffer[256]	=	{0};

	int				iCtr				=	0;

	__try
	{
		__try
		{
			iNumServices	=	0;
			iNumProcess		=	0;
			iNumPorts		=	0;
			iNumAppInfo		=	0;

			if(0 == ::SetFileAttributes(pszXMLFile,FILE_ATTRIBUTE_NORMAL))
			{
				LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								::GetLastError(),
								0,
								NULL,
								"ERR:   Open File failed of XML Configuration",
								pszXMLFile,
								NULL);
				return 0;
			}

			hFile = ::CreateFile(pszXMLFile,
									GENERIC_READ,
									FILE_SHARE_READ,
									NULL,
									OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL,
									NULL);

			if((hFile == INVALID_HANDLE_VALUE)		||
				(hFile == NULL))
			{
				//Unable to open file, return from her
				hFile = NULL;
				LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								::GetLastError(),
								0,
								NULL,
								"ERR:   Open File failed of XML Configuration",
								pszXMLFile,
								NULL);
				return 0;
			}

			dwFileSize = ::GetFileSize(hFile,NULL);

			if(0 == dwFileSize)
			{
				::CloseHandle(hFile);
				hFile = NULL;

				LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								0,
								0,
								NULL,
								"ERR:   XML file is corrupt",
								pszXMLFile,
								NULL);

				return -1;
			}

			pszBuffer	=	new char[(dwFileSize * 2) + 100];
			if(NULL == pszBuffer)
			{
				return 0;
			}
			memset(pszBuffer,NULL,(dwFileSize * 2) + 100);

			::ReadFile(hFile,pszBuffer,dwFileSize,&dwBytesRead,NULL);

			::CloseHandle(hFile);
			hFile = NULL;

			_strlwr(pszBuffer);


			pszFindChar	=	pszBuffer;

			do
			{
				pszFindChar	=	strstr(pszFindChar,"<app name=\"");
				if(NULL != pszFindChar)
				{
					++iNumAppInfo;

					////////////////////////////For PORTS///////////////////////////////////
					pszFindChar = strstr(pszFindChar,"port=\"");
					if(NULL != pszFindChar)
					{
						//get the port
						pszStartAdd = pszFindChar + lstrlen("port=\"");

						//get the port second quotes
						pszFindChar = pszFindChar + lstrlen("port=\"");
						pszFindChar = strstr(pszFindChar,"\"");
						if(NULL == pszFindChar)
						{
							iNumAppInfo		=	0;
							iNumPorts		=	0;
							iNumProcess		=	0;
							iNumServices	=	0;	

							LogLastError(g_szErrorLogPath,
											9999,
											NULL,
											0,
											0,
											NULL,
											"ERR:   XML file is corrupt for ports",
											pszXMLFile,
											NULL);
							return -1;
						}
						*pszFindChar	=	NULL;

						if(NULL != *pszStartAdd)
						{
							++iNumPorts;
							pszComma = pszStartAdd;
							do
							{
								pszComma = strchr(pszComma,',');
								if(NULL != pszComma)
								{
									++iNumPorts;
									pszComma = pszComma + 1;
								}
							}while(NULL != pszComma);
						}
						*pszFindChar	=	'"';
					}
					else
					{
						iNumAppInfo		=	0;
						iNumPorts		=	0;
						iNumProcess		=	0;
						iNumServices	=	0;	

						LogLastError(g_szErrorLogPath,
										9999,
										NULL,
										0,
										0,
										NULL,
										"ERR:   XML file is corrupt for ports",
										pszXMLFile,
										NULL);
						return -1;
					}

					////////////////////////////For PROCESS///////////////////////////////////
					pszFindChar = strstr(pszFindChar,"process=\"");
					if(NULL != pszFindChar)
					{
						//get the process
						pszStartAdd = pszFindChar + lstrlen("process=\"");

						//get the process second quotes
						pszFindChar = pszFindChar + lstrlen("process=\"");
						pszFindChar = strstr(pszFindChar,"\"");
						if(NULL == pszFindChar)
						{
							iNumAppInfo		=	0;
							iNumPorts		=	0;
							iNumProcess		=	0;
							iNumServices	=	0;	

							LogLastError(g_szErrorLogPath,
											9999,
											NULL,
											0,
											0,
											NULL,
											"ERR:   XML file is corrupt for process",
											pszXMLFile,
											NULL);
							return -1;
						}

						*pszFindChar	=	NULL;


						if(NULL != *pszStartAdd)
						{
							++iNumProcess;
							pszComma = pszStartAdd;
							do
							{
								pszComma = strchr(pszComma,',');
								if(NULL != pszComma)
								{
									++iNumProcess;
									pszComma = pszComma + 1;
								}
							}while(NULL != pszComma);
						}
						*pszFindChar	=	'"';
					}
					else
					{
						iNumAppInfo		=	0;
						iNumPorts		=	0;
						iNumProcess		=	0;
						iNumServices	=	0;	

						LogLastError(g_szErrorLogPath,
										9999,
										NULL,
										0,
										0,
										NULL,
										"ERR:   XML file is corrupt for process",
										pszXMLFile,
										NULL);
						return -1;
					}

					////////////////////////////////For SERVICE////////////////////////////////////
					pszFindChar = strstr(pszFindChar,"service=\"");
					if(NULL != pszFindChar)
					{
						//get the service
						pszStartAdd = pszFindChar + lstrlen("service=\"");

						//get the service second quotes
						pszFindChar = pszFindChar + lstrlen("service=\"");
						pszFindChar = strstr(pszFindChar,"\"");
						if(NULL == pszFindChar)
						{
							iNumAppInfo		=	0;
							iNumPorts		=	0;
							iNumProcess		=	0;
							iNumServices	=	0;	

							LogLastError(g_szErrorLogPath,
											9999,
											NULL,
											0,
											0,
											NULL,
											"ERR:   XML file is corrupt for services",
											pszXMLFile,
											NULL);
							return -1;
						}

						*pszFindChar	=	NULL;


						if(NULL != *pszStartAdd)
						{
							++iNumServices;
							pszComma = pszStartAdd;
							do
							{
								pszComma = strchr(pszComma,',');
								if(NULL != pszComma)
								{
									++iNumServices;
									pszComma = pszComma + 1;
								}
							}while(NULL != pszComma);
						}
						*pszFindChar	=	'"';
					}
					else
					{
						iNumAppInfo		=	0;
						iNumPorts		=	0;
						iNumProcess		=	0;
						iNumServices	=	0;	

						LogLastError(g_szErrorLogPath,
										9999,
										NULL,
										0,
										0,
										NULL,
										"ERR:   XML file is corrupt for services",
										pszXMLFile,
										NULL);
						return -1;
					}


					pszFindChar = pszFindChar + 1;
				}

			}while(NULL != pszFindChar);


			if((0 == iNumServices)		&&
				(0 == iNumProcess)		&&
				(0 == iNumPorts))
			{
				LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								0,
								0,
								NULL,
								"ERR:   XML file is corrupt",
								pszXMLFile,
								NULL);

				return -1;
			}

			if(iNumAppInfo > 0)
			{
				pstAppInfo	=	new	stAppInfo[iNumAppInfo + 1];
				if(NULL == pstAppInfo)
				{
					return 0;
				}
				memset(pstAppInfo,NULL,sizeof(stAppInfo) * (iNumAppInfo + 1));
			}

			if(iNumServices > 0)
			{
				pstServiceInfo	=	new stServiceInfo[iNumServices + 1];
				if(NULL == pstServiceInfo)
				{
					return 0;
				}

				memset(pstServiceInfo,NULL,sizeof(stServiceInfo) * (iNumServices + 1));
			}

			if(iNumProcess > 0)
			{
				pstProcessInfo	=	new stProcessInfo[iNumProcess + 1];
				if(NULL == pstProcessInfo)
				{
					return 0;
				}

				memset(pstProcessInfo,NULL,sizeof(stProcessInfo) * (iNumProcess + 1));
			}

			if(iNumPorts > 0)
			{
				pstPortInfo	=	new stPortInfo[iNumPorts + 1];
				if(NULL == pstPortInfo)
				{
					return 0;
				}

				memset(pstPortInfo,NULL,sizeof(stPortInfo) * (iNumPorts + 1));
			}


			//Now fill up the structures

			iNumServices	=	0;
			iNumProcess		=	0;
			iNumPorts		=	0;
			iNumAppInfo		=	0;

			pszFindChar	=	pszBuffer;
			do
			{
				pszFindChar	=	strstr(pszFindChar,"<app name=\"");
				if(NULL != pszFindChar)
				{
					memset(szAppId,NULL,51);
					pszStartAdd = pszFindChar + lstrlen("<app name=\"");
					pszFindChar = pszFindChar + lstrlen("<app name=\"");

					pszFindChar = strstr(pszFindChar,"\"");

					*pszFindChar	=	NULL;

					lstrcpy(szAppId,pszStartAdd);
					StrTrim(szAppId," ");
					lstrcpy(pstAppInfo[iNumAppInfo].szAppId,szAppId);
					++iNumAppInfo;
					*pszFindChar	=	'"';

					
					pszFindChar = strstr(pszFindChar,"port=\"");
					if(NULL != pszFindChar)
					{
						//get the port
						pszStartAdd = pszFindChar + lstrlen("port=\"");

						//get the port second quotes
						pszFindChar = pszFindChar + lstrlen("port=\"");
						pszFindChar = strstr(pszFindChar,"\"");

						*pszFindChar	=	NULL;


						if(NULL != *pszStartAdd)
						{
							pszComma = pszStartAdd;
							do
							{
								pszComma = strchr(pszComma,',');
								if(NULL != pszComma)
								{
									*pszComma = NULL;

									if(NULL != *pszStartAdd)
									{
										memset(szTempBuffer,NULL,256);
										lstrcpy(szTempBuffer,pszStartAdd);
										//Check for spaces
										StrTrim(szTempBuffer," ");

										if(NULL != *szTempBuffer)
										{
											pstPortInfo[iNumPorts].iPort	=	atoi(szTempBuffer);
											if(pstPortInfo[iNumPorts].iPort > 0)
											{
												lstrcpy(pstPortInfo[iNumPorts].szAppId,szAppId);
												++iNumPorts;
											}
										}
									}
									*pszComma = ',';

									pszComma = pszComma + 1;
									pszStartAdd = pszComma;
								}
								else
								{
									memset(szTempBuffer,NULL,256);
									lstrcpy(szTempBuffer,pszStartAdd);
									//Check for spaces
									StrTrim(szTempBuffer," ");

									if(NULL != *szTempBuffer)
									{
										pstPortInfo[iNumPorts].iPort	=	atoi(szTempBuffer);
										if(pstPortInfo[iNumPorts].iPort > 0)
										{
											lstrcpy(pstPortInfo[iNumPorts].szAppId,szAppId);
											++iNumPorts;
										}
									}
								}
							}while(NULL != pszComma);
						}
						*pszFindChar	=	'"';
					}
					pszFindChar = strstr(pszFindChar,"process=\"");
					if(NULL != pszFindChar)
					{
						//get the process
						pszStartAdd = pszFindChar + lstrlen("process=\"");

						//get the process second quotes
						pszFindChar = pszFindChar + lstrlen("process=\"");
						pszFindChar = strstr(pszFindChar,"\"");

						*pszFindChar	=	NULL;


						if(NULL != *pszStartAdd)
						{
							pszComma = pszStartAdd;
							do
							{
								pszComma = strchr(pszComma,',');
								if(NULL != pszComma)
								{
									*pszComma = NULL;

									if(NULL != *pszStartAdd)
									{
										memset(szTempBuffer,NULL,256);
										lstrcpy(szTempBuffer,pszStartAdd);
										//Check for spaces
										StrTrim(szTempBuffer," ");

										if(NULL != *szTempBuffer)
										{
											lstrcpy(pstProcessInfo[iNumProcess].szProcessName,szTempBuffer);
											lstrcpy(pstProcessInfo[iNumProcess].szAppId,szAppId);
											++iNumProcess;
										}
									}
									*pszComma = ',';

									pszComma = pszComma + 1;
									pszStartAdd = pszComma;
								}
								else
								{
									memset(szTempBuffer,NULL,256);
									lstrcpy(szTempBuffer,pszStartAdd);
									//Check for spaces
									StrTrim(szTempBuffer," ");

									if(NULL != *szTempBuffer)
									{
										lstrcpy(pstProcessInfo[iNumProcess].szProcessName,szTempBuffer);
										lstrcpy(pstProcessInfo[iNumProcess].szAppId,szAppId);
										++iNumProcess;
									}
								}
							}while(NULL != pszComma);
						}
						*pszFindChar	=	'"';
					}

					pszFindChar = strstr(pszFindChar,"service=\"");
					if(NULL != pszFindChar)
					{
						//get the service
						pszStartAdd = pszFindChar + lstrlen("service=\"");

						//get the service second quotes
						pszFindChar = pszFindChar + lstrlen("service=\"");
						pszFindChar = strstr(pszFindChar,"\"");

						*pszFindChar	=	NULL;

						if(NULL != *pszStartAdd)
						{
							pszComma = pszStartAdd;
							do
							{
								pszComma = strchr(pszComma,',');
								if(NULL != pszComma)
								{
									*pszComma = NULL;
									if(NULL != *pszStartAdd)
									{
										memset(szTempBuffer,NULL,256);
										lstrcpy(szTempBuffer,pszStartAdd);
										//Check for spaces
										StrTrim(szTempBuffer," ");

										if(NULL != *szTempBuffer)
										{
											lstrcpy(pstServiceInfo[iNumServices].szServiceName,szTempBuffer);
											lstrcpy(pstServiceInfo[iNumServices].szAppId,szAppId);
											++iNumServices;
										}
									}
									*pszComma = ',';

									pszComma = pszComma + 1;
									pszStartAdd = pszComma;
								}
								else
								{
									memset(szTempBuffer,NULL,256);
									lstrcpy(szTempBuffer,pszStartAdd);
									//Check for spaces
									StrTrim(szTempBuffer," ");

									if(NULL != *szTempBuffer)
									{
										lstrcpy(pstServiceInfo[iNumServices].szServiceName,szTempBuffer);
										lstrcpy(pstServiceInfo[iNumServices].szAppId,szAppId);
										++iNumServices;
									}
								}
							}while(NULL != pszComma);
						}
						*pszFindChar	=	'"';
					}

					pszFindChar = pszFindChar + 1;
				}

			}while(NULL != pszFindChar);

			if((0 == iNumServices)		&&
				(0 == iNumProcess)		&&
				(0 == iNumPorts))
			{
				LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								0,
								0,
								NULL,
								"ERR:   XML file is corrupt",
								pszXMLFile,
								NULL);
				return -1;
			}
			else
			{
				//Check the APPIDS
				for(iCtr = 0; iCtr < iNumAppInfo; iCtr++)
				{
					if(NULL == pstAppInfo[iCtr].szAppId[0])
					{
						iNumAppInfo		=	0;
						iNumPorts		=	0;
						iNumProcess		=	0;
						iNumServices	=	0;	

						LogLastError(g_szErrorLogPath,
										9999,
										NULL,
										0,
										0,
										NULL,
										"ERR:   XML file is corrupt for App ids",
										pszXMLFile,
										NULL);

						return -1;
					}
				}

				//Check the services
				for(iCtr = 0; iCtr < iNumServices; iCtr++)
				{

					if(NULL == pstServiceInfo[iCtr].szServiceName[0])
					{
						iNumAppInfo		=	0;
						iNumPorts		=	0;
						iNumProcess		=	0;
						iNumServices	=	0;	

						LogLastError(g_szErrorLogPath,
										9999,
										NULL,
										0,
										0,
										NULL,
										"ERR:   XML file is corrupt for services",
										pszXMLFile,
										NULL);

						return -1;
					}
				}

				//Check the process
				for(iCtr = 0; iCtr < iNumProcess; iCtr++)
				{
					if(NULL == pstProcessInfo[iCtr].szProcessName[0])
					{
						iNumAppInfo		=	0;
						iNumPorts		=	0;
						iNumProcess		=	0;
						iNumServices	=	0;	

						LogLastError(g_szErrorLogPath,
										9999,
										NULL,
										0,
										0,
										NULL,
										"ERR:   XML file is corrupt for process",
										pszXMLFile,
										NULL);

						return -1;
					}
				}

				//Check the ports
				for(iCtr = 0; iCtr < iNumPorts; iCtr++)
				{
					if(pstPortInfo[iCtr].iPort <= 0)
					{
						iNumAppInfo		=	0;
						iNumPorts		=	0;
						iNumProcess		=	0;
						iNumServices	=	0;	

						LogLastError(g_szErrorLogPath,
										9999,
										NULL,
										0,
										0,
										NULL,
										"ERR:   XML file is corrupt for ports",
										pszXMLFile,
										NULL);

						return -1;
					}
				}

			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			iNumAppInfo		=	0;
			iNumPorts		=	0;
			iNumProcess		=	0;
			iNumServices	=	0;	

			LogLastError(g_szErrorLogPath,
							9999,
							NULL,
							0,
							0,
							NULL,
							"ERR:   XML file is corrupt",
							pszXMLFile,
							NULL);
			return -1;
		}
	}
	__finally
	{
		if(NULL != hFile)
		{
			::CloseHandle(hFile);
			hFile = NULL;
		}

		if(NULL != pszBuffer)
		{
			delete [] pszBuffer;
			pszBuffer = NULL;
		}
	}
	return 1;
}


void GetDateFormatYYYYMMDD(char * szDate)
{
	int		iDateSize		=	0;
	int		iDateTimeSize	=	0;
	char	szAMPM[11]		=	{0};	

	SYSTEMTIME		sysTime;

	__try
	{
		__try
		{
			if((NULL == szDate))
			{
				return;
			}

			memset(&sysTime,NULL,sizeof(sysTime));	
			::GetLocalTime(&sysTime);

			sprintf(szDate,
					"%04d%02d%02d",
					sysTime.wYear,
					sysTime.wMonth,
					sysTime.wDay);

		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			return;
		}
	}

	__finally
	{



	}
	return;

}

void	GetTempFileName(char* pszDateTime)
{
	SYSTEMTIME		sysTime;
	DWORD			dwThreadId		=	0;

	__try
	{
		__try
		{
			//Init the Systime
			memset(&sysTime,NULL,sizeof(SYSTEMTIME));	


			//get the local time
			::GetLocalTime(&sysTime);

			dwThreadId	=	::GetCurrentThreadId();


			//format the date and time according to the requirement
			sprintf(pszDateTime,
					"%04d%02d%02d%02d%02d%04d%d",
					sysTime.wYear,
					sysTime.wMonth,
					sysTime.wDay,
					sysTime.wHour,
					sysTime.wMinute,
					sysTime.wSecond,
					sysTime.wMilliseconds,
					dwThreadId);

		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			return;
		}
	}

	__finally
	{



	}
}

DWORD	GetAlertDateTime(char* szDateTime)
{
	int		iDateSize		=	0;
	int		iDateTimeSize	=	0;
	char am_pm[] = "AM\0";


	SYSTEMTIME		sysTime;

	__try
	{
		__try
		{
			if((NULL == szDateTime))
			{
				return 1;
			}

			memset(&sysTime,NULL,sizeof(sysTime));	


			::GetLocalTime(&sysTime);

			if( sysTime.wHour > 12)
				strcpy( am_pm, "PM\0" );
			if(sysTime.wHour == 12)
			{				
				//WORD nSeconds = sysTime.wSecond;
				//if( nSeconds > 0 || sysTime.wMinute > 0)
					strcpy( am_pm, "PM\0" );
			}
			if( sysTime.wHour > 12 )        // Convert from 24-hour to 12-hour clock.
				sysTime.wHour -= 12;		
			if( sysTime.wHour == 0 )        // Set hour to 12 if midnight.
				sysTime.wHour = 12;

			sprintf(szDateTime,
					"%02d/%02d/%04d %02d:%02d:%02d %s\0",
					sysTime.wMonth,
					sysTime.wDay,
					sysTime.wYear,
					sysTime.wHour,
					sysTime.wMinute,
					sysTime.wSecond,
					am_pm);
		}

		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			return 1;
		}
	}


	__finally
	{



	}
	return 0;
}


void GetDateFormatYYYYMM(char * szDate)
{
	int		iDateSize		=	0;
	int		iDateTimeSize	=	0;
	char	szAMPM[11]		=	{0};	

	SYSTEMTIME		sysTime;

	__try
	{
		__try
		{
			if((NULL == szDate))
			{
				return;
			}

			memset(&sysTime,NULL,sizeof(sysTime));	
			::GetLocalTime(&sysTime);

			sprintf(szDate,
					"%04d%02d",
					sysTime.wYear,
					sysTime.wMonth);
		}

		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			return;
		}
	}

	__finally
	{

	}
	return;
}

DWORD	GetDateTime(char*	szDate, char* szDateTime)
{
	int		iDateSize		=	0;
	int		iDateTimeSize	=	0;
	char	szAMPM[11]		=	{0};	

	SYSTEMTIME		sysTime;

	__try
	{
		__try
		{
			if((NULL == szDate)	||
				(NULL == szDateTime))
			{
				return 1;
			}

			memset(&sysTime,NULL,sizeof(sysTime));	


			::GetLocalTime(&sysTime);



			sprintf(szDateTime,
					"%04d/%02d/%02d %02d:%02d:%02d",
					sysTime.wYear,
					sysTime.wMonth,
					sysTime.wDay,
					sysTime.wHour,
					sysTime.wMinute,
					sysTime.wSecond);

			sprintf(szDate,
					"%04d%02d%02d",
					sysTime.wYear,
					sysTime.wMonth,
					sysTime.wDay);

		}

		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			return 1;
		}
	}

	__finally
	{



	}
	return 0;
}

void	GetLogDateTime(char* szDateTime)
{
	SYSTEMTIME		sysTime;

	__try
	{
		__try
		{
			if(NULL == szDateTime)
			{
				return;
			}

			memset(&sysTime,NULL,sizeof(sysTime));	


			::GetLocalTime(&sysTime);

			//YYYY-MM-DD HH:MM:SS 

			sprintf(szDateTime,
					"%04d-%02d-%02d %02d:%02d:%02d",
					sysTime.wYear,
					sysTime.wMonth,
					sysTime.wDay,
					sysTime.wHour,
					sysTime.wMinute,
					sysTime.wSecond);
		}

		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			return;
		}
	}

	__finally
	{



	}
}


DWORD	CreateLogDirectory(char* szDirectoryName)
{
	int	iLen	=	0;
	int iCtr	=	0;	
	try
	{
		::SetFileAttributes(szDirectoryName,FILE_ATTRIBUTE_NORMAL);
		iLen = lstrlen(szDirectoryName);

		for(iCtr = 0; iCtr < iLen; ++iCtr)
		{
			if('\\' == szDirectoryName[iCtr])
			{
				szDirectoryName[iCtr] = NULL;
				::CreateDirectory(szDirectoryName,NULL);
				szDirectoryName[iCtr] = '\\';
			}
		}
		::CreateDirectory(szDirectoryName,NULL);
	}
	catch(...)
	{
	}
	return 0;
}

DWORD	LogDataToDisk(TCHAR *szModuleFilePath,
							TCHAR*	szData,
							BOOL bAlertData)
{
	TCHAR		szFullPathDirectory[MAX_PATH + 100]				=	{0};
	TCHAR		szFileName[MAX_PATH + 100]						=	{0};	
	TCHAR		szLogPath[MAX_PATH + 100]						=	{0};
	TCHAR		szTempLogPath[MAX_PATH + 100]					=	{0};
	TCHAR		szFullLogPath[MAX_PATH + 100]					=	{0};
	TCHAR		szMutexName[MAX_PATH + 100]						=	{0};
	TCHAR		szLogFileMapName[MAX_PATH + 100]				=	{0};
	TCHAR*		lpTraverser									=	NULL;
	TCHAR*		szFindChar									=	NULL;
	TCHAR*		pszString									=	NULL;
	TCHAR*		pszFinalError								=	NULL;
	TCHAR		chString									=	NULL;

	HANDLE		hFile										=	NULL;
	HANDLE		hMutex										=	NULL;

	DWORD		nResult										=	0;
	DWORD		dwNumOfBytesWritten							=	0;

	long		lRet										=	0;
	int			iCtr										=	0;
	int			iSubCtr										=	0;
	int			iLen										=	0;	

	BOOL		bFirstCreate								=	FALSE;

	__try
	{
		__try
		{
			if((NULL == szData)	||
				(NULL == szModuleFilePath))
			{
				return 1;
			}


			iLen = _tcslen(szModuleFilePath);
			if(0 == iLen)
			{
				return 1;
			}
			_tcscat(szLogFileMapName,_T("FILE_MAP_"));
			_tcscat(szMutexName,_T("FILE_MUTEX_"));

			for(iCtr = 0; iCtr < iLen; ++iCtr)
			{
				pszString = _tcsninc(szModuleFilePath,iCtr);	
				chString = _tcsnextc(pszString);
				if(_T('\\') == chString)
				{
					_tcscat(szLogFileMapName,_T("-"));
					_tcscat(szMutexName,_T("-"));
				}
				else
				{
					_tcsncat(szLogFileMapName,pszString,1);
					_tcsncat(szMutexName,pszString,1);
				}
			}


			//Create the Mutex and wait
			hMutex = CreateMutex(NULL,FALSE,szMutexName);
			if(NULL == hMutex)
			{
				return 1;
			}

			nResult = WaitForSingleObject(hMutex,(300 * 1000));
			if(WAIT_OBJECT_0 != nResult)
			{
				::ReleaseMutex(hMutex);
				CloseHandle(hMutex);
				hMutex = NULL;
				return 1;
			}
			

			_tcscpy(szFullLogPath,szModuleFilePath);
			//Remove the file name
			szFindChar = strrchr(szFullLogPath,'\\');
			if(NULL != szFindChar)
			{
				_tcscpy(szFileName,szFindChar);
				*szFindChar = NULL;
				//++szFindChar;
			}


			_tcscpy(szFullPathDirectory,szFullLogPath);
			_tcscat(szFullPathDirectory,szFileName);

			if(0 == ::SetFileAttributes(szFullPathDirectory,FILE_ATTRIBUTE_NORMAL))
			{
				CreateLogDirectory(szFullLogPath);
				bFirstCreate = TRUE;
			}
		
			//::SetFileAttributes(szFullPathDirectory,FILE_ATTRIBUTE_NORMAL);

			//Create or opens the existing log file
			hFile = CreateFile(szFullPathDirectory,
								GENERIC_READ | GENERIC_WRITE,
								FILE_SHARE_READ,
								NULL,
								OPEN_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
					
			if (hFile == INVALID_HANDLE_VALUE)
			{
				return 1;
			}


			
			if(TRUE == bFirstCreate)
			{
				if(FALSE == bAlertData)
				{
					::WriteFile(hFile,
								"DateTime\tApplication\tApp_Status\tMonitor\tCounter\tCounterStatus\r\n",
								lstrlen("DateTime\tApplication\tApp_Status\tMonitor\tCounter\tCounterStatus\r\n"),
								&dwNumOfBytesWritten,
								NULL);

					dwNumOfBytesWritten = 0;
				}
			}
			
			
			SetFilePointer(hFile, 0, NULL, FILE_END);

			::WriteFile(hFile,szData,lstrlen(szData),&dwNumOfBytesWritten,NULL); //Formatting the log to make it readable
			
			CloseHandle(hFile);
			hFile = NULL;


			::ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			hMutex = NULL;



			return 0;
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			//Its better to flush all the buffers
			//FlushFileBuffers(hFile);

			//Close the file handle
			if(NULL != hFile)
			{
				CloseHandle(hFile);
				hFile = NULL;
			}


			if(NULL != hMutex)
			{
				::ReleaseMutex(hMutex);
				CloseHandle(hMutex);
				hMutex = NULL;
			}

			return 1;
		}
	}

	__finally
	{
		//Its better to flush all the buffers
		//FlushFileBuffers(hFile);

		//Close the file handle
		if(NULL != hFile)
		{
			CloseHandle(hFile);
			hFile = NULL;
		}

		if(NULL != hMutex)
		{
			::ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			hMutex = NULL;
		}

	}
	return 0;
}


HRESULT WriteToBuffer(LPTSTR	&lpszBuffer,	//[in,out]Destination pointer where the buffer is located
						LPCTSTR lpszString,		//[in]String to be copied to the buffer
						BOOL bRefresh)			//[in]Initialize the buffer if it is TRUE, default is FALSE
{
	DWORD dwSize = 0;
	try
	{
		if(NULL == lpszString)
		{
			return E_POINTER;
		}

		//Get the actual size of the string
		dwSize = _tcslen(lpszString);

		if(NULL == lpszBuffer)
		{
			//allocate the required initial memory
			DWORD dwMemoryReq = g_dwExtraBuffer + dwSize;

			lpszBuffer = new TCHAR [dwMemoryReq + 1];
			if(NULL == lpszBuffer)
			{
				//allocation failed
				return ERROR_NOT_ENOUGH_MEMORY;
			}

			//allocation successful, set the memory
			//_tcsnset(lpszBuffer,_T('\0'),dwMemoryReq);

			memset(lpszBuffer,_T('\0'),dwMemoryReq);
			//copy the source string to the destination
			_tcscpy(lpszBuffer,lpszString);
		}
		else
		{
			//Check for the validity of the lpszBuffer pointer
			BOOL bRet1 = IsBadWritePtr(lpszBuffer,1);
			BOOL bRet2 = IsBadReadPtr(lpszBuffer,1);
			if(0 != bRet1 ||
				0 != bRet2)
			{
				//bad memory pointer, allocate a new memory
				DWORD dwMemoryReq = g_dwExtraBuffer + dwSize;
				lpszBuffer = new TCHAR [dwMemoryReq + 1];
				if(NULL == lpszBuffer)
				{
					return ERROR_NOT_ENOUGH_MEMORY;
				}
				//allocation successful, set the memory
				//_tcsnset(lpszBuffer,_T('\0'),dwMemoryReq);
				memset(lpszBuffer,_T('\0'),dwMemoryReq);
			}

			//see whether the current memory is sufficient to add the string to the buffer
			DWORD dwActualSize = _msize(lpszBuffer);
			--dwActualSize;
			DWORD dwSizeUsed = _tcslen(lpszBuffer);

			//Initialize the buffer if set to TRUE
			if(TRUE == bRefresh)
			{
				//_tcsnset(lpszBuffer,_T('\0'),dwActualSize);
				memset(lpszBuffer,_T('\0'),dwActualSize);
				dwSizeUsed = 0;
			}
			
			//calculate the required size for the store string & the string to be stored
			DWORD dwReqSize = dwSizeUsed + dwSize + 1;
			if(dwReqSize > dwActualSize)
			{
				//Memory is insufficient allocate a new block
				DWORD dwMemoryReq = g_dwExtraBuffer + dwReqSize;

				LPTSTR lpszTemp = new TCHAR[dwMemoryReq + 1];
				if(NULL == lpszTemp)
				{
					//allocation failed
					return ERROR_NOT_ENOUGH_MEMORY;
				}
				//allocation found successful, initialize the mem block 
				//_tcsnset(lpszTemp,_T('\0'),dwMemoryReq);
				memset(lpszTemp,_T('\0'),dwMemoryReq);

				//copy the old string back to the newly allocated block
				_tcscpy(lpszTemp,lpszBuffer);
				
				//delete the old allocated block
				try
				{
					delete [] lpszBuffer;
				}
				catch(...)
				{
					//Something has gone wrong while deleting, it might be Corrupt Memory
					lpszBuffer = NULL;
					//Store the newly allocated memory
					lpszBuffer = lpszTemp;

					//copy the final string
					_tcscpy(lpszBuffer,lpszString);
					return S_OK;
				}
				lpszBuffer = NULL;
				lpszBuffer = lpszTemp;

				//copy the final string
				_tcscat(lpszBuffer,lpszString);
			}
			else
			{
				//Memory is sufficient copy the string
				_tcscat(lpszBuffer,lpszString);
			}
		}
	}
	catch(...)
	{
		//some thing has gone wrong ???????
		return E_FAIL;
	}
	return S_OK;
}

BOOL EnablePrivilege(HANDLE hToken, LPCTSTR szPrivName, BOOL fEnable) 
{
	try
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, szPrivName, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		return((GetLastError() == ERROR_SUCCESS));
	}
	catch(...)
	{

	}
}

void	WriteCheckedData(char*	pszMachineName,
							stServiceInfo*&		pstServiceInfo,
							int&				iNumServices,		
							stProcessInfo*&		pstProcessInfo,
							int&				iNumProcess,
							stPortInfo*&		pstPortInfo,
							int&				iNumPorts)

{
	char*		pszFinalData						=	NULL;
	char*		pszDataFile							=	NULL;
	char		szPort[25]							=	{0};

	char		szFinalLogFile[MAX_PATH + 100]		=	{0};
	char		szYYYYMM[20]						=	{0};
	char		szYYYYMMDD[20]						=	{0};
	char		szLogDateTime[51]					=	{0};

	int			iCtr								=	0;

	__try
	{
		__try
		{
			//Put the data in the data directory
			GetDateFormatYYYYMM(szYYYYMM);
			GetDateFormatYYYYMMDD(szYYYYMMDD);
			GetLogDateTime(szLogDateTime);

			lstrcpy(szFinalLogFile,g_szModulePath);
			lstrcat(szFinalLogFile,"\\DataDirectory\\");
			lstrcat(szFinalLogFile,pszMachineName);
			lstrcat(szFinalLogFile,"\\");
			lstrcat(szFinalLogFile,szYYYYMM);
			lstrcat(szFinalLogFile,"\\APP\\APP");
			lstrcat(szFinalLogFile,szYYYYMMDD);
			lstrcat(szFinalLogFile,".log");



			pszFinalData = new char[1024];

			if(NULL == pszFinalData)
			{
				return;
			}

			//First log the services

			for(iCtr = 0; iCtr < iNumServices; ++iCtr)
			{
				memset(pszFinalData,NULL,1024);

				lstrcpy(pszFinalData,szLogDateTime);
				lstrcat(pszFinalData,"\t");
				lstrcat(pszFinalData,pstServiceInfo[iCtr].szAppId);
				if(SERVICE_GOOD == pstServiceInfo[iCtr].iStatus)
				{
					lstrcat(pszFinalData,"\tGOOD\tSVC\t");
				}
				else if(SERVICE_REMOVED == pstServiceInfo[iCtr].iStatus )
				{
					lstrcat(pszFinalData,"\tREMOVED\tSVC\t");
				}
				else if(SERVICE_NOTFOUND == pstServiceInfo[iCtr].iStatus )
				{
					lstrcat(pszFinalData,"\tNOTFOUND\tSVC\t");
				}
				else
				{
					lstrcat(pszFinalData,"\tERROR\tSVC\t");
				}
				lstrcat(pszFinalData,pstServiceInfo[iCtr].szServiceName);


				if(1 == pstServiceInfo[iCtr].iStatus)
				{
					lstrcat(pszFinalData,"\tON\t\r\n");
				}
				else
				{
					lstrcat(pszFinalData,"\tOFF\t\r\n");
				}

				WriteToBuffer(pszDataFile,pszFinalData);
			}
			if(NULL != pszDataFile)
			{
				if(NULL != pszDataFile[0])
				{
					LogDataToDisk(szFinalLogFile, pszDataFile);
				}
			}


			//Now log for the process
			WriteToBuffer(pszDataFile,"",TRUE);

			for(iCtr = 0; iCtr < iNumProcess; ++iCtr)
			{
				memset(pszFinalData,NULL,1024);

				lstrcpy(pszFinalData,szLogDateTime);
				lstrcat(pszFinalData,"\t");
				lstrcat(pszFinalData,pstProcessInfo[iCtr].szAppId);
				if(1 == pstProcessInfo[iCtr].iStatus)
				{
					lstrcat(pszFinalData,"\tGOOD\tPRS\t");
				}
				else
				{
					lstrcat(pszFinalData,"\tERROR\tPRS\t");
				}
				lstrcat(pszFinalData,pstProcessInfo[iCtr].szProcessName);


				if(1 == pstProcessInfo[iCtr].iStatus)
				{
					lstrcat(pszFinalData,"\tON\t\r\n");
				}
				else
				{
					lstrcat(pszFinalData,"\tOFF\t\r\n");
				}
				WriteToBuffer(pszDataFile,pszFinalData);
			}

			if(NULL != pszDataFile)
			{
				if(NULL != pszDataFile[0])
				{
					LogDataToDisk(szFinalLogFile, pszDataFile);
				}
			}


			//Now log the ports

			WriteToBuffer(pszDataFile,"",TRUE);

			for(iCtr = 0; iCtr < iNumPorts; ++iCtr)
			{
				memset(pszFinalData,NULL,1024);

				lstrcpy(pszFinalData,szLogDateTime);
				lstrcat(pszFinalData,"\t");
				lstrcat(pszFinalData,pstPortInfo[iCtr].szAppId);
				if(1 == pstPortInfo[iCtr].iStatus)
				{
					lstrcat(pszFinalData,"\tGOOD\tPRT\t");
				}
				else
				{
					lstrcat(pszFinalData,"\tERROR\tPRT\t");
				}
				memset(szPort,NULL,25);
				itoa(pstPortInfo[iCtr].iPort,szPort,10);
				lstrcat(pszFinalData,szPort);


				if(1 == pstPortInfo[iCtr].iStatus)
				{
					lstrcat(pszFinalData,"\tON\t\r\n");
				}
				else
				{
					lstrcat(pszFinalData,"\tOFF\t\r\n");
				}
				WriteToBuffer(pszDataFile,pszFinalData);
			}

			if(NULL != pszDataFile)
			{
				if(NULL != pszDataFile[0])
				{
					LogDataToDisk(szFinalLogFile, pszDataFile);
				}
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{


		}
	}
	__finally
	{
		if(NULL != pszFinalData)
		{
			delete [] pszFinalData;
			pszFinalData = NULL;
		}

		if(NULL != pszDataFile)
		{
			delete [] pszDataFile;
			pszDataFile = NULL;
		}
	}
}

void	WriteAlertData(char*	pszMachineName,
						char*				pszRegId,
						stAppInfo*&			pstAppInfo,
						int&				iNumAppInfo,
						stServiceInfo*&		pstServiceInfo,
						int&				iNumServices,		
						stProcessInfo*&		pstProcessInfo,
						int&				iNumProcess,
						stPortInfo*&		pstPortInfo,
						int&				iNumPorts)
{
	char*		pszDataFile							=	NULL;
	char*		pszDataFile1							=	NULL;
	char*		pszServiceErr						=	NULL;
	char*		pszWindowsErr						=	NULL;
	char*		pszSCMErr							=	NULL;
	char		szPort[25]							=	{0};
	char		szError[51]							=	{0};

	char		szFinalLogFile[MAX_PATH + 100]		=	{0};
	char		szFinalAlertLogFile[MAX_PATH + 100]	=	{0};
	char		szFinalAlertDir[MAX_PATH + 100]		=	{0};
	char		szYYYYMM[20]						=	{0};
	char		szYYYYMMDD[20]						=	{0};
	char		szDateTime[51]						=	{0};
	char		szLogDateTime[51]					=	{0};
	char		szAppID[51]							=	{0};	
	char		szAlertDateTime[51]					=	{0};

	int			iCtr								=	0;
	int			iMainCtr							=	0;

	BOOL		bError								=	FALSE;
	BOOL		bServiceError						=	FALSE;	

	__try
	{
		__try
		{
			//Put the data in the data directory
			GetDateFormatYYYYMM(szYYYYMM);
			GetDateFormatYYYYMMDD(szYYYYMMDD);
			GetLogDateTime(szLogDateTime);
			GetTempFileName(szDateTime);
			GetAlertDateTime(szAlertDateTime);

			lstrcpy(szFinalLogFile,g_szModulePath);
			lstrcat(szFinalLogFile,"\\wpappmondata_N_");
			lstrcat(szFinalLogFile,pszMachineName);
			lstrcat(szFinalLogFile,"_");
			lstrcat(szFinalLogFile,szDateTime);
			lstrcat(szFinalLogFile,".txt");

			lstrcpy(szFinalAlertLogFile,g_szModulePath);
			lstrcat(szFinalAlertLogFile,"\\AlertTkt\\wpappmondata_N_");
			lstrcat(szFinalAlertLogFile,pszMachineName);
			lstrcat(szFinalAlertLogFile,"_");
			lstrcat(szFinalAlertLogFile,szDateTime);
			lstrcat(szFinalAlertLogFile,".txt");

			///////////////////////////////////////////////////////////////
			//Added by GaneshS on 9 April 2014
			FILE *pfile;
			char tempFile[512]	=	{""};

			sprintf(tempFile, "%s\\tempAppData.txt", g_szModulePath);
			pfile = fopen(tempFile, "w");

			///////////////////////////////////////////////////////////////

			WriteToBuffer(pszDataFile,"<wpappmondata>\r\n\r\n");
			WriteToBuffer(pszDataFile1,"<wpappmondata>\r\n\r\n");

			for(iMainCtr = 0; iMainCtr < iNumAppInfo; ++iMainCtr)
			{
				bError			=	FALSE;
				bServiceError	=	FALSE;

				WriteToBuffer(pszDataFile,"<device>\r\n<regid>");
				WriteToBuffer(pszDataFile1,"<device>\r\n<regid>");
				WriteToBuffer(pszDataFile,pszRegId);
				WriteToBuffer(pszDataFile1,pszRegId);
				WriteToBuffer(pszDataFile,"</regid>\r\n<dtime>");
				WriteToBuffer(pszDataFile,szAlertDateTime);
				WriteToBuffer(pszDataFile,"</dtime>\r\n<apid>");
				WriteToBuffer(pszDataFile1,"</regid>\r\n<apid>");
				WriteToBuffer(pszDataFile,pstAppInfo[iMainCtr].szAppId);
				WriteToBuffer(pszDataFile1,pstAppInfo[iMainCtr].szAppId);
				WriteToBuffer(pszDataFile,"</apid>\r\n<port>");
				WriteToBuffer(pszDataFile1,"</apid>\r\n<port>");


				//Check for ports
				for(iCtr = 0; iCtr < iNumPorts; ++iCtr)
				{
					if(0 == lstrcmpi(pstAppInfo[iMainCtr].szAppId,pstPortInfo[iCtr].szAppId))
					{
						if(0 == pstPortInfo[iCtr].iStatus)
						{
							memset(szPort,NULL,25);
							itoa(pstPortInfo[iCtr].iPort,szPort,10);
							WriteToBuffer(pszDataFile,szPort);
							WriteToBuffer(pszDataFile1,szPort);
							WriteToBuffer(pszDataFile,",");
							WriteToBuffer(pszDataFile1,",");
							bError = TRUE;
						}
					}
				}

				if(',' == pszDataFile[lstrlen(pszDataFile) - 1])
				{
					pszDataFile[lstrlen(pszDataFile) - 1] = NULL;	
					pszDataFile1[lstrlen(pszDataFile1) - 1] = NULL;	
				}

				WriteToBuffer(pszDataFile,"</port>\r\n<prs>");
				WriteToBuffer(pszDataFile1,"</port>\r\n<prs>");

				//Check for process
				for(iCtr = 0; iCtr < iNumProcess; ++iCtr)
				{
					if(0 == lstrcmpi(pstAppInfo[iMainCtr].szAppId,pstProcessInfo[iCtr].szAppId))
					{
						if(0 == pstProcessInfo[iCtr].iStatus)
						{
							WriteToBuffer(pszDataFile,pstProcessInfo[iCtr].szProcessName);
							WriteToBuffer(pszDataFile1,pstProcessInfo[iCtr].szProcessName);
							WriteToBuffer(pszDataFile,",");
							WriteToBuffer(pszDataFile1,",");
							bError = TRUE;
						}
					}
				}

				if(',' == pszDataFile[lstrlen(pszDataFile) - 1])
				{
					pszDataFile[lstrlen(pszDataFile) - 1] = NULL;	
					pszDataFile1[lstrlen(pszDataFile1) - 1] = NULL;	
				}

				WriteToBuffer(pszDataFile,"</prs>\r\n<svc>");
				WriteToBuffer(pszDataFile1,"</prs>\r\n<svc>");

				//Check for services
				WriteToBuffer(pszWindowsErr,"<wexcode>",TRUE);
				WriteToBuffer(pszServiceErr,"<svexcode>",TRUE);
				WriteToBuffer(pszSCMErr,"<scmcode>",TRUE);

				for(iCtr = 0; iCtr < iNumServices; ++iCtr)
				{
					if(0 == lstrcmpi(pstAppInfo[iMainCtr].szAppId,pstServiceInfo[iCtr].szAppId))
					{
						if(SERVICE_ERROR == pstServiceInfo[iCtr].iStatus) //if ERROR in service 
						{
							WriteToBuffer(pszDataFile,pstServiceInfo[iCtr].szServiceName);
							WriteToBuffer(pszDataFile1,pstServiceInfo[iCtr].szServiceName);
							WriteToBuffer(pszDataFile,",");
							WriteToBuffer(pszDataFile1,",");
							bError = TRUE;
							bServiceError = TRUE;

							memset(szError,NULL,51);
							itoa(pstServiceInfo[iCtr].iWinExitCode,szError,10);
							WriteToBuffer(pszWindowsErr,szError);
							WriteToBuffer(pszWindowsErr,",");

							memset(szError,NULL,51);
							itoa(pstServiceInfo[iCtr].iServiceExitCode,szError,10);
							WriteToBuffer(pszServiceErr,szError);
							WriteToBuffer(pszServiceErr,",");

							memset(szError,NULL,51);
							itoa(pstServiceInfo[iCtr].iEventErrorCode,szError,10);
							WriteToBuffer(pszSCMErr,szError);
							WriteToBuffer(pszSCMErr,",");
						}
						if(SERVICE_REMOVED == pstServiceInfo[iCtr].iStatus)
						{
							serviceRemovedFlag = 1;
						}
						if(SERVICE_NOTFOUND == pstServiceInfo[iCtr].iStatus)
						{
							serviceNotFoundFlag = 1;
						}
					}
				}

				if(',' == pszDataFile[lstrlen(pszDataFile) - 1])
				{
					pszDataFile[lstrlen(pszDataFile) - 1] = NULL;
					pszDataFile1[lstrlen(pszDataFile1) - 1] = NULL;
				}

				WriteToBuffer(pszDataFile,"</svc>\r\n");
				WriteToBuffer(pszDataFile1,"</svc>\r\n");

				if(TRUE == bError)
				{
					if(TRUE == bServiceError)
					{
						if(',' == pszWindowsErr[lstrlen(pszWindowsErr) - 1])
						{
							pszWindowsErr[lstrlen(pszWindowsErr) - 1] = NULL;	
						}
						WriteToBuffer(pszWindowsErr,"</wexcode>\r\n");

						WriteToBuffer(pszDataFile,pszWindowsErr);
						WriteToBuffer(pszDataFile1,pszWindowsErr);
						WriteToBuffer(pszWindowsErr,"",TRUE);


						if(',' == pszServiceErr[lstrlen(pszServiceErr) - 1])
						{
							pszServiceErr[lstrlen(pszServiceErr) - 1] = NULL;	
						}
						WriteToBuffer(pszServiceErr,"</svexcode>\r\n");

						WriteToBuffer(pszDataFile,pszServiceErr);
						WriteToBuffer(pszDataFile1,pszServiceErr);
						WriteToBuffer(pszServiceErr,"",TRUE);


						if(',' == pszSCMErr[lstrlen(pszSCMErr) - 1])
						{
							pszSCMErr[lstrlen(pszSCMErr) - 1] = NULL;	
						}
						WriteToBuffer(pszSCMErr,"</scmcode>\r\n");

						WriteToBuffer(pszDataFile,pszSCMErr);
						WriteToBuffer(pszDataFile1,pszSCMErr);
						WriteToBuffer(pszSCMErr,"",TRUE);
					}
					else
					{
						WriteToBuffer(pszDataFile,"<wexcode></wexcode>\r\n<svexcode></svexcode>\r\n<scmcode></scmcode>\r\n");
						WriteToBuffer(pszDataFile1,"<wexcode></wexcode>\r\n<svexcode></svexcode>\r\n<scmcode></scmcode>\r\n");
					}
					WriteToBuffer(pszDataFile,"<status>E</status>\r\n</device>\r\n\r\n");
					WriteToBuffer(pszDataFile1,"<status>E</status>\r\n</device>\r\n\r\n");
				}
				else
				{
					WriteToBuffer(pszDataFile,"<wexcode></wexcode>\r\n<svexcode></svexcode>\r\n<scmcode></scmcode>\r\n");
					WriteToBuffer(pszDataFile1,"<wexcode></wexcode>\r\n<svexcode></svexcode>\r\n<scmcode></scmcode>\r\n");
					if(serviceRemovedFlag)
					{
						WriteToBuffer(pszDataFile,"<status>R</status>\r\n</device>\r\n\r\n");
						WriteToBuffer(pszDataFile1,"<status>R</status>\r\n</device>\r\n\r\n");
						serviceRemovedFlag = 0; //clear the flag
					}
					else if(serviceNotFoundFlag)
					{
						WriteToBuffer(pszDataFile,"<status>N</status>\r\n</device>\r\n\r\n");
						WriteToBuffer(pszDataFile1,"<status>N</status>\r\n</device>\r\n\r\n");
						serviceNotFoundFlag = 0; //clear the flag
					}
					else
					{
						WriteToBuffer(pszDataFile,"<status>G</status>\r\n</device>\r\n\r\n");
						WriteToBuffer(pszDataFile1,"<status>G</status>\r\n</device>\r\n\r\n");
					}
				}

				LogDataToDisk(szFinalLogFile,pszDataFile,TRUE);
				fprintf(pfile, "%s", pszDataFile1);//Added by                   GaneshS on 9 April 2014
				WriteToBuffer(pszDataFile,"",TRUE);
				WriteToBuffer(pszDataFile1,"",TRUE);
			} //End of iMainCtr 

						
			WriteToBuffer(pszDataFile,"</wpappmondata>\r\n",TRUE);
			WriteToBuffer(pszDataFile1,"</wpappmondata>\r\n",TRUE);
			LogDataToDisk(szFinalLogFile,pszDataFile,TRUE);
			
			////////////Added by GaneshS on 9 April 2014//////////////////////////////
			fprintf(pfile, "%s", pszDataFile1);
			fclose(pfile);

			LogLastError(g_szErrorLogPath,
										9999,
										NULL,
										0,
										0,
										NULL,
										"INFO: checking NeedToWebPost",
										"",
										"");

			bool bRet = NeedToWebPost(tempFile, pszMachineName);
			//DeleteFile(tempFile);

			////////////////////////////////////////////////////////////////////////

			if(bRet == true)
			{
				//Now we need to move the file to the alert folder
				if(0 == MoveFileEx(szFinalLogFile,szFinalAlertLogFile,MOVEFILE_REPLACE_EXISTING))
				{
					lstrcpy(szFinalAlertDir,g_szModulePath);
					lstrcat(szFinalAlertDir,"\\AlertTkt");
					::CreateDirectory(szFinalAlertDir,NULL);
					MoveFileEx(szFinalLogFile,szFinalAlertLogFile,MOVEFILE_REPLACE_EXISTING);
					/*if(0 == MoveFileEx(szFinalLogFile,szFinalAlertLogFile,MOVEFILE_REPLACE_EXISTING))
					{
						::DeleteFile(szFinalLogFile);
					}*/
				}
			}
			else
			{	
				DeleteFile(szFinalLogFile);
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{


		}
	}
	__finally
	{
		if(NULL != pszDataFile)
		{
			delete [] pszDataFile;
			pszDataFile = NULL;
		}

		if(NULL != pszDataFile1)
		{
			delete [] pszDataFile1;
			pszDataFile1 = NULL;
		}

		if(NULL != pszServiceErr)
		{
			delete [] pszServiceErr;
			pszServiceErr = NULL;
		}

		if(NULL != pszWindowsErr)
		{
			delete [] pszWindowsErr;
			pszWindowsErr = NULL;
		}

		if(NULL != pszSCMErr)
		{
			delete [] pszSCMErr;
			pszSCMErr = NULL;
		}
	}
}






bool GetRegValue(char* RegPath, char* regKey, char* retVal)
{
	CRegKey key;
	DWORD size	=	512;

	try
	{
		long ret = key.Open(HKEY_LOCAL_MACHINE, RegPath, KEY_READ);
		if(ret != ERROR_SUCCESS)
		{
			return false;
		}
		
		ret = key.QueryStringValue(regKey, retVal, &size);
		if(ret != ERROR_SUCCESS)
		{
			key.Close();
			return false;
		}

		key.Close();
	}
	catch(...)
	{
		return false;
	}

	return true;
}



BOOL IsWow64()
{
    BOOL bIsWow64 = FALSE;

    fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
        GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

    if(NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
            return FALSE;
        }
    }
    return bIsWow64;
}





bool GetDNSFromINI(char* Section, char* RetVal)
{
	try
	{
		char		WBUrlIniPath[512]		=		{""};
		char		AppPath[255]					=		{""};

		GetModuleFileName(NULL, AppPath, 255);
		PathRemoveFileSpec(AppPath);


		sprintf(WBUrlIniPath, "%s\\Configuration", AppPath);
		if(PathFileExists(WBUrlIniPath) == FALSE)
		{
			return false;
		}

		strcat(WBUrlIniPath, "\\zScriptDNS.ini");
		GetPrivateProfileString(Section, "URL", NULL, RetVal, 255, WBUrlIniPath);

		if(strcmp(RetVal, "")==0)
		{
			return false;
		}

		
		if(RetVal[strlen(RetVal)-1]=='/')		RetVal[strlen(RetVal)-1] = NULL;
		
	}
	catch(...)
	{
		return false;
	}

	return true;
}




bool CallWPForCount(char* Count)
{
	char		strREGID[100],
					strAgentType[100];
	char		RegPath[255]	=	{""};

		USES_CONVERSION;

		DWORD			dwSize						=		0;
		DWORD			dwDownloaded	=		0;
		LPSTR				pszOutBuffer;
		BOOL				bResults					=		FALSE;
		HINTERNET  hSession					=		NULL, 
									hConnect					=		NULL,
									hRequest					=		NULL;

		bool					found						=		false;
		bool					bFailed						=		false;

		char					URL[512]				=		{""};

	try
	{
		BOOL bIs64 = IsWow64();
			if(bIs64 == TRUE){
				strcpy(RegPath, "Software\\Wow6432Node\\SAAZOD");
			}else{
				strcpy(RegPath, "Software\\SAAZOD");
			}

			if(!GetRegValue(RegPath,  "REGID",  strREGID))
			{
				return 0;
			}
			if(!GetRegValue(RegPath,  "TYPE",  strAgentType))
			{
				return 0;
			}


		char retDns[255] = {""};
		if(!GetDNSFromINI("wpappmoncyclecnt", retDns))
		{
			LogLastError(g_szErrorLogPath,
										9999,
										NULL,
										0,
										0,
										NULL,
										"INFO:   could not find wpappmoncyclecnt DNS in zScriptDns.ini",
										"",
										"");
			return false;
		}
		/* http://10.2.27.11:82/Webpost-Master files_WEBSERVER28/webpost2.itsupport247.net/tfr_wpappmoncyclecnt.asp?rty=MSMA*/
		
		//sprintf(URL, "http://webpost2.itsupport247.net/tfr_wpappmoncyclecnt.asp?rty=MSMA");
		sprintf(URL, "%s/tfr_wpappmoncyclecnt.asp?rty=%s", retDns,strAgentType);
		

		URL_COMPONENTS urlComp;
		WCHAR pwszUrl1[512] =  {0};
		mbstowcs(pwszUrl1, URL, 512);
		DWORD dwUrlLen = 0;

		ZeroMemory(&urlComp, sizeof(urlComp));
		urlComp.dwStructSize = sizeof(urlComp);

		urlComp.dwSchemeLength    = (DWORD)-1;
		urlComp.dwHostNameLength  = (DWORD)-1;
		urlComp.dwUrlPathLength   = (DWORD)-1;
		urlComp.dwExtraInfoLength = (DWORD)-1;

		
		if (!WinHttpCrackUrl( pwszUrl1, (DWORD)wcslen(pwszUrl1), 0, &urlComp))
		{
			return false;
		}

		
		
		if(urlComp.nPort == INTERNET_DEFAULT_HTTPS_PORT){
			found = true;
		}else if(urlComp.nPort == INTERNET_DEFAULT_HTTP_PORT){
			found = false;
		}

	
		string myhostname(W2A(urlComp.lpszHostName));
		string myurlpath(W2A(urlComp.lpszUrlPath));
		int strindex = (int)myhostname.find(myurlpath);
		string newhostname(myhostname.substr(0,strindex));


		hSession = WinHttpOpen( L"",  WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, 	WINHTTP_NO_PROXY_BYPASS, 0);
		if (hSession == NULL)
		{
			return false;
		}

		

		//hConnect = WinHttpConnect( hSession, A2W(newhostname.c_str()),  82, 0);//For local server port 82 is used.
		hConnect = WinHttpConnect( hSession, A2W(newhostname.c_str()),  urlComp.nPort, 0);
		if (hConnect == NULL)
		{
			bFailed = true;
			goto cleanup;
		}



		if(found){
			hRequest = WinHttpOpenRequest( hConnect, L"GET", urlComp.lpszUrlPath,  NULL, WINHTTP_NO_REFERER, 
										   WINHTTP_DEFAULT_ACCEPT_TYPES,  WINHTTP_FLAG_SECURE);
		}else{
			hRequest = WinHttpOpenRequest( hConnect, L"GET", urlComp.lpszUrlPath, NULL, WINHTTP_NO_REFERER, 
										   WINHTTP_DEFAULT_ACCEPT_TYPES,  0);
		}
		if (hRequest == NULL)
		{
			bFailed = true;
			goto cleanup;
		}

		char Data[10] = {""};
		strcpy(Data, "");


		bResults = WinHttpSendRequest( hRequest,  WINHTTP_NO_ADDITIONAL_HEADERS,  0, LPVOID(Data), (DWORD)strlen(Data), (DWORD)strlen(Data), 0);
		if (!bResults)
		{
			bFailed = true;
			goto cleanup;
		}
		
		
		
		bResults = WinHttpReceiveResponse( hRequest, NULL);
		if (!bResults)
		{
			bFailed = true;
			goto cleanup;
		}

		
		
		dwSize = 1;

		while(dwSize)
		{
				dwSize = 0;
				 if (!WinHttpQueryDataAvailable( hRequest, &dwSize)) 
				 {
					bFailed = true;
					break;
				 }
		            
				if (!dwSize)
				{
					bFailed = true;
					break;	// No more data available.
				}


				pszOutBuffer = new char[dwSize+1];
				if (!pszOutBuffer)
				{
					bFailed = true;
					break;
				}
		            
				ZeroMemory(pszOutBuffer, dwSize+1);
				
				if (!WinHttpReadData( hRequest, (LPVOID)pszOutBuffer,  dwSize, &dwDownloaded)) //Reading zero only
				{
					bFailed = true;
					break;
				}
				else
				{
					strcpy(Count, pszOutBuffer);
					break;
				}

				if (!dwDownloaded)
				{
					bFailed	=	true;
					break;
				}
		}			
	

cleanup:

		if (hRequest) WinHttpCloseHandle(hRequest);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hSession) WinHttpCloseHandle(hSession);

		if(bFailed)		return false;

		if(pszOutBuffer != NULL)
		{
			delete [] pszOutBuffer;
			pszOutBuffer = NULL;
		}

	}
	catch(...)
	{
		return false;
	}

	return true;
}




bool GetMachineNames(char* IniFilePath)
{
	char*	chBuffer		= NULL;
	char*	chTempBuffer	= NULL;

	DWORD	dwSize;
	DWORD	dwNumber;
	int		size;
	
	
	try
	{
		dwSize = GetCompressedFileSize(IniFilePath, 0);

		if(dwSize == INVALID_FILE_SIZE)
		{
			return false;
		}

		size = dwSize*2;
			
		chBuffer = new char[size];
		memset(chBuffer, 0, size);

		dwNumber = GetPrivateProfileSectionNames(chBuffer, size, IniFilePath);
			
		chTempBuffer = chBuffer;

		while(NULL != *chTempBuffer)
		{
			MachineNameList.push_back(chTempBuffer);
			chTempBuffer = chTempBuffer + lstrlen(chTempBuffer) + 1;
		}


		if(NULL != chBuffer)
		{
			delete []chBuffer;
			chBuffer = NULL;
			chTempBuffer = NULL;
		}

	}
	catch(...)
	{
		return false;
	}

	return true;
}




bool NeedToGetCount(char* Inipath, char* Section)
{
	char NowDate[100] = {""};
	char PrevDate[100] = {""};

	int y1=0, m1=0, d1=0,
			y2=0, m2=0, d2=0;

	SYSTEMTIME st;

	try
	{
		GetPrivateProfileString(Section, "LastSent", NULL, PrevDate, 100, Inipath);
		if(strcmp(PrevDate, "")==0)
		{
			return true;
		}

		GetLocalTime(&st);
		sprintf(NowDate, "%02d-%02d-%04d", st.wDay, st.wMonth, st.wYear);

		sscanf(PrevDate, "%d-%d-%d", &d1, &m1, &y1);
		sscanf(NowDate, "%d-%d-%d", &d2, &m2, &y2);
		
		y1 = y1 - 1900;
		y2 = y2 - 1900;



		struct std::tm a = {0,0,0,d1,m1,y1}; 
		struct std::tm b = {0,0,0,d2,m2,y2};
		std::time_t x = std::mktime(&a);
		std::time_t y = std::mktime(&b);
		if ( x != (std::time_t)(-1) && y != (std::time_t)(-1) )
		{
			double difference = std::difftime(y, x) / (60 * 60 * 24);
			
			if(difference >= 7)
			{
				return true;
			}
			else if(difference < 0)
			{
				WritePrivateProfileString(Section, "LastSent", NowDate, Inipath);
			}else{}
		}
	}
	catch(...)
	{
		return false;
	}

	return false;
}





bool NeedToWebPost(char* szCurrentFile,  char* szMachineName)
{
	char strPrevChecksum[512]	=	{""};
	char strCurChecksum[512]	=	{""};
	char strCount[10]						=	{""};
	char szInterval[10]					=	{""};
	char szInter[10]							=	{""};
	char count[10]								=	{""};
	char Section[100]						=	{""};
	char szType[10]							=	{""};
	char msg[255]								= {""};

	
	char szLastSent[10]		=	{""};

	char NowDate[100] = {""};
	SYSTEMTIME st;
	

	try
	{
		if(!MachineNameList.empty())	MachineNameList.clear();

		GetLocalTime(&st);
		sprintf(NowDate, "%02d-%02d-%04d", st.wDay, st.wMonth, st.wYear);

		memset(strPrevChecksum, 0, 512);

		BOOL bSuccCheckSum	=	FALSE;
		bSuccCheckSum		=	ZGetMD5(szCurrentFile, strCurChecksum);


		GetMachineNames(g_szDeviceListINI);
		

		list<string>::iterator iter;
		iter = MachineNameList.begin();
		
		for(; iter!=MachineNameList.end(); iter++)
		{
			GetPrivateProfileString((*iter).c_str(), "Type", NULL, szType, 10,  g_szDeviceListINI);
			if(strcmp(szType, "1")==0)
			{
				strcpy(Section, (*iter).c_str());
				break;
			}
		}

		char InternalSection[255] = {""};
		sprintf(InternalSection, "%s_WebPost", szMachineName);

		
		GetPrivateProfileString(Section, "Interval", NULL, szInterval, 10, g_szDeviceListINI);

		LogLastError(g_szErrorLogPath,
									9999,
									NULL,
									NULL,
									0,
									NULL,
									"MSG:   Interval value found in DeviceList.ini is",
									szInterval,
									NULL);


		
	
		if((strcmp(szInterval, "")==0) || (NeedToGetCount(g_szInternalStatusINI, InternalSection)))
		{
			WritePrivateProfileString(InternalSection, "LastSent", NowDate, g_szInternalStatusINI);

			bool bRet = CallWPForCount(count);
			LogLastError(g_szErrorLogPath,
									9999,
									NULL,
									NULL,
									0,
									NULL,
									"MSG:   Cycle count received from DB is ",
									count,
									NULL);

			if(bRet)
			{
				WritePrivateProfileString(Section, "Interval", count, g_szDeviceListINI); //write the updated value of Interval from DB
			}
			else
			{
				WritePrivateProfileString(Section, "Interval", "11", g_szDeviceListINI); //Default is 11 ie 0-11
			}
		}


		GetPrivateProfileString(Section, "Interval", NULL, szInterval, 10, g_szDeviceListINI);
		GetPrivateProfileString(InternalSection, "checksum", NULL, strPrevChecksum, 512, g_szInternalStatusINI);

		
		sprintf(msg, "MSG:  %s Current MD5 checksum", szMachineName);
		LogLastError(g_szErrorLogPath,
									9999,
									NULL,
									NULL,
									0,
									NULL,
									msg,
									strCurChecksum,
									NULL);

		sprintf(msg, "MSG:  %s Previous MD5 checksum", szMachineName);
		LogLastError(g_szErrorLogPath,
									9999,
									NULL,
									NULL,
									0,
									NULL,
									msg,
									strPrevChecksum,
									NULL);


		if((bSuccCheckSum == FALSE) || (strcmp(strCurChecksum, "")==0))
		{
			WritePrivateProfileString(InternalSection, "checksum", strCurChecksum, g_szInternalStatusINI);
			WritePrivateProfileString(InternalSection, "count", "1", g_szInternalStatusINI);

			LogLastError(g_szErrorLogPath,
									9999,
									NULL,
									NULL,
									0,
									NULL,
									"MSG:  moving file to AlertTkt folder as failed to calculate MD5 checksum",
									NULL,
									NULL);
			return true;
		}



		if(strcmp(strPrevChecksum, "")==0)
		{
			WritePrivateProfileString(InternalSection, "checksum", strCurChecksum, g_szInternalStatusINI);
			WritePrivateProfileString(InternalSection, "count", "1", g_szInternalStatusINI);

			LogLastError(g_szErrorLogPath,
									9999,
									NULL,
									NULL,
									0,
									NULL,
									"MSG:  moving file to AlertTkt folder as could not read previous checksum from ini file",
									NULL,
									NULL);
			return true;
		}


		if(stricmp(strPrevChecksum, strCurChecksum)!=0)
		{
			WritePrivateProfileString(InternalSection, "checksum", strCurChecksum, g_szInternalStatusINI);
			//WritePrivateProfileString(InternalSection, "count", "1", g_szInternalStatusINI); 
			
			GetPrivateProfileString(InternalSection, "count",szInterval,strCount,10, g_szInternalStatusINI);
			int count = atoi(strCount);
			
			if(count==11) count=0;
			else
				count++;

			char szCount[3];
			_itoa(count,szCount,10);/*char *_itoa(int value,char *str,int radix);*/
			WritePrivateProfileString(InternalSection, "count", szCount, g_szInternalStatusINI); //write updated count

			LogLastError(g_szErrorLogPath,
									9999,
									NULL,
									NULL,
									0,
									NULL,
									"MSG:  moving file to AlertTkt folder as data changed",
									NULL,
									NULL);
			return true;
		}


														//default=11  //count from internalstatus.ini
		GetPrivateProfileString(InternalSection,"count", szInterval, strCount, 10, g_szInternalStatusINI);
		int count1 = atoi(strCount);
		int iInterval = atoi(szInterval);

		//if(strcmp(strCount, szInterval)>=0)
		if(count1 >= iInterval)
		{
			WritePrivateProfileString(InternalSection, "checksum", strCurChecksum, g_szInternalStatusINI);
			WritePrivateProfileString(InternalSection, "count", "0", g_szInternalStatusINI); //ie from 0-1-2-3...-11-0-1-2---11-0

			LogLastError(g_szErrorLogPath,
									9999,
									NULL,
									NULL,
									0,
									NULL,
									"MSG:   moving file to AlertTkt folder as web post interval occurred",
									NULL,
									NULL);

			return true;
		}

		//if count value(InternalStatus.ini) < 
		char *cnt;
		int iCount = atoi(strCount);
		if(iCount==11) iCount=0;
		else iCount++;

		cnt = itoa(iCount, strCount,10);

		WritePrivateProfileString(InternalSection, "checksum", strCurChecksum, g_szInternalStatusINI);
		WritePrivateProfileString(InternalSection, "count", cnt, g_szInternalStatusINI);

	}
	catch(...)
	{
		return true;
	}

	return false;
}



BOOL	ZGetMD5(char *pszFilePath, char*	g_szCheckSum)
{
	char			szExpFilePath[MAX_PATH + 100]	=	{0};
	BYTE*			pbyBuffer						=	NULL;
	char			szErrorCode[101]				=	{0};
	char			szMD5Buffer[256]				=	{0};
	char			chEach[10]						=	{0};
	unsigned char*	pszDigest						=	NULL;
	char			szFileSize[100]					=	{0};
	HANDLE			hSrcFile						=	NULL;

	DWORD			dwBytesToRead					=	0;	
	DWORD			dwBytesRead						=	0;
	DWORD			dwMemSize						=	0;
	DWORD			dwAttributes					=	0;
	DWORD64			ulFileSize						=	0;
	long			lErrorCode						=	0;
	int				nCount							=	0;
													
	BOOL			bRet							=	FALSE;
	__try
	{
		__try
		{
			md5 alg;

			::ExpandEnvironmentStrings(pszFilePath,szExpFilePath,MAX_PATH);
			if(NULL == szExpFilePath[0])
			{
				lstrcpy(szExpFilePath,pszFilePath);
			}

			dwMemSize	=	(1024 * 1024);
			do
			{
				pbyBuffer	=	new BYTE[dwMemSize];		
				if(NULL != pbyBuffer)
				{
					break;
				}
				dwMemSize	/=	2;
			}while(dwMemSize >= (4 * 1024));
			
			if(NULL == pbyBuffer)
			{
				lErrorCode = ERROR_OUTOFMEMORY;
				return bRet;
			}

			//Open the file

			dwAttributes = ::GetFileAttributes(szExpFilePath);
			if(0 == ::SetFileAttributes(szExpFilePath,FILE_ATTRIBUTE_NORMAL))
			{
				LogLastError(g_szErrorLogPath,
											9999,
											NULL,
											::GetLastError(),
											0,
											NULL,
											"ERR:   SetFileAttributes Fails to FILE_ATTRIBUTE_NORMAL.",
											szExpFilePath,
											NULL);
				lErrorCode = ::GetLastError();
				return bRet;
			}

			hSrcFile = ::CreateFile(szExpFilePath,
									GENERIC_READ,
									FILE_SHARE_READ,
									NULL,
									OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL,
									NULL);
			
			if((INVALID_HANDLE_VALUE	==	hSrcFile)	||
				(NULL	==	hSrcFile))
			{
				LogLastError(g_szErrorLogPath,
											9999,
											NULL,
											::GetLastError(),
											0,
											NULL,
											"ERR:   CreateFile Fails to Read.",
											szExpFilePath,
											NULL);
				lErrorCode = ::GetLastError();
				return bRet;
			}

			dwBytesToRead = dwMemSize;

			do
			{
				dwBytesRead = 0;

				if(0 == ::ReadFile(hSrcFile,pbyBuffer,dwBytesToRead,&dwBytesRead,NULL))
				{
					::Sleep(5000);
					dwBytesRead = 0;
					if(0 == ::ReadFile(hSrcFile,pbyBuffer,dwBytesToRead,&dwBytesRead,NULL))
					{
						::Sleep(5000);
						dwBytesRead = 0;
						if(0 == ::ReadFile(hSrcFile,pbyBuffer,dwBytesToRead,&dwBytesRead,NULL))
						{
							lErrorCode = ::GetLastError();
							return bRet;
						}
					}
				}
				ulFileSize += dwBytesRead;
				alg.Update(pbyBuffer, dwBytesRead);

			}while(dwBytesToRead == dwBytesRead);


			::CloseHandle(hSrcFile);
			hSrcFile = NULL;

			alg.Finalize();
			pszDigest = alg.Digest();

			for (nCount = 0; nCount < 16; nCount++)
			{
				sprintf(chEach, "%02x", pszDigest[nCount]);
				strncat(szMD5Buffer, chEach, sizeof(chEach));
			}
		//	vtMD5.vt			=	VT_BSTR;//	vtMD5.bstrVal		=	_com_util::ConvertStringToBSTR(szMD5Buffer);
			memset(g_szCheckSum,0,sizeof(g_szCheckSum));
			strcpy(g_szCheckSum,szMD5Buffer);

			_ui64toa(ulFileSize,szFileSize,10);

			if(NULL == szFileSize[0])
			{
				LogLastError(g_szErrorLogPath,
											9999,
											NULL,
											::GetLastError(),
											0,
											NULL,
											"ERR:   FileSize 0.",
											szExpFilePath,
											NULL);
				memset(g_szCheckSum,0,sizeof(g_szCheckSum));
				lErrorCode = -1;
				return bRet;
			}
			else
			{
			//	vtFileSize.vt			=	VT_BSTR;
			//	vtFileSize.bstrVal		=	_com_util::ConvertStringToBSTR(szFileSize);
			}

			bRet = TRUE;

		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			lErrorCode = -1;
			return bRet;
		}
	}
	__finally
	{
		if(0 != lErrorCode)
		{
			ltoa(lErrorCode,szErrorCode,10);
		}

		if(NULL != hSrcFile)
		{
			::CloseHandle(hSrcFile);
			hSrcFile = NULL;
		}

		if(NULL != pbyBuffer)
		{
			delete [] pbyBuffer;
			pbyBuffer = NULL;
		}

		::SetFileAttributes(szExpFilePath,dwAttributes);//RevertBack.

		/*if(NULL != pszFilePath)
		{
			delete [] pszFilePath;
			pszFilePath = NULL;
		}*/

	}
	return bRet;
}




DWORD	RemotePortCheck(char* pszMachineName)
{
	char		szRegId[201]						=	{0};	
	char		szDef[10]							=	{0};
	char		szXMLFilePath[MAX_PATH + 100]		=	{0};
	char		szWeekDays[51]						=	{0};
	char		szFromTime[11]						=	{0};
	char		szToTime[11]						=	{0};
	char		szPIDInfo[356]						=	{0};

	char*		pszFindChar							=	NULL;


	stServiceInfo*		pstServiceInfo	=	NULL;
	stProcessInfo*		pstProcessInfo	=	NULL;
	stPortInfo*			pstPortInfo		=	NULL;
	stAppInfo*			pstAppInfo		=	NULL;


	int					iNumServices	=	0;
	int					iNumProcess		=	0;
	int					iNumPorts		=	0;
	int					iNumAppInfo		=	0;
	int					iNumofThreads	=	0;
	int					iCtr			=	0;
	int					iRet			=	0;

	HANDLE				hThreads[58]	=	{0};	
	DWORD				dwThreadId		=	0;

	float				fFromTime		=	0.0;
	float				fToTime			=	0.0;

	__try
	{
		__try
		{
			lstrcpy(szXMLFilePath,g_szModulePath);
			lstrcat(szXMLFilePath,"\\Configuration\\App\\");
			lstrcat(szXMLFilePath,pszMachineName);
			lstrcat(szXMLFilePath,"_Config.xml");


			iRet = ReadXMLConfigFile(szXMLFilePath,
										pstAppInfo,
										iNumAppInfo,
										pstServiceInfo,
										iNumServices,
										pstProcessInfo,
										iNumProcess,
										pstPortInfo,
										iNumPorts);

			if(iNumPorts <= 0)
			{
				if(-1 == iRet)
				{
					GetPrivateProfileString(pszMachineName,
											"RegId",
											szDef,
											szRegId,
											200,
											g_szDeviceListINI);

					LogLastError(g_szErrorLogPath,
									9999,
									NULL,
									0,
									0,
									NULL,
									"MSG:   Policies not found for the machine",
									pszMachineName,
									NULL);

					if(NULL != szRegId[0])
					{
						sprintf(szPIDInfo,"Policies not found for the machine %s",pszMachineName);
						WritePID(pszMachineName,szRegId,szPIDInfo);
					}
				}
				return 0;
			}



			GetPrivateProfileString(pszMachineName,
									"RegId",
									szDef,
									szRegId,
									200,
									g_szDeviceListINI);

			if(NULL == szRegId[0])
			{
				LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								0,
								0,
								NULL,
								"ERR:   RegId not available",
								pszMachineName,
								NULL);
				return 0;
			}



			GetPrivateProfileString(pszMachineName,
									"DaysToMonitor",
									szDef,
									szWeekDays,
									50,
									g_szDeviceListINI);


			GetPrivateProfileString(pszMachineName,
									"FromTime",
									szDef,
									szFromTime,
									10,
									g_szDeviceListINI);

			GetPrivateProfileString(pszMachineName,
									"ToTime",
									szDef,
									szToTime,
									10,
									g_szDeviceListINI);

			if(0 == CheckTime(szFromTime, szToTime))
			{
				LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								0,
								0,
								NULL,
								"MSG:   Not in current schedule",
								pszMachineName,
								NULL);
				sprintf(szPIDInfo,"Not in current schedule  %s",pszMachineName);
				WritePID(pszMachineName,szRegId,szPIDInfo);
				return 0;
			}

			pszFindChar = strchr(szFromTime,':');
			if(NULL != pszFindChar)
			{
				*pszFindChar = '.';
			}

			pszFindChar = strchr(szToTime,':');
			if(NULL != pszFindChar)
			{
				*pszFindChar = '.';
			}

			sscanf(szFromTime,"%f",&fFromTime);
			sscanf(szToTime,"%f",&fToTime);


			if(1 != CheckSchedule(szWeekDays,fFromTime,fToTime))
			{
				LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								0,
								0,
								NULL,
								"MSG:   Not in current schedule",
								pszMachineName,
								NULL);
				sprintf(szPIDInfo,"Not in current schedule  %s",pszMachineName);
				WritePID(pszMachineName,szRegId,szPIDInfo);
				return 0;
			}




			for(iCtr = 0; iCtr < iNumPorts; ++iCtr)
			{
				
				dwThreadId	=	0;
				lstrcpy(pstPortInfo[iCtr].szIP,pszMachineName);
				pszFindChar	=	strchr(pstPortInfo[iCtr].szIP,'(');
				if(NULL != pszFindChar)
				{
					*pszFindChar = NULL;
					StrTrim(pstPortInfo[iCtr].szIP," ");
				}

				hThreads[iCtr]	=	::CreateThread(NULL,
													0,
													(LPTHREAD_START_ROUTINE)CheckRemotePort,
													&pstPortInfo[iCtr],
													0,
													&dwThreadId);
			}
			::WaitForMultipleObjects(iNumPorts,hThreads,TRUE,INFINITE);

			for(iCtr = 0; iCtr < iNumPorts; ++iCtr)
			{
				::CloseHandle(hThreads[iCtr]);
				hThreads[iCtr] = NULL;
			}

			WriteCheckedData(pszMachineName,
								pstServiceInfo,
								iNumServices,
								pstProcessInfo,
								iNumProcess,
								pstPortInfo,
								iNumPorts);

			WriteAlertData(pszMachineName,
							szRegId,
							pstAppInfo,
							iNumAppInfo,
							pstServiceInfo,
							iNumServices,
							pstProcessInfo,
							iNumProcess,
							pstPortInfo,
							iNumPorts);

			::WritePrivateProfileString(pszMachineName,
										"Status",
										"1",
										g_szInternalStatusINI);

		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{


		}
	}
	__finally
	{
		if(NULL != pstServiceInfo)
		{
			delete [] pstServiceInfo;
			pstServiceInfo = NULL;
		}

		if(NULL != pstProcessInfo)
		{
			delete [] pstProcessInfo;
			pstProcessInfo = NULL;
		}

		if(NULL != pstPortInfo)
		{
			delete [] pstPortInfo;
			pstPortInfo = NULL;
		}

		if(NULL != pstAppInfo)
		{
			delete [] pstAppInfo;
			pstAppInfo = NULL;
		}
	}
	return 0;
}

DWORD	CheckRemotePort(stPortInfo* pstPortInfo)
{
	DWORD	dwRet		=	0;
	DWORD	dwThreadId	=	0;

	HANDLE	hThread		=	NULL;
	int		nResult		=	0;

	char	szError[51]	=	{0};
	char	szIP[101]	=	{0};	

	unsigned long		nInternetAddress	=	0;
	HOSTENT*			tempHost			=	NULL;
	in_addr				addr				=	{0};

	try
	{
		try
		{

			//OK resolve the IP
			nInternetAddress = inet_addr(pstPortInfo->szIP);
			if( nInternetAddress == INADDR_NONE)
			{
				// This means it is a ServerName
				// Find Server Address

				//here use the getaddrinfo
				
				tempHost = gethostbyname(pstPortInfo->szIP);
				memset(&addr,0,sizeof(addr));
				if(tempHost)
				{
					memcpy(&(addr),tempHost->h_addr,tempHost->h_length);
					strcpy(szIP , inet_ntoa(addr));
				}
				else
				{
					LogLastError(g_szErrorLogPath,
									9999,
									NULL,
									::WSAGetLastError(),
									0,
									NULL,
									"ERR:   Unable to resolve machine name",
									pstPortInfo->szIP,
									NULL);
					return 0;
				}
			}
			else
			{
				// This means that the input is server IPAddress
				strcpy(szIP , pstPortInfo->szIP);
			}


			CSAMSocket		objServer;

			nResult = objServer.Create();
			if(0 == nResult)
			{
				dwRet = objServer.m_nLastError;
			}

			nResult = objServer.Connect(szIP,pstPortInfo->iPort);
			if(0 == nResult)
			{
				LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								::WSAGetLastError(),
								pstPortInfo->iPort,
								NULL,
								"ERR:   Unable to conect to the remote port",
								pstPortInfo->szIP,
								NULL);

				pstPortInfo->iStatus	=	0;
			}
			else
			{
				pstPortInfo->iStatus	=	1;
			}


			dwThreadId = 0;
			hThread = ::CreateThread(NULL,
									0,
									(LPTHREAD_START_ROUTINE)ClosePortGracefully,
									&objServer,
									0,
									&dwThreadId);

			if(WAIT_OBJECT_0 != ::WaitForSingleObject(hThread,(45 * 1000)))
			{
				::TerminateThread(hThread,0);
			}
			::CloseHandle(hThread);
			hThread = NULL;

		}
		catch(...)
		{


		}
	}
	catch(...)
	{


	}
	return 0;
}

DWORD WINAPI ClosePortGracefully(CSAMSocket* pSamSocket)
{
	__try
	{
		__try
		{
			pSamSocket->GracefullClose();

		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{


		}
	}
	__finally
	{


	}
	return 0;
}


int	TerminateProcess(char*	szProcess,
					 char*	szProcessExe)
{
	char	szDef[10]						=	{0};
	char	szFilePath[MAX_PATH + 100]		=	{0};
	char	szBuffer[55]					=	{0};
											
	int		iProcNum						=	0;
	int		iCount							=	0;
	int		iRet							=	1;
											
	int*	pProcCount						=	NULL;
	DWORD	dwThreadId						=	0;
	DWORD	dwRet							=	0;
											
	char	szFileName[305]					=	{0};
	char	szProcessName[305]				=	{0};
	char	szKillPath[305]					=	{0};
	char*	pszFindChar						=	NULL;
											
	int		iCtr							=	0;
	int		iSubCtr							=	0;
	DWORD	iEnumCtr						=	0;
	LPARAM	lmsgParam						=	0;
	DWORD	dwResult						=	0;
	DWORD	dwPID[5004]						=	{0};
	DWORD	dwProcessCount					=	0;
	DWORD	dwProcessId						=	0;
	HANDLE	hProcess						=	0;	
	HANDLE	hThread							=	NULL;	

	PROCESS_INFORMATION piProcInfo			=	{0}; 
	STARTUPINFO			sInfo				=	{0}; 


	try
	{
		try
		{
			dwProcessId = ::GetCurrentProcessId();
			dwProcessCount = 0;
			memset(dwPID,NULL,(sizeof(DWORD) * 5004));
			//Enumerate the process
			::EnumProcesses(dwPID,5000,&dwProcessCount);
			dwProcessCount = dwProcessCount/sizeof(DWORD);
			for(iEnumCtr = 0; iEnumCtr < dwProcessCount;++iEnumCtr)
			{
				if(dwProcessId == dwPID[iEnumCtr])
				{
					continue;
				}

				hProcess = ::OpenProcess(PROCESS_ALL_ACCESS,TRUE,dwPID[iEnumCtr]);
				if(NULL != hProcess)
				{
					memset(szFileName,NULL,300);
					memset(szProcessName,NULL,300);

					::GetModuleFileNameEx(hProcess,NULL,szFileName,300);
					pszFindChar = strrchr(szFileName,'\\');
					if(NULL != pszFindChar)
					{
						lstrcpy(szProcessName,pszFindChar + 1);
						if((0 == lstrcmpi(szProcess,szProcessName))		||
							(0 == lstrcmpi(szProcessExe,szProcessName)))
						{
							iRet = ::TerminateProcess(hProcess,0);
							::CloseHandle(hProcess);
							hProcess = NULL;
						}
						else
						{
							::CloseHandle(hProcess);
							hProcess = NULL;
						}
					}
					else
					{
						::CloseHandle(hProcess);
						hProcess = NULL;
					}
				}
			}
		}
		catch(...)
		{

		}
	}
	catch(...)
	{

	}
	return iRet;
}


int	LogLastError(char*	szFilePath,
				 long	llastLineCode,
				 char*	szModuleCode,
				 long	lSystemCode,
				 long	lMiscCode,
				 char*	pszModuleErrorString,
				 char*	pAddInfo1,
				 char*	pAddInfo2,
				 char*	pAddInfo3)
{
	char*	pszFinalErrorString							=	NULL;
	LPVOID	pMsgBuf										=	NULL;

	char*	pchBackChar									=	NULL;	
	char*	pszFindChar									=	NULL;

	char	szFinalErrLogFile[MAX_PATH + 100]			=	{0};
	char	szDateTime[101]								=	{0};
	char	szDate[101]									=	{0};

	char	szLineCode[25]								=	{0};
	char	szSystemCode[25]							=	{0};
	char	szMiscCode[25]								=	{0};

	int		iReqMemAllocated							=	0;
	__try
	{
		__try
		{
			//prepare the error file
			lstrcpy(szFinalErrLogFile,szFilePath);

			memset(szDate,NULL,25);
			memset(szDateTime,NULL,25);

			GetDateTime(szDate,szDateTime);

			ltoa(llastLineCode,szLineCode,10);
			ltoa(lSystemCode,szSystemCode,10);
			ltoa(lMiscCode,szMiscCode,10);



			//calculate the memory requirements
			//szDateTime
			iReqMemAllocated	+=	30;
			iReqMemAllocated	+=	5;	
			//llastLineCode   
			iReqMemAllocated	+=	30;
			iReqMemAllocated	+=	5;	
			//lModuleCode
			iReqMemAllocated	+=	30;
			iReqMemAllocated	+=	5;	
			//lSystemCode
			iReqMemAllocated	+=	30;
			iReqMemAllocated	+=	5;	
			//lMiscCode
			iReqMemAllocated	+=	30;
			iReqMemAllocated	+=	5;	



			if(NULL != pszModuleErrorString)
			{
				iReqMemAllocated	+= lstrlen(pszModuleErrorString);
				iReqMemAllocated	+=	5;	
			}

			if(NULL != pAddInfo1)
			{
				iReqMemAllocated	+= lstrlen(pAddInfo1);
				iReqMemAllocated	+=	5;	
			}

			if(NULL != pAddInfo2)
			{
				iReqMemAllocated	+= lstrlen(pAddInfo2);
				iReqMemAllocated	+=	5;	
			}

			if(NULL != pAddInfo3)
			{
				iReqMemAllocated	+= lstrlen(pAddInfo3);
				iReqMemAllocated	+=	5;	
			}

			if(0 != lSystemCode)
			{
				//Chek out for any last error memory req
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER	| 
								FORMAT_MESSAGE_FROM_SYSTEM		| 
								FORMAT_MESSAGE_IGNORE_INSERTS,
								NULL,
								lSystemCode,
								MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
								(LPTSTR) &pMsgBuf,
								0,
								NULL);

				if(NULL != pMsgBuf)
				{
					iReqMemAllocated	+= lstrlen((char*)pMsgBuf);
					iReqMemAllocated	+=	5;	

					pchBackChar = strrchr((LPTSTR) pMsgBuf,'\n');
					if(NULL != pchBackChar)
					{
						*pchBackChar = NULL;
					}

					pchBackChar = strrchr((LPTSTR) pMsgBuf,'\r');
					if(NULL != pchBackChar)
					{
						*pchBackChar = NULL;
					}
				}
			}

			iReqMemAllocated	+=	151;

			pszFinalErrorString	=	new char[iReqMemAllocated];
			if(NULL == pszFinalErrorString)
			{
				return 1;
			}

			memset(pszFinalErrorString,NULL,iReqMemAllocated);
			//Copy the whole strings
			lstrcat(pszFinalErrorString,szDateTime);
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);
			lstrcat(pszFinalErrorString,szLineCode);
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);
			if(NULL != szModuleCode)
			{
				lstrcat(pszFinalErrorString,szModuleCode);
			}
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);
			lstrcat(pszFinalErrorString,szSystemCode);
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);
			lstrcat(pszFinalErrorString,szMiscCode);
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);

			if(NULL != pszModuleErrorString)
			{
				lstrcat(pszFinalErrorString,pszModuleErrorString);
			}
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);

			if(NULL != pMsgBuf)
			{
				lstrcat(pszFinalErrorString,(char*)pMsgBuf);
			}
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);
			

			if(NULL != pAddInfo1)
			{
				lstrcat(pszFinalErrorString,pAddInfo1);
			}
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);

			if(NULL != pAddInfo2)
			{
				lstrcat(pszFinalErrorString,pAddInfo2);
			}
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);

			if(NULL != pAddInfo3)
			{
				lstrcat(pszFinalErrorString,pAddInfo3);
			}
			lstrcat(pszFinalErrorString,g_szFieldDelimiter);

			lstrcat(pszFinalErrorString,g_szRecDelimiter);


			//Log the Error

			LogErrorToDisk(szFinalErrLogFile,pszFinalErrorString);


		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			return 1;
		}
	}
	__finally
	{
		delete [] pszFinalErrorString;
		pszFinalErrorString = NULL;


		if(NULL != pMsgBuf)
		{
			LocalFree(pMsgBuf);
			pMsgBuf = NULL;
		}
	}
	return 0;	
}


DWORD	LogErrorToDisk(TCHAR *szModuleFilePath,
							TCHAR* szErrorString)
{
	TCHAR		szFullPathDirectory[MAX_PATH + 100]			=	{0};
	TCHAR		szFileName[MAX_PATH + 100]					=	{0};	
	TCHAR		szLogPath[MAX_PATH + 100]					=	{0};
	TCHAR		szTempLogPath[MAX_PATH + 100]				=	{0};
	TCHAR		szFullLogPath[MAX_PATH + 100]				=	{0};
	TCHAR		szMutexName[MAX_PATH + 100]					=	{0};
	TCHAR		szBuffer[51]								=	{0};
	TCHAR*		szFindChar									=	NULL;
	TCHAR*		pszString									=	NULL;
	TCHAR		chString									=	NULL;


	HANDLE		hFile										=	NULL;
	HANDLE		hMutex										=	NULL;
	HANDLE		hWaitHandle[2]								=	{0};					
	
	DWORD		nResult										=	0;
	DWORD		dwFileSizeLow								=	0;
	DWORD		dwFileSizeHigh								=	0;
	DWORD		dwBufferSize								=	0;
	DWORD		dwSize										=	0;
	DWORD		dwNumOfBytesWritten							=	0;

	long		lRet										=	0;
	int			iCtr										=	0;
	int			iLen										=	0;	

	BOOL		bln											=	FALSE;
	BOOL		bMutexFail									=	FALSE;

	__try
	{
		__try
		{
			//First check for the ErrorString and the Module Path
			if((NULL == szErrorString)	||
				(NULL == szModuleFilePath))
			{
				return 1;
			}


			//Get the length of the module path
			iLen = lstrlen(szModuleFilePath);
			if(0 == iLen)
			{
				return 1;
			}

			//Replace the Slashes with the '-' for the mutex, since
			//its the restriction placed by the Mutex
			for(iCtr = 0; iCtr < iLen; ++iCtr)
			{
				if('\\' == szModuleFilePath[iCtr])
				{
					szMutexName[iCtr] = '-';
				}
				else
				{
					szMutexName[iCtr] = szModuleFilePath[iCtr];
				}
			}


			//Create the Mutex and wait
			hMutex = CreateMutex(NULL,FALSE,szMutexName);
			if(NULL == hMutex)
			{
				return 1;
			}
			
			//Wait for the mutex atleast 5 Minutes
			nResult = WaitForSingleObject(hMutex,(300 * 1000));
			if(WAIT_OBJECT_0 != nResult)
			{
				//Unable to get Mutex, return from here
				::ReleaseMutex(hMutex);
				CloseHandle(hMutex);
				hMutex = NULL;
				return 1;
			}
			

			lstrcpy(szFullLogPath,szModuleFilePath);

			//Remove the file name
			szFindChar = strrchr(szFullLogPath,'\\');
			if(NULL != szFindChar)
			{
				lstrcpy(szFileName,szFindChar);
				*szFindChar = NULL;
			}

			//Create the full path here
			lstrcpy(szFullPathDirectory,szFullLogPath);
			lstrcat(szFullPathDirectory,szFileName);

			if(0 == ::SetFileAttributes(szFullPathDirectory,FILE_ATTRIBUTE_NORMAL))
			{
				CreateLogDirectory(szFullLogPath);
			}

			//Create respective directories and sub directories
			//CreateRecursiveDirectory(szFullLogPath);

			//Get the length of the error string
			dwBufferSize = lstrlen(szErrorString);


			//Set the file attributes to normal
			//::SetFileAttributes(szFullPathDirectory,FILE_ATTRIBUTE_NORMAL);

			//Create or opens the existing log file
			hFile = CreateFile(szFullPathDirectory,
								GENERIC_READ | GENERIC_WRITE,
								FILE_SHARE_READ,
								NULL,
								OPEN_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
					
			if((hFile == INVALID_HANDLE_VALUE)		||
				(hFile == NULL))
			{
				//Unable to open file, return from her
				hFile = NULL;
				return 1;
			}
			
			//Get the log file size
			dwFileSizeLow = GetFileSize(hFile,&dwFileSizeHigh);

			if(dwFileSizeLow > (1024 * 1024))
			{
				//If the log file size is more than 1 MB, need to delete the log file

				//Close all the previous file handles
				::CloseHandle(hFile);
				hFile = NULL;

				//Set the file attributes to normal
				::SetFileAttributes(szFullPathDirectory,FILE_ATTRIBUTE_NORMAL);

				//Delete the log file
				::DeleteFile(szFullPathDirectory);


				//Create or opens the existing log file
				hFile = CreateFile(szFullPathDirectory,
									GENERIC_READ | GENERIC_WRITE,
									FILE_SHARE_READ,
									NULL,
									OPEN_ALWAYS,
									FILE_ATTRIBUTE_NORMAL,
									NULL);
						
				if((hFile == INVALID_HANDLE_VALUE)		||
					(hFile == NULL))
				{
					//Unable to open file for writting, return from here
					hFile = NULL;
					return 1;
				}
				
				//Get the log file size
				dwNumOfBytesWritten =	0;
				dwFileSizeLow		=	0;	
			}

			if(0 != dwFileSizeLow)
			{
				//Set the file pointer to the last offset
				SetFilePointer(hFile, 0, NULL, FILE_END);
			}

			dwNumOfBytesWritten = 0;
			//Write the data to the file
			::WriteFile(hFile,szErrorString,dwBufferSize,&dwNumOfBytesWritten,NULL);

			//Close the file handle
			CloseHandle(hFile);
			hFile = NULL;


			//Release the mutex now
			::ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			hMutex = NULL;



			return 0;
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			//Close the file handle
			if(NULL != hFile)
			{
				CloseHandle(hFile);
				hFile = NULL;
			}


			//Release the mutex now
			if(NULL != hMutex)
			{
				::ReleaseMutex(hMutex);
				CloseHandle(hMutex);
				hMutex = NULL;
			}

			return 1;
		}
	}

	__finally
	{
		//Close the file handle
		if(NULL != hFile)
		{
			CloseHandle(hFile);
			hFile = NULL;
		}

		//Release the nutex now
		if(NULL != hMutex)
		{
			::ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			hMutex = NULL;
		}
	}
	return 0;
}

int		WritePID(char* szMachineName, char* szRegId, char* szDescription)
{
	char	szUploadPath[MAX_PATH + 100]		=	{0};
	char	szUploadDir[MAX_PATH + 100]			=	{0};
	char	szPath[MAX_PATH + 100]				=	{0};
	char	szINIFileName[MAX_PATH + 100]		=	{0};
	char	szLastRunDateTime[MAX_PATH + 100]	=	{0};
	char	szDef[10]							=	{0};
	char*	pszPIDDesc							=	NULL;


	DWORD	dwBufferSize						=	0;
	DWORD	dwNumOfBytesWritten					=	0;

	HANDLE	hFile								=	NULL;

	__try
	{
		__try
		{
			if(0 == ::GetPrivateProfileInt(szMachineName,
											"Status",
											1,
											g_szInternalStatusINI))
			{
				return 0;
			}
			else
			{
				::WritePrivateProfileString(szMachineName,
											"Status",
											"0",
											g_szInternalStatusINI);
			}

			::GetPrivateProfileString("SAAZFolders",
										"UploadFolder",
										szDef,
										szPath,
										(MAX_PATH + 99),
										g_szSAAZMobilePath);

			if(NULL == szPath[0])
			{
				lstrcpy(szPath,"\\PMSTATUS");
			}

			lstrcpy(szUploadPath,g_szModulePath);
			lstrcat(szUploadPath,szPath);
			GetTempFileName(szINIFileName);
			
			lstrcat(szUploadPath,"\\");
			lstrcat(szUploadPath,szINIFileName);
			lstrcat(szUploadPath,".ini");

			//Write the last run date time
			GetAlertDateTime(szLastRunDateTime);

			pszPIDDesc = new char[1024];
			if(NULL == pszPIDDesc)
			{
				return 0;
			}

			memset(pszPIDDesc,NULL,1024);

			sprintf(pszPIDDesc,
					"[STATUS_INFO]\r\nREGID=%s\r\nDesc=%s\r\nPID=Application Monitor\r\nSysID=\r\nLastRunDateTime=%s\r\nNextRunDateTime=\r\nLastRunStatusCode=%s\r\n",
					szRegId,
					szDescription,
					szLastRunDateTime,
					"FAILED");

			dwBufferSize = lstrlen(pszPIDDesc);
			

			hFile = CreateFile(szUploadPath,
								GENERIC_READ | GENERIC_WRITE,
								FILE_SHARE_READ,
								NULL,
								OPEN_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
					
			if((hFile == INVALID_HANDLE_VALUE)		||
				(hFile == NULL))
			{
				//Unable to open file, return from her
				hFile = NULL;

				lstrcpy(szUploadDir,g_szModulePath);
				lstrcat(szUploadDir,szPath);
				::CreateDirectory(szUploadDir,NULL);


				hFile = CreateFile(szUploadPath,
									GENERIC_READ | GENERIC_WRITE,
									FILE_SHARE_READ,
									NULL,
									OPEN_ALWAYS,
									FILE_ATTRIBUTE_NORMAL,
									NULL);
						
				if((hFile == INVALID_HANDLE_VALUE)		||
					(hFile == NULL))
				{
					//Unable to open file, return from her
					hFile = NULL;
					return 1;
				}
			}

			
			::WriteFile(hFile,pszPIDDesc,dwBufferSize,&dwNumOfBytesWritten,NULL);

			::CloseHandle(hFile);
			hFile = NULL;

			delete [] pszPIDDesc;
			pszPIDDesc = NULL;

			return 0;
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{

		}
	}

	__finally
	{
		if(NULL != pszPIDDesc)
		{
			delete [] pszPIDDesc;
			pszPIDDesc = NULL;
		}

		if(NULL != hFile)
		{
			::CloseHandle(hFile);
			hFile = NULL;
		}
	}
	return 0;
}

int		CheckSchedule(char* szWeekDays, float fstartTime, float fendtime)
{
	SYSTEMTIME		sysTime			=	{0};
	char			szCurrDay[10]	=	{0};
	char			szTime[10]		=	{0};
	float			fCurrTime		=	0.0;

	int				iRet			=	0;

	try
	{
		::GetLocalTime(&sysTime);
		sprintf(szCurrDay,"%d",sysTime.wDayOfWeek);
		sprintf(szTime,"%02d.%02d",sysTime.wHour,sysTime.wMinute);


		if(NULL != szWeekDays)
		{
			if(NULL != strchr(szWeekDays,*szCurrDay))
			{
				sscanf(szTime,"%f",&fCurrTime);
				//Check for time
				if((fCurrTime >=  fstartTime )	&&
					(fCurrTime <= fendtime))
				{
					iRet = 1;
				}
			}
		}
	}
	catch(...)
	{


	}
	return iRet;
}

int		CheckTime(char* szStartTime, char* szEndTime)
{
	int		iNum			=	0;
	char*	pszFindChar		=	NULL;
	try
	{
		//For Start Time
		pszFindChar = strchr(szStartTime,':');
		if(NULL != pszFindChar)
		{
			*pszFindChar	=	NULL;
			iNum	=	atoi(szStartTime);	
			if((iNum < 0)	||
				(iNum > 23))
			{
				return 0;
			}

			*pszFindChar	=	':';
			iNum	=	atoi(pszFindChar + 1);	
			if((iNum < 0)	||
				(iNum > 59))
			{
				return 0;
			}
		}
		else
		{
			iNum	=	atoi(szStartTime);	
			if((iNum < 0)	||
				(iNum > 23))
			{
				return 0;
			}
		}

		//For End Time
		pszFindChar = strchr(szEndTime,':');
		if(NULL != pszFindChar)
		{
			*pszFindChar	=	NULL;
			iNum	=	atoi(szEndTime);	
			if((iNum < 0)	||
				(iNum > 23))
			{
				return 0;
			}

			*pszFindChar	=	':';
			iNum	=	atoi(pszFindChar + 1);	
			if((iNum < 0)	||
				(iNum > 59))
			{
				return 0;
			}
		}
		else
		{
			iNum	=	atoi(szEndTime);	
			if((iNum < 0)	||
				(iNum > 23))
			{
				return 0;
			}
		}

	}
	catch(...)
	{
		return 0;
	}
	return 1;
}

void	CheckPerfProcess(stProcessInfo*	pstProcessInfo,
							int	iNumProcess)
{
	 //First get the number of process
	long iNum = PerfDataGetProcessCount();
	ULONG i    = 0;
	int		iCtr	=	0;
	ULONG iProcCount = 0;
	char	szProcessExe[MAX_PATH + 10] = {0};
	char	szProcess[MAX_PATH + 10]	=	{0};
	char*	pszFindChar				=	NULL;


	
	try
	{
		for(i = 0; i < iNum; ++i)
		{
			memset(szProcess,NULL,(MAX_PATH + 10));
			memset(szProcessExe,NULL,(MAX_PATH + 10));
			PerfDataGetImageName(i,szProcessExe,MAX_PATH);

			for(iCtr = 0; iCtr < iNumProcess; ++iCtr)
			{
				lstrcpy(szProcess,szProcessExe);

				strupr(szProcess);
				pszFindChar = strstr(szProcess,".EXE");

				if(NULL != pszFindChar)
				{
					*pszFindChar = '\0';
				}	


				if((0 == lstrcmpi(szProcessExe,pstProcessInfo[iCtr].szProcessName))		||
					(0 == lstrcmpi(szProcess,pstProcessInfo[iCtr].szProcessName)))
				{
					pstProcessInfo[iCtr].iStatus	=	1;
					//break;
				}
			}
		}
	}
	catch(...)
	{

	}
}


void	CaptureServiceEvents()
{
	char				szMainKey[MAX_PATH + 100]			=	{0};
	char				szFile[MAX_PATH + 100]				=	{0};
	char				szExFile[MAX_PATH + 100]			=	{0};
	char				szTimeINIFile[MAX_PATH + 100]		=	{0};
	char				szDef[10]							=	{0};
	char				szLastTime[101]						=	{0};
	HKEY				hKey								=	NULL;	
	HANDLE				hHandle								=	NULL;
	
	DWORD				dwSize								=	0;
	DWORD				dwType								=	0;
	DWORD				dwMaxBuffer							=	BUFFER_SIZE;
	DWORD				dwTimeGenerated						=	0;
	DWORD				dwFirstTime							=	0;
	time_t				tmLastFiveMinutes					=	0;

	EVENTLOGRECORD*		pevlr								=	NULL;
	BYTE*				pBuffer								=	NULL;
	DWORD				dwRead								=	0;
	DWORD				dwNeeded							=	0;
	BOOL				bRet								=	TRUE;	
	int					iCtr								=	0;
	int					iMainLen							=	0;
	int					iEventId							=	0;
	char*				pszString							=	NULL;

	try
	{
		lstrcpy(szTimeINIFile,g_szModulePath);
		lstrcat(szTimeINIFile,"\\Configuration\\App\\ZAppEvent.ini");

		lstrcpy(szMainKey,"SYSTEM\\CurrentControlSet\\Services\\EventLog\\System");

		::RegOpenKeyEx(HKEY_LOCAL_MACHINE,szMainKey,0,KEY_READ,&hKey);
		if(NULL == hKey)
		{
			return;
		}

		dwSize = MAX_PATH + 99;
		::RegQueryValueEx(hKey,"File",0,&dwType,(LPBYTE)szFile,&dwSize);

		::RegCloseKey(hKey);
		hKey = NULL;


		if(NULL == szFile[0])
		{
			return;
		}

		ExpandEnvironmentStrings(szFile,szExFile,(MAX_PATH + 99));
		if(NULL == szExFile[0])
		{
			return;
		}



		hHandle	=	::OpenEventLog(NULL,"System");
		if(NULL == hHandle)
		{
			return;
		}

		pBuffer	=	new BYTE[dwMaxBuffer];
		if(NULL == pBuffer)
		{
			return;
		}
		memset(pBuffer,NULL,dwMaxBuffer);

		pevlr = (EVENTLOGRECORD*)pBuffer;

		GetPrivateProfileString("System",
								"Time",
								szDef,
								szLastTime,
								100,
								szTimeINIFile);

		if(NULL != szLastTime[0])
		{
			sscanf(szLastTime,"%u",&tmLastFiveMinutes);	
		}
		else
		{
			time(&tmLastFiveMinutes);
			tmLastFiveMinutes -= 300;
		}


		while(bRet)
		{
			dwNeeded = 0;
			dwRead = 0;

			bRet = ::ReadEventLog(hHandle,
									EVENTLOG_BACKWARDS_READ | EVENTLOG_SEQUENTIAL_READ,
									0,
									pevlr,
									dwMaxBuffer,
									&dwRead,
									&dwNeeded);
									
		
			if(FALSE == bRet)
			{
				if(ERROR_INSUFFICIENT_BUFFER == ::GetLastError())
				{
					delete [] pBuffer;
					pBuffer = NULL;
					pevlr = NULL;

					dwMaxBuffer += dwNeeded;

					pBuffer	=	new BYTE[dwMaxBuffer];
					if(NULL == pBuffer)
					{
						return;
					}
					memset(pBuffer,NULL,dwMaxBuffer);

					pevlr = (EVENTLOGRECORD*)pBuffer;
					bRet = TRUE;
					continue;
				}
				else
				{
					break;
				}
			}

			if(0 == dwRead)
			{
				break;
			}


			while(dwRead > 0)
			{
				if(NULL == szLastTime[0])
				{
					if(tmLastFiveMinutes > pevlr->TimeGenerated)
					{
						bRet = FALSE;
						break;
					}
				}
				else
				{
					if(tmLastFiveMinutes >= pevlr->TimeGenerated)
					{
						bRet = FALSE;
						break;
					}
				}

				if(0 == dwFirstTime)
				{
					dwFirstTime = pevlr->TimeGenerated;
				}


				if(EVENTLOG_ERROR_TYPE == pevlr->EventType)
				{
					if(0 == lstrcmpi("Service Control Manager",(LPSTR) ((LPBYTE) pevlr + sizeof(EVENTLOGRECORD))))
					{
						iEventId = LOWORD(pevlr->EventID);
						if((7016 == iEventId)	||
							(7021 == iEventId)	||
							(7037 == iEventId)	||
							(7000 == iEventId)	||
							(7001 == iEventId)	||
							(7002 == iEventId)	||
							(7003 == iEventId)	||
							(7017 == iEventId)	||
							(7019 == iEventId)	||
							(7020 == iEventId)	||
							(7022 == iEventId)	||
							(7038 == iEventId)	||
							(7041 == iEventId)	||
							(7023 == iEventId)	||
							(7024 == iEventId)	||
							(7032 == iEventId)	||
							(7034 == iEventId)	||
							(7043 == iEventId))
						{
							pszString = NULL;
							if(pevlr->NumStrings > 0)
							{
								pszString = ((char*) pevlr  + pevlr->StringOffset);
								AddToList(g_pServiceEventRoot,pszString,iEventId);
							}
						}
					}
				}

				dwRead -= pevlr->Length;
				pevlr = (EVENTLOGRECORD*) ((LPBYTE)pevlr + pevlr->Length);

			}//end of inside while loop
			pevlr = (EVENTLOGRECORD*)pBuffer;
			memset(pBuffer,NULL,dwMaxBuffer);
		}//end of outer while loop

		::CloseEventLog(hHandle);
		hHandle = NULL;

		delete [] pBuffer;
		pBuffer = NULL;

		if(0 != dwFirstTime)
		{
			memset(szLastTime,NULL,101);
			ultoa(dwFirstTime,szLastTime,10);
			::WritePrivateProfileString("System",
										"Time",
										szLastTime,
										szTimeINIFile);
		}

	}
	catch(...)
	{

	}
}

int GetRandomHour(){

	int num;
	srand(time(NULL));
	num = rand();
	num = num % 24;

	return num;
}

bool CallWPForUpdateXML(char * pszMachineName)
{
	char		strREGID[100],strAgentType[100];
	char		RegPath[255]				=	{""};
	char		szMemberId[30]				=   {""};
	char		szSiteId[30]				=   {""};
	char		errmsg[256]					=   {""};
	char		URL[512]					=	{""};
	char		configxmlpath[MAX_PATH+100] =   {""};
	char		retDns[356]					=	{""};
	HANDLE hFile = NULL ;
	
					
	USES_CONVERSION ;

	try{
		
			string data = "<wpappmonconfigxml></wpappmonconfigxml>";
			BOOL bIs64 = IsWow64();
			if(bIs64 == TRUE){
				strcpy(RegPath, "Software\\Wow6432Node\\SAAZOD");
			}else{
				strcpy(RegPath, "Software\\SAAZOD");
			}

			if(!GetRegValue(RegPath,  "REGID",  strREGID))
			{
				return false;
			}
			if(!GetRegValue(RegPath,  "TYPE",  strAgentType))
			{
				return false;
			}
			//Get mmid,sid form SAAZMobile.ini
			GetPrivateProfileString("CONTROLLER_INFO", //sectionname
									"MemberId", //key
									 NULL,		//default
									 szMemberId, //szReturnedString
									 30,		 //dsSize
									 g_szSAAZMobilePath //filepath
									);

			GetPrivateProfileString("CONTROLLER_INFO", //sectionname
									"SiteId", //key
									 NULL,		//default
									 szSiteId, //szReturnedString
									 30,		 //dsSize
									 g_szSAAZMobilePath //filepath
									);
			if(!GetDNSFromINI("wpappmonconfigxml", retDns))
			{
				LogLastError(g_szErrorLogPath,
											9999,
											NULL,
											0,
											0,
											NULL,
											"INFO:   could not find wpappmonconfigxml DNS in zScriptDns.ini",
											"",
											"");
				return false;
			}
		
		    sprintf(URL, 
				    "%s/tfr_wpappmonconfigxml.asp?mmid=%s&sid=%s&rid=%s&rty=%s",
					 retDns,
					 szMemberId,
					 szSiteId,
					 strREGID,
					 strAgentType);
		  
			WinHttpClient client(A2W(URL));
			
			//Set post data
			client.SetAdditionalDataToSend((BYTE *)data.c_str(), data.size());

			// Set request headers.
			wchar_t szSize[50] = L"";
			swprintf_s(szSize, L"%d", data.size());
			wstring headers = L"Content-Length: ";
			headers += szSize;
			headers += L"\r\nContent-Type: application/x-www-form-urlencoded\r\n";
			client.SetAdditionalRequestHeaders(headers);

			// Send http post request.
			client.SendHttpRequest(L"POST");

			wstring httpResponseHeader = client.GetResponseHeader();
			wstring ResStatusCode = client.GetResponseStatusCode();

			if( ResStatusCode == L"200")
			{
			   wstring httpResponseContent = client.GetResponseContent();
			   if(!httpResponseContent.empty())
			   {
				  	lstrcpy(configxmlpath,g_szModulePath);
					lstrcat(configxmlpath,"\\Configuration\\APP\\");
					lstrcat(configxmlpath,pszMachineName);
					lstrcat(configxmlpath,"_Config.xml");
					sprintf(errmsg,"ERR: Open %s_Config.xml failed",pszMachineName);
					
					//convert wstring to string
					 const wchar_t *str = httpResponseContent.c_str();
					 int xmlfilesize = httpResponseContent.length();
					 //xmlfilesize=xmlfilesize+1;
					 char *xmlData = new char[xmlfilesize];
					 memset(xmlData,0,xmlfilesize);
					 int ret = wcstombs(xmlData,str,xmlfilesize);
					 if(ret == xmlfilesize) 
						 xmlData[xmlfilesize] = '\0';
									 	
					/* Open file in text mode: */
					FILE *fHandle; int numwritten;
					if( (fHandle= fopen( configxmlpath, "w+t" )) != NULL )
					{
						numwritten = fwrite( xmlData, sizeof( char ),xmlfilesize,fHandle);
						fclose(fHandle);
					}
					else
					{
						LogLastError(g_szErrorLogPath,
								9999,
								NULL,
								0,
								0,
								NULL,
								"ERR:Error in opening config.xml file while writing status",
								NULL,
								NULL);
						return false;
					}

			}
			else{ 
				
				LogLastError(g_szErrorLogPath,
										9999,
										NULL,
										0,
										0,
										NULL,
										"ERR:WP for Config.xml Update Respoce is Blank",
										NULL,
										NULL);
				return false; 
			}
		}
		else
		{	
				const wchar_t *wstr =ResStatusCode.c_str();
				int statusSize=ResStatusCode.length();
				char *pszstatusCode = new char[statusSize];
				int ret = wcstombs(pszstatusCode,wstr,statusSize);
				if(ret == statusSize) 
						 pszstatusCode[statusSize] = '\0';

				LogLastError(g_szErrorLogPath,
										9999,
										NULL,
										0,
										0,
										NULL,
										pszstatusCode,
										NULL,
										NULL);
				delete [] pszstatusCode;
				pszstatusCode = NULL;
			   return false; 
		}
	}
	catch(...){
	}
}

//this function is getting the MD5 checksum of Config.xml and returns bool status
bool CallWPForXMLMD5(char * pszMD5)
{
	char		strREGID[100],strAgentType[100];
	char		RegPath[255]				=	{""};
	char		szMemberId[30]				=   {""};
	char		szSiteId[30]				=   {""};
	char		errmsg[256]					=   {""};
	char		URL[512]					=	{""};
	char		configxmlpath[MAX_PATH+100] =   {""};
	char		retDns[356]					=	{""};
	HANDLE hFile = NULL ;
	
					
	USES_CONVERSION ;

	try{
		
			string data = "<wpappmonmd5></wpappmonmd5>";
			BOOL bIs64 = IsWow64();
			if(bIs64 == TRUE){
				strcpy(RegPath, "Software\\Wow6432Node\\SAAZOD");
			}else{
				strcpy(RegPath, "Software\\SAAZOD");
			}

			if(!GetRegValue(RegPath,  "REGID",  strREGID))
			{
				return false;
			}
			if(!GetRegValue(RegPath,  "TYPE",  strAgentType))
			{
				return false;
			}
			//Get mmid,sid form SAAZMobile.ini
			GetPrivateProfileString("CONTROLLER_INFO", //sectionname
									"MemberId", //key
									 NULL,		//default
									 szMemberId, //szReturnedString
									 30,		 //dsSize
									 g_szSAAZMobilePath //filepath
									);

			GetPrivateProfileString("CONTROLLER_INFO", //sectionname
									"SiteId", //key
									 NULL,		//default
									 szSiteId, //szReturnedString
									 30,		 //dsSize
									 g_szSAAZMobilePath //filepath
									);
			if(!GetDNSFromINI("wpappmonmd5", retDns))
			{
				LogLastError(g_szErrorLogPath,
											9999,
											NULL,
											0,
											0,
											NULL,
											"INFO:   could not find wpappmonmd5 DNS in zScriptDns.ini",
											"",
											"");
				return false;
			}
		
		    sprintf(URL, 
				    "%s/tfr_wpappmonmd5.asp?mmid=%s&sid=%s&rid=%s&rty=%s",
					 retDns,
					 szMemberId,
					 szSiteId,
					 strREGID,
					 strAgentType);
		  
			WinHttpClient client(A2W(URL));
			
			//Set post data
			client.SetAdditionalDataToSend((BYTE *)data.c_str(), data.size());

			// Set request headers.
			wchar_t szSize[50] = L"";
			swprintf_s(szSize, L"%d", data.size());
			wstring headers = L"Content-Length: ";
			headers += szSize;
			headers += L"\r\nContent-Type: application/x-www-form-urlencoded\r\n";
			client.SetAdditionalRequestHeaders(headers);

			// Send http post request.
			client.SendHttpRequest(L"POST");

			wstring httpResponseHeader = client.GetResponseHeader();
			wstring ResStatusCode = client.GetResponseStatusCode();

			if( ResStatusCode == L"200")
			{
			   wstring httpResponseContent = client.GetResponseContent();
			   if(!httpResponseContent.empty())
			   {
				  	
					//convert wstring to string
					 const wchar_t *str = httpResponseContent.c_str();
					 int xmlMd5Size = httpResponseContent.length();
					 wcstombs(pszMD5,str,xmlMd5Size);// conversion from wide char set to multy byte string
     			}
				return true;
			}
			else{ 
				
				const wchar_t *wstr =ResStatusCode.c_str();
				int statusSize=ResStatusCode.length();
				char *pszstatusCode = new char[statusSize];
				wcstombs(pszstatusCode,wstr,statusSize);
				LogLastError(g_szErrorLogPath,
										9999,
										NULL,
										0,
										0,
										NULL,
										pszstatusCode,
										NULL,
										NULL);
				delete [] pszstatusCode;
				pszstatusCode = NULL;
			   return false; 
			}
		}
		catch(...){
		}
}


int GetUpdateConfigXML(char *pszMachineName)
{
	char NowSecs[100]		= {""};
	char PrevSecs[100]		= {""};
	char configxmlpath[356] = {""};
	char sectionName[50]	= {""};
	char curConfigMd5[50]	= {""};
	char curMD5[50]        = {""};
	char szPrevMD5[50]     = {""};
	char szHr[3]           = {""};
	char szRandomHr[3]      ={""};
	int randomHour=0;
	
	bool statusMd5,statusConfigFile;
	time_t curTimeInSec;

	time(&curTimeInSec); //Get Current time in sec elapsed since 1 Jan 1970    1421309186

	itoa(curTimeInSec,NowSecs,10);

	strcpy(sectionName,pszMachineName);
	strcat(sectionName,"_DailyConfigUpdate");
	
	try
	{
		GetPrivateProfileString(sectionName,"LastSent",NULL, PrevSecs, 100, g_szInternalStatusINI);
		
		if(strcmp(PrevSecs, "")==0) //first time
		{
			WritePrivateProfileString(sectionName,"LastSent",NowSecs,g_szInternalStatusINI);
			//Get ConfigFile and its checksum from DataCenter
			statusConfigFile =	CallWPForUpdateXML(pszMachineName);
			if(!statusConfigFile)
			{
				LogLastError(g_szErrorLogPath,
										9999,
										NULL,
										0,
										0,
										NULL,
										"ERR: WebPost Failed while getting Config.xml file update first time",
										NULL,
				     					NULL);
				return 0;	
			}
			statusMd5        =	CallWPForXMLMD5(szPrevMD5);
			if(statusMd5)
			{
				WritePrivateProfileString(sectionName,"Checksum",szPrevMD5,g_szInternalStatusINI);
				LogLastError(g_szErrorLogPath,
										9999,
										NULL,
										0,
										0,
										NULL,
										"MSG: WebPost success Config.xml file is updated first time",
										NULL,
				     					NULL);
				return 1; 
			}
			else
			{
				LogLastError(g_szErrorLogPath,
										9999,
										NULL,
										0,
										0,
										NULL,
										"ERR: WebPost Failed while getting MD5 of Config.xml file first time",
										NULL,
				     					NULL);
				return 0;	
			}
		}

		unsigned int prevSec = atoi(PrevSecs);					// (sec to min) * (min to hrs)
		int differenceInHrs = ((unsigned int)curTimeInSec - prevSec) / (60*60) ; //time in hrs

		if(differenceInHrs>=24) //time to getupdate configxml file randomly once in a day
		{
			  //get random hour
			  int randomHour = GetRandomHour();
			  itoa(randomHour,szHr,10);
			  WritePrivateProfileString(sectionName,"RandomHour",szHr,g_szInternalStatusINI);
		}

		GetPrivateProfileString(sectionName,"RandomHour",NULL,szRandomHr,3,g_szInternalStatusINI);
		int randomHr = atoi(szRandomHr);
		SYSTEMTIME st;
		GetLocalTime(&st);
		   
		if(randomHr == st.wHour) //if matched send webpost for config md5
		{
				statusMd5        =	CallWPForXMLMD5(szPrevMD5);

				//Putting the blank to RandomHour Indicating xml updation has done in a day
				WritePrivateProfileString(sectionName,"RandomHour"," ",g_szInternalStatusINI);
				if(statusMd5)
				{
					GetPrivateProfileString(sectionName,"Checksum",NULL,curMD5,50, g_szInternalStatusINI);
					
					if( lstrcmpi(szPrevMD5,curMD5) != 0 ) //time to send webpost for update config.xml
					{
						statusConfigFile =	CallWPForUpdateXML(pszMachineName);
						if(statusConfigFile)
						{
							LogLastError(g_szErrorLogPath,
										9999,
										NULL,
										0,
										0,
										NULL,
										"MSG: WebPost success Config.xml file is updated",
										NULL,
				     					NULL);
							return 1;	
							
						}
						else
						{
							LogLastError(g_szErrorLogPath,
										9999,
										NULL,
										0,
										0,
										NULL,
										"ERR: WebPost Failed while getting Config.xml file update first time",
										NULL,
				     					NULL);
							return 0;	
						}
					}
					return 1; 
				}
				else{
					LogLastError(g_szErrorLogPath,
											9999,
											NULL,
											0,
											0,
											NULL,
											"ERR: WebPost Failed while getting MD5 of Config.xml file first time",
											NULL,
				     						NULL);
					return 0;	
				}
		   }
		
	}
	catch(...)
	{
		return false;
	}

	return false;
}






///////////////////////////////////////////////////////////////////////////////////////////////////////////