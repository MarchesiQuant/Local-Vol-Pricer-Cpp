#include "test_suite.hpp"

#include <iostream>

int RunAllTests()
{
    int failures = 0;

    failures += RunUnitInterpolTests();

    std::cout << "\nTest summary: " << (failures == 0 ? "PASS" : "FAIL")
              << " (failures=" << failures << ")\n";

    return failures == 0 ? 0 : 1;
}

#ifdef LVP_ENABLE_TEST_MAIN
int main()
{
    return RunAllTests();
}
#endif
