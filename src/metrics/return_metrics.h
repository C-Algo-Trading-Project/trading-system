#pragma once

#include <vector>

namespace trading
{

    struct ReturnMetrics
    {
        double cumulative_return = 0.0;  // total growth - 1
        double avg_period_return = 0.0;  // arithmetic mean of per-period returns
        double annualized_return = 0.0;  // avg_period_return * periods_per_year
    };

    // Stateless helper to compute return metrics. You can reuse one instance with a fixed
    // periods_per_year to avoid passing it around.
    class ReturnCalculator
    {
    public:
        explicit ReturnCalculator(int periods_per_year = 252);

        // Compute return metrics from an equity curve (portfolio value per bar).
        ReturnMetrics from_equity(const std::vector<double> &equity) const;

        // Compute return metrics directly from a vector of per-period arithmetic returns.
        ReturnMetrics from_returns(const std::vector<double> &returns) const;

        int periods_per_year() const noexcept { return periods_per_year_; }

    private:
        static int normalize_periods_per_year(int periods_per_year);
        int periods_per_year_;
    };

} // namespace trading
