#include "pricer.hpp"

#include "../utils.hpp"

#include <algorithm>
#include <cmath>

namespace lvp
{
double EuropeanOptionPricer::BlackScholesCall(double spot, double strike, double rate, double dividendYield, double maturity, double volatility)
{
    if (maturity <= 0.0 || volatility <= 0.0 || strike <= 0.0)
    {
        return std::max(0.0, spot - strike);
    }

    const double sqrtT = std::sqrt(maturity);
    const double vsqrtT = volatility * sqrtT;
    const double d1 = (std::log(spot / strike) + (rate - dividendYield + 0.5 * volatility * volatility) * maturity) / vsqrtT;
    const double d2 = d1 - vsqrtT;

    const double discountedSpot = spot * std::exp(-dividendYield * maturity);
    const double discountedStrike = strike * std::exp(-rate * maturity);

    return discountedSpot * NormalCdf(d1) - discountedStrike * NormalCdf(d2);
}
}
