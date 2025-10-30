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
    std::chrono::steady_clock::time_point timestamp{ std::chrono::steady_clock::now() };

    Order() = default;
    Order(uint64_t _id, Side _side, OrderType _type, double _price, uint64_t _qty)
        : id{ _id }, side{ _side }, type{ _type }, price{ _price }, quantity{ _qty },
            timestamp{ std::chrono::steady_clock::now() }
    {
    } 
};

#endif //ORDER_HPP