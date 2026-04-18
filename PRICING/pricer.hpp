#pragma once

namespace lvp
{
class EuropeanOptionPricer
{
public:
    static double BlackScholesCall(double spot, double strike, double rate, double dividendYield, double maturity, double volatility);
};
}
