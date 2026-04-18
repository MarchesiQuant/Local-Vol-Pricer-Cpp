#include "interpol.hpp"

#include <algorithm>

namespace lvp
{
double BilinearInterpolator::Interpolate(double x, double y,
                                         double x0, double x1,
                                         double y0, double y1,
                                         double q00, double q10,
                                         double q01, double q11)
{
    const double dx = std::max(1e-12, x1 - x0);
    const double dy = std::max(1e-12, y1 - y0);
    const double tx = (x - x0) / dx;
    const double ty = (y - y0) / dy;

    const double a = q00 * (1.0 - tx) + q10 * tx;
    const double b = q01 * (1.0 - tx) + q11 * tx;
    return a * (1.0 - ty) + b * ty;
}
}
