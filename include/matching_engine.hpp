#ifndef MATCHING_ENGINE
#define MATCHING_ENGINE

#include "order_book.hpp"
#include <string>
#include "logger.hpp"

struct ExecutionReport
{
    uint64_t id;
    std::string status; // accepted, filled, partially_filled, rejected
    double price;
    uint64_t quantity; 
};

class MatchingEngine
{
public:
    MatchingEngine();
    void processOrder(Order order) noexcept;
    void processBatchOrders(const std::vector<Order>& orders);
    const auto& getReports() const noexcept { return _reports; }
    const auto& getOrderBook() const noexcept { return _orderBook; }
    void printReports() const noexcept;


private:
    OrderBook _orderBook;
    Logger _logger{ "trades.log" };
    std::vector<ExecutionReport> _reports;
};


#endif // MATCHING_ENGINE