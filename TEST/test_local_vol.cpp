#include "../PRICING/pricer.hpp"

#include <iostream>

int run_local_vol_sample_test()
{
    const double price = lvp::EuropeanOptionPricer::BlackScholesCall(100.0, 100.0, 0.02, 0.0, 1.0, 0.2);
    std::cout << "Sample call price: " << price << '\n';
    return 0;
}
