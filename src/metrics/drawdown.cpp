#include "metrics/drawdown.h"

#include <algorithm>
#include <stdexcept>

namespace trading
{
    double max_drawdown(const std::vector<double>& equity)
    {
        if (equity.size() < 2)
        {
            throw std::invalid_argument("equity vector must contain at least two values");
        }

        const double first = equity.front();
        if (first <= 0.0)
        {
            throw std::invalid_argument("equity values must be > 0");
        }

        double peak = first;
        double max_dd = 0.0;

        // Maximum drawdown is computed over the running peak.
        for (double e : equity)
        {
            if (e <= 0.0)
            {
                throw std::invalid_argument("equity values must be > 0");
            }

            peak = std::max(peak, e);
            const double dd = (peak - e) / peak; // in [0, 1)
            max_dd = std::max(max_dd, dd);
        }

        return max_dd;
    }
} // namespace trading