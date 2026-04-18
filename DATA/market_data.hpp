#pragma once

#include <string>

namespace lvp
{
struct MarketData
{
    std::string underlying;
    double spot{};
    double riskFreeRate{};
    double dividendYield{};
};
}
