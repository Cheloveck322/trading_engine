#include <gtest/gtest.h>
#include "../include/matching_engine.hpp"

// --- 1. Базовый сценарий: полное исполнение ---
TEST(MatchingEngineTest, SimpleFilled) {
    MatchingEngine engine;
    engine.processOrder({1, Side::Sell, OrderType::Limit, 100.0, 5});
    engine.processOrder({2, Side::Buy, OrderType::Limit, 100.0, 5});
    auto trade = engine.getOrderBook().getTrades();

    ASSERT_TRUE(!trade.empty());
    const auto& reports = engine.getReports();
    EXPECT_EQ(reports.back().status, "filled");
}

// --- 2. Частичное исполнение ---
TEST(MatchingEngineTest, PartialFill) {
    MatchingEngine engine;
    engine.processOrder({1, Side::Sell, OrderType::Limit, 100.0, 10});
    engine.processOrder({2, Side::Buy, OrderType::Limit, 100.0, 15});

    const auto& reports = engine.getReports();
    EXPECT_EQ(reports.back().status, "partially_filled");
}

// --- 3. Нет сделок (accepted) ---
TEST(MatchingEngineTest, NoMatchAccepted) {
    MatchingEngine engine;
    engine.processOrder({1, Side::Buy, OrderType::Limit, 100.0, 10});
    const auto& reports = engine.getReports();

    ASSERT_EQ(reports.back().status, "accepted");
}

// --- 4. Несколько сделок подряд ---
TEST(MatchingEngineTest, MultipleSequentialTrades) {
    MatchingEngine engine;
    engine.processOrder({1, Side::Sell, OrderType::Limit, 100.0, 5});
    engine.processOrder({2, Side::Sell, OrderType::Limit, 101.0, 5});
    engine.processOrder({3, Side::Buy,  OrderType::Limit, 101.0, 8});

    const auto& reports = engine.getReports();
    ASSERT_GE(reports.size(), 3);
}

// --- 5. Рыночный ордер (Market Buy) ---
TEST(MatchingEngineTest, MarketBuyOrderExecutesImmediately) {
    MatchingEngine engine;
    engine.processOrder({1, Side::Sell, OrderType::Limit, 100.0, 5});
    engine.processOrder({2, Side::Buy, OrderType::Market, 0.0, 5});
    auto trade = engine.getOrderBook().getTrades();

    ASSERT_TRUE(!trade.empty());
    const auto& reports = engine.getReports();
    EXPECT_EQ(reports.back().status, "filled");
}

// --- 6. Рыночный ордер без продавцов ---
TEST(MatchingEngineTest, MarketOrderWithoutCounterparty) {
    MatchingEngine engine;
    engine.processOrder({1, Side::Buy, OrderType::Market, 0.0, 5});
    auto trade = engine.getOrderBook().getTrades();
    EXPECT_FALSE(!trade.empty());
    const auto& reports = engine.getReports();
    EXPECT_EQ(reports.back().status, "accepted"); // не было сделок
}

// --- 7. Последовательность: Limit + Market ---
TEST(MatchingEngineTest, LimitThenMarketOrder) {
    MatchingEngine engine;
    engine.processOrder({1, Side::Sell, OrderType::Limit, 100.0, 10});
    engine.processOrder({2, Side::Sell, OrderType::Limit, 101.0, 10});
    engine.processOrder({3, Side::Buy, OrderType::Market, 0.0, 15});
    auto trade = engine.getOrderBook().getTrades();

    ASSERT_TRUE(!trade.empty());
    const auto& reports = engine.getReports();
    EXPECT_EQ(reports.back().status, "filled");
}

// --- 8. Проверка репортов ---
TEST(MatchingEngineTest, ExecutionReportLogging) {
    MatchingEngine engine;
    engine.processOrder({1, Side::Sell, OrderType::Limit, 100.0, 10});
    engine.processOrder({2, Side::Buy,  OrderType::Limit, 100.0, 10});
    const auto& reports = engine.getReports();

    ASSERT_GE(reports.size(), 2);
    EXPECT_EQ(reports[0].status, "accepted");
    EXPECT_EQ(reports.back().status, "filled");
}

// --- 9. Несколько уровней цен в одном тесте ---
TEST(MatchingEngineTest, MultiLevelMarketBuy) {
    MatchingEngine engine;
    engine.processOrder({1, Side::Sell, OrderType::Limit, 99.0, 10});
    engine.processOrder({2, Side::Sell, OrderType::Limit, 100.0, 10});
    engine.processOrder({3, Side::Sell, OrderType::Limit, 101.0, 10});
    engine.processOrder({4, Side::Buy, OrderType::Market, 0.0, 25});
    auto trade = engine.getOrderBook().getTrades();

    ASSERT_TRUE(!trade.empty());
    const auto& reports = engine.getReports();
    EXPECT_EQ(reports.back().status, "filled");
}
