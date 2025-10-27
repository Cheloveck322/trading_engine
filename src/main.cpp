#include "../include/order_book.hpp"

int main()
{
    OrderBook order_book;
    order_book.processOrder({ 00, Side::Buy, OrderType::Limit, 99.9, 5 });

    order_book.printBook();
    order_book.processOrder({ 1, Side::Sell, OrderType::Limit, 100.0, 10 });
    order_book.processOrder({ 5, Side::Sell, OrderType::Limit, 90.0, 15 });

    order_book.printBook();
    order_book.processOrder({ 6, Side::Buy, OrderType::Limit, 110.0, 20 });
    order_book.processOrder({ 2, Side::Sell, OrderType::Limit, 15.0, 50 });
    order_book.processOrder({ 10, Side::Buy, OrderType::Limit, 256.0, 2 });

    order_book.printBook();
    order_book.processOrder({ 3, Side::Buy, OrderType::Limit, 99.9, 5 });
    order_book.processOrder({ 9, Side::Sell, OrderType::Limit, 999.9, 8 });
    order_book.processOrder({ 4, Side::Buy, OrderType::Limit, 150.0, 1100 });
    order_book.processOrder({ 11, Side::Buy, OrderType::Limit, 10, 500 });

    order_book.printBook();

    return 0;
}