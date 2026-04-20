#include "test_suite.hpp"

#include <iostream>
#include "DATA/market_data.hpp"
#include "INTERPOL/interpol.hpp"
#include <cmath>
#include <vector>
#include <utility>

int RunAllTests()
{

    int expiryIndex = 12;
    std::vector<std::pair<double, double>> market_vols;
    lvp::getColumnForExpiry(vs, expiryIndex, market_vols);
    Interpol interpol(/*init params and forward curve*/);
    auto res = interpol.calibrate_hagan(market_vols, vs.expiries[expiryIndex], true, 250);




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
