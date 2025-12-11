cmake_minimum_required(VERSION 3.16)

# Set project name and language
project(trading-system LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTING "Enable tests" ON)

add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(trading_system
    src/main.cpp
    src/data/csv_loader.cpp
    src/metrics/moving_average.cpp
    src/metrics/return_metrics.cpp
)

target_include_directories(trading_system
    PRIVATE
        src
        external/fast-cpp-csv-parser
)

if(BUILD_TESTING)
    enable_testing()

    add_executable(csv_loader_test
        tests/csv_loader_test.cpp
        src/data/csv_loader.cpp
    )

    target_include_directories(csv_loader_test
        PRIVATE
            src
            external/fast-cpp-csv-parser
    )

    add_test(NAME csv_loader COMMAND csv_loader_test)

    add_executable(twma_test
        tests/twma_test.cpp
        src/data/csv_loader.cpp
        src/metrics/moving_average.cpp
    )

    target_include_directories(twma_test
        PRIVATE
            src
            external/fast-cpp-csv-parser
    )

    add_test(NAME twma COMMAND twma_test)

    add_executable(vwma_test
        tests/vwma_test.cpp
        src/metrics/moving_average.cpp
        src/data/csv_loader.cpp
    )

    target_include_directories(vwma_test
        PRIVATE
            src
            external/fast-cpp-csv-parser
    )

    add_test(NAME vwma COMMAND vwma_test)

    add_executable(return_metrics_test
        tests/return_metrics_test.cpp
        src/metrics/return_metrics.cpp
    )

    target_include_directories(return_metrics_test
        PRIVATE
            src
    )

    add_test(NAME return_metrics COMMAND return_metrics_test)


endif()
