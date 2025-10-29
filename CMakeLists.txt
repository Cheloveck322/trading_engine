cmake_minimum_required(VERSION 3.15)
project(Trading_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(source
    src/order_book.cpp
    src/matching_engine.cpp)

add_executable(trading_engine src/main.cpp)

target_link_libraries(trading_engine source)

# unit-тесты
find_package(GTest REQUIRED)

add_executable(test_orderbook
    tests/orderbook_test.cpp
)

target_link_libraries(test_orderbook
    GTest::gtest
    GTest::gtest_main
    source
)

enable_testing()
add_test(NAME OrderBookTest COMMAND test_orderbook)

# banchmark 
find_package(benchmark REQUIRED)

add_executable(benchmark_orderbook
    benchmark/orderbook_benchmark.cpp
)

target_link_libraries(benchmark_orderbook
    benchmark::benchmark
    source
)