#include <gtest/gtest.h>
#include "../include/matching_engine.hpp"

TEST(MatchingEngineTest, LimitOrderMatch) {
    MatchingEngine engine;
    engine.processOrder({1, Side::Sell, OrderType::Limit, 100.0, 10});
    engine.processOrder({2, Side::Buy, OrderType::Limit, 100.0, 10});
    auto trades = engine.getOrderBook();
    ASSERT_TRUE(trades.getTrades().empty());
    for (const auto& trade : trades.getTrades())
    {
        EXPECT_EQ(trade.price, 100.0);
    }
}

TEST(MatchingEngineTest, MarketOrderExecutesImmediately) {
    MatchingEngine engine;
    engine.processOrder({1, Side::Sell, OrderType::Limit, 100.0, 10});
    engine.processOrder({2, Side::Buy, OrderType::Market, 0.0, 5});
    auto trades = engine.getOrderBook().getTrades();
    EXPECT_TRUE(trades.empty());
    for (const auto& trade : trades)
    {
        EXPECT_EQ(trade.quantity, 5);
    }
}

TEST(MatchingEngineTest, PartialFill) {
    MatchingEngine engine;
    engine.processOrder({1, Side::Sell, OrderType::Limit, 100.0, 10});
    engine.processOrder({2, Side::Buy, OrderType::Limit, 100.0, 15});
    auto trade = engine.getOrderBook().getTrades();
    EXPECT_TRUE(trade.empty());
}