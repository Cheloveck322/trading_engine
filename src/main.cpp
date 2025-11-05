#include "../include/spscqueue.hpp"
#include "../include/matching_engine.hpp"
#include <thread>
#include <iostream>
#include <chrono>

int main() 
{
    SPSCQueue<Order, 128> queue;
    MatchingEngine engine;

    // Producer: кладёт 20 ордеров
    auto producer1 = [&]() {
        for (int i = 0; i < 20; ++i) {
            Order order{
                static_cast<uint64_t>(i),
                (i % 2 == 0) ? Side::Buy : Side::Sell,
                OrderType::Limit,
                100.0 + (i % 5),
                10
            };
            while (!queue.push(order))
                std::this_thread::yield();  // ждём, если очередь полна
            std::cout << "[Producer] pushed order " << order.id << "\n";
        }
    };

    // Consumer: извлекает сразу batch из 5 ордеров и обрабатывает их
    auto consumer1 = [&]() {
        std::vector<Order> batch;
        batch.reserve(8); // немного запас

        for (int cycle = 0; cycle < 5; ++cycle) { // 5 итераций по ~4 ордера
            batch.clear();
            std::size_t count = queue.popBatch(batch, 8);

            if (count > 0) {
                std::cout << "\n[Consumer] Processing batch of " << count << " orders\n";
                engine.processBatchOrders(batch);
                for (const auto& o : batch)
                    std::cout << "  └─ processed order " << o.id << " (" 
                              << ((o.side == Side::Buy) ? "BUY" : "SELL") << ")\n";
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        std::cout << "\n[Consumer] Done processing.\n";
    };

    std::thread tProd(producer1);
    std::thread tCons(consumer1);

    tProd.join();
    tCons.join();

    std::cout << "\nAll orders processed successfully ✅\n";
    return 0;
}
