// FormatSpecifier_star.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>


int _tmain(int argc, _TCHAR* argv[])
{
	char szDate[50]="30 November 2015 Thursday";
	char Month[20];
	char Day[20];
	char Year[20];

	sscanf(szDate,"%*s%s%s%s",Month,Day,Year);	

	/* char sentence []="Rudolph 12 years old";
	 char str [20];
	 char str2[30];
	 int i;

     sscanf (sentence,"%s %*d %s",str,str2);*/
     //printf ("%s -> %d\n",str,i);
	return 0;
}

