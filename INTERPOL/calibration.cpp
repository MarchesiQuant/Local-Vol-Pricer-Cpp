#include "INTERPOL/interpol.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>


// Clamp a value between a lower and upper bound
double Clamp(double x, double lo, double hi){
    return std::max(lo, std::min(hi, x));
}

HaganCalibrationResult Interpol::calibrate_hagan(
    const std::vector<std::pair<double, double>>& market_vols,
    const double expiry,
    bool verbose,
    int max_iterations){

    struct DataPoint { double K; double expiry; double vol; };
    std::vector<DataPoint> data;
    data.reserve(market_vols.size());


	// Filter out invalid points and convert vol from percentage to decimal
    for (const auto& [strike, vol_pct] : market_vols) {
        if (std::isfinite(strike) && std::isfinite(vol_pct) && std::isfinite(expiry)) {
            data.push_back({ strike, expiry, vol_pct / 100.0 });
        }
    }

    if (data.empty()) {
        throw std::invalid_argument("No valid market volatility points found for expiry '" + expiry + "'.");
    }

    constexpr double alpha_min = 1e-6;
    constexpr double alpha_max = 5.0;
    constexpr double rho_min = -0.999;
    constexpr double rho_max = 0.999;
    constexpr double gamma_min = 1e-6;
    constexpr double gamma_max = 5.0;

    auto objective = [&](double a, double r, double g) {
        alpha = a;
        beta = 1.0;
        rho = r;
        gamma = g;

        const auto sabr_func = interpolate_hagan();

        double sum_sq = 0.0;
        int count = 0;
        for (const auto& p : data) {
            try {
                const double model_vol = sabr_func(p.K, p.expiry);
                const double err = model_vol - p.vol;
                sum_sq += err * err;
                ++count;
            }
            catch (...) {
                sum_sq += 1e6;
                ++count;
            }
        }

        return std::sqrt(sum_sq / static_cast<double>(std::max(1, count)));
    };

    double best_alpha = Clamp(alpha, alpha_min, alpha_max);
    double best_rho = Clamp(rho, rho_min, rho_max);
    double best_gamma = Clamp(gamma, gamma_min, gamma_max);
    double best_rmse = objective(best_alpha, best_rho, best_gamma);

    std::array<double, 3> step{ 0.10, 0.10, 0.10 };
    int iterations = 0;

    auto try_candidate = [&](double a, double r, double g) {
        const double score = objective(a, r, g);
        if (score < best_rmse) {
            best_rmse = score;
            best_alpha = a;
            best_rho = r;
            best_gamma = g;
            return true;
        }
        return false;
    };

    for (iterations = 0; iterations < std::max(1, max_iterations); ++iterations) {
        bool improved = false;

        improved = try_candidate(Clamp(best_alpha + step[0], alpha_min, alpha_max), best_rho, best_gamma) || improved;
        improved = try_candidate(Clamp(best_alpha - step[0], alpha_min, alpha_max), best_rho, best_gamma) || improved;
        improved = try_candidate(best_alpha, Clamp(best_rho + step[1], rho_min, rho_max), best_gamma) || improved;
        improved = try_candidate(best_alpha, Clamp(best_rho - step[1], rho_min, rho_max), best_gamma) || improved;
        improved = try_candidate(best_alpha, best_rho, Clamp(best_gamma + step[2], gamma_min, gamma_max)) || improved;
        improved = try_candidate(best_alpha, best_rho, Clamp(best_gamma - step[2], gamma_min, gamma_max)) || improved;

        if (!improved) {
            step[0] *= 0.5;
            step[1] *= 0.5;
            step[2] *= 0.5;
        }

        if (step[0] < 1e-6 && step[1] < 1e-6 && step[2] < 1e-6) {
            ++iterations;
            break;
        }
    }

    // Optimal parameters
    alpha = best_alpha;
    rho = best_rho;
    gamma = best_gamma;
    beta = 1.0;


    // Results 
    HaganCalibrationResult result;
    result.expiry = expiry;
    result.iterations = iterations;
    result.rmse = best_rmse;
    result.params = { alpha, beta, rho, gamma };
    result.optimal_params = {
        { "alpha", alpha },
        { "beta", beta },
        { "rho", rho },
        { "gamma", gamma }
    };
    result.calibration_results = {
        { "Iterations", static_cast<double>(iterations) },
        { "RMSE", best_rmse },
        { "Expiry", expiry }
    };


	// Verbose output
    if (verbose) {
        std::cout << std::string(120, '=') << "\n";
        std::cout << "Calibration finished in " << iterations << " iterations for expiry " << expiry << ".\n";
        std::cout << "Optimal parameters: alpha=" << std::fixed << std::setprecision(6) << alpha
                  << ", beta=" << beta
                  << ", rho=" << rho
                  << ", gamma=" << gamma << "\n";
        std::cout << "Final Error (RMSE): " << (best_rmse * 100.0) << "%\n";
        std::cout << std::string(120, '=') << "\n";

        std::cout << "\nError matrix (Model Vol - Market Vol) for expiry " << expiry << ":\n";
        std::cout << std::setw(10) << "Strike" << "  "
                  << std::setw(8) << "Expiry" << "  "
                  << std::setw(7) << "T" << "  "
                  << std::setw(14) << "Model Vol (%)" << "  "
                  << std::setw(15) << "Market Vol (%)" << "  "
                  << std::setw(12) << "Error (%)" << "\n";

        const auto sabr_func = interpolate_hagan();
        for (const auto& p : data) {
            try {
                const double model_vol = sabr_func(p.K, p.expiry);
                const double err = model_vol - p.vol;
                std::cout << std::setw(10) << std::fixed << std::setprecision(3) << p.K << "  "
                          << std::setw(8) << expiry << "  "
                          << std::setw(7) << std::setprecision(4) << p.expiry << "  "
                          << std::setw(14) << std::setprecision(6) << (model_vol * 100.0) << "  "
                          << std::setw(15) << std::setprecision(6) << (p.vol * 100.0) << "  "
                          << std::setw(12) << std::setprecision(6) << (err * 100.0) << "\n";
            }
            catch (...) {
                std::cout << std::setw(10) << std::fixed << std::setprecision(3) << p.K << "  "
                          << std::setw(8) << expiry << "  "
                          << std::setw(7) << std::setprecision(4) << p.T << "  "
                          << "Error computing model volatility\n";
            }
        }
    }

    return result;
}
