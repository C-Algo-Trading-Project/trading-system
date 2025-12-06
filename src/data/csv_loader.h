#pragma once

#include <filesystem>
#include <vector>

#include "core/bar.h"

namespace trading
{

    // Load bar data from a CSV file using fast-cpp-csv-parser.
    std::vector<Bar> loadBarsFromCsv(const std::filesystem::path &csvPath);

} // namespace trading
