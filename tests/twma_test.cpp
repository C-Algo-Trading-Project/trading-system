#include <cassert>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <fstream>

#include "data/csv_loader.h"
#include "functions/time_weighted_moving_average.h"

using trading::time_weighted_moving_average;

int main()
{
    // Load the same CSV as in csv_loader_test.cpp
    const auto fixture =
        std::filesystem::path(__FILE__).parent_path() / "data" / "sample.csv";

    const auto bars = trading::load_bars_from_csv(fixture);
    assert(!bars.empty());

    // Construct TWMA with T = 5 days
    time_weighted_moving_average twma(5.0);

    // Reset state explicitly to exercise reset()
    twma.reset();
    assert(!twma.has_value());  // uses has_value()

    // open output CSV file
    std::ofstream ofs("twma_output.csv");
    if (!ofs.is_open()) {
        std::cerr << "Failed to open output CSV file\n";
        return 1;
    }

    // Write CSV header
    ofs << "date,close,twma_5d\n";

    // write to console
    std::cout << "date,close,twma_5d\n";
    
    double last_ema = 0.0;

    // Recursively update EMA for each bar
    for (const auto& bar : bars) {
        last_ema = twma.update(bar);  // update() returns current EMA
        
        // Write to CSV file
        ofs << bar.date << ","
            << bar.close << ","
            << last_ema << "\n";
    }

    // After feeding all bars, EMA should be initialized
    assert(twma.has_value());

    // value() should match the last EMA returned from update()
    const double ema_from_value = twma.value();  // uses value()
    assert(std::fabs(ema_from_value - last_ema) < 1e-12);

    std::cout << "twma_test passed\n";
    return 0;
}