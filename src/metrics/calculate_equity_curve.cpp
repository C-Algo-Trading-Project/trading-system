#include "metrics/calculate_equity_curve.h"

#include <stdexcept>

#include "data/csv_loader.h"

namespace trading
{
    std::vector<double> calculate_equity_curve_from_bars(const std::vector<Bar>& bars, double starting_equity)
    {
        if (starting_equity <= 0.0)
        {
            throw std::invalid_argument("starting_equity must be > 0");
        }
        if (bars.empty())
        {
            throw std::invalid_argument("bars must not be empty");
        }

        std::vector<double> equity;
        equity.reserve(bars.size());
        equity.push_back(starting_equity);

        // If only one bar, equity curve is just the starting value.
        if (bars.size() == 1)
        {
            return equity;
        }

        for (std::size_t i = 1; i < bars.size(); ++i)
        {
            const double prev_close = bars[i - 1].close;
            const double curr_close = bars[i].close;

            if (prev_close <= 0.0 || curr_close <= 0.0)
            {
                throw std::invalid_argument("bar close must be > 0 to compute returns");
            }

            const double gross_return = curr_close / prev_close; // 1 + r
            equity.push_back(equity.back() * gross_return);
        }

        return equity;
    }

    std::vector<double> calculate_equity_curve_from_csv(const std::filesystem::path& csv_path, double starting_equity)
    {
        const auto bars = trading::loadBarsFromCsv(csv_path);
        return calculate_equity_curve_from_bars(bars, starting_equity);
    }
} // namespace trading