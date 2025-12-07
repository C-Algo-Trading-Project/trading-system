#pragma once

#include <string>
#include <vector>
#include <deque>
#include "core/bar.h"

namespace trading
{

    class TimeWeightedMovingAverage
    {
    public:
        using Days = long;    // number of days
        using SysDays = long; // days since epoch (custom)

        // Constructor
        explicit TimeWeightedMovingAverage(double timeConstantDays = 5.0);

        // Reset internal state
        void reset();

        // Update EMA using a new Bar
        double update(const Bar &bar);

        // Has EMA been initialized?
        bool hasValue() const noexcept;

        // Return current EMA value (throws if not initialized)
        double value() const;

        // Return EMA for all bars.
        static std::vector<double> compute(const std::vector<Bar> &bars, std::size_t windowSize);

    private:
        // Convert "YYYYMMDD" → sys_days using JDN
        static SysDays parseYyyyMmDd(const std::string &s);

        // Internal state
        double timeConstantDays_;
        bool initialized_ = false;
        double ema_ = 0.0;
        SysDays lastDate_ = 0;
    };

    // Volume-weighted moving average over a fixed window.
    // Supports incremental updates via update() or batch computation via compute().
    class VolumeWeightedMovingAverage
    {
    public:
        // Constructor
        explicit VolumeWeightedMovingAverage(std::size_t windowSize);

        // Reset internal state
        void reset();

        // Update VWMA using a new Bar
        // Returns current VWMA; NaN if window not full
        double update(const Bar &bar);

        // Compute VWMA for each bar.
        // Returns a vector the same length as bars; the first (windowSize - 1) entries are NaN
        // because the window is not yet full.
        static std::vector<double> compute(const std::vector<Bar> &bars, std::size_t windowSize);

    private:
        std::size_t windowSize_;
        std::size_t count_;
        std::deque<double> priceVolumeProducts_;
        std::deque<double> volumes_;

        double sumPriceVolumeProduct_;
        double sumVolume_;
    };

} // namespace trading
