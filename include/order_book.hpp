#ifndef ORDER_BOOK_HPP
#define ORDER_BOOK_HPP

#include <deque>
#include <map>
#include <functional>
#include <optional>
#include "order.hpp"
#include "trade.hpp"
#include <vector>

class OrderBook
{
private:
    std::map<double, std::deque<Order>, std::greater<>> _bids;
    std::map<double, std::deque<Order>, std::less<>> _asks;
    std::vector<Trade> _trades;

public:
    OrderBook() = default;

    void processOrder(Order order) noexcept;
    void addOrder(const Order& order) noexcept;
    void printBook() const noexcept;
    const std::vector<Trade>& getTrades() const noexcept { return _trades; }
};

#endif //ORDER_BOOK_HPP