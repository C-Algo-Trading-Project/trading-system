#include <cassert>
#include <filesystem>
#include <iostream>

#include "data/csv_loader.h"

int main()
{
    const auto fixture = std::filesystem::path(__FILE__).parent_path() / "data" / "sample.csv";
    const auto bars = trading::load_bars_from_csv(fixture);

    // file has 440 data rows (wc -l minus header).
    assert(bars.size() == 440);

    const auto &first = bars.front();
    assert(first.symbol == "130A.JP");
    assert(first.period == "D");
    assert(first.date == "20240209");
    assert(first.time == "000000");
    assert(first.open == 2851.0);
    assert(first.high == 3005.0);
    assert(first.low == 2606.0);
    assert(first.close == 3005.0);
    assert(first.volume == 2075000);
    assert(first.open_interest == 0);

    const auto &last = bars.back();
    assert(last.date == "20251127");
    assert(last.close == 507.0);
    assert(last.volume == 17300);

    std::cout << "csv_loader_test passed\n";
    return 0;
}
