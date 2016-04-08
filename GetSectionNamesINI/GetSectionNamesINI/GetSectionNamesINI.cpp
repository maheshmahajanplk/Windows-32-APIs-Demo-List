// GetSectionNamesINI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include<iostream>
#include<string>

using namespace std;

void GetINISectionNames();

char szSectionNames[1024];

int g_iNotOkAttributeIDs[50]; //max attribute checking for smart is 22 only

 
int _tmain(int argc, _TCHAR* argv[])
{
	int Bytes_Read,nul_ocr;
	
	
	GetINISectionNames();
     
	return 0;
}

void GetINISectionNames()
{
	
	int i,j=0;
	int Bytes_Read=0;
	int iDigit=0;
	char *ptr[30] = {""};
	int iNumOfSections=0;
	char szAttributeID[256] = {""};

	try{
		 Bytes_Read = GetPrivateProfileSectionNames(szSectionNames,1000,".\\mahi.ini");
		 memset(ptr,0,30);
		 ptr[j] = szSectionNames;

		 for(i=0;!(szSectionNames[i]==NULL&&szSectionNames[i+1]==NULL);i++)
		 {
			 if(szSectionNames[i] == '\0')
		 			 ptr[++j]= (szSectionNames+i+1);
		 }

		 iNumOfSections= j+1; //j starts from 0

		 for(j=0,i=0;i<iNumOfSections;i++)
		 {
			strcpy(szAttributeID,ptr[i]);
			if(iDigit=atoi(szAttributeID))    //if(sscanf(ptr[i],"%d",&iDigit))
		 		g_iNotOkAttributeIDs[j++] = iDigit; 
		 }
	}catch(std::exception const & e)
	{
     cout<<"error : " << e.what() <<endl;
	}
	
	 
	 return;

}