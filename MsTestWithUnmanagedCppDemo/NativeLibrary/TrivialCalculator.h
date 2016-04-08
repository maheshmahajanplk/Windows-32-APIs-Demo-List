// MsTestWithUnmanagedCppDemo.h

#pragma once;


namespace MsTestWithUnmanagedCppDemo {

	class TrivialCalculator
	{
	public:
		static double Add(double x, double y);
		static double Subtract(double x, double y);
		static double Multiply(double x, double y);
		static double Divide(double x, double y);
	};
}
