#pragma once

#include <string>
#include <vector>
#include <utility>
#include <cstddef>



// VolSurface holds a volatility surface read from a semicolon separated file
// - expiries: values parsed from the second row (columns after the first "Strike" column)
// - strikes: first column of each following row
// - vols: matrix of vol values where vols[row][col] corresponds to strikes[row], expiries[col]
struct VolSurface
{
    std::vector<double> expiries;
    std::vector<double> strikes;
    std::vector<std::vector<double>> vols;
};

// Load a semicolon-separated volatility surface file into a VolSurface structure.
// Returns false on IO or parse error.
bool loadVolSurfaceCSV(const std::string& path, VolSurface& out, char sep = ';');

// Extract a single column (expiry) as vector of (strike, vol) pairs.
// Returns false if colIndex is out of range.
bool getColumnForExpiry(const VolSurface& vs, std::size_t colIndex, std::vector<std::pair<double,double>>& out);

// Extract a single row by row index (strike row). Returns false if rowIndex invalid.
bool getRowByIndex(const VolSurface& vs, std::size_t rowIndex, double& strikeOut, std::vector<double>& volsOut);


