// StringWriteToBuffer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "StringWriteToBuffer.h"
#include <iostream>
#include <string>

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
  std::string firstlevel ("com");
  std::string secondlevel ("cplusplus");
  std::string scheme ("http://");
  std::string hostname;
  std::string url;

  hostname = "www." + secondlevel + '.' + firstlevel;
  url = scheme + hostname;

  //std::cout << url << '\n';
  //OutputDebugString();

  return 0;
}




