#include "../include/order_book.hpp"
#include <algorithm>
#include <iostream>

std::optional<Trade> OrderBook::processOrder(Order order)
{
    if (order.side == Side::Buy)
    {
        auto bestAsk { _asks.begin() };
        if (bestAsk != _asks.end() && order.price >= bestAsk->first)
        {
            auto& queue{ bestAsk->second };
            Order& sellOrder{ queue.front() };

            uint64_t qty { std::min(sellOrder.quantity, order.quantity) };
            Trade trade{ order.id, sellOrder.id, sellOrder.price, qty, std::chrono::steady_clock::now() };

            sellOrder.quantity -= qty;
            order.quantity -= qty;

            if (sellOrder.quantity == 0)
                queue.pop_front();
            if (queue.empty())
                _asks.erase(bestAsk);

            if (order.quantity > 0)
                addOrder(order);

            return trade;
        }
        else 
        {
            addOrder(order);
        }
    }
    else if (order.side == Side::Sell)
    {
        auto bestBid { _bids.begin() };

        if (bestBid != _bids.end() && order.price <= bestBid->first)
        {
            auto& queue{ bestBid->second };
            Order& buyOrder{ queue.front() };
            
            uint64_t qty { std::min(buyOrder.quantity, order.quantity) };
            Trade trade{ buyOrder.id, order.id, buyOrder.price, qty, std::chrono::steady_clock::now() };

            buyOrder.quantity -= qty;
            order.quantity -= qty;

            if (buyOrder.quantity == 0)
                queue.pop_front();
            if (queue.empty())
                _bids.erase(bestBid);

            if (order.quantity > 0)
                addOrder(order);

            return trade;
        }
        else 
        {
            addOrder(order);
        }
    }

    return std::nullopt;
}

void OrderBook::addOrder(const Order& order)
{
    if (order.side == Side::Buy)
        _bids[order.price].push_back(order);
    else if (order.side == Side::Sell)
        _asks[order.price].push_back(order);
}

void OrderBook::printBook() const 
{
    std::cout << "--- Asks ---\n";
    for (auto& [price, queue] : _asks)
        std::cout << price << " (" << queue.size() << " orders)\n";

    std::cout << "--- Bids ---\n";
    for (auto& [price, queue] : _bids)
        std::cout << price << " (" << queue.size() << " orders)\n";
}