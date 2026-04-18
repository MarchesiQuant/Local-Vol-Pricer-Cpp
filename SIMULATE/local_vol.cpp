#include "local_vol.hpp"

#include <cmath>

namespace lvp
{
double LocalVolSimulator::StepEuler(double spot, double dt, double z, const MarketData& marketData, const VolSurface& surface, double maturity, double strike)
{
    const double localVol = surface.GetVolatility(maturity, strike);
    const double drift = (marketData.riskFreeRate - marketData.dividendYield) * spot * dt;
    const double diffusion = localVol * spot * std::sqrt(dt) * z;
    return spot + drift + diffusion;
}
}
