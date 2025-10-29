#include <gtest/gtest.h>
#include "../include/order_book.hpp"

TEST(OrderBookTest, SimpleMatch)
{
    OrderBook ob{};
    ob.processOrder({1, Side::Sell, OrderType::Limit, 100.0, 10});
    auto trades = ob.getTrades();
    ob.processOrder({2, Side::Buy, OrderType::Limit, 100.0, 10});

    ASSERT_TRUE(!trades.empty());
    for (const auto& trade : trades)
    {
        EXPECT_EQ(trade.price, 100.0);
        EXPECT_EQ(trade.quantity, 10);
    }
}

TEST(OrderBookTest, PartialFill) {
    OrderBook ob;
    ob.processOrder({1, Side::Sell, OrderType::Limit, 100.0, 10});
    auto trades = ob.getTrades();
    ob.processOrder({2, Side::Buy, OrderType::Limit, 100.0, 15});

    ASSERT_TRUE(!trades.empty());
    for (const auto& trade : trades)
    {
        EXPECT_EQ(trade.quantity, 10);
    }
}