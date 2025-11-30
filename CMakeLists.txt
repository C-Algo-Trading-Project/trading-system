cmake_minimum_required(VERSION 3.16)

project(trading-system LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTING "Enable tests" ON)

add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(trading_system
    src/main.cpp
    src/data/csv_loader.cpp
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
endif()
