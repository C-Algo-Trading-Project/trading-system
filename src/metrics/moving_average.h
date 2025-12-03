#pragma once

#include <string>
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

    private:
        // Convert "YYYYMMDD" → sys_days using JDN
        static SysDays parseYyyyMmDd(const std::string &s);

        // Internal state
        double timeConstantDays_;
        bool initialized_ = false;
        double ema_ = 0.0;
        SysDays lastDate_ = 0;
    };

} // namespace trading
