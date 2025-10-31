#include "spscqueue.hpp"
#include <thread>
#include <iostream>
#include <chrono>

int main() {
    SPSCQueue<int, 4> q;

    std::thread producer([&]{
        for (int i = 0; i < 5; ++i) {
            while (!q.push(i))
                std::this_thread::yield();
            std::cout << "Produced: " << i << std::endl;
        }
    });

    std::thread consumer([&]{
        int item;
        for (int i = 0; i < 5; ++i) {
            while (!q.pop(item))
                std::this_thread::yield();
            std::cout << "Consumed: " << item << std::endl;
        }
    });

    producer.join();
    consumer.join();
}
