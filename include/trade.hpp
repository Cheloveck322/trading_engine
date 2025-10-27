#ifndef TRADE_HPP
#define TRADE_HPP

#include <stdint.h>
#include <chrono>

struct Trade
{
    uint64_t buy_id;
    uint64_t sell_id;
    double price;
    uint64_t quantity;
    std::chrono::steady_clock::time_point timestamp;
};

#endif //TRADE_HPP