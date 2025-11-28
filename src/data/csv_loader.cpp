#include "data/csv_loader.h"

#include <stdexcept>

#include "csv.h"

namespace trading
{

    std::vector<Bar> load_bars_from_csv(const std::filesystem::path &csv_path)
    {
        if (!std::filesystem::exists(csv_path))
        {
            throw std::runtime_error("CSV file not found: " + csv_path.string());
        }

        io::CSVReader<10, io::trim_chars<' ', '\t', '<', '>'>, io::no_quote_escape<','>> in(csv_path.string());
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
        std::uint64_t volume{};
        std::uint64_t open_interest{};

        while (in.read_row(ticker, period, date, time, open, high, low, close, volume, open_interest))
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
                open_interest});
        }

        return bars;
    }

} // namespace trading
