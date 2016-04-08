// SizeOfObject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include<iostream>

using namespace std;

class sample
{
	char a;
	int b;

public:
	
	void print(void *ptr){
		 cout<<"value is:"<<ptr<<endl;
	}
};


int _tmain(int argc, _TCHAR* argv[])
{
	sample s1;
	cout<<"sizeof(sample):"<<sizeof(s1);

	return 0;
}

