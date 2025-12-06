#pragma once

#include <string>
#include <cstdint>

namespace trading
{

    struct Bar
    {
        std::string symbol;
        std::string period;
        std::string date;
        std::string time;
        double open{0.0};
        double high{0.0};
        double low{0.0};
        double close{0.0};
        double volume{0.0};
        std::uint64_t openInterest{0};
    };

} // namespace trading
