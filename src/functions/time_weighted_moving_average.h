#pragma once

#include <string>
#include "core/bar.h"

namespace trading
{

class time_weighted_moving_average
{
public:
    using days     = long; // number of days
    using sys_days = long; // days since epoch (custom)

    // Constructor
    explicit time_weighted_moving_average(double time_constant_days = 5.0);

    // Reset internal state
    void reset();

    // Update EMA using a new Bar
    double update(const Bar& bar);

    // Has EMA been initialized?
    bool has_value() const noexcept;

    // Return current EMA value (throws if not initialized)
    double value() const;

private:
    // Convert "YYYYMMDD" → sys_days using JDN
    static sys_days parse_yyyymmdd(const std::string& s);

    // Internal state
    double   T_days_;
    bool     initialized_ = false;
    double   ema_         = 0.0;
    sys_days last_date_   = 0;
};

} // namespace trading