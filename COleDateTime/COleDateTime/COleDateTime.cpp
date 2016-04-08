// COleDateTime.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


/*int _tmain(int argc, _TCHAR* argv[])
{

	return 0;
}*/


#include "stdafx.h"
#include <Windows.h>
#include <atlcomtime.h> 
#include <string>


/* void trim2(string& str)
{
  string::size_type pos = str.find_last_not_of(' ');
  if(pos != string::npos) {
    str.erase(pos + 1);
    pos = str.find_first_not_of(' ');
    if(pos != string::npos) str.erase(0, pos);
  }
  else str.erase(str.begin(), str.end());
}*/

int _tmain(int argc, _TCHAR* argv[])
{

 COleDateTime date;
 TCHAR strDate[100] = { L"2016 March 31" };



 //MAKELCID(MAKELANGID(0x09, 0x01), SORT_DEFAULT)
 if (date.ParseDateTime(strDate, LANG_USER_DEFAULT))
 {

	  int day = date.GetDay();
	  int  m = date.GetMonth();
	  int d = date.GetDay();
	  int yyyy = date.GetYear();
	  printf("%d %d %d", d, m, yyyy, day);

 }

 return 0;
}
