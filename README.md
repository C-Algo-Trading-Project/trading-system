# C++ Stock Trading System
A C++ based stock trading system.

## Dependencies
- [fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser): Already included in the repository at `external/fast-cpp-csv-parser/csv.h`. Use it with `#include "csv.h"`.

## Tests and sample data
- Sample JPX CSV lives at `tests/data/sample.csv`.
- Configure CMake for testing: `cmake -S . -B build -DBUILD_TESTING=ON`.
- Run tests w/o output: `cmake --build build && ctest --test-dir build --output-on-failure`.
- Run tests w output: `cmake --build build && ctest --test-dir build -V`.

## CSV format
- Expected header columns (case-sensitive): `TICKER,PER,DATE,TIME,OPEN,HIGH,LOW,CLOSE,VOL,OPENINT`, wrapped in angle brackets (e.g., `<TICKER>`).
- Date format `YYYYMMDD`, time `HHMMSS` (e.g., `000000`) as provided by source.
