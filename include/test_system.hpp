#include <cstdlib>
#include <cstdio>
#include <stdexcept>

namespace TestSystem
{
    typedef bool (*TestScenario)();

    enum TestResult
    {
        FAIL      = 0,
        OK        = 1,
        EXCEPTION = 2,
        ERROR     = 3,
        TIMEOUT   = 4
    };

    TestResult run_test(const char* name, TestScenario test, bool inspect = false);
};