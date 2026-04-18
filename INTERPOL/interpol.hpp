#pragma once

namespace lvp
{
class BilinearInterpolator
{
public:
    static double Interpolate(double x, double y,
                              double x0, double x1,
                              double y0, double y1,
                              double q00, double q10,
                              double q01, double q11);
};
}
