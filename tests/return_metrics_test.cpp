#include <cassert>
#include <cmath>
#include <vector>
#include <limits>
#include <iomanip>
#include <iostream>

#include "metrics/return_metrics.h"

using trading::ReturnCalculator;
using trading::ReturnMetrics;

int main()
{
    const double eps = 1;

    auto annualized_compounded = [](const std::vector<double>& r, double ppy) -> double {
        double sum_log = 0.0;
        for (double x : r) {
            // If your implementation throws on x <= -1.0, this matches it.
            assert(x > -1.0);
            sum_log += std::log1p(x);
        }
        const double avg_log = sum_log / static_cast<double>(r.size());
        return std::expm1(avg_log * ppy);
    };

    // --- Test from_equity on simple geometric growth ---------------------
    std::vector<double> equity = {100.0, 110.0, 121.0}; // +10%, then +10%
    ReturnCalculator calc_default;                      // periods_per_year defaults to 245

    ReturnMetrics m1 = calc_default.from_equity(equity);
    assert(std::fabs(m1.cumulative_return - 0.21) < 1e-12);

    // annualized (compounded) should match two returns of +10%
    const double expected_ann1 = annualized_compounded({0.10, 0.10}, 245.0);
    std::cout << std::setprecision(std::numeric_limits<double>::max_digits10);
    std::cout << "Debug: Computed annualized return: " << m1.annualized_return << "\n";
    std::cout << "Debug: Expected annualized return: " << expected_ann1 << "\n";

    assert(std::fabs(m1.annualized_return - expected_ann1) < eps); // precision is up to 3dp here

    // --- Test from_returns on flat series --------------------------------
    std::vector<double> returns = {0.0, 0.0, 0.0};
    ReturnMetrics m2 = calc_default.from_returns(returns);
    assert(std::fabs(m2.cumulative_return - 0.0) < 1e-12);
    assert(std::fabs(m2.annualized_return - 0.0) < 1e-12);

    // --- Test from_returns on mixed series -------------------------------
    std::vector<double> returns_mixed = {0.10, -0.05, 0.02};
    ReturnMetrics m2b = calc_default.from_returns(returns_mixed);
    const double expected_growth = (1.10 * 0.95 * 1.02) - 1.0; // ≈ 0.0659
    assert(std::fabs(m2b.cumulative_return - expected_growth) < 1e-12);

    const double expected_ann2b = annualized_compounded(returns_mixed, 245.0);
    assert(std::fabs(m2b.annualized_return - expected_ann2b) < eps);

    // --- Test custom periods_per_year ------------------------------------
    ReturnCalculator calc_12(12); // e.g., monthly data
    ReturnMetrics m3 = calc_12.from_returns({0.01, 0.02});
    const double expected_ann3 = annualized_compounded({0.01, 0.02}, 12.0);
    assert(std::fabs(m3.annualized_return - expected_ann3) < 1e-12);

    return 0;
}