#include "functions/time_weighted_moving_average.h"

#include <cmath>
#include <stdexcept>
#include <string>

namespace trading
{

time_weighted_moving_average::time_weighted_moving_average(double time_constant_days)
    : T_days_(time_constant_days)
{
    if (T_days_ <= 0.0) {
        throw std::invalid_argument("Invalid argument: time_constant_days must be > 0");
    }
}

void time_weighted_moving_average::reset()
{
    initialized_ = false;
    ema_ = 0.0;
    last_date_ = 0;
}

double time_weighted_moving_average::update(const Bar& bar)
{
    sys_days current_date = parse_yyyymmdd(bar.date);

    if (!initialized_) {
        // First sample: just start EMA at the first close
        ema_ = bar.close;
        last_date_ = current_date;
        initialized_ = true;
        return ema_;
    }

    // Compute time difference in days
    auto delta = current_date - last_date_;
    last_date_ = current_date;

    double delta_days = static_cast<double>(delta);
    if (delta_days < 0.0) {
        // If dates go backwards, treat as 0 gap (or you could throw)
        delta_days = 0.0;
    }

    // a = (t_n - t_{n-1}) / T
    const double a = delta_days / T_days_;

    // u = e^{-a}
    const double u = std::exp(-a);

    // x_n = bar.close
    const double x_n = bar.close;

    // ema_n = u * ema_{n-1} + (1 - u) * x_n
    ema_ = u * ema_ + (1.0 - u) * x_n;

    return ema_;
}

bool time_weighted_moving_average::has_value() const noexcept
{
    return initialized_;
}

double time_weighted_moving_average::value() const
{
    if (!initialized_) {
        throw std::logic_error("time_weighted_moving_average has no value yet");
    }
    return ema_;
}

// Convert YYYYMMDD → days since epoch using Julian day number (JDN)
time_weighted_moving_average::sys_days
time_weighted_moving_average::parse_yyyymmdd(const std::string& s)
{
    if (s.size() != 8) {
        throw std::invalid_argument("expected date in YYYYMMDD format");
    }

    int y = std::stoi(s.substr(0, 4));
    int m = std::stoi(s.substr(4, 2));
    int d = std::stoi(s.substr(6, 2));

    // JDN algorithm (valid for Gregorian calendar)
    int a  = (14 - m) / 2;
    int y2 = y + 4800 - a;
    int m2 = m + 12 * a - 3;

    long jdn = d
             + (153 * m2 + 2) / 5
             + 365L * y2
             + y2 / 4
             - y2 / 100
             + y2 / 400
             - 32045;

    return static_cast<sys_days>(jdn);
}

} // namespace trading