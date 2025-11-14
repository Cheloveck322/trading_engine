#include "../include/order_book.hpp"
#include <algorithm>
#include <iostream>

void OrderBook::processOrder(Order order)
{
    if (order.type == OrderType::Market)
    {
        if (order.side == Side::Buy)
        {
            auto bestAsk { getBestAskIndex() };
            while (bestAsk != SIZE_MAX && order.quantity > 0)
            {
                auto& queue{ _asks[bestAsk] };
                std::shared_ptr<Order> sellOrder{ queue.front() };

                uint64_t qty{ std::min(order.quantity, sellOrder->quantity) };
                Trade trade{ order.id, sellOrder->id, sellOrder->price, qty, std::chrono::steady_clock::now() };
                _trades.emplace_back(trade);
                
                if (_onTradeCallback) 
                    _onTradeCallback(trade);

                sellOrder->quantity -= qty;
                order.quantity -= qty;

                if (sellOrder->quantity == 0)
                    queue.pop_front();
                if (queue.empty())
                    bestAsk = getBestAskIndex();
            }
        }
        else if (order.side == Side::Sell)
        {
            auto bestBid { getBestBidIndex() };
            while (bestBid != SIZE_MAX && order.quantity > 0)
            {
                auto& queue{ _bids[bestBid] };
                std::shared_ptr<Order> buyOrder{ queue.front() };

                uint64_t qty{ std::min(order.quantity, buyOrder->quantity) };
                Trade trade{ buyOrder->id, order.id, buyOrder->price, qty, std::chrono::steady_clock::now() };
                _trades.emplace_back(trade);
                
                if (_onTradeCallback)
                    _onTradeCallback(trade);

                buyOrder->quantity -= qty;
                order.quantity -= qty;

                if (buyOrder->quantity == 0)
                    queue.pop_front();
                if (queue.empty())
                    bestBid = getBestBidIndex();
            }
        }
    }
    else if (order.type == OrderType::Limit)
    {
        if (order.side == Side::Buy)
        {
            auto bestAsk { getBestAskIndex() };
            if (bestAsk != SIZE_MAX  && order.price >= indexToPrice(bestAsk))
            {
                auto& queue{ _asks[bestAsk] };
                std::shared_ptr<Order> sellOrder{ queue.front() };

                uint64_t qty { std::min(sellOrder->quantity, order.quantity) };
                Trade trade{ order.id, sellOrder->id, sellOrder->price, qty, std::chrono::steady_clock::now() };
                _trades.emplace_back(trade);

                if (_onTradeCallback)
                    _onTradeCallback(trade);

                sellOrder->quantity -= qty;
                order.quantity -= qty;

                if (sellOrder->quantity == 0)
                    queue.pop_front();
                if (order.quantity > 0)
                    addOrder(order);
            }
            else
                addOrder(order);
        }
        else if (order.side == Side::Sell)
        {
            auto bestBid { getBestBidIndex() };
            if (bestBid != SIZE_MAX && order.price <= indexToPrice(bestBid))
            {
                auto& queue{ _bids[bestBid] };
                std::shared_ptr<Order> buyOrder{ queue.front() };
                
                uint64_t qty { std::min(buyOrder->quantity, order.quantity) };
                Trade trade{ buyOrder->id, order.id, buyOrder->price, qty, std::chrono::steady_clock::now() };
                _trades.emplace_back(trade);

                if (_onTradeCallback)
                    _onTradeCallback(trade);

                buyOrder->quantity -= qty;
                order.quantity -= qty;

                if (buyOrder->quantity == 0)
                    queue.pop_front();
                if (order.quantity > 0)
                    addOrder(order);
            }
            else 
                addOrder(order);
        }
    }
    _lastOrder = order;
}

void OrderBook::addOrder(const Order& order) noexcept
{
    std::size_t index{ priceToIndex(order.price) };

    if (order.side == Side::Buy)
        _bids[index].emplace_back(std::make_shared<Order>(order));
    else if (order.side == Side::Sell)
        _asks[index].emplace_back(std::make_shared<Order>(order));
}

void OrderBook::printBook() const noexcept
{
    std::cout << "--- Asks ---\n";
    for (size_t i = 0; i < _numPriceLevels; ++i)
    {
        if (!_asks[i].empty())
        {
            double price = indexToPrice(i);
            std::cout << price << " (" << _asks[i].size() << " orders)\n";
        }
    }

    std::cout << "--- Bids ---\n";
    for (size_t i = _numPriceLevels; i-- > 0;)
    {
        if (!_bids[i].empty())
        {
            double price = indexToPrice(i);
            std::cout << price << " (" << _bids[i].size() << " orders)\n";
        }
    }
    std::cout << std::endl;
}