#include "test_suite.hpp"
#include "test_assert.hpp"

#include "../PRICING/pricer.hpp"

int RunUnitPricerTests()
{
    int failures = 0;

    // Known Black-Scholes reference (approximately 8.916 for these inputs).
    {
        const double actual = lvp::EuropeanOptionPricer::BlackScholesCall(100.0, 100.0, 0.02, 0.0, 1.0, 0.2);
        failures += CheckNear(actual, 8.9160, 1e-3, "BlackScholesCall ATM reference value");
    }

    {
        const double actual = lvp::EuropeanOptionPricer::BlackScholesCall(120.0, 100.0, 0.02, 0.0, 0.0, 0.2);
        failures += CheckNear(actual, 20.0, 1e-12, "BlackScholesCall maturity zero returns intrinsic");
    }

    {
        const double actual = lvp::EuropeanOptionPricer::BlackScholesCall(80.0, 100.0, 0.02, 0.0, 1.0, 0.0);
        failures += CheckNear(actual, 0.0, 1e-12, "BlackScholesCall vol zero returns intrinsic");
    }

    {
        const double lowSpot = lvp::EuropeanOptionPricer::BlackScholesCall(95.0, 100.0, 0.02, 0.0, 1.0, 0.2);
        const double highSpot = lvp::EuropeanOptionPricer::BlackScholesCall(105.0, 100.0, 0.02, 0.0, 1.0, 0.2);
        failures += CheckTrue(highSpot > lowSpot, "BlackScholesCall increases with spot");
    }

    return failures;
}
