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
    const std::shared_ptr<Order> getLastOrder() const noexcept { return _lastOrder; };

private:
    std::map<double, std::deque<std::shared_ptr<Order>>, std::greater<>> _bids;
    std::map<double, std::deque<std::shared_ptr<Order>>, std::less<>> _asks;
    std::vector<Trade> _trades;
    std::shared_ptr<Order> _lastOrder;
};

#endif //ORDER_BOOK_HPP