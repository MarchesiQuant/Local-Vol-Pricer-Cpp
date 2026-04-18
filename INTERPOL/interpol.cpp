#include "interpol.hpp"
#include "../utils.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>

// Flat curve for default setting
std::vector<std::pair<int, double>> flat_curve = []() {
    std::vector<std::pair<int, double>> curve;
    for (int i = 1; i <= 100; ++i) {
        curve.push_back({ i, 100.0 });
    }
    return curve;
}();

// Constructor
Interpol::Interpol(double alpha_, double beta_, double gamma_, std::vector<std::pair<int, double>> fwd_curve_)
    : alpha(alpha_), beta(beta_), gamma(gamma_), fwd_curve(makeInterpolator(fwd_curve_))
{
}

// Set parameters
void Interpol::setParams(double alpha_, double beta_, double gamma_, double rho_, std::vector<std::pair<int, double>> fwd_curve_)
{
    alpha     = alpha_;
    beta      = beta_;
    gamma     = gamma_;
    rho       = rho_;
    fwd_curve = makeInterpolator(fwd_curve_);
}

// Hagan interpolator
std::function<double(double, double)> Interpol::interpolate_hagan()
{
    double alpha_  = alpha;
    double beta_   = beta;
    double rho_    = rho;
    double gamma_  = gamma;
    auto   get_fwd = fwd_curve;

    return [alpha_, beta_, rho_, gamma_, get_fwd](double K, double T) -> double {
        double F   = get_fwd(T);
        double eps = 1e-04;

        // ATM case: F == K
        if (std::abs(F - K) < eps) {
            double denom     = std::pow(F, 1 - beta_);
            double term1     = ((1 - beta_) * (1 - beta_) / 24) * (alpha_ * alpha_) / std::pow(F, 2 - 2 * beta_);
            double term2     = (1.0 / 4.0) * (rho_ * beta_ * gamma_ * alpha_) / std::pow(F, 1 - beta_);
            double term3     = (2 - 3 * rho_ * rho_) / 24 * gamma_ * gamma_;
            double expansion = 1 + (term1 + term2 + term3) * T;
            return (alpha_ / denom) * expansion;
        }

        // Non-ATM case (Hagan standard)
        double logFK     = std::log(F / K);
        double FK_beta   = std::pow(F * K, (1 - beta_) / 2);
        double denom     = 1 + ((1 - beta_) * (1 - beta_) / 24) * logFK * logFK +
                           ((1 - beta_) * (1 - beta_) * (1 - beta_) * (1 - beta_) / 1920) * std::pow(logFK, 4);
        double a_hat     = alpha_ / (FK_beta * denom);
        double c_hat     = (gamma_ / alpha_) * FK_beta * logFK;
        double g_c_hat   = std::log((std::sqrt(1 - 2 * rho_ * c_hat + c_hat * c_hat) + c_hat - rho_) / (1 - rho_));
        double term1     = ((1 - beta_) * (1 - beta_) / 24) * (alpha_ * alpha_) / std::pow(F * K, 1 - beta_);
        double term2     = (1.0 / 4.0) * (rho_ * beta_ * gamma_ * alpha_) / FK_beta;
        double term3     = (2 - 3 * rho_ * rho_) / 24 * gamma_ * gamma_;
        double expansion = 1 + (term1 + term2 + term3) * T;
        return (a_hat * c_hat / g_c_hat) * expansion;
    };
}





