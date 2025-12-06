#include "data/csv_loader.h"

#include <stdexcept>

#include "csv.h"

namespace trading
{

    std::vector<Bar> loadBarsFromCsv(const std::filesystem::path &csvPath)
    {
        if (!std::filesystem::exists(csvPath))
        {
            throw std::runtime_error("CSV file not found: " + csvPath.string());
        }

        io::CSVReader<10, io::trim_chars<' ', '\t', '<', '>'>, io::no_quote_escape<','>> in(csvPath.string());
        in.read_header(io::ignore_extra_column,
                       "TICKER",
                       "PER",
                       "DATE",
                       "TIME",
                       "OPEN",
                       "HIGH",
                       "LOW",
                       "CLOSE",
                       "VOL",
                       "OPENINT");

        std::vector<Bar> bars;
        std::string ticker;
        std::string period;
        std::string date;
        std::string time;
        double open{};
        double high{};
        double low{};
        double close{};
        double volume{};
        std::uint64_t openInterest{};

        while (in.read_row(ticker, period, date, time, open, high, low, close, volume, openInterest))
        {
            bars.push_back(Bar{
                ticker,
                period,
                date,
                time,
                open,
                high,
                low,
                close,
                volume,
                openInterest});
        }

        return bars;
    }

} // namespace trading
