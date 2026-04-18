#include "test_suite.hpp"
#include "test_assert.hpp"

#include "../INTERPOL/surface.hpp"

namespace lvp::test
{
int RunUnitSurfaceTests()
{
    int failures = 0;

    lvp::VolSurface surface;
    surface.AddPoint(1.0, 100.0, 0.20);
    surface.AddPoint(2.0, 100.0, 0.25);
    surface.AddPoint(1.0, 110.0, 0.22);

    {
        const double vol = surface.GetVolatility(1.0, 100.0);
        failures += CheckNear(vol, 0.20, 1e-12, "VolSurface exact node lookup");
    }

    {
        const double vol = surface.GetVolatility(1.9, 100.0);
        failures += CheckNear(vol, 0.25, 1e-12, "VolSurface nearest-node behavior in maturity");
    }

    {
        const double vol = surface.GetVolatility(1.05, 109.0);
        failures += CheckNear(vol, 0.22, 1e-12, "VolSurface nearest-node behavior in strike");
    }

    return failures;
}
}
