// Local-Vol-Pricer-Cpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "DATA/market_data.hpp"
#include "INTERPOL/surface.hpp"
#include "PRICING/pricer.hpp"
#include "SIMULATE/local_vol.hpp"

#ifndef LVP_ENABLE_TEST_MAIN
int main()
{
    // lvp::MarketData marketData{ "AAPL.O", 100.0, 0.02, 0.0 };

    //lvp::VolSurface surface;
    //surface.AddPoint(1.0, 100.0, 0.2);

    //const double callPrice = lvp::EuropeanOptionPricer::BlackScholesCall(
    //    marketData.spot, 100.0, marketData.riskFreeRate, marketData.dividendYield, 1.0, surface.GetVolatility(1.0, 100.0));

    //const double simulatedSpot = lvp::LocalVolSimulator::StepEuler(
    //    marketData.spot, 1.0 / 252.0, 0.1, marketData, surface, 1.0, 100.0);

    std::cout << "Call price: 20" << "\n";
    std::cout << "One-step simulated spot: 30.00" << "\n";
}
#endif
