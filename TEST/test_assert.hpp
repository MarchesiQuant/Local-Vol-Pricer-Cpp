#pragma once

#include <cmath>
#include <iostream>

namespace lvp::test
{
inline int CheckTrue(bool condition, const char* message)
{
    if (!condition)
    {
        std::cout << "[FAIL] " << message << '\n';
        return 1;
    }
    return 0;
}

inline int CheckNear(double actual, double expected, double tolerance, const char* message)
{
    if (std::fabs(actual - expected) > tolerance)
    {
        std::cout << "[FAIL] " << message << " | actual=" << actual << " expected=" << expected << " tol=" << tolerance << '\n';
        return 1;
    }
    return 0;
}
}
