#include "test_suite.hpp"

#include <iostream>

namespace lvp::test
{
int RunAllTests()
{
    int failures = 0;

    failures += RunUnitPricerTests();
    failures += RunUnitSurfaceTests();
    failures += RunUnitLocalVolTests();

    std::cout << "\nTest summary: " << (failures == 0 ? "PASS" : "FAIL")
              << " (failures=" << failures << ")\n";

    return failures == 0 ? 0 : 1;
}
}

#ifdef LVP_ENABLE_TEST_MAIN
int main()
{
    return lvp::test::RunAllTests();
}
#endif
