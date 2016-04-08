// BEXShare.cpp : Defines the entry point for the application.
//


// BEXShare.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <atlbase.h>
#include <lm.h>
#include <Lmshare.h>

#define		FILE_SIZE	300

char *ToWideChar(const char *ANSIstring); // Convertit une chaine de caracteres en widechar
long SplitString(char *szString, char *sep, char** &arrValues);
long WriteToLog(char *szFileName,char *szMessage);
DWORD	GetDateTime(char*	szDate, char* szDateTime);
long CreateDirectoriesRecursively(char *szInDir);
bool GeneratePID(char *szRegId,char *szStatus,char *Desc);
bool ReadRegistryForLocalRegid(char *szRegPath,char *szRegId,char *&szError);
DWORD	GetCurrentDateTime(char* szDate);
HRESULT WriteToBuffer(LPTSTR	&lpszBuffer,	//[in,out]Destination pointer where the buffer is located
						LPCTSTR lpszString,		//[in]String to be copied to the buffer
						BOOL bRefresh = FALSE);	//[in]Initialize the buffer if it is TRUE, default is FALSE

DWORD	GetTimeStamp(char* szDate);
long ServiceDetails(char* szPath);

const DWORD g_dwExtraBuffer	= 256;


#define PID_TEXT "[STATUS_INFO]\r\nREGID=%s\r\nPID=VeritasBackup \r\nSysID=\r\nLastRunDateTime=%s\r\nNextRunDateTime=\r\nLastRunStatusCode=%s\r\nDesc=%s\r\n\0"



int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.




	char
		szDate[30]					= {0},
		szDateTime[30]				= {0},
		szTemp[_MAX_PATH]			= {0},
		szApplicationLog[_MAX_PATH] = {0};

	try
	{
		char
			*ptr  = NULL,
			*ptr1 = NULL,
			szBexRegVerPath[_MAX_PATH]		= {0},
			szBexRegDataPath[_MAX_PATH]		= {0},
			szBexVerion[30]					= {0},
			szBexDataFilePath[_MAX_PATH]	= {0},
			szAppPath[_MAX_PATH]			= {0};

		Sleep(30*1000);
		memset(szBexRegVerPath, NULL, sizeof(szBexRegVerPath));
		memset(szBexRegDataPath, NULL, sizeof(szBexRegDataPath));
		memset(szBexVerion, NULL, sizeof(szBexVerion));
		memset(szBexDataFilePath, NULL, sizeof(szBexDataFilePath));
		memset(szTemp, NULL, sizeof(szTemp));
		memset(szAppPath, NULL, sizeof(szAppPath));	
		memset(szApplicationLog, NULL, sizeof(szApplicationLog));
		
		
		::GetModuleFileName(NULL,szAppPath, sizeof(szAppPath));
		ptr = strrchr(szAppPath, '\\');
		if(ptr)
		{
			ptr++;
			*ptr = '\0';
		}  
		sprintf(szApplicationLog, "%s\\ApplicationLog\\BEXShare.log", szAppPath);

		//Log
		memset(szDateTime, NULL, sizeof(szDateTime));
		memset(szTemp, NULL, sizeof(szTemp));
		GetDateTime(szDate,szDateTime);
		sprintf(szTemp, "%s\tStarting Application BEX Share \r\n",szDateTime);
		WriteToLog(szApplicationLog, szTemp);

		CRegKey
			objReg;

		long
			lError = 0;

		bool 
			bBackupInstalled = false;
		

////////////////////////////////////////////////////////////////
		//Read Reg ID
		char 
			szRegID[100] = {0},
			szLocalRegPath[MAX_PATH + 100]	=	{0},
			*szError= NULL;

		DWORD 
			dwBexVersn = 8;

		memset(szRegID, NULL, sizeof(szRegID));

		memset(szLocalRegPath,0,sizeof(szLocalRegPath));
		strcpy(szLocalRegPath,"SOFTWARE\\SAAZOD\0");

		if(!ReadRegistryForLocalRegid(szLocalRegPath,szRegID, szError))
		{
			memset(szTemp, NULL, sizeof(szTemp));
			sprintf(szTemp, "%s\tFailed Reading Local Registry %s. \r\n",szDateTime, szError);
			WriteToLog(szApplicationLog, szTemp);
			delete [] szError; 
			szError = NULL;
			
			return 0;
		}


////////////////////////////////////////////////////////////////
		do {

/*	Checking for Symantec
 *  Read Symantec Install Version 12.x
 */

			DWORD 
				dwCount    = _MAX_PATH;

			lstrcpy(szBexRegVerPath, "SOFTWARE\\Symantec\\Backup Exec For Windows\\Backup Exec\\12.0\\Install");
			lError = objReg.Open(HKEY_LOCAL_MACHINE, szBexRegVerPath, KEY_READ);
			
			if (ERROR_SUCCESS != lError)
			{
				lstrcpy(szBexRegVerPath, "SOFTWARE\\Symantec\\Backup Exec For Windows\\Backup Exec\\11.0\\Install");
				lError = objReg.Open(HKEY_LOCAL_MACHINE, szBexRegVerPath, KEY_READ);
			}

			
			if (ERROR_SUCCESS != lError)
			{
				lstrcpy(szBexRegVerPath, "SOFTWARE\\Symantec\\Backup Exec For Windows\\Backup Exec\\Install");
				lError = objReg.Open(HKEY_LOCAL_MACHINE, szBexRegVerPath, KEY_READ);
			}


			if(ERROR_SUCCESS == lError)
			{
				//Log
				memset(szTemp, NULL, sizeof(szTemp));
				sprintf(szTemp, "%s\tSuccessfully Opened Registry for Symantec :\r\n",szDateTime);
				WriteToLog(szApplicationLog, szTemp);

				lError = objReg.QueryValue(dwBexVersn,"Major Version\0"); 
				
				if(ERROR_SUCCESS == lError)
				{
					//Log
					memset(szTemp, NULL, sizeof(szTemp));
					sprintf(szTemp, "%s\tSymantec Version Installed [%d]:\r\n",szDateTime, dwBexVersn);
					WriteToLog(szApplicationLog, szTemp);

					bBackupInstalled = true;
					if (dwBexVersn > 8)
					{
						dwBexVersn = 9;
					}

					//Read Symantec Data Dir Path
					memset(szBexRegDataPath, NULL, sizeof(szBexRegDataPath));
					lstrcpy(szBexRegDataPath, "SOFTWARE\\Symantec\\Backup Exec For Windows\\Backup Exec\\Engine\\Misc");
			
					lError = objReg.Close();
					lError = objReg.Open(HKEY_LOCAL_MACHINE, szBexRegDataPath, KEY_READ);
					lError = objReg.QueryValue(szBexDataFilePath,"Data Path\0",&dwCount);

					if(ERROR_SUCCESS == lError)
					{
						//Log
						memset(szTemp, NULL, sizeof(szTemp));
						sprintf(szTemp, "%s\tSymantec DataFilePath [%s]:\r\n",szDateTime, szBexDataFilePath);
						WriteToLog(szApplicationLog, szTemp);
					}
					else
					{
						//Symantec data file path not found
						//Log
						lError = objReg.Close();

						memset(szTemp, NULL, sizeof(szTemp));
						sprintf(szTemp, "%s\tSymantec Data File Path not Found. \r\n",szDateTime);
						WriteToLog(szApplicationLog, szTemp);
						GeneratePID(szRegID,"SUCCESS\0", "Symantec Data File Path not Found.\0");
						
						bBackupInstalled = false;
						//return 0;
						///		goto SRV;
						break;
					}

					lError = objReg.Close();

				}//Query Value 
				else
				{
					objReg.Close();


/*					
				//Log
					memset(szTemp, NULL, sizeof(szTemp));
					sprintf(szTemp, "%s\tTrying Symmantec Version 2nd Path :\r\n",szDateTime);
					WriteToLog(szApplicationLog, szTemp);

//Symmantec 2nd Path
					memset(szBexRegVerPath, NULL, sizeof(szBexRegVerPath));
					lstrcpy(szBexRegVerPath, "SOFTWARE\\Symantec\\Backup Exec For Windows\\Backup Exec\\11.0\\Install");
					lError = objReg.Open(HKEY_LOCAL_MACHINE, szBexRegVerPath, KEY_READ);

					if(ERROR_SUCCESS == lError)
					{
						//Log
						memset(szTemp, NULL, sizeof(szTemp));
						sprintf(szTemp, "%s\tSuccessfully Opened Registry for Symantec :\r\n",szDateTime);
						WriteToLog(szApplicationLog, szTemp);

						lError = objReg.QueryValue(dwBexVersn,"Major Version\0"); 
						
						if(ERROR_SUCCESS == lError)
						{
							//Log
							memset(szTemp, NULL, sizeof(szTemp));
							sprintf(szTemp, "%s\tSymantec Version Installed [%d]:\r\n",szDateTime, dwBexVersn);
							WriteToLog(szApplicationLog, szTemp);

							bBackupInstalled = true;
							if (dwBexVersn > 8)
							{
								dwBexVersn = 9;
							}

			//Read Symantec Data Dir Path
							memset(szBexRegDataPath, NULL, sizeof(szBexRegDataPath));
							lstrcpy(szBexRegDataPath, "SOFTWARE\\Symantec\\Backup Exec For Windows\\Backup Exec\\Engine\\Misc");
					
							lError = objReg.Close();
							lError = objReg.Open(HKEY_LOCAL_MACHINE, szBexRegDataPath, KEY_READ);
							lError = objReg.QueryValue(szBexDataFilePath,"Data Path\0",&dwCount);

							if(ERROR_SUCCESS == lError)
							{
								//Log
								memset(szTemp, NULL, sizeof(szTemp));
								sprintf(szTemp, "%s\tSymantec DataFilePath [%s]:\r\n",szDateTime, szBexDataFilePath);
								WriteToLog(szApplicationLog, szTemp);
							}
							else
							{
								//Symantec data file path not found
								//Log
								lError = objReg.Close();

								memset(szTemp, NULL, sizeof(szTemp));
								sprintf(szTemp, "%s\tSymantec Data File Path not Found. \r\n",szDateTime);
								WriteToLog(szApplicationLog, szTemp);
								GeneratePID(szRegID,"SUCCESS\0", "Symantec Data File Path not Found.\0");			

								bBackupInstalled = false;

								//return 0;
//								goto SRV;
								break;
							}

							lError = objReg.Close();

						}
						else
						{
								//Log
								lError = objReg.Close();

								bBackupInstalled = false;

								memset(szTemp, NULL, sizeof(szTemp));
								sprintf(szTemp, "%s\tSymantec Version not Found. \r\n",szDateTime);
								WriteToLog(szApplicationLog, szTemp);
								GeneratePID(szRegID,"SUCCESS\0", "Symantec Version not Found.\0");			
								
								//return 0;
//								goto SRV;
								break;
						}
					}
*/
//symantec over
/////////////////////////////////////////////////////////////////////////////////////////////////////
				}
			}
			else
			{
    		//Log
				memset(szTemp, NULL, sizeof(szTemp));
				sprintf(szTemp, "%s\tSYMANTEC Path not found in registry. NOW LOOKING FOR VERITAS \r\n",szDateTime);
				WriteToLog(szApplicationLog, szTemp);


// Checking for Symantec has failed
//			Now Checking for Veritas

//Read Veritas Install Version
				lError = objReg.Close();
				lstrcpy(szBexRegVerPath, "SOFTWARE\\VERITAS\\Backup Exec\\Install");
				lError = objReg.Open(HKEY_LOCAL_MACHINE, szBexRegVerPath, KEY_READ);

				if(ERROR_SUCCESS == lError)
				{
					//Log
					memset(szTemp, NULL, sizeof(szTemp));
					sprintf(szTemp, "%s\tSuccessfully Opened Registry for Veritas :\r\n",szDateTime);
					WriteToLog(szApplicationLog, szTemp);

					lError = objReg.QueryValue(dwBexVersn,"Major Version\0"); 
					
					if(ERROR_SUCCESS == lError)
					{
						//Log
						memset(szTemp, NULL, sizeof(szTemp));
						sprintf(szTemp, "%s\tVeritas Version Installed [%d]:\r\n",szDateTime, dwBexVersn);
						WriteToLog(szApplicationLog, szTemp);
						if (dwBexVersn > 8)
						{
							dwBexVersn = 9;
						}

						//Read Veritas Data Dir Path
						lstrcpy(szBexRegDataPath, "SOFTWARE\\VERITAS\\Backup Exec\\Engine\\Misc");
				
						lError = objReg.Close();
						lError = objReg.Open(HKEY_LOCAL_MACHINE, szBexRegDataPath, KEY_READ);
						lError = objReg.QueryValue(szBexDataFilePath,"Data Path\0",&dwCount);

						if(ERROR_SUCCESS == lError)
						{
							bBackupInstalled = true;

							//Log
							memset(szTemp, NULL, sizeof(szTemp));
							sprintf(szTemp, "%s\tVeritas DataFilePath [%s]:\r\n",szDateTime, szBexDataFilePath);
							WriteToLog(szApplicationLog, szTemp);
					
						}
						else
						{
							//Veritas data file path not found

							bBackupInstalled = false;

							//Log
							memset(szTemp, NULL, sizeof(szTemp));
							sprintf(szTemp, "%s\tVeritas Data File Path not Found. \r\n",szDateTime);
							WriteToLog(szApplicationLog, szTemp);
							GeneratePID(szRegID,"SUCCESS\0", "Veritas Data File Path not Found.\0");			
							
							//return 0;
///							goto SRV;
							break;
						}

						lError = objReg.Close();
					}
					else
					{
						objReg.Close();

						//Log
						memset(szTemp, NULL, sizeof(szTemp));
						sprintf(szTemp, "%s\tVeritas Version could not be read from registry :\r\n",szDateTime);
						WriteToLog(szApplicationLog, szTemp);
						GeneratePID(szRegID,"SUCCESS\0", "Veritas Version could not be read from registry.\0");			
						
						//return 0;
///						goto SRV;
						break;
					}
				}
				else
				{
					//Veritas path not found in registry
					lError = objReg.Close();

					bBackupInstalled = false;

					memset(szTemp, NULL, sizeof(szTemp));
					sprintf(szTemp, "%s\tVeritas Path not Found in registry.\r\n",szDateTime);
					WriteToLog(szApplicationLog, szTemp);
					
					GeneratePID(szRegID,"SUCCESS\0", "Veritas Path not Found in registry.\0");
					
				}
			}

		}while (0);


//////////////////////////////////////////////////////////////////////////////////////////////////		

			memset(szTemp, NULL, sizeof(szTemp));
			sprintf(szTemp, "%s\tBefore Service Checking. [%d]\r\n",szDateTime, bBackupInstalled);
			WriteToLog(szApplicationLog, szTemp);
			

			if (false == bBackupInstalled)
			{
				//Checking Service Path

				memset(szTemp, NULL, sizeof(szTemp));
				sprintf(szTemp, "%s\tChecking Service. \r\n",szDateTime);
				WriteToLog(szApplicationLog, szTemp);

				char 
					szPath[256] = {0};
				memset(szPath, NULL, sizeof(szPath));

				ServiceDetails(szPath);

				if (0 < lstrlen(szPath) )
				{

					//Service Path
					memset(szTemp, NULL, sizeof(szTemp));
					sprintf(szTemp, "%s\tService Path [%s].\r\n",szDateTime, szPath);
					WriteToLog(szApplicationLog, szTemp);
					
					ptr = NULL;
					ptr1 = NULL;
					ptr1= szPath;
					ptr1++;
					ptr = strrchr(szPath, '\\');
					memset(ptr+1, NULL, lstrlen(ptr+1));
					lstrcat(ptr+1, "Data\0");

					dwBexVersn = 9; 
					memset(szBexDataFilePath, NULL, sizeof(szBexDataFilePath));
					lstrcpy(szBexDataFilePath, ptr1);

					dwBexVersn = 9;
					bBackupInstalled    = true;
				}
				else
				{
					memset(szTemp, NULL, sizeof(szTemp));
					sprintf(szTemp, "%s\tNo Symantec Software detected. Quitting !!!.\r\n",szDateTime);
					WriteToLog(szApplicationLog, szTemp);
				}
			}

//////////////////////////////////////////////////////////////////////////////////////////////////		


			if (false == bBackupInstalled)
			{
				GeneratePID(szRegID,"SUCCESS\0", "No veritas found.\0");

				return 0;
				//GeneratePID(oThreadData->szRegId,"FAILED\0","Opening Last Parsed ini File Failed.\0");
			}
			else
			{
				memset(szTemp, NULL, sizeof(szTemp));
				sprintf(szTemp, "Veritas Backup %d ", dwBexVersn);

				GeneratePID(szRegID,"SUCCESS\0", szTemp);
			}

			if (!(((8==dwBexVersn) || (9==dwBexVersn)) && (0 < lstrlen(szBexDataFilePath)))) 
			{
				memset(szTemp, NULL, sizeof(szTemp));
				sprintf(szTemp, "%s\tBex Version [%d]. Path [%s]. Quitting !!!.\r\n",szDateTime, szBexDataFilePath);
				WriteToLog(szApplicationLog, szTemp);

				return 0;
			}


		
	//Make the Bex Data Directory Shareable

//Get Host Name

		char 
			szHostName[100] = {0};
		WSADATA 
			wsData;
		
		WSAStartup(MAKEWORD( 2, 2 ),&wsData);
		
		ZeroMemory(szHostName, sizeof(szHostName));
		gethostname(szHostName,sizeof(szHostName));
		
		WSACleanup();

//Check For share
//		char 
//			RemoteTerminalName[100] = {0},
//			szPathName[_MAX_PATH] = {0};

//		memset(szPathName, NULL, _MAX_PATH);

//		lstrcpy(szPathName, szBexDataFilePath);

		char * szPathC = NULL;
		wchar_t *str = NULL;
 
		char* szSortVal = NULL;
		szSortVal = new char[_MAX_PATH];
		memset(szSortVal, NULL, _MAX_PATH);
 
		PSHARE_INFO_502 
					bufPtr;
		SHARE_INFO_502 
					info;
		NET_API_STATUS 
					res = 0;
 
		memset(&info, NULL, sizeof(info));

		//Share Name
		 //sprintf(szSortVal, "%c$", szPathName[0]);
		sprintf(szSortVal, "zBEX%dX", dwBexVersn);
 

		//Log
		memset(szDateTime, NULL, sizeof(szDateTime));
		memset(szTemp, NULL, sizeof(szTemp));
		GetDateTime(szDate,szDateTime);
		sprintf(szTemp, "%s\tFinding Share Name [%s]:\r\n",szDateTime, szSortVal);
		WriteToLog(szApplicationLog, szTemp);
		
//		memset(RemoteTerminalName, NULL, 100);
//		sprintf(RemoteTerminalName,"\\\\%s", szHostName);
//		wchar_t *term = new wchar_t[(strlen(RemoteTerminalName)*2)+2];
//		memset(term, NULL, strlen(RemoteTerminalName)*2 + 2);
//		swprintf(term, L"%S",RemoteTerminalName);
 
		wchar_t *name = new wchar_t[(strlen(szSortVal)*2)+2];
		memset(name, NULL, strlen(szSortVal)*2 + 2);
		swprintf(name,L"%S",szSortVal);
 
		 //check if share share exits 
 
		  if((res = NetShareGetInfo(NULL /*(LPTSTR)term*/, //local server
									(LPWSTR)name,   //share name
									502,
									(LPBYTE *) &bufPtr)) != ERROR_SUCCESS)
		  {

			  // Share does not exists

			//Log
			memset(szDateTime, NULL, sizeof(szDateTime));
			memset(szTemp, NULL, sizeof(szTemp));
			GetDateTime(szDate,szDateTime);
			sprintf(szTemp, "%s\tShare Name [%s] not found Return Value [%d]:\r\n", szDateTime, szSortVal, res);
			WriteToLog(szApplicationLog, szTemp);
				  
			  
    		  szPathC = new char[_MAX_PATH];
			  memset(szPathC, NULL, _MAX_PATH);

			   sprintf(szPathC, "%s", szBexDataFilePath);
   
   
			   info.shi502_netname = (LPWSTR)name; //ToWideChar(szSortVal);
			   info.shi502_type  = STYPE_SPECIAL;
			   info.shi502_remark = NULL;//ToWideChar ("Hello");
			   info.shi502_permissions  = ACCESS_ALL;
			   info.shi502_max_uses = -1;
			   info.shi502_path  = (LPWSTR)ToWideChar(szPathC);
			   info.shi502_passwd = NULL;
   
			   res = NetShareAdd ( NULL,/*(LPTSTR)term,*/ // local
									502,         
									(LPBYTE)&info,    
									NULL);        
			   if (0 == res)
			   {

					//Log
					memset(szDateTime, NULL, sizeof(szDateTime));
					memset(szTemp, NULL, sizeof(szTemp));
					GetDateTime(szDate,szDateTime);
					sprintf(szTemp, "%s\tSuccessfully Created  Share Name [%s]:\r\n",szDateTime, szSortVal);
					WriteToLog(szApplicationLog, szTemp);

					GeneratePID(szRegID,"SUCCESS\0", "Share created.\0");

			   }
   			   else
			   {
					//Log
					memset(szDateTime, NULL, sizeof(szDateTime));
					memset(szTemp, NULL, sizeof(szTemp));
					GetDateTime(szDate,szDateTime);
					sprintf(szTemp, "%s\tFailed Creating Share Name [%s]. Return Value [%d]:\r\n",szDateTime, szSortVal, res);
					WriteToLog(szApplicationLog, szTemp);

					GeneratePID(szRegID,"FAILED\0", "Unable to create share.\0");

			   }

		  }
		  else
		  {
			//Share Exists	

				//Log
				memset(szDateTime, NULL, sizeof(szDateTime));
				memset(szTemp, NULL, sizeof(szTemp));
				GetDateTime(szDate,szDateTime);
				sprintf(szTemp, "%s\tShare Name [%s] already Exists. Return Value [%d] :\r\n",szDateTime, szSortVal, res);
				WriteToLog(szApplicationLog, szTemp);

			//Check that data path are same
				memset(szTemp, NULL, sizeof(szTemp));
				sprintf(szTemp, "%S",bufPtr->shi502_path);
			

				if (stricmp(szTemp, szBexDataFilePath) )
				{
					//Data path vary
					memset(szDateTime, NULL, sizeof(szDateTime));
					memset(szTemp, NULL, sizeof(szTemp));
					GetDateTime(szDate,szDateTime);
					sprintf(szTemp, "%s\tShare Name is same but data path varies. Deleting Previous share and creating new share [%s] with path [%s]:\r\n",szDateTime, szSortVal, szBexDataFilePath);
					WriteToLog(szApplicationLog, szTemp);

					res = NetShareDel(NULL, (LPWSTR)name, NULL);

					if (0 == res)
					{
						  szPathC = new char[_MAX_PATH];
						  memset(szPathC, NULL, _MAX_PATH);
   
						  sprintf(szPathC, "%s", szBexDataFilePath);
   
   
						   info.shi502_netname = (LPWSTR)name; //ToWideChar(szSortVal);
						   info.shi502_type  = STYPE_SPECIAL;
						   info.shi502_remark = NULL;//ToWideChar ("Hello");
						   info.shi502_permissions  = ACCESS_ALL;
						   info.shi502_max_uses = -1;
						   info.shi502_path  = (LPWSTR)ToWideChar(szPathC);
						   info.shi502_passwd = NULL;
   
						   res = NetShareAdd ( NULL,
												502,         
												(LPBYTE)&info,    
												NULL);        
					}

				    if (0 == res)
					{
						//Log
						memset(szDateTime, NULL, sizeof(szDateTime));
						memset(szTemp, NULL, sizeof(szTemp));
						GetDateTime(szDate,szDateTime);
						sprintf(szTemp, "%s\tSuccessfully Created  Share Name [%s]:\r\n",szDateTime, szSortVal);
						WriteToLog(szApplicationLog, szTemp);

						GeneratePID(szRegID,"SUCCESS\0", "Share created.\0");	
					}
   				    else
					{
						//Log
						memset(szDateTime, NULL, sizeof(szDateTime));
						memset(szTemp, NULL, sizeof(szTemp));
						GetDateTime(szDate,szDateTime);
						sprintf(szTemp, "%s\tFailed Creating Share Name [%s]. Return Value [%d]:\r\n",szDateTime, szSortVal, res);
						WriteToLog(szApplicationLog, szTemp);

						GeneratePID(szRegID,"FAILED\0", "Unable to create share.\0");
					}
				}
			}

			res = NetApiBufferFree((LPVOID)bufPtr);

			//Checking For Last Parsed ini
			//If not Present create it with Last Parsed time as SAAZOD Installed date  

			//Log
			memset(szDateTime, NULL, sizeof(szDateTime));
			memset(szTemp, NULL, sizeof(szTemp));
			GetDateTime(szDate,szDateTime);
			sprintf(szTemp, "%s\tChecking for Last Parsed Ini File :\r\n", szDateTime);
			WriteToLog(szApplicationLog, szTemp);


			char 
				szSAAZODRegPath[100] = {0},
				szSAAZODInstallDate[25] = {0},
				szSAAZODInstallPath[_MAX_PATH] = {0},
				szInstallDate[50]= {0};



			DWORD 
				dwCount;

			memset(szSAAZODRegPath, 0, 100);
			lstrcpy(szSAAZODRegPath, "SOFTWARE\\SAAZOD");

			memset(szSAAZODInstallDate, 0, 25);
			memset(szSAAZODInstallPath, 0, _MAX_PATH);
			memset(szInstallDate, 0, 50);

			lError = objReg.Open(HKEY_LOCAL_MACHINE, szSAAZODRegPath, KEY_READ);

			dwCount = 25;
			lError = objReg.QueryValue(szSAAZODInstallDate,"InstallDate\0",&dwCount);
			if (lError)
			{
				//Log
				memset(szDateTime, NULL, sizeof(szDateTime));
				memset(szTemp, NULL, sizeof(szTemp));
				GetDateTime(szDate,szDateTime);
				sprintf(szTemp, "%s\tError Opening SAAZOD Install Date in registry: \r\n",szDateTime);
				WriteToLog(szApplicationLog, szTemp);

				objReg.Close();

				return 1;
			}

			lError = objReg.Close();
			
			lError = objReg.Open(HKEY_LOCAL_MACHINE, szSAAZODRegPath, KEY_READ);
			if (lError)
			{
				//Log
				memset(szDateTime, NULL, sizeof(szDateTime));
				memset(szTemp, NULL, sizeof(szTemp));
				GetDateTime(szDate,szDateTime);
				sprintf(szTemp, "%s\tError Opening SAAZOD Install Date in registry:\r\n",szDateTime);
				WriteToLog(szApplicationLog, szTemp);

				objReg.Close();
				return 1;
			}
			dwCount = _MAX_PATH;
			lError = objReg.QueryValue(szSAAZODInstallPath, "InstallationPath\0",&dwCount);
			if (lError)
			{
				//Log
				memset(szDateTime, NULL, sizeof(szDateTime));
				memset(szTemp, NULL, sizeof(szTemp));
				GetDateTime(szDate,szDateTime);
				sprintf(szTemp, "%s\tError Opening SAAZOD Install Path in registry: \r\n",szDateTime);
				WriteToLog(szApplicationLog, szTemp);

				objReg.Close();

				return 1;
			}

			lError = objReg.Close();

			//Log
			memset(szDateTime, NULL, sizeof(szDateTime));
			memset(szTemp, NULL, sizeof(szTemp));
			GetDateTime(szDate,szDateTime);
			sprintf(szTemp, "%s\tSAAZOD Install Path [%s]  SAAZOD Install Date [%s]: \r\n",szDateTime, szSAAZODInstallPath, szSAAZODInstallDate);
			WriteToLog(szApplicationLog, szTemp);

			lstrcat(szSAAZODInstallDate, "/");
			char **arrValues = NULL;
			
			SplitString(szSAAZODInstallDate, "/", arrValues);
			
			sprintf(szInstallDate,"%s-%s-%s 00:00:00", arrValues[2], arrValues[0], arrValues[1]);
			
			HANDLE	
				hIniCheck = NULL; 
			char 
				szLastParseIni[_MAX_PATH] = {0};

			memset(szLastParseIni, NULL, _MAX_PATH);
			
			sprintf(szLastParseIni, "%s\\Configuration\\Bex%dx\\%s_LastParsed.ini", szSAAZODInstallPath, dwBexVersn, szHostName);
		
			hIniCheck = CreateFile(szLastParseIni, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

			if(hIniCheck  == INVALID_HANDLE_VALUE)
			{
				//	memset(szTemp, 0, sizeof(szTemp));
				//	lstrcpy(szTemp, szLastParseIni);
				//	memset(szLastParseIni, 0, sizeof(szLastParseIni));
				//	sprintf(szLastParseIni, "%s\\%s_LastParsed.ini", szTemp, szHostName);
				//	Create Bex Directory

				char 
					szConfigDirectory[_MAX_PATH] = {0};

				memset(szConfigDirectory, NULL, _MAX_PATH);
				
				sprintf(szConfigDirectory, "%s\\Configuration\\Bex%dx\\", szSAAZODInstallPath, dwBexVersn);

				CreateDirectoriesRecursively(szConfigDirectory);

				DWORD
					dwBytesWritten = 0;
				hIniCheck = CreateFile(szLastParseIni, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
				WriteFile(hIniCheck, szInstallDate, strlen(szInstallDate), &dwBytesWritten, NULL);	

				CloseHandle(hIniCheck);

				//Log
				memset(szDateTime, NULL, sizeof(szDateTime));
				memset(szTemp, NULL, sizeof(szTemp));
				GetDateTime(szDate,szDateTime);
				sprintf(szTemp, "%s\tCreated File [%s] with data [%s]: \r\n",szDateTime, szLastParseIni, szInstallDate);
				WriteToLog(szApplicationLog, szTemp);
			}
			else
			{
				//Log
				memset(szDateTime, NULL, sizeof(szDateTime));
				memset(szTemp, NULL, sizeof(szTemp));
				GetDateTime(szDate,szDateTime);
				sprintf(szTemp, "%s\tFile [%s] found : \r\n",szDateTime, szLastParseIni);
				WriteToLog(szApplicationLog, szTemp);
				
				CloseHandle(hIniCheck);
				hIniCheck = NULL;
			}
	}
	catch(...)
	{
		//Log
		memset(szDateTime, NULL, sizeof(szDateTime));
		memset(szTemp, NULL, sizeof(szTemp));
		GetDateTime(szDate,szDateTime);
		sprintf(szTemp, "%s\tException occured in main : \r\n",szDateTime);
		WriteToLog(szApplicationLog, szTemp);
	}

	return 0;
}
					




char *ToWideChar(const char *ANSIstring) // Convertit une chaine de caracteres en widechar
{
	
	int nBufSize = 0;
	WCHAR *wString = NULL;
	try {
	if(ANSIstring == NULL) return NULL; //retourne NULL si empty
	
	nBufSize = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ANSIstring, -1, (LPWSTR)NULL, 0 );
	wString = (WCHAR *)malloc(nBufSize + 1);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ANSIstring, -1, (LPWSTR)wString, nBufSize);
	}
	catch(...)
	{
	}
	return (char*)(wString);
}

long SplitString(char *szString, char *sep, char** &arrValues)
{
	char *pPrevious = NULL;
	char *pNext = NULL;
	char **arrLocal = NULL;
	pPrevious = szString;
	pNext = szString;
	long result = 0;
	int nFields = 0;
	try {
		
		while(pPrevious != NULL)
		{
			pPrevious += strlen(sep);
			pPrevious = strstr(pPrevious,sep);
			nFields ++;
		}
		arrLocal = new char*[nFields];
		if(arrLocal == NULL)
		{
			return 1;
		}
		pPrevious = szString;
		nFields = 0;
		while(pPrevious != NULL)
		{
			pNext = strstr(pPrevious,sep);
			if(pNext == NULL)
			{
				break;
			}
			else {
				result = pNext - pPrevious;
			}
			if(result < 0)
			{
				result = 1;
			}
			arrLocal[nFields] = new char[(result + 10)];
			if(arrLocal[nFields] == NULL)
			{
				return 1;
			}
			memset(arrLocal[nFields],NULL,(result +10));
			memcpy(arrLocal[nFields],pPrevious,result);
			pPrevious = pNext;
			pPrevious++;
			nFields++;
		}
		arrLocal[nFields] = NULL;
		arrValues = arrLocal;
	}
	catch(...)
	{
		return 1;
	}
	return 0;
}


long WriteToLog(char *szFileName,char *szMessage)
{
	HANDLE hMutex = NULL;
	HANDLE hFile = NULL;
	HANDLE hMMFile = NULL;
	LPVOID lpMapAddress = NULL;
	char * lpTraverser = NULL;
	char szMakeDir[FILE_SIZE];
	char szMutex[250];
	char szMapName[250];
	DWORD nResult		=			0;
	DWORD dwFileSizeLow =			0;
	DWORD dwFileSizeHigh=			0;
	DWORD dwSysGran		=			0;
	DWORD dwFileMapStart=			0;
	DWORD dwBufferSize  =			0;
	DWORD dwFileMapSize =			0;
	DWORD iViewDelta	=			0;
	DWORD dwMapViewSize =			0;


	__try{
	__try{
	
		if(szMessage == NULL)
		{
			return 1;
		}
		dwBufferSize = strlen(szMessage);
		memset(szMutex,0,250);
		memset(szMapName,0,250);

		char *szTempPtr = strrchr(szFileName,'\\');		
		memset(szMakeDir,0,250);
		if(szTempPtr != NULL)
		{
			memcpy(szMakeDir,szFileName,strlen(szFileName) - strlen(szTempPtr));
			memcpy(szMutex,(szTempPtr+1),(strlen(szTempPtr)-1));
			strcpy(szMapName,szMutex);
			strcat(szMapName,"_Map\0");
			CreateDirectoriesRecursively(szMakeDir);	
        }
		else {
			return 1;
		}

		hMutex = CreateMutex(NULL,FALSE,szMutex);
		if(hMutex == NULL)
		{
			return 1;
		}

		nResult = WaitForSingleObject(hMutex,2*1000);
		if(WAIT_OBJECT_0 != nResult)
		{
			CloseHandle(hMutex);
			hMutex = NULL;
			return 1;
		}
		hFile = CreateFile(szFileName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			OPEN_ALWAYS,
			0,
			NULL);
		
		if (hFile == INVALID_HANDLE_VALUE)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			hMutex = NULL;
			return 1;
		}
		
		//Get the log file size
		dwFileSizeLow = GetFileSize(hFile,&dwFileSizeHigh);
		
		// Finding the system granuality 
		SYSTEM_INFO		Sys_Info;
		memset(&Sys_Info,0,sizeof(Sys_Info));
		GetSystemInfo(&Sys_Info);
		
		dwSysGran	=	Sys_Info.dwAllocationGranularity;
		dwFileMapStart = (dwFileSizeLow / dwSysGran) * dwSysGran;
		// Calculate the size of the file mapping view.
		dwMapViewSize = (dwFileSizeLow % dwSysGran)  + dwBufferSize;
		
		// How large will the file-mapping object be?
		dwFileMapSize = dwFileSizeLow + dwBufferSize;
		
		iViewDelta = dwFileSizeLow - dwFileMapStart;
		
		//Create the file mapping accordingly
		hMMFile = CreateFileMapping(hFile,
			NULL,
			PAGE_READWRITE,
			dwFileSizeHigh,
			dwFileMapSize,
			szMapName);
		
		if(!hMMFile)
		{
			int err = GetLastError();
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			hMutex = NULL;
			CloseHandle(hFile);
			hFile = NULL;
			return 1;
		}
		
		//Map the view of the file
		lpMapAddress =	MapViewOfFile(hMMFile,
			FILE_MAP_WRITE,
			dwFileSizeHigh,
			dwFileMapStart,
			dwMapViewSize);
		if(NULL == lpMapAddress)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			hMutex = NULL;
			CloseHandle(hFile);
			hFile = NULL;
			CloseHandle(hMMFile);
			hMMFile = NULL;
			return 1;
		}
		
		//Increment the pointer to the end of file
		lpTraverser = (TCHAR *) lpMapAddress + iViewDelta;
		memset(lpTraverser,0,dwBufferSize * sizeof(TCHAR));
		_tcsncpy(lpTraverser,szMessage,dwBufferSize);
		
		lpTraverser = NULL;
		
		FlushViewOfFile(lpMapAddress,dwMapViewSize);//for writting to disk process being fast
		
		UnmapViewOfFile(lpMapAddress);
		CloseHandle(hMMFile);
		hMMFile = NULL;

		CloseHandle(hFile);
		hFile = NULL;
		
		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
		hMutex = NULL;

		return 0;
		
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return 1;
	}
	}
	__finally
	{
		if(hMMFile)
			CloseHandle(hMMFile);
		hMMFile = NULL;
		if(hFile)
			CloseHandle(hFile);
		hFile = NULL;
		if(hMutex)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
		}
		hMutex = NULL;
	}
	return 0;
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
					"%04d-%02d-%02d %02d:%02d:%02d\0",
					sysTime.wYear,
					sysTime.wMonth,
					sysTime.wDay,
					sysTime.wHour,
					sysTime.wMinute,
					sysTime.wSecond);

			sprintf(szDate,
					"%04d%02d%02d\0",
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

long CreateDirectoriesRecursively(char *szInDir)
{
	try {
		char *szLocalPtr = szInDir;
		
		szLocalPtr = strstr(szLocalPtr,"\\\0");
		szLocalPtr++;
	
		char szLocalDir[250];
		memset(szLocalDir,0,250);
		while(szLocalPtr != NULL)
		{
		
			szLocalPtr = strstr(szLocalPtr,"\\\0");
			if(szLocalPtr == NULL)
			{
				break;
			}
			long result  = (szLocalPtr - szInDir) +1 ;
			memcpy(szLocalDir,szInDir,result);
			CreateDirectory(szLocalDir,NULL);
			szLocalPtr++;
		}
		CreateDirectory(szInDir,NULL);
	}
	catch(...)
	{
	}
	return 0;
}


bool GeneratePID(char *szRegId,char *szStatus,char *Desc)
{
	try
	{
		char szTempPIDFile[FILE_SIZE];
		char szPIDFile[FILE_SIZE];
		char szAlertDateTime[50];
		char szAppPath[FILE_SIZE];
		char szText[2048];

		memset(szAppPath,0,FILE_SIZE);
		
		::GetModuleFileName(NULL,szAppPath,FILE_SIZE);
		
		char *ptr = strrchr(szAppPath,'\\');
		
		if(ptr)
		{
			ptr++;
			*ptr='\0';
		}

		memset(szAlertDateTime,0,sizeof(szAlertDateTime));
		memset(szText,0,sizeof(szText));
		
		GetCurrentDateTime(szAlertDateTime);
		
		char szTimeStamp[100] = {0};
		memset(szTimeStamp, NULL, sizeof(szTimeStamp));
		GetTimeStamp(szTimeStamp);
		
		memset(szTempPIDFile,0,sizeof(szTempPIDFile));
		memset(szPIDFile,0,sizeof(szPIDFile));
		sprintf(szTempPIDFile,"%s\\Configuration\\%s_zBEXShare_%s.ini\0",szAppPath,szRegId,szTimeStamp);
		
		sprintf(szPIDFile,"%s\\PMSTATUS\\%s_zBEXShare_%s.ini\0",szAppPath,szRegId,szTimeStamp);
		memset(szText,0,sizeof(szText));
		sprintf(szText,PID_TEXT,szRegId,szAlertDateTime,szStatus,Desc);
		
		WriteToLog(szTempPIDFile,szText);
		CopyFile(szTempPIDFile,szPIDFile,FALSE);
		DeleteFile(szTempPIDFile);
		return true;
	}
	catch(...)
	{
		return false;
	}
	return true;
}


DWORD	GetTimeStamp(char* szDate)
{
	SYSTEMTIME		sysTime;

	__try
	{
		__try
		{
			if(NULL == szDate)	
			{
				return 1;
			}

			memset(&sysTime,NULL,sizeof(sysTime));	


			::GetLocalTime(&sysTime);



			sprintf(szDate,
					"%04d%02d%02d%02d%02d%02d%03d\0",
					sysTime.wYear,
					sysTime.wMonth,
					sysTime.wDay,
					sysTime.wHour,
					sysTime.wMinute,
					sysTime.wSecond,
					sysTime.wMilliseconds);


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

bool ReadRegistryForLocalRegid(char *szRegPath,char *szRegId,char *&szError)
{
	CRegKey regObj;
	try{
		DWORD dwCount = 0;
		
		LONG	lError	=	0;
		
		lError = regObj.Open(HKEY_LOCAL_MACHINE,
			szRegPath,
			KEY_READ);
		if(ERROR_SUCCESS != lError)
		{
			WriteToBuffer(szError,"Opening Registry Failed\0");
			WriteToBuffer(szError,szRegPath);
			regObj.Close();
			return false;
		}
		
		
		dwCount = 100;
		lError = regObj.QueryValue(szRegId,"regid\0",&dwCount);
		if(ERROR_SUCCESS != lError)
		{
			WriteToBuffer(szError,"Reading Registry Key For regid Failed.\0");
			regObj.Close();
			return false;

			
		}
		return true;
		
	}
	catch(...)
	{
		WriteToBuffer(szError,"Exception occurred. while reading registry\0");
		return false;
	}
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

DWORD	GetCurrentDateTime(char* szDateTime)
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
				WORD nSeconds = sysTime.wSecond;
				if( nSeconds > 0)
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


long ServiceDetails(char *szPath)
{

	LPQUERY_SERVICE_CONFIG
		lpqscBuf = NULL; 
	
	SC_HANDLE 
		scHandle   = NULL,
		schService = NULL;
	
	
	__try
	{
		__try
		{

			
			DWORD 
				dwBytesNeeded = 0; 

			long 
				lRet = 0;
 

			scHandle = 	OpenSCManager(NULL,
									  SERVICES_ACTIVE_DATABASE,
									  SC_MANAGER_CONNECT);

			if (NULL == scHandle)
			{
				lRet = ::GetLastError();
			}


			// Open a handle to service. 
 			schService = OpenService(scHandle,                   
									 "BackupExecJobEngine\0",    
									  SERVICE_QUERY_CONFIG);      

			if (schService == NULL) 
			{
				lRet = ::GetLastError();
				return 1;
			}

 
 			lpqscBuf = (LPQUERY_SERVICE_CONFIG) LocalAlloc(LPTR, 4096); 
			if (lpqscBuf == NULL) 
			{
				return 1;
			}
 

			if (! QueryServiceConfig(schService, 
									lpqscBuf, 
									4096, 
									&dwBytesNeeded) ) 
			{
				return 1;
			}

			CloseServiceHandle(scHandle);
			scHandle = NULL;
			CloseServiceHandle(schService); 
			schService = NULL;

			//char pStr[512] = {0};
			
			if (NULL != lpqscBuf->lpBinaryPathName)
			{
				lstrcpy(szPath, lpqscBuf->lpBinaryPathName);
			}

 
			LocalFree(lpqscBuf); 
			lpqscBuf = NULL;

		}

		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			
		}

	}
	__finally
	{
			CloseServiceHandle(scHandle);
			scHandle = NULL;
			CloseServiceHandle(schService); 
			schService = NULL;
			LocalFree(lpqscBuf); 
			lpqscBuf = NULL;
	}

	return 0;

}
