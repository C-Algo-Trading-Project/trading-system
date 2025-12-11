#include <cassert>
#include <cmath>
#include <vector>

#include "metrics/return_metrics.h"

using trading::ReturnCalculator;
using trading::ReturnMetrics;

int main()
{
    const double eps = 1e-12;

    // --- Test from_equity on simple geometric growth ---------------------
    std::vector<double> equity = {100.0, 110.0, 121.0}; // +10%, then +10%
    ReturnCalculator calc_default;                      // periods_per_year defaults to 252

    ReturnMetrics m1 = calc_default.from_equity(equity);
    assert(std::fabs(m1.cumulative_return - 0.21) < eps);
    assert(std::fabs(m1.avg_period_return - 0.10) < eps);
    assert(std::fabs(m1.annualized_return - (0.10 * 252.0)) < eps);

    // --- Test from_returns on flat series --------------------------------
    std::vector<double> returns = {0.0, 0.0, 0.0};
    ReturnMetrics m2 = calc_default.from_returns(returns);
    assert(std::fabs(m2.cumulative_return - 0.0) < eps);
    assert(std::fabs(m2.avg_period_return - 0.0) < eps);
    assert(std::fabs(m2.annualized_return - 0.0) < eps);

    // --- Test from_returns on mixed series -------------------------------
    std::vector<double> returns_mixed = {0.10, -0.05, 0.02};
    ReturnMetrics m2b = calc_default.from_returns(returns_mixed);
    const double expected_growth = (1.10 * 0.95 * 1.02) - 1.0; // ≈ 0.0659
    const double expected_avg = (0.10 - 0.05 + 0.02) / 3.0;
    assert(std::fabs(m2b.cumulative_return - expected_growth) < 1e-10);
    assert(std::fabs(m2b.avg_period_return - expected_avg) < 1e-12);
    assert(std::fabs(m2b.annualized_return - (expected_avg * 252.0)) < 1e-10);

    // --- Test custom periods_per_year ------------------------------------
    ReturnCalculator calc_12(12); // e.g., monthly data
    ReturnMetrics m3 = calc_12.from_returns({0.01, 0.02});
    assert(std::fabs(m3.avg_period_return - 0.015) < eps);
    assert(std::fabs(m3.annualized_return - (0.015 * 12.0)) < eps);

    return 0;
}
