#ifndef ORDER_BOOK_HPP
#define ORDER_BOOK_HPP

#include <deque>
#include <map>
#include <functional>
#include <optional>
#include "order.hpp"
#include "trade.hpp"

class OrderBook
{
private:
    std::map<double, std::deque<Order>, std::greater<>> _bids;
    std::map<double, std::deque<Order>, std::less<>> _asks;

public:
    OrderBook() = default;

    std::optional<Trade> processOrder(const Order& order);
    void addOrder(const Order& order);
    void printBook() const;
};

#endif //ORDER_BOOK_HPP