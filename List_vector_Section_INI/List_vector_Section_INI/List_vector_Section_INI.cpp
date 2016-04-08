// List_vector_Section_INI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <list>
#include<Windows.h>

using namespace std;

list<string> MachineNameList;


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

		 //std::list<std::wstring> l;
		for (char * p = chBuffer; *p; ++p)
		{
			std::string str(p);
			MachineNameList.push_back(str);
			p += str.size();
			_ASSERT(0 == *p);
		}

		/*while(NULL != chTempBuffer) 
		{
			MachineNameList.push_back(chTempBuffer);
			chTempBuffer = chTempBuffer + lstrlen(chTempBuffer) + 1;
		}*/


		if(NULL != chBuffer)
		{
			delete []chBuffer;
			chBuffer = NULL;
			//chTempBuffer = NULL;
		}

	}
	catch(...)
	{
		return false;
	}

	return true;
}

int _tmain(int argc, _TCHAR* argv[])
{
	GetMachineNames("C:\\Program Files (x86)\\SAAZOD\\Configuration\\DeviceList.ini");

	return 0;
}

