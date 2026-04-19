#pragma once

#include <utility>
#include <vector>
#include <functional>

double interpolate1D(const std::vector<std::pair<double, double>>& data, double x);
std::function<double(double)> makeInterpolator(const std::vector<std::pair<double, double>>& data);
double NormalCdf(double x);