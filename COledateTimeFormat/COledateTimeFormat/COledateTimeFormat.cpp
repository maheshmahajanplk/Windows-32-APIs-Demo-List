// COledateTimeFormat.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <atlcomtime.h> 
#include <string>
#include<conio.h>

int _tmain(int argc, _TCHAR* argv[])
{

 COleDateTime date;
 TCHAR strDate[100] = { L"Jueves, 07 de Abril de 2016" }; //Jueves, 07 de Abril de 2016
 //MAKELCID(MAKELANGID(0x09, 0x01), SORT_DEFAULT)
 if (date.ParseDateTime(strDate, LANG_USER_DEFAULT))
 {

  int  m = date.GetMonth();
  int d = date.GetDay();
  int yyyy = date.GetYear();
  printf("%d %d %d", d, m, yyyy);

 }
 printf("Unable to fetch DateTime Format");

 getch();

 return 0;
}
	