// string_copy.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include <iostream>
#include <string>

std::string str;

void func(char * pszRegID)
{
	
	str="<regid>";
	str.append(pszRegID);
	
}

int main ()
{

 char buffer[20] = "21345678";
  /*std::string str;
  str.append("<regid>");*/

 func(buffer);
 std::cout<<str<<std::endl;

   return 0;
}

/*int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}*/

