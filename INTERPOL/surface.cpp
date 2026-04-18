#include "surface.hpp"

#include "interpol.hpp"

#include <algorithm>
#include <limits>

namespace lvp
{
void VolSurface::AddPoint(double maturity, double strike, double volatility)
{
    points_.push_back(SurfacePoint{ maturity, strike, volatility });
}

double VolSurface::GetVolatility(double maturity, double strike) const
{
    if (points_.empty())
    {
        return 0.0;
    }

    auto nearest = points_.front();
    double minDistance = std::numeric_limits<double>::max();

    for (const auto& p : points_)
    {
        const double dm = p.maturity - maturity;
        const double dk = p.strike - strike;
        const double d = dm * dm + dk * dk;
        if (d < minDistance)
        {
            minDistance = d;
            nearest = p;
        }
    }

    return nearest.volatility;
}
}
