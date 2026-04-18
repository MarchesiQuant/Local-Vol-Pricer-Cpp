#pragma once

#include "../DATA/market_data.hpp"
#include "../INTERPOL/surface.hpp"

namespace lvp
{
class LocalVolSimulator
{
public:
    static double StepEuler(double spot, double dt, double z, const MarketData& marketData, const VolSurface& surface, double maturity, double strike);
};
}
