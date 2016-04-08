#include "stdafx.h"
#include "Assert.h"
#include "..\NativeLibrary\TrivialString.h"

using namespace MsTestWithUnmanagedCppDemo;
using namespace std;

namespace
{
    void CombineTest()
    {
        string actual = TrivialString::Combine("The quick brown", " fox jumps");
        string expected = "The quick brown fox jumps";

        Assert::AreEqual(expected, actual);
    }

}

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace	Microsoft::VisualStudio::TestTools::UnitTesting;

namespace UnitTests
{
    [TestClass]
    public ref class TrivialStringTest
    {
    public: 
        [TestMethod]
        void CombineTest()
        {
            try
            {
                ::CombineTest();
            }
            catch (UnitTestAssertException^) { throw; }
            catch (...) { FailTest(); }
        }

    private:
        void FailTest()
        {
            Microsoft::VisualStudio::TestTools::UnitTesting::Assert::Fail("Unhandled exception");
        }
    };
}
