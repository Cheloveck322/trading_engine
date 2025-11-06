#include "../include/matching_engine.hpp"
#include <iostream>

MatchingEngine::MatchingEngine():
    _orderBook{}, _logger{ "trades.log" }, _reports{}
{
    _orderBook.setOnTradeCallback([this](const Trade& t) {
        _logger.log("TRADE ",
                    t.buy_id, "->", t.sell_id,
                    " qty=", t.quantity,
                    " price=", t.price);
    });
}

void MatchingEngine::processOrder(Order order) noexcept
{
    ExecutionReport report;
    report.id = order.id;
    report.price = order.price;
    report.quantity = order.quantity;

    auto start = std::chrono::high_resolution_clock::now();
    _orderBook.processOrder(order);
    auto end = std::chrono::high_resolution_clock::now();

    _metrics.processed_orders++;
    double latency = std::chrono::duration<double, std::micro>(end - start).count();
    _metrics.avg_latency_us += (latency - _metrics.avg_latency_us) / _metrics.processed_orders;

    if (!_orderBook.getTrades().empty())
    {
        report.status = (_orderBook.getLastOrder().quantity == 0) ? "filled" : "partially_filled";
        _metrics.executed_trades = _orderBook.getTrades().size();
    }
    else
    {
        report.status = "accepted";
    }
    _reports.emplace_back(report);
}

void MatchingEngine::printReports() const noexcept
{
    std::cout << "--- Execution Reports ---\n";
    for (const auto& r : _reports) {
        std::cout << "Order " << r.id
                  << " [" << r.status << "] qty=" << r.quantity
                  << " @ " << r.price << std::endl;
    }
}

void MatchingEngine::processBatchOrders(const std::vector<Order>& orders)
{
    for (const auto& order : orders)
    {
        processOrder(order);
    }
}