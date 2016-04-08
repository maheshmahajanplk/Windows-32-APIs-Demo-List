// ShadowPro_GetVolumeInformation.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Windows.h"
#include <iostream>
#include <fstream>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	char ActualVolumeWithGUID[MAX_PATH + MAX_PATH] = {0};
	TCHAR volumeName[MAX_PATH + 1] = { 0 };
	DWORD serialNumber = 0;
	DWORD maxComponentLen = 0;
	DWORD fileSystemFlags = 0;
	TCHAR fileSystemName[MAX_PATH + 1] = { 0 };
	bool failedVal=false;

	
	strcpy(ActualVolumeWithGUID,"C:\\");

	char GUID[250] = {0} ;

	GetVolumeNameForVolumeMountPoint(ActualVolumeWithGUID, GUID, 250 ) ;
	
	cout<<"GUID="<<GUID<<endl;

	if (GetVolumeInformation(
						GUID,
						volumeName,
						sizeof(volumeName),
						&serialNumber,
						&maxComponentLen,
						&fileSystemFlags,
						fileSystemName,
						sizeof(fileSystemName)))
					{
						failedVal = true;
					}

	cout << "failedVal="<<failedVal<<endl;
	cout << "volumeName="<<volumeName<<endl;
	cout << "fileSystemName="<<fileSystemName<<endl;
	

	return 0;
}

