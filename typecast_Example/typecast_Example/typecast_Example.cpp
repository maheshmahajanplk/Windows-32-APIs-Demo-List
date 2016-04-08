// typecast_Example.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include<math.h>
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	short i = 32768;
	unsigned int sizeOfInt;
	cout<<"\n sizeof(short) :"<<sizeof(short)<<endl;
	cout<<"\n sizeof(int) : "<<sizeof(int)<<endl;
	cout<<"i="<<i<<endl;
	sizeOfInt = pow(2,32);   /* 2^32 = 4,294,967,296 */
	cout<<"Size of int = "<<sizeOfInt<<endl;

	return 0;
}

