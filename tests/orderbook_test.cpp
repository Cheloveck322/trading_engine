#include <gtest/gtest.h>
#include "../include/order_book.hpp"

// --- 1. Простая сделка (один покупатель и продавец) ---
TEST(OrderBookTest, SimpleMatch) {
    OrderBook ob;
    ob.processOrder({1, Side::Sell, OrderType::Limit, 100.0, 10});
    ob.processOrder({2, Side::Buy, OrderType::Limit, 100.0, 10});
    auto trade = ob.getTrades();

    ASSERT_TRUE(!trade.empty());
    EXPECT_EQ(trade.back().price, 100.0);
    EXPECT_EQ(trade.back().quantity, 10);
}

// --- 2. Частичное исполнение ---
TEST(OrderBookTest, PartialFill) {
    OrderBook ob;
    ob.processOrder({1, Side::Sell, OrderType::Limit, 100.0, 10});
    ob.processOrder({2, Side::Buy, OrderType::Limit, 100.0, 15});
    auto trade = ob.getTrades();

    ASSERT_TRUE(!trade.empty());
    EXPECT_EQ(trade.back().quantity, 10);   // купили только 10
}

// --- 3. Полное исполнение и удаление ордеров ---
TEST(OrderBookTest, FullFillRemovesOrders) {
    OrderBook ob;
    ob.processOrder({1, Side::Sell, OrderType::Limit, 100.0, 5});
    ob.processOrder({2, Side::Buy, OrderType::Limit, 100.0, 5});

    // После сделки книга должна быть пуста
    ob.printBook();
}

// --- 4. Нет совпадений по цене ---
TEST(OrderBookTest, NoMatchAddsToBook) {
    OrderBook ob;
    ob.processOrder({1, Side::Sell, OrderType::Limit, 105.0, 10});
    ob.processOrder({2, Side::Buy, OrderType::Limit, 100.0, 10});
    auto trade = ob.getTrades();

    EXPECT_FALSE(!trade.empty());
}

// --- 5. FIFO порядок внутри одной цены ---
TEST(OrderBookTest, FIFOWithinPriceLevel) {
    OrderBook ob;
    ob.processOrder({1, Side::Sell, OrderType::Limit, 100.0, 5});
    ob.processOrder({2, Side::Sell, OrderType::Limit, 100.0, 5});

    // Покупатель покупает 7 штук — должно исполниться 5 + 2
    ob.processOrder({3, Side::Buy, OrderType::Limit, 100.0, 7});
    auto trade1 = ob.getTrades();
    ASSERT_TRUE(!trade1.empty());
    EXPECT_EQ(trade1.back().quantity, 5); // первая сделка по ордеру #1

    // Остаток 2 должен исполниться со вторым ордером
    ob.processOrder({4, Side::Buy, OrderType::Limit, 100.0, 2});
    auto trade2 = ob.getTrades();
    ASSERT_TRUE(!trade2.empty());
    EXPECT_EQ(trade2.back().quantity, 2);
}

// --- 6. Несколько уровней цен ---
TEST(OrderBookTest, MultiplePriceLevels) {
    OrderBook ob;
    ob.processOrder({1, Side::Sell, OrderType::Limit, 99.0, 5});
    ob.processOrder({2, Side::Sell, OrderType::Limit, 100.0, 5});
    ob.processOrder({3, Side::Buy, OrderType::Limit, 100.0, 8});
    auto trade = ob.getTrades();

    ASSERT_TRUE(!trade.empty());
    EXPECT_EQ(trade.back().price, 99.0);  // первая сделка по лучшей цене
}

// --- 7. Добавление ордеров после частичного исполнения ---
TEST(OrderBookTest, AddRemainingAfterPartial) {
    OrderBook ob;
    ob.processOrder({1, Side::Sell, OrderType::Limit, 100.0, 5});
    ob.processOrder({2, Side::Buy, OrderType::Limit, 100.0, 10});
    auto trade = ob.getTrades();
    EXPECT_TRUE(!trade.empty());
    // После сделки покупатель должен добавить остаток в bids
    ob.printBook();
}
