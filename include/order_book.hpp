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
#include <functional>

class OrderBook
{
public:  
    OrderBook(double minPrice = 0.0, double maxPrice = 10000.0, double tickSize = 0.01)
        : _minPrice{ minPrice }, _maxPrice{ maxPrice }, _tickSize{ tickSize }
    {
        _numPriceLevels = static_cast<size_t>((maxPrice - minPrice) / tickSize) + 1;
        _bids.resize(_numPriceLevels);
        _asks.resize(_numPriceLevels);
    }

    void processOrder(Order order);
    void addOrder(const Order& order) noexcept;
    void printBook() const noexcept;
    const std::vector<Trade>& getTrades() const noexcept { return _trades; }
    const Order getLastOrder() const noexcept { return _lastOrder; };
    void setOnTradeCallback(std::function<void(const Trade&)> callback) {
        _onTradeCallback = std::move(callback);
    }

private:
    inline size_t priceToIndex(double price) const noexcept {
        return static_cast<size_t>((price - _minPrice) / _tickSize);
    }
    
    inline double indexToPrice(size_t index) const noexcept {
        return _minPrice + (index * _tickSize);
    }

    // Find best bid (highest price with orders)
    inline size_t getBestBidIndex() const noexcept {
        for (size_t i = _numPriceLevels; i-- > 0;) {
            if (!_bids[i].empty()) return i;
        }
        return SIZE_MAX;
    }

    // Find best ask (lowest price with orders)
    inline size_t getBestAskIndex() const noexcept {
        for (size_t i = 0; i < _numPriceLevels; ++i) {
            if (!_asks[i].empty()) return i;
        }
        return SIZE_MAX;
    }
    
    double _minPrice;
    double _maxPrice;
    double _tickSize;
    std::size_t _numPriceLevels;

    std::vector<std::deque<std::shared_ptr<Order>>> _bids;
    std::vector<std::deque<std::shared_ptr<Order>>> _asks;
    std::vector<Trade> _trades;
    Order _lastOrder;
    std::function<void(const Trade&)> _onTradeCallback;
};

#endif //ORDER_BOOK_HPP