// ClassExample1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include "WebpostFormat.h"
#define MAX_CONFIG_ENTRY 1024

int _tmain(int argc, _TCHAR* argv[])
{
	std::string FinalStatus;
	WebpostFormat wp[MAX_CONFIG_ENTRY];

	wp[0].setProcessName("MAHI");
	wp[0].setStatus("E");

	FinalStatus = wp[0].getStatus();

	if(FinalStatus.compare("E")==0)
		std::cout<<"Status got Error"<<std::endl;
	else
		std::cout<<"Status is good"<<std::endl;
		
	std::cout<<FinalStatus<<std::endl;
	return 0;
}

