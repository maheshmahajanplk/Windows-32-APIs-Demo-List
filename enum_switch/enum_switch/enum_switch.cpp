// enum_switch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

enum STATE {Disabled=1,Queued,Ready,Running};

int _tmain(int argc, _TCHAR* argv[])
{
	STATE job_state=Queued;

	printf("%s",job_state==1?"Disabled":job_state==2?"Queued":job_state==3?"Ready":"Running");

	return 0;
}

