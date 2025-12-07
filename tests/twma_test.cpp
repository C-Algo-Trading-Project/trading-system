#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>

#include "core/bar.h"
#include "metrics/moving_average.h"
#include "data/csv_loader.h"

using trading::Bar;
using trading::TimeWeightedMovingAverage;

int main()
{
    // --- Synthetic in-memory test data ------------------------------------
    std::vector<Bar> barsEqualVol = {
        {"", "", "20240101", "", 0.0, 0.0, 0.0, 10.0, 1.0, 0},
        {"", "", "20240102", "", 0.0, 0.0, 0.0, 20.0, 1.0, 0},
        {"", "", "20240103", "", 0.0, 0.0, 0.0, 30.0, 1.0, 0},
        {"", "", "20240104", "", 0.0, 0.0, 0.0, 40.0, 1.0, 0},
    };

    // Construct TWMA with T = 5 days
    TimeWeightedMovingAverage twma(5.0);

    // --- Test initial state / reset() / hasValue() -------------------------

    // Initially should have no value
    assert(!twma.hasValue());

    // Reset should keep it in "no value" state
    twma.reset();
    assert(!twma.hasValue());

    // --- Test update() and value() on first bar ----------------------------

    double ema = twma.update(barsEqualVol[0]);

    // After first update, TWMA should be initialized
    assert(twma.hasValue());

    // By design, first EMA should equal the first close
    assert(std::fabs(ema - barsEqualVol[0].close) < 1e-12);
    assert(std::fabs(twma.value() - ema) < 1e-12);

    // --- Feed remaining bars, check consistency ---------------------------

    for (std::size_t i = 1; i < barsEqualVol.size(); ++i)
    {
        double newEma = twma.update(barsEqualVol[i]);

        // update() should return the same as value()
        assert(std::fabs(newEma - twma.value()) < 1e-12);
    }

    double emaAfterAll = twma.value();
    (void)emaAfterAll; // just to silence unused-variable warnings if you like

    // --- Test reset + re-run to ensure reset works ------------------------

    twma.reset();
    assert(!twma.hasValue());

    double ema2 = twma.update(barsEqualVol[0]);
    assert(twma.hasValue());
    assert(std::fabs(ema2 - barsEqualVol[0].close) < 1e-12);

    // --- Test static compute() helper on synthetic data -------------------

    const auto twmaSeries = TimeWeightedMovingAverage::compute(barsEqualVol, 5.0);
    assert(twmaSeries.size() == barsEqualVol.size());

    // Last element of compute() series should match streaming EMA
    TimeWeightedMovingAverage twmaCheck(5.0);
    twmaCheck.reset();
    for (const auto &bar : barsEqualVol)
    {
        twmaCheck.update(bar);
    }
    assert(std::fabs(twmaSeries.back() - twmaCheck.value()) < 1e-12);

    // --- Test CSV loader + TWMA end-to-end --------------------------------

    const auto fixture =
        std::filesystem::path(__FILE__).parent_path() / "data" / "sample.csv";

    const auto barsFromCsv = trading::loadBarsFromCsv(fixture);
    assert(!barsFromCsv.empty());

    // Compute TWMA series from CSV data
    const auto twmaFromCsv = TimeWeightedMovingAverage::compute(barsFromCsv, 5.0);
    assert(twmaFromCsv.size() == barsFromCsv.size());

    // Sanity: after streaming again, last value should match compute()
    TimeWeightedMovingAverage twmaCsvCheck(5.0);
    twmaCsvCheck.reset();
    for (const auto &bar : barsFromCsv)
    {
        twmaCsvCheck.update(bar);
    }
    assert(std::fabs(twmaFromCsv.back() - twmaCsvCheck.value()) < 1e-12);

    // Compute TWMA series with time-constant (or window) = 5
    const auto twmaSeries_test = TimeWeightedMovingAverage::compute(barsFromCsv, 5.0);

    // Open CSV output file
    std::ofstream ofs("twma_output.csv");
    if (!ofs.is_open())
    {
        std::cerr << "Failed to open twma_output.csv\n";
        return 1;
    }

    // Write CSV header
    ofs << "date,close,twma_5d\n";
    std::cout << "date,close,twma_5d\n";

    // Dump bars + precomputed TWMA values
    for (std::size_t i = 0; i < barsFromCsv.size(); ++i)
    {
        const auto &bar = barsFromCsv[i];
        double currentTWMA = twmaSeries_test[i];

        ofs << bar.date << ","
            << bar.close << ","
            << currentTWMA << "\n";

        std::cout << bar.date << ", "
                << bar.close << ", "
                << currentTWMA << "\n";
    }
    return 0;
}