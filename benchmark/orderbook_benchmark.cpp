#include <benchmark/benchmark.h>
#include "../include/order_book.hpp"

static void BM_ProcessOrders(benchmark::State& state) {
    OrderBook ob;
    for (auto _ : state) {
        for (uint64_t i = 0; i < 1000; ++i) {
            ob.processOrder({i, Side::Buy, OrderType::Limit, 100.0 + (i % 5), 10});
            ob.processOrder({i + 1, Side::Sell, OrderType::Limit, 100.0 + (i % 5), 10});
        }
    }
}
BENCHMARK(BM_ProcessOrders);
BENCHMARK_MAIN();
