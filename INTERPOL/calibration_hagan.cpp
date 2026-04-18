#include "interpol.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <tuple>

namespace
{
double Clip(double value, double low, double high)
{
    return std::max(low, std::min(value, high));
}

double ExpiryToYears(const std::string& expiry)
{
    if (expiry.size() < 2)
    {
        throw std::invalid_argument("Invalid expiry format. Expected examples: 2W, 3M, 1Y.");
    }

    const char unit = static_cast<char>(std::toupper(static_cast<unsigned char>(expiry.back())));
    const std::string amountText = expiry.substr(0, expiry.size() - 1);

    double amount = 0.0;
    try
    {
        amount = std::stod(amountText);
    }
    catch (...)
    {
        throw std::invalid_argument("Invalid expiry format. Expected numeric amount before unit.");
    }

    if (amount < 0.0)
    {
        throw std::invalid_argument("Expiry must be non-negative.");
    }

    switch (unit)
    {
    case 'D': return amount / 365.0;
    case 'W': return amount / 52.0;
    case 'M': return amount / 12.0;
    case 'Y': return amount;
    default:
        throw std::invalid_argument("Invalid expiry unit. Supported units: D, W, M, Y.");
    }
}

std::string FormatAvailableExpiries(const std::map<std::string, std::vector<std::pair<double, double>>>& market_vols)
{
    std::ostringstream oss;
    oss << "[";
    bool first = true;
    for (const auto& kv : market_vols)
    {
        if (!first)
        {
            oss << ", ";
        }
        first = false;
        oss << kv.first;
    }
    oss << "]";
    return oss.str();
}
}

HaganCalibrationResult Interpol::calibrate_hagan(
    const std::map<std::string, std::vector<std::pair<double, double>>>& market_vols,
    const std::string& expiry,
    bool verbose,
    int max_iterations)
{
    const auto it = market_vols.find(expiry);
    if (it == market_vols.end())
    {
        throw std::invalid_argument("Expiry '" + expiry + "' not found in market_vols. Available expiries: " +
                                    FormatAvailableExpiries(market_vols));
    }

    const double T = ExpiryToYears(expiry);

    std::vector<std::tuple<double, double, double>> data;
    data.reserve(it->second.size());

    for (const auto& point : it->second)
    {
        const double K = point.first;
        const double vol = point.second / 100.0;
        if (std::isfinite(K) && std::isfinite(vol) && std::isfinite(T))
        {
            data.emplace_back(K, T, vol);
        }
    }

    if (data.empty())
    {
        throw std::invalid_argument("No valid strike/volatility points found for expiry '" + expiry + "'.");
    }

    auto objective = [this, &data](const std::array<double, 3>& params) -> double {
        const double alpha_ = params[0];
        const double rho_ = params[1];
        const double gamma_ = params[2];

        alpha = alpha_;
        beta = 1.0;
        rho = rho_;
        gamma = gamma_;

        auto sabr_func = interpolate_hagan();

        double mse = 0.0;
        for (const auto& row : data)
        {
            const double K = std::get<0>(row);
            const double T = std::get<1>(row);
            const double market_vol = std::get<2>(row);

            try
            {
                const double model_vol = sabr_func(K, T);
                const double err = model_vol - market_vol;
                if (!std::isfinite(err))
                {
                    mse += 1e6;
                }
                else
                {
                    mse += err * err;
                }
            }
            catch (...)
            {
                mse += 1e6;
            }
        }

        return std::sqrt(mse / static_cast<double>(data.size()));
    };

    std::array<double, 3> x = {
        Clip(alpha, 1e-6, 5.0),
        Clip(rho, -0.999, 0.999),
        Clip(gamma, 1e-6, 5.0)
    };

    std::array<double, 3> lower = { 1e-6, -0.999, 1e-6 };
    std::array<double, 3> upper = { 5.0, 0.999, 5.0 };

    std::array<double, 3> step = {
        std::max(0.10 * x[0], 0.01),
        0.10,
        std::max(0.10 * x[2], 0.01)
    };

    double best = objective(x);
    int iterations = 0;

    for (; iterations < max_iterations; ++iterations)
    {
        bool improved = false;

        for (int i = 0; i < 3; ++i)
        {
            for (int dir = -1; dir <= 1; dir += 2)
            {
                auto candidate = x;
                candidate[i] = Clip(candidate[i] + static_cast<double>(dir) * step[i], lower[i], upper[i]);

                const double value = objective(candidate);
                if (value < best)
                {
                    best = value;
                    x = candidate;
                    improved = true;
                }
            }
        }

        if (!improved)
        {
            step[0] *= 0.5;
            step[1] *= 0.5;
            step[2] *= 0.5;

            if (std::max({ step[0], step[1], step[2] }) < 1e-6)
            {
                break;
            }
        }
    }

    alpha = x[0];
    rho = x[1];
    gamma = x[2];
    beta = 1.0;

    HaganCalibrationResult results;
    results.expiry = expiry;
    results.iterations = iterations;
    results.rmse = best;
    results.params = HaganCalibrationParams{ alpha, beta, rho, gamma };

    if (verbose)
    {
        std::cout << std::string(120, '=') << "\n";
        std::cout << "Calibration finished in " << iterations << " iterations for expiry " << expiry << ".\n";
        std::cout << "Optimal parameters: alpha=" << std::fixed << std::setprecision(6) << alpha
                  << ", beta=" << beta
                  << ", rho=" << rho
                  << ", gamma=" << gamma << "\n";
        std::cout << "Final Error (RMSE): " << std::fixed << std::setprecision(6) << (best * 100.0) << "%\n";
        std::cout << std::string(120, '=') << "\n";

        std::cout << "\nError matrix (Model Vol - Market Vol) for expiry " << expiry << ":\n";
        std::cout << std::right
                  << std::setw(10) << "Strike" << "  "
                  << std::setw(8) << "Expiry" << "  "
                  << std::setw(7) << "T" << "  "
                  << std::setw(14) << "Model Vol (%)" << "  "
                  << std::setw(15) << "Market Vol (%)" << "  "
                  << std::setw(12) << "Error (%)" << "\n";

        auto sabr_func = interpolate_hagan();
        for (const auto& row : data)
        {
            const double K = std::get<0>(row);
            const double TT = std::get<1>(row);
            const double market_vol = std::get<2>(row);

            try
            {
                const double model_vol = sabr_func(K, TT);
                const double err = model_vol - market_vol;
                std::cout << std::fixed << std::setprecision(3) << std::setw(10) << K << "  "
                          << std::setw(8) << expiry << "  "
                          << std::setw(7) << std::setprecision(4) << TT << "  "
                          << std::setw(14) << std::setprecision(6) << (model_vol * 100.0) << "  "
                          << std::setw(15) << std::setprecision(6) << (market_vol * 100.0) << "  "
                          << std::setw(12) << std::setprecision(6) << (err * 100.0) << "\n";
            }
            catch (...)
            {
                std::cout << std::fixed << std::setprecision(3)
                          << std::setw(10) << K << "  "
                          << std::setw(8) << expiry << "  "
                          << std::setw(7) << std::setprecision(4) << TT
                          << "  Error computing model volatility\n";
            }
        }
    }

    return results;
}
