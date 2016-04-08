#pragma once;

#include <string>

namespace MsTestWithUnmanagedCppDemo {

	class TrivialString
	{
	public:
		static std::string Combine(const std::string& str1, const std::string& str2);
	};

}