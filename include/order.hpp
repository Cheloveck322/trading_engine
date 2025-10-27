#ifndef ORDER_HPP
#define ORDER_HPP

#include <stdint.h>
#include <chrono>

enum class Side { Buy, Sell };
enum class OrderType { Limit, Market };

struct Order
{
    uint64_t id;
    Side side;
    OrderType type;
    double price;
    uint64_t quantity;
    std::chrono::steady_clock::time_point timestamp;
};

#endif //ORDER_HPP