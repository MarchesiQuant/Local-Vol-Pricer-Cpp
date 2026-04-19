#pragma once

#include <functional>
#include <vector>
#include <map>
#include <string>

extern std::vector<std::pair<int, double>> flat_curve;

std::function<double(double)> makeInterpolator(const std::vector<std::pair<int, double>>& curve);

struct HaganCalibrationParams
{
    double alpha{};
    double beta{};
    double rho{};
    double gamma{};
};

struct HaganCalibrationResult
{
    std::string expiry;
    int iterations{};
    double rmse{};
    HaganCalibrationParams params;
    std::map<std::string, double> optimal_params;
    std::map<std::string, double> calibration_results;
};

class Interpol
{
public:
    double alpha = 0.2;
    double beta  = 1.0;
    double gamma = 0.3;
    double rho   = 0.0;
    std::function<double(double)> fwd_curve;

    Interpol(double alpha_, double beta_, double gamma_, std::vector<std::pair<double, double>> fwd_curve_);

    void setParams(double alpha_, double beta_, double gamma_, double rho_, std::vector<std::pair<double, double>> fwd_curve_);

    std::function<double(double, double)> interpolate_hagan();

    HaganCalibrationResult calibrate_hagan(
        const std::map<std::string, std::vector<std::pair<double, double>>>& market_vols,
        const double expiry,
        bool verbose = true,
        int max_iterations = 250);
};
