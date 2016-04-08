#include "StdAfx.h"
#include "TrivialString.h"

using namespace std;
using namespace MsTestWithUnmanagedCppDemo;

string TrivialString::Combine(const string& str1, const string& str2)
{
    return str1 + str2;
}
