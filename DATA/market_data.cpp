#include "DATA/market_data.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <limits>


bool loadVolSurfaceCSV(const std::string& path, VolSurface& out, char sep)
{
    std::ifstream ifs(path);
    if (!ifs.is_open()) return false;

    std::string line;

    // 1) Ignore title line
    if (!std::getline(ifs, line)) return false;

    // 2) header line contains expiries (first token is "Strike")
    if (!std::getline(ifs, line)) return false;
    {
        std::stringstream ss(line);
        std::string token;
        if (!std::getline(ss, token, sep)) return false;

        // Remaining tokens are expiries
        while (std::getline(ss, token, sep)) {
            if (token.empty()) continue;
            try {
                out.expiries.push_back(std::stod(token));
            }
            catch (...) {
                out.expiries.push_back(std::numeric_limits<double>::quiet_NaN());
            }
        }
    }

    // Remaining lines
    while (std::getline(ifs, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string token;
        // strike
        if (!std::getline(ss, token, sep)) continue;
        try {
            double strike = std::stod(token);
            out.strikes.push_back(strike);
        }
        catch (...) {
            out.strikes.push_back(std::numeric_limits<double>::quiet_NaN());
        }

        std::vector<double> row;
        while (std::getline(ss, token, sep)) {
            if (token.empty()) { row.push_back(std::numeric_limits<double>::quiet_NaN()); continue; }
            try { row.push_back(std::stod(token)); }
            catch (...) { row.push_back(std::numeric_limits<double>::quiet_NaN()); }
        }

        // if row shorter than expiries, pad
        if (row.size() < out.expiries.size()) row.resize(out.expiries.size(), std::numeric_limits<double>::quiet_NaN());
        out.vols.push_back(std::move(row));
    }

    return true;
}

bool getColumnForExpiry(const VolSurface& vs, std::size_t colIndex, std::vector<std::pair<double,double>>& out)
{
    out.clear();
    if (colIndex >= vs.expiries.size()) return false;
    for (std::size_t i = 0; i < vs.strikes.size(); ++i) {
        double vol = std::numeric_limits<double>::quiet_NaN();
        if (i < vs.vols.size() && colIndex < vs.vols[i].size()) vol = vs.vols[i][colIndex];
        out.emplace_back(vs.strikes[i], vol);
    }
    return true;
}

bool getRowByIndex(const VolSurface& vs, std::size_t rowIndex, double& strikeOut, std::vector<double>& volsOut)
{
    if (rowIndex >= vs.strikes.size() || rowIndex >= vs.vols.size()) return false;
    strikeOut = vs.strikes[rowIndex];
    volsOut = vs.vols[rowIndex];
    return true;
}

