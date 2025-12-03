#include "metrics/moving_average.h"

#include <cmath>
#include <stdexcept>
#include <string>

namespace trading
{

    TimeWeightedMovingAverage::TimeWeightedMovingAverage(double timeConstantDays)
        : timeConstantDays_(timeConstantDays)
    {
        if (timeConstantDays_ <= 0.0)
        {
            throw std::invalid_argument("Invalid argument: time_constant_days must be > 0");
        }
    }

    void TimeWeightedMovingAverage::reset()
    {
        initialized_ = false;
        ema_ = 0.0;
        lastDate_ = 0;
    }

    double TimeWeightedMovingAverage::update(const Bar &bar)
    {
        SysDays currentDate = parseYyyyMmDd(bar.date);

        if (!initialized_)
        {
            // First sample: just start EMA at the first close
            ema_ = bar.close;
            lastDate_ = currentDate;
            initialized_ = true;
            return ema_;
        }

        // Compute time difference in days
        auto delta = currentDate - lastDate_;
        lastDate_ = currentDate;

        double deltaDays = static_cast<double>(delta);
        if (deltaDays < 0.0)
        {
            // If dates go backwards, treat as 0 gap (or you could throw)
            deltaDays = 0.0;
        }

        // a = (t_n - t_{n-1}) / T
        const double a = deltaDays / timeConstantDays_;

        // u = e^{-a}
        const double u = std::exp(-a);

        // x_n = bar.close
        const double xN = bar.close;

        // ema_n = u * ema_{n-1} + (1 - u) * x_n
        ema_ = u * ema_ + (1.0 - u) * xN;

        return ema_;
    }

    bool TimeWeightedMovingAverage::hasValue() const noexcept
    {
        return initialized_;
    }

    double TimeWeightedMovingAverage::value() const
    {
        if (!initialized_)
        {
            throw std::logic_error("TimeWeightedMovingAverage has no value yet");
        }
        return ema_;
    }

    // Convert YYYYMMDD → days since epoch using Julian day number (JDN)
    TimeWeightedMovingAverage::SysDays
    TimeWeightedMovingAverage::parseYyyyMmDd(const std::string &s)
    {
        if (s.size() != 8)
        {
            throw std::invalid_argument("expected date in YYYYMMDD format");
        }

        int y = std::stoi(s.substr(0, 4));
        int m = std::stoi(s.substr(4, 2));
        int d = std::stoi(s.substr(6, 2));

        // JDN algorithm (valid for Gregorian calendar)
        int a = (14 - m) / 2;
        int y2 = y + 4800 - a;
        int m2 = m + 12 * a - 3;

        long jdn = d + (153 * m2 + 2) / 5 + 365L * y2 + y2 / 4 - y2 / 100 + y2 / 400 - 32045;

        return static_cast<SysDays>(jdn);
    }

} // namespace trading
