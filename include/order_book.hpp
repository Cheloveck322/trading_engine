#ifndef ORDER_BOOK_HPP
#define ORDER_BOOK_HPP

#include <deque>
#include <map>
#include <functional>
#include <optional>
#include "order.hpp"
#include "trade.hpp"
#include <vector>
#include <memory>

class OrderBook
{
public:
    OrderBook() = default;    
    void processOrder(Order order);
    void addOrder(const Order& order) noexcept;
    void printBook() const noexcept;
    const std::vector<Trade>& getTrades() const noexcept { return _trades; }
    const Order getLastOrder() const noexcept { return _lastOrder; };

private:
    std::map<double, std::deque<Order>, std::greater<>> _bids;
    std::map<double, std::deque<Order>, std::less<>> _asks;
    std::vector<Trade> _trades;
    Order _lastOrder;
};

#endif //ORDER_BOOK_HPP