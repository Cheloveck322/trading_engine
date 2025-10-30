#include "../include/order_book.hpp"
#include "../include/matching_engine.hpp"
#include <iostream>

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    OrderBook ob;
    ob.processOrder({1, Side::Buy, OrderType::Limit, 100.0, 10});
    ob.processOrder({2, Side::Sell, OrderType::Limit, 100.0, 5});
    ob.processOrder({3, Side::Sell, OrderType::Limit, 101.0, 15});
    ob.processOrder({5, Side::Buy, OrderType::Limit, 100.0, 5});
    ob.processOrder({4, Side::Buy, OrderType::Limit, 101.0, 10});
    ob.printBook();

    MatchingEngine engine;

    engine.processOrder({1, Side::Sell, OrderType::Limit, 100.0, 10});
    engine.processOrder({2, Side::Buy, OrderType::Limit, 100.0, 15});
    engine.processOrder({3, Side::Buy, OrderType::Market, 0.0, 5});
    engine.printReports();

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Latency: "
          << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
          << " μs\n";
}