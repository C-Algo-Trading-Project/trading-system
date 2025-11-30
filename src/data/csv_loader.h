#pragma once

#include <filesystem>
#include <vector>

#include "core/bar.h"

namespace trading
{

    // Load bar data from a CSV file using fast-cpp-csv-parser.
    std::vector<Bar> load_bars_from_csv(const std::filesystem::path &csv_path);

} // namespace trading
