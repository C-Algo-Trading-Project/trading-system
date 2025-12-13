#include "metrics/return_metrics.h"

#include <stdexcept>

namespace trading
{

    ReturnCalculator::ReturnCalculator(int periods_per_year)
        : periods_per_year_(normalize_periods_per_year(periods_per_year))
    {
    }

    int ReturnCalculator::normalize_periods_per_year(int periods_per_year)
    {
        return (periods_per_year > 0) ? periods_per_year : 245;
    }

    ReturnMetrics ReturnCalculator::from_equity(const std::vector<double> &equity) const
    {
        if (equity.size() < 2)
        {
            throw std::invalid_argument("equity vector must contain at least two values");
        }

        const double starting_value = equity.front();
        if (starting_value == 0.0)
        {
            throw std::invalid_argument("equity start value cannot be zero");
        }

        double sum_returns = 0.0;
        std::size_t count = 0;

        for (std::size_t i = 1; i < equity.size(); ++i)
        {
            const double prev = equity[i - 1];
            if (prev == 0.0)
            {
                throw std::invalid_argument("equity values cannot contain zero (division by zero)");
            }

            const double r = (equity[i] / prev) - 1.0;
            sum_returns += r;
            ++count;
        }

        ReturnMetrics metrics;
        metrics.cumulative_return = (equity.back() / starting_value) - 1.0;
        metrics.avg_period_return = (count > 0) ? (sum_returns / static_cast<double>(count)) : 0.0;
        metrics.annualized_return = metrics.avg_period_return * static_cast<double>(periods_per_year_);

        return metrics;
    }

    ReturnMetrics ReturnCalculator::from_returns(const std::vector<double> &returns) const
    {
        if (returns.empty())
        {
            throw std::invalid_argument("returns vector must not be empty");
        }

        double sum_returns = 0.0;
        double growth_product = 1.0;

        for (double r : returns)
        {
            growth_product *= (1.0 + r);
            sum_returns += r;
        }

        ReturnMetrics metrics;
        metrics.cumulative_return = growth_product - 1.0;
        metrics.avg_period_return = sum_returns / static_cast<double>(returns.size());
        metrics.annualized_return = metrics.avg_period_return * static_cast<double>(periods_per_year_);

        return metrics;
    }

} // namespace trading
