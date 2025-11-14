#include <benchmark/benchmark.h>
#include "../include/order_book.hpp"

static void BM_Process10000Orders(benchmark::State& state) {
    for (auto _ : state) {
        // Only allocate the price range we actually use (90-110 with 0.01 ticks)
        OrderBook ob(90.0, 110.0, 0.01);
        for (size_t i = 0; i < 5000; ++i)
            ob.processOrder({i, Side::Buy, OrderType::Limit, 100.0 + (i % 10), 10});
        for (size_t i = 0; i < 5000; ++i)
            ob.processOrder({i + 5000, Side::Sell, OrderType::Limit, 100.0 + (i % 10), 10});
    }
}
BENCHMARK(BM_Process10000Orders);
BENCHMARK_MAIN();
