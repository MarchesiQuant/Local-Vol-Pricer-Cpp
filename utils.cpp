#include "utils.hpp"

#include <cmath>
#include <functional>
#include <stdexcept>
#include <vector>

double interpolate1D(const std::vector<std::pair<int, double>>& data, double x) {
    if (data.empty()) {
        throw std::invalid_argument("Data vector is empty");
    }
    
    if (data.size() == 1) {
        return data[0].second;
    }
    
    // Extrapolation: return boundary values
    if (x <= data.front().first) {
        return data.front().second;
    }
    if (x >= data.back().first) {
        return data.back().second;
    }
    
    // Find interval and perform linear interpolation
    for (size_t i = 0; i < data.size() - 1; ++i) {
        if (x >= data[i].first && x <= data[i + 1].first) {
            double x0 = data[i].first;
            double y0 = data[i].second;
            double x1 = data[i + 1].first;
            double y1 = data[i + 1].second;
            
            return y0 + (y1 - y0) * (x - x0) / (x1 - x0);
        }
    }
    
    return data.back().second;
}

// Interpolation function generator
std::function<double(double)> makeInterpolator(const std::vector<std::pair<int, double>>& data) {
    auto dataCopy = data;
    
    return [dataCopy](double x) -> double {
        return interpolate1D(dataCopy, x);
    };
}

double NormalCdf(double x)
{
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}




