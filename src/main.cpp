#include "../include/order_book.hpp"
#include <iostream>

int main() {
    OrderBook ob;
    ob.processOrder({1, Side::Buy, OrderType::Limit, 100.0, 10});
    ob.processOrder({2, Side::Sell, OrderType::Limit, 100.0, 5});
    ob.processOrder({3, Side::Sell, OrderType::Limit, 101.0, 15});

    ob.printBook();
}
