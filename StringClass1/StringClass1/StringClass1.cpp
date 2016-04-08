// StringClass1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


/*int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}*/

#include <iostream>
#include <string>
using namespace std;

int main () 
{
    string str = "Hello";
	char cstr[10]= "Mahesh";
    cout << "str is : " << str << endl;
    str += ",";
    str += ' ';
    cout << "str is : " << str << endl;

    string s;
    s = str + "World";
    cout << "s is : " << s << endl;

    char ch = '!';
    s += " ";
	s+=cstr;
    cout << "s is : " << s << endl;

    return 0;
}
