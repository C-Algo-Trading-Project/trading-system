#include <cassert>
#include <cmath>
#include <vector>

#include "metrics/moving_average.h"

using trading::Bar;
using trading::VolumeWeightedMovingAverage;

namespace
{
    constexpr double kEps = 1e-9;

    bool nearlyEqual(double a, double b, double eps = kEps)
    {
        return std::fabs(a - b) < eps;
    }
}

int main()
{
    // Simple equal-volume case, window=2
    std::vector<Bar> barsEqualVol = {
        {"", "", "", "", 0.0, 0.0, 0.0, 10.0, 1.0, 0},
        {"", "", "", "", 0.0, 0.0, 0.0, 20.0, 1.0, 0},
        {"", "", "", "", 0.0, 0.0, 0.0, 30.0, 1.0, 0},
        {"", "", "", "", 0.0, 0.0, 0.0, 40.0, 1.0, 0},
    };

    VolumeWeightedMovingAverage vwma2(2);
    assert(std::isnan(vwma2.update(barsEqualVol[0])));       // window not full
    assert(nearlyEqual(vwma2.update(barsEqualVol[1]), 15.0)); // (10+20)/2
    assert(nearlyEqual(vwma2.update(barsEqualVol[2]), 25.0)); // (20+30)/2
    assert(nearlyEqual(vwma2.update(barsEqualVol[3]), 35.0)); // (30+40)/2

    const auto batch2 = VolumeWeightedMovingAverage::compute(barsEqualVol, 2);
    assert(batch2.size() == barsEqualVol.size());
    assert(std::isnan(batch2[0]));        // first window not full
    assert(nearlyEqual(batch2[1], 15.0)); // first full window
    assert(nearlyEqual(batch2[2], 25.0));
    assert(nearlyEqual(batch2[3], 35.0));

    // Unequal volumes, window=3
    std::vector<Bar> barsUnequalVol = {
        {"", "", "", "", 0.0, 0.0, 0.0, 10.0, 1.0, 0},
        {"", "", "", "", 0.0, 0.0, 0.0, 20.0, 3.0, 0},
        {"", "", "", "", 0.0, 0.0, 0.0, 30.0, 1.0, 0},
        {"", "", "", "", 0.0, 0.0, 0.0, 40.0, 2.0, 0},
    };

    const auto batch3 = VolumeWeightedMovingAverage::compute(barsUnequalVol, 3);
    assert(batch3.size() == barsUnequalVol.size());
    assert(std::isnan(batch3[0]));
    assert(std::isnan(batch3[1]));

    // VWMA over [10*1, 20*3, 30*1] / (1+3+1) = 100 / 5 = 20
    assert(nearlyEqual(batch3[2], 20.0));

    // Next window: [20*3, 30*1, 40*2] / (3+1+2) = (60+30+80)/6 = 170/6
    assert(nearlyEqual(batch3[3], 170.0 / 6.0));

    return 0;
}
