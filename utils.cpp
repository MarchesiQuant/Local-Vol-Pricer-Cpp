#include "utils.hpp"

#include <cmath>

namespace lvp
{
double NormalCdf(double x)
{
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}
}
