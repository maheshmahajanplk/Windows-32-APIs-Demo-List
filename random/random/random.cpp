// random.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include <windows.h>
#include<conio.h>

int _tmain(int argc, _TCHAR* argv[])
{
	int num;
	SYSTEMTIME st;
	GetLocalTime(&st);
	time_t	tmLastFiveMinutes =	0;

	time(&tmLastFiveMinutes); //Get System time in seconds elapsed 1 January 1970 midnight
	tmLastFiveMinutes -= 300; //5x60

	srand(time(NULL));
	num = rand(); 
	num = num % 24; //0-23

	printf("\n num = %d  ",num);
	

	if(num == st.wHour)
	{
		printf("\nMatchHappens\n");
	}

	getch();
	return 0;
}

