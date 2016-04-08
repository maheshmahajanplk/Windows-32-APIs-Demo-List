#pragma once
#pragma managed(push, on)

#include <string>

class Assert
{
public:
    static void AreEqual(int expected, int actual)
    {
        Microsoft::VisualStudio::TestTools::UnitTesting::Assert::AreEqual(expected, actual);
    }

    static void AreEqual(double expected, double actual, double delta)
    {
        Microsoft::VisualStudio::TestTools::UnitTesting::Assert::AreEqual(expected, actual, delta);
    }

    static void AreEqual(const std::string& expected, const std::string& actual)
    {
        System::String^ act = gcnew System::String(actual.c_str());
        System::String^ exp = gcnew System::String(expected.c_str());

        Microsoft::VisualStudio::TestTools::UnitTesting::Assert::AreEqual(exp, act, false);
    }

    static void Fail(const std::string& message)
    {
        System::String^ msg = gcnew System::String(message.c_str());
        Microsoft::VisualStudio::TestTools::UnitTesting::Assert::Fail(msg);
    }
}


#pragma managed(pop)