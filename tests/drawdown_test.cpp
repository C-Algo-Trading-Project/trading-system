// drawdown_test.cpp

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "metrics/calculate_equity_curve.h"
#include "metrics/drawdown.h"

int main()
{
    try
    {
        const auto fixture =
            std::filesystem::path(__FILE__).parent_path() / "data" / "sample.csv";

        // 1) Compute equity curve
        const auto equity = trading::calculate_equity_curve_from_csv(fixture, 100000.0);
        assert(equity.size() >= 1);

        // 2) Write equity curve to CSV next to this test file
        const auto out_path =
            std::filesystem::path(__FILE__).parent_path() / "output" / "drawdown_equity_curve.csv";

        std::ofstream out("drawdown_equity_curve.csv");
        if (!out.is_open())
        {
            throw std::runtime_error("Failed to open output file: " + out_path.string());
        }

        out << "index,equity\n";
        for (std::size_t i = 0; i < equity.size(); ++i)
        {
            out << i << "," << equity[i] << "\n";
        }
        out.close();

        // 3) Compute max drawdown and print to console
        if (equity.size() < 2)
        {
            std::cout << "Max drawdown: 0% (equity curve has < 2 points)\n";
            return 0;
        }

        const double mdd = trading::max_drawdown(equity);
        std::cout << "Wrote equity curve to: " << out_path << "\n";
        std::cout << "Max drawdown: " << (mdd * 100.0) << "%\n";


        
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "drawdown_test.cpp error: " << e.what() << "\n";
        return 1;
    }
}