#pragma once

#include <vector>

namespace lvp
{
struct SurfacePoint
{
    double maturity{};
    double strike{};
    double volatility{};
};

class VolSurface
{
public:
    void AddPoint(double maturity, double strike, double volatility);
    double GetVolatility(double maturity, double strike) const;

private:
    std::vector<SurfacePoint> points_;
};
}
