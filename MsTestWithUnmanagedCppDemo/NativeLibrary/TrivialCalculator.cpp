// This is the main DLL file.

#include "stdafx.h"

#include "TrivialCalculator.h"

using namespace MsTestWithUnmanagedCppDemo;

double TrivialCalculator::Add(double x, double y)
{
    return x + y;
}

double TrivialCalculator::Subtract(double x, double y)
{
    return x - y;
}

double TrivialCalculator::Multiply(double x, double y)
{
    return x * y;
}

double TrivialCalculator::Divide(double x, double y)
{
    return x / y;
}

