// createFilezExVssW.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"

#define  MAXIMUM 1024

/*void GetHostName(char* Hostname)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	char name[MAX_PATH]="";
	wVersionRequested = MAKEWORD( 2,2);

	if ( WSAStartup( wVersionRequested, &wsaData ) == 0 )
	{
		gethostname ( name, sizeof(name));
		strcpy(Hostname,name);
		WSACleanup( );
	}
}

bool IsDirExist(char *mdirectory)
{
	try
	{
		WIN32_FIND_DATA mwfd;
		HANDLE mhandle;
		mhandle = FindFirstFile(mdirectory, &mwfd);

		if(mhandle == INVALID_HANDLE_VALUE)
		{
			return(false);
		}
		else
		{
			FindClose(mhandle); 
			return(true);
		}
	}
	catch(...)
	{
		//Write_W3C_Log("zExVssW" , "1.0" , "IsDirExist" , "ERROR" , "Exception Occured" , EMPTY_STRING , EMPTY_STRING);
		exit(0);
	}
}
long CreateDirectoriesRecursively(char *szInDir)
{
	try
	{
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
void ComputeDataLogFilePath()
{
	
	char mmCurentDir[1024]="";

	GetHostName(Hostname);

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	if(lstrcmp(Hostname,"")!=0)
		sprintf(mmCurentDir,"%sDataDirectory\\%s\\%04d%02d\\zExVssW",SzInstallationPath,Hostname,sysTime.wYear,sysTime.wMonth);

	if (IsDirExist(mmCurentDir) == false)
	{
		::CreateDirectoriesRecursively(mmCurentDir);
	}
	sprintf(DatalogFileName, "%s\\zExVssW_%04d%02d%02d.Log",mmCurentDir,sysTime.wYear,sysTime.wMonth,sysTime.wDay);

}*/

bool IsDirExist(char *mdirectory)
{
	try
	{
		WIN32_FIND_DATA mwfd;
		HANDLE mhandle;
		mhandle = FindFirstFile(mdirectory, &mwfd);

		if(mhandle == INVALID_HANDLE_VALUE)
		{
			return(false);
		}
		else
		{
			FindClose(mhandle); 
			return(true);
		}
	}
	catch(...)
	{
		//Write_W3C_Log("zExVssW" , "1.0" , "IsDirExist" , "ERROR" , "Exception Occured" , EMPTY_STRING , EMPTY_STRING);
		exit(0);
	}
}

void CheckHeader()
{
	if (IsDirExist(DatalogFileName) == false)
	{
		FILE *fp = fopen(DatalogFileName,"a");
		if(fp!=NULL)
		{
			char LogBuffer[MAXIMUM]="";
			sprintf(LogBuffer,"#Fields:DATE TIME\tFunction Name\tKEYWORD\tState\tSTATUS\n");
			fprintf(fp,"%s",LogBuffer);
			fclose(fp);
			//Write_W3C_Log("zExVssW" ,"5.0.0.7", "Datalog" , "Info" , "DatalogFileName" ,"File Created","" );
		}
	}
}
bool is64Machine()
{
	char infoBuf[512];
	try
	{		
		GetWindowsDirectory( infoBuf, 512 );
		char *ptr = strrchr(infoBuf,'\\');
		if(ptr)
		{
			ptr++;
			*ptr='\0';
		}
		lstrcat(infoBuf,"Program Files (x86)");
	}

	catch(...)
	{
		exit(0);
	}
	return IsDirExist(infoBuf); 
}

BOOL GetModulePath(char *InstallationPath,char *SaazRegID)
{
	try
	{
		HKEY hKey = 0;
		char buf[MAXIMUM] = {0};    
		char buf1[MAXIMUM] = {0};    
		DWORD dwType = 0;    
		char m_keytoset[MAXIMUM];
		DWORD dwBufSize = sizeof(buf);    
			DWORD bufCharCount=MAXIMUM;
			char mUSER_NAME[256] = {""};
		GetUserName(mUSER_NAME,&bufCharCount);
		if(is64Machine()==true)
		{
			strcpy_s(m_keytoset ,MAXIMUM, "SOFTWARE\\Wow6432Node\\SAAZOD");
		}
		else
		{
			strcpy_s(m_keytoset ,MAXIMUM, "SOFTWARE\\SAAZOD");
		}
		
		if( RegOpenKey(HKEY_LOCAL_MACHINE,(LPCSTR)m_keytoset,&hKey) == ERROR_SUCCESS)    
		{    
			dwType = REG_SZ;        
			if( RegQueryValueEx(hKey,"InstallationPath",0, &dwType, (BYTE*)buf, &dwBufSize) == ERROR_SUCCESS)        
			{            
				strcpy(InstallationPath,buf);
			}   
			dwType = REG_SZ;        
			dwBufSize=sizeof(buf);

			if( RegQueryValueEx(hKey,"REGID",0, &dwType, (BYTE*)buf1, &dwBufSize) == ERROR_SUCCESS)        
			{            
				strcpy_s(SaazRegID,256,buf1);
				RegCloseKey(hKey);     
				return 1;
			}
			RegCloseKey(hKey);     
		}
		return 0;
	}
	catch(...)
	{
		exit(0);
	}
}
int _tmain(int argc, _TCHAR* argv[])
{

	return 0;
}

