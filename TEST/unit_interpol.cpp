#include "test_suite.hpp"
#include "test_assert.hpp"

#include "../INTERPOL/interpol.hpp"

#include <map>
#include <string>

int RunUnitInterpolTests()
{
    int failures = 0;
    std::vector<std::pair<int, double>> flat = flat_curve;
    std::vector<std::pair<int, double>> sample_curve = {{1, 95.0},{2, 100.0}, {3, 105.0}, {5, 110.0}};

    // --- Constructor & default parameters ---
    {
        Interpol interp(0.2, 1.0, 0.3, flat);
        failures += CheckNear(interp.alpha, 0.2,  1e-12, "Interpol constructor sets alpha");
        failures += CheckNear(interp.beta,  1.0,  1e-12, "Interpol constructor sets beta");
        failures += CheckNear(interp.gamma, 0.3,  1e-12, "Interpol constructor sets gamma");
        failures += CheckNear(interp.rho,   0.0,  1e-12, "Interpol constructor default rho");
    }

    // --- setParams ---
    {
        Interpol interp(0.2, 1.0, 0.3, flat);
        interp.setParams(0.3, 0.5, 0.4, -0.1, flat);
        failures += CheckNear(interp.alpha, 0.3,  1e-12, "setParams updates alpha");
        failures += CheckNear(interp.beta,  0.5,  1e-12, "setParams updates beta");
        failures += CheckNear(interp.gamma, 0.4,  1e-12, "setParams updates gamma");
        failures += CheckNear(interp.rho,  -0.1,  1e-12, "setParams updates rho");
    }

    // --- Forward curve interpolation ---
    {
        Interpol interp(0.2, 1.0, 0.3, flat);
        const double fwd = interp.fwd_curve(2.00);
        failures += CheckNear(fwd, 100.0, 1e-12, "Flat forward curve returns 100.0");

        interp.setParams(0.3, 0.5, 0.4, -0.1, sample_curve);
        const double fwd_sample = interp.fwd_curve(2.50);
        failures += CheckNear(fwd_sample, 102.5, 1e-12, "Sample curve returns correct value when interpolated");
        
    }

    // --- SABR ATM: vol should be positive and finite ---
    {
        Interpol interp(0.2, 1.0, 0.3, flat);
        auto hagan = interp.interpolate_hagan();
        const double vol = hagan(100.0, 1.0); // ATM: K == F == 100
        failures += CheckTrue(vol > 0.0,  "SABR ATM vol is positive");
        failures += CheckTrue(vol < 10.0, "SABR ATM vol is finite");
    }

    // --- Hagan calibration ---
    {
        Interpol target(0.25, 1.0, 0.45, flat);
        target.setParams(0.25, 1.0, 0.45, -0.20, flat);
        auto target_hagan = target.interpolate_hagan();

        std::vector<std::pair<double, double>> points;
        for (double strike = 80.0; strike <= 120.0; strike += 10.0)
        {
            points.push_back({ strike, target_hagan(strike, 1.0) * 100.0 });
        }

        std::map<std::string, std::vector<std::pair<double, double>>> market_vols;
        market_vols["1Y"] = points;

        Interpol interp(0.15, 1.0, 0.20, flat);
        interp.setParams(0.15, 1.0, 0.20, 0.05, flat);
        const auto result = interp.calibrate_hagan(market_vols, "1Y", false, 300);

        failures += CheckNear(interp.alpha, 0.25, 4e-2, "calibrate_hagan alpha fit");
        failures += CheckNear(interp.rho,  -0.20, 4e-2, "calibrate_hagan rho fit");
        failures += CheckNear(interp.gamma, 0.45, 5e-2, "calibrate_hagan gamma fit");
        failures += CheckTrue(result.rmse < 5e-4, "calibrate_hagan low RMSE");
    }

    return failures;
}

