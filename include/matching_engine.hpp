#ifndef MATCHING_ENGINE
#define MATCHING_ENGINE

#include "order_book.hpp"
#include <string>

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
    MatchingEngine() = default;
    void processOrder(Order order) noexcept;
    const auto& getReports() const noexcept { return _reports; }
    const auto& getOrderBook() const noexcept { return _orderBook; }
    void printReports() const noexcept;

private:
    OrderBook _orderBook;
    std::vector<ExecutionReport> _reports;
};


#endif // MATCHING_ENGINE