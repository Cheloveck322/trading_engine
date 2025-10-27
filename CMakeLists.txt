cmake_minimum_required(VERSION 3.15)

project(Trading_engine)

include_directories(${CMAKE_BINARY_DIR}/include)

add_library(source
    src/order_book.cpp)

add_executable(trading_engine src/main.cpp)

target_link_libraries(trading_engine source)