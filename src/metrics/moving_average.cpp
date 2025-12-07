#include "metrics/moving_average.h"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>
#include <chrono>

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

    // Convert YYYYMMDD to days since epoch
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

        std::chrono::year_month_day ymd{
            std::chrono::year{y},
            std::chrono::month{ static_cast<unsigned int>(m) },
            std::chrono::day{ static_cast<unsigned int>(d) }
        };

        std::chrono::sys_days sd{ ymd };

        return static_cast<SysDays>(sd.time_since_epoch().count());
    }

     std::vector<double> TimeWeightedMovingAverage::compute(const std::vector<Bar> &bars, std::size_t windowSize) {
        if (bars.empty())
        {
            throw std::invalid_argument("Data vector is empty");
        }

        TimeWeightedMovingAverage twma(windowSize);
        std::vector<double> result;
        result.reserve(bars.size());

        for (const auto &bar : bars)
        {
            double current = twma.update(bar);
            result.push_back(current);
        }

        return result;
    }

    VolumeWeightedMovingAverage::VolumeWeightedMovingAverage(std::size_t windowSize)
        : windowSize_(windowSize), count_(0), sumPriceVolumeProduct_(0.0), sumVolume_(0.0)
    {
        if (windowSize_ == 0)
        {
            throw std::invalid_argument("Invalid argument: windowSize must be > 0");
        }
    }

    void VolumeWeightedMovingAverage::reset()
    {
        count_ = 0;
        priceVolumeProducts_.clear();
        volumes_.clear();
        sumPriceVolumeProduct_ = 0.0;
        sumVolume_ = 0.0;
    }

    double VolumeWeightedMovingAverage::update(const Bar &bar)
    {
        double priceVolumeProduct = bar.close * bar.volume;

        priceVolumeProducts_.push_back(priceVolumeProduct);
        volumes_.push_back(bar.volume);

        sumPriceVolumeProduct_ += priceVolumeProduct;
        sumVolume_ += bar.volume;
        ++count_;

        if (count_ > windowSize_)
        {
            // Remove oldest
            sumPriceVolumeProduct_ -= priceVolumeProducts_.front();
            sumVolume_ -= volumes_.front();
            priceVolumeProducts_.pop_front();
            volumes_.pop_front();
        }

        if (count_ < windowSize_)
        {
            return std::numeric_limits<double>::quiet_NaN();
        }

        return (sumVolume_ > 0.0) ? (sumPriceVolumeProduct_ / sumVolume_) : 0.0;
    }

    std::vector<double> VolumeWeightedMovingAverage::compute(const std::vector<Bar> &bars, std::size_t windowSize)
    {
        if (windowSize == 0 || bars.size() < windowSize)
        {
            throw std::invalid_argument("Invalid argument: windowSize must be > 0 and <= number of bars");
        }

        VolumeWeightedMovingAverage vwma(windowSize);
        std::vector<double> result;
        result.reserve(bars.size());

        for (std::size_t i = 0; i < bars.size(); ++i)
        {
            double current = vwma.update(bars[i]);
            if (i + 1 < windowSize)
            {
                // Window not full yet; indicate missing value
                result.push_back(std::numeric_limits<double>::quiet_NaN());
            }
            else
            {
                result.push_back(current);
            }
        }

        return result;
    }

} // namespace trading
