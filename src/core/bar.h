#pragma once

#include <string>

namespace trading {

struct Bar {
    std::string symbol;
    std::string date;
    double open{0.0};
    double high{0.0};
    double low{0.0};
    double close{0.0};
    double volume{0.0};
};

}  // namespace trading
