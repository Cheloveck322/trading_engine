#include "../include/matching_engine.hpp"
#include <iostream>

void MatchingEngine::processOrder(Order order) noexcept
{
    ExecutionReport report;
    report.id = order.id;
    report.price = order.price;
    report.quantity = order.quantity;

    if (!_orderBook.getTrades().empty())
        report.status = (order.quantity == 0) ? "filled" : "partially_filled";
    else
        report.status = "accepted";

    _reports.push_back(report);
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