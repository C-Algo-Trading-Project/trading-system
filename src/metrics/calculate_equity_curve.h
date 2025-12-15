#pragma once

#include <filesystem>
#include <vector>

#include "core/bar.h"
#warning "USING calculate_equity_curve.h"

namespace trading
{
    // Build a buy-and-hold equity curve from bars using close-to-close returns.
    // Output length == bars.size()
    // equity[0] = starting_equity
    // equity[i] = equity[i-1] * (bars[i].close / bars[i-1].close)
    std::vector<double> calculate_equity_curve_from_bars(
        const std::vector<Bar>& bars,
        double starting_equity = 1.0
    );

    // Convenience: load bars using CSV loader then compute equity curve.
    std::vector<double> calculate_equity_curve_from_csv(
        const std::filesystem::path& csv_path,
        double starting_equity = 1.0
    );
} // namespace trading