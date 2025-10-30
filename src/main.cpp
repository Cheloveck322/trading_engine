#include "../include/matching_engine.hpp"
#include "../include/mpmc_queue.hpp"
#include <thread>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>

int main() {
    constexpr size_t QUEUE_SIZE = 1024;
    constexpr int PRODUCERS = 4;
    constexpr int CONSUMERS = 2;
    constexpr int ORDERS_PER_PRODUCER = 500;

    MPMCqueue<Order, QUEUE_SIZE> queue;
    MatchingEngine engine;

    auto producer = [&](int id) {
        std::mt19937 gen(std::random_device{}());
        std::uniform_real_distribution<double> price(99.0, 101.0);
        std::uniform_int_distribution<int> sideDist(0, 1);

        for (int i = 0; i < ORDERS_PER_PRODUCER; ++i) {
            Order o{
                static_cast<uint64_t>(id * 10000 + i),
                sideDist(gen) == 0 ? Side::Buy : Side::Sell,
                OrderType::Limit,
                price(gen),
                10
            };
            while (!queue.push(o))
                std::this_thread::yield();
        }
    };

    auto consumer = [&]() {
        while (true) {
            auto item = queue.pop();
            if (item.has_value()) {
                engine.processOrder(*item);
            } else {
                std::this_thread::sleep_for(std::chrono::microseconds(50));
            }
        }
    };

    std::vector<std::thread> producers;
    for (int i = 0; i < PRODUCERS; ++i)
        producers.emplace_back(producer, i);

    std::vector<std::thread> consumers;
    for (int i = 0; i < CONSUMERS; ++i)
        consumers.emplace_back(consumer);

    for (auto& p : producers) p.join();

    // ждём, пока очередь опустеет
    while (queue.pop().has_value())
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    for (auto& c : consumers) c.detach();

    std::cout << "✅ All orders processed successfully!" << std::endl;
    engine.printReports();
}
