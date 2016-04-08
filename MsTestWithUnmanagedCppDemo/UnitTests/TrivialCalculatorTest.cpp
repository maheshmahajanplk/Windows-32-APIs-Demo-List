#include "stdafx.h"
#include "Assert.h"

#pragma unmanaged
#include "..\NativeLibrary\TrivialCalculator.h"

using namespace MsTestWithUnmanagedCppDemo;

namespace
{
    void AddTest()
    {
        double actual = TrivialCalculator::Add(1.0, 2.0);
        double expected = 2.0;

        Assert::AreEqual(expected, actual, 0.0001);
    }

    void SubtractTest()
    {
        double actual = TrivialCalculator::Subtract(2.0, 1.0);
        double expected = 1.0;

        Assert::AreEqual(expected, actual, 0.0001);
    }
}

#pragma managed

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace Microsoft::VisualStudio::TestTools::UnitTesting;

namespace UnitTests
{
    [TestClass]
    public ref class TrivialCalculatorTest
    {
    public: 

#pragma region Additional test attributes
        //
        //You can use the following additional attributes as you write your tests:
        //
        //Use ClassInitialize to run code before running the first test in the class
        //[ClassInitialize()]
        //static void MyClassInitialize(TestContext^ testContext) {};
        //
        //Use ClassCleanup to run code after all tests in a class have run
        //[ClassCleanup()]
        //static void MyClassCleanup() {};
        //
        //Use TestInitialize to run code before running each test
        //[TestInitialize()]
        //void MyTestInitialize() {};
        //
        //Use TestCleanup to run code after each test has run
        //[TestCleanup()]
        //void MyTestCleanup() {};
        //
#pragma endregion 

        [TestMethod]
        void AddTest()
        {
            try
            {
                ::AddTest();
            }
            catch (UnitTestAssertException^) { throw; }
            catch (...) { FailTest(); }
        }

        [TestMethod]
        void SubtractTest()
        {
            try
            {
                ::SubtractTest();
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
