#include "test_suite.hpp"
#include "test_assert.hpp"

#include "../DATA/market_data.hpp"
#include "../INTERPOL/surface.hpp"
#include "../SIMULATE/local_vol.hpp"

#include <cmath>

namespace lvp::test
{
int RunUnitLocalVolTests()
{
    int failures = 0;

    lvp::MarketData md{"AAPL.O", 100.0, 0.02, 0.0};
    lvp::VolSurface surface;
    surface.AddPoint(1.0, 100.0, 0.20);

    {
        const double out = lvp::LocalVolSimulator::StepEuler(100.0, 0.0, 0.5, md, surface, 1.0, 100.0);
        failures += CheckNear(out, 100.0, 1e-12, "StepEuler dt=0 keeps spot unchanged");
    }

    {
        const double out = lvp::LocalVolSimulator::StepEuler(100.0, 1.0 / 252.0, 0.0, md, surface, 1.0, 100.0);
        failures += CheckTrue(std::isfinite(out), "StepEuler output is finite for z=0");
    }

    {
        const double dt = 1.0 / 252.0;
        const double expected = 100.0 + (md.riskFreeRate - md.dividendYield) * 100.0 * dt;
        const double out = lvp::LocalVolSimulator::StepEuler(100.0, dt, 0.0, md, surface, 1.0, 100.0);
        failures += CheckNear(out, expected, 1e-12, "StepEuler with z=0 equals drift-only step");
    }

    return failures;
}
}
