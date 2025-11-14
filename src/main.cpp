#include "../include/matching_engine.hpp"
#include "../include/order_book.hpp"
#include "../include/spscqueue.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

// Вспомогательные функции для красивого вывода

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(70, '=') << "\n";
}

void printSeparator() {
    std::cout << std::string(70, '-') << "\n";
}

std::string orderTypeToString(OrderType type) {
    return (type == OrderType::Limit) ? "LIMIT" : "MARKET";
}

std::string sideToString(Side side) {
    return (side == Side::Buy) ? "BUY " : "SELL";
}

// Демонстрационные сценарии

// Сценарий 1: Простой матчинг лимитных ордеров
void demo1_SimpleMatching() {
    printHeader("СЦЕНАРИЙ 1: Простой матчинг лимитных ордеров");

    OrderBook book(90.0, 110.0, 0.01);

    // Настраиваем callback для отслеживания сделок
    book.setOnTradeCallback([](const Trade& trade) {
        std::cout << "  ✓ СДЕЛКА: "
                  << "Order #" << trade.buy_id
                  << " × Order #" << trade.sell_id
                  << " | " << trade.quantity << " акций @ $"
                  << std::fixed << std::setprecision(2) << trade.price << "\n";
    });

    std::cout << "\n1. Продавец выставляет ордер: SELL 100 акций @ $100.00\n";
    book.processOrder({1, Side::Sell, OrderType::Limit, 100.00, 100});

    std::cout << "\n2. Покупатель выставляет ордер: BUY 50 акций @ $100.00\n";
    book.processOrder({2, Side::Buy, OrderType::Limit, 100.00, 50});

    std::cout << "\n3. Ещё один покупатель: BUY 30 акций @ $100.00\n";
    book.processOrder({3, Side::Buy, OrderType::Limit, 100.00, 30});

    std::cout << "\nСостояние книги ордеров:\n";
    printSeparator();
    book.printBook();

    std::cout << "\nИтого сделок: " << book.getTrades().size() << "\n";
}

// Сценарий 2: Рыночные ордера
void demo2_MarketOrders() {
    printHeader("СЦЕНАРИЙ 2: Рыночные ордера");

    OrderBook book(90.0, 110.0, 0.01);

    book.setOnTradeCallback([](const Trade& trade) {
        std::cout << "  ✓ СДЕЛКА: " << trade.quantity << " @ $"
                  << std::fixed << std::setprecision(2) << trade.price << "\n";
    });

    std::cout << "\n1. Создаём стакан ордеров:\n";
    book.processOrder({1, Side::Sell, OrderType::Limit, 99.00, 10});
    std::cout << "   SELL 10 @ $99.00\n";

    book.processOrder({2, Side::Sell, OrderType::Limit, 100.00, 20});
    std::cout << "   SELL 20 @ $100.00\n";

    book.processOrder({3, Side::Sell, OrderType::Limit, 101.00, 15});
    std::cout << "   SELL 15 @ $101.00\n";

    std::cout << "\n2. Рыночный ордер на покупку 35 акций (по любой цене):\n";
    book.processOrder({4, Side::Buy, OrderType::Market, 0.0, 35});

    std::cout << "\nСостояние книги после market order:\n";
    printSeparator();
    book.printBook();
}

// Сценарий 3: Несколько ценовых уровней и FIFO
void demo3_MultipleLevelsAndFIFO() {
    printHeader("СЦЕНАРИЙ 3: Несколько уровней цен и FIFO порядок");

    OrderBook book(90.0, 110.0, 0.01);

    std::cout << "\n1. Три продавца на уровне $100.00 (FIFO порядок):\n";
    book.processOrder({1, Side::Sell, OrderType::Limit, 100.00, 10});
    std::cout << "   Seller #1: 10 акций\n";

    book.processOrder({2, Side::Sell, OrderType::Limit, 100.00, 15});
    std::cout << "   Seller #2: 15 акций\n";

    book.processOrder({3, Side::Sell, OrderType::Limit, 100.00, 20});
    std::cout << "   Seller #3: 20 акций\n";

    std::cout << "\n2. Два продавца на уровне $100.50:\n";
    book.processOrder({4, Side::Sell, OrderType::Limit, 100.50, 25});
    std::cout << "   Seller #4: 25 акций\n";

    book.processOrder({5, Side::Sell, OrderType::Limit, 100.50, 30});
    std::cout << "   Seller #5: 30 акций\n";

    std::cout << "\nСтакан перед покупкой:\n";
    printSeparator();
    book.printBook();

    std::cout << "\n3. Покупатель берёт 32 акции по лучшей цене ($100.00):\n";
    book.setOnTradeCallback([](const Trade& trade) {
        std::cout << "  ✓ Matched with Seller #" << trade.sell_id
                  << ": " << trade.quantity << " акций\n";
    });

    book.processOrder({6, Side::Buy, OrderType::Limit, 100.00, 32});

    std::cout << "\nСтакан после покупки (остались Seller #2, #3, #4, #5):\n";
    printSeparator();
    book.printBook();
}

// Сценарий 4: Частичное исполнение
void demo4_PartialFill() {
    printHeader("СЦЕНАРИЙ 4: Частичное исполнение ордеров");

    OrderBook book(90.0, 110.0, 0.01);

    std::cout << "\n1. Продавец выставляет 50 акций @ $100.00\n";
    book.processOrder({1, Side::Sell, OrderType::Limit, 100.00, 50});

    std::cout << "\n2. Покупатель хочет купить 100 акций @ $100.00\n";
    std::cout << "   (но в стакане только 50 - произойдёт частичное исполнение)\n";

    book.setOnTradeCallback([](const Trade& trade) {
        std::cout << "  ✓ Исполнено: " << trade.quantity << " акций\n";
    });

    book.processOrder({2, Side::Buy, OrderType::Limit, 100.00, 100});

    std::cout << "\nРезультат: исполнено 50, остальные 50 добавлены в книгу как BID\n";
    printSeparator();
    book.printBook();
}

// Сценарий 5: Использование MatchingEngine с отчетами
void demo5_MatchingEngine() {
    printHeader("СЦЕНАРИЙ 5: MatchingEngine с execution reports");

    MatchingEngine engine;

    std::cout << "\n1. Обрабатываем серию ордеров:\n";

    engine.processOrder({1, Side::Sell, OrderType::Limit, 100.00, 10});
    std::cout << "   Order #1: SELL 10 @ $100.00\n";

    engine.processOrder({2, Side::Sell, OrderType::Limit, 100.50, 5});
    std::cout << "   Order #2: SELL 5 @ $100.50\n";

    engine.processOrder({3, Side::Buy, OrderType::Limit, 100.00, 10});
    std::cout << "   Order #3: BUY 10 @ $100.00 (полное исполнение)\n";

    engine.processOrder({4, Side::Buy, OrderType::Limit, 99.50, 20});
    std::cout << "   Order #4: BUY 20 @ $99.50 (нет совпадений)\n";

    std::cout << "\n2. Execution Reports:\n";
    printSeparator();

    const auto& reports = engine.getReports();
    for (const auto& report : reports) {
        std::cout << "  Order #" << std::setw(2) << report.id
                  << " | Status: " << std::setw(16) << std::left << report.status
                  << " | Price: $" << std::fixed << std::setprecision(2)
                  << std::setw(7) << std::right << report.price
                  << " | Qty: " << report.quantity << "\n";
    }

    std::cout << "\n3. Состояние OrderBook:\n";
    printSeparator();
    engine.getOrderBook().printBook();

    std::cout << "\nВсего сделок: " << engine.getOrderBook().getTrades().size() << "\n";
}

// Сценарий 6: Реалистичная торговая сессия
void demo6_TradingSession() {
    printHeader("СЦЕНАРИЙ 6: Симуляция торговой сессии");

    OrderBook book(95.0, 105.0, 0.01);

    // Счётчики статистики
    size_t total_orders = 0;
    size_t total_volume = 0;

    book.setOnTradeCallback([&total_volume](const Trade& trade) {
        total_volume += trade.quantity;
    });

    std::cout << "\nНачинаем симуляцию торговой сессии...\n";
    std::cout << "Генерируем случайные ордера вокруг цены $100.00\n\n";

    auto start = std::chrono::high_resolution_clock::now();

    // Генерируем начальную ликвидность
    for (int i = 0; i < 10; ++i) {
        double price = 100.0 + (i - 5) * 0.1;  // $99.50 - $100.40
        book.processOrder({total_orders++, Side::Sell, OrderType::Limit, price, static_cast<uint64_t>(10 + i * 2)});
    }

    for (int i = 0; i < 10; ++i) {
        double price = 99.9 - i * 0.1;  // $99.90 - $99.00
        book.processOrder({total_orders++, Side::Buy, OrderType::Limit, price, static_cast<uint64_t>(10 + i * 2)});
    }

    // Симулируем активную торговлю
    std::vector<Order> trading_orders = {
        {total_orders++, Side::Buy, OrderType::Market, 0.0, 25},
        {total_orders++, Side::Buy, OrderType::Limit, 100.10, 50},
        {total_orders++, Side::Sell, OrderType::Market, 0.0, 30},
        {total_orders++, Side::Sell, OrderType::Limit, 99.90, 40},
        {total_orders++, Side::Buy, OrderType::Market, 0.0, 15},
    };

    for (const auto& order : trading_orders) {
        book.processOrder(order);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "\n📊 СТАТИСТИКА ТОРГОВОЙ СЕССИИ\n";
    printSeparator();
    std::cout << "  Всего ордеров:       " << total_orders << "\n";
    std::cout << "  Всего сделок:        " << book.getTrades().size() << "\n";
    std::cout << "  Объём торгов:        " << total_volume << " акций\n";
    std::cout << "  Время обработки:     " << duration.count() << " мкс\n";
    std::cout << "  Среднее на ордер:    "
              << std::fixed << std::setprecision(2)
              << (double)duration.count() / total_orders << " мкс\n";

    std::cout << "\nСостояние книги после сессии:\n";
    printSeparator();
    book.printBook();
}

// Сценарий 7: SPSC Queue и многопоточность
void demo7_SPSCMultithreading() {
    printHeader("СЦЕНАРИЙ 7: Lock-free SPSC Queue и многопоточность");

    std::cout << "\nДемонстрация работы с lock-free очередью:\n";
    std::cout << "  - Producer (поток 1): генерирует ордера и кладет в очередь\n";
    std::cout << "  - Consumer (поток 2): забирает ордера и обрабатывает\n\n";

    // Создаем SPSC очередь на 256 элементов
    SPSCQueue<Order, 256> orderQueue;
    MatchingEngine engine;

    std::atomic<bool> producerDone{false};
    std::atomic<size_t> ordersProduced{0};
    std::atomic<size_t> ordersConsumed{0};

    // PRODUCER: генерирует ордера
    auto producer = [&]() {
        std::cout << "[PRODUCER] Старт генерации ордеров...\n";

        // Создаем начальную ликвидность
        for (uint64_t i = 0; i < 20; ++i) {
            Order order{
                i,
                (i % 2 == 0) ? Side::Sell : Side::Buy,
                OrderType::Limit,
                100.0 + (i % 5) * 0.5,  // Цены: 100.0, 100.5, 101.0, 101.5, 102.0
                static_cast<uint64_t>(10 + (i % 3) * 5)
            };

            // Пытаемся положить в очередь
            while (!orderQueue.push(order)) {
                std::this_thread::yield();  // Ждем, если очередь полна
            }

            ordersProduced++;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        producerDone = true;
        std::cout << "[PRODUCER] Завершил работу. Сгенерировано ордеров: " << ordersProduced << "\n";
    };

    // CONSUMER: обрабатывает ордера батчами
    auto consumer = [&]() {
        std::cout << "[CONSUMER] Старт обработки ордеров...\n";

        std::vector<Order> batch;
        batch.reserve(10);

        while (!producerDone || ordersProduced != ordersConsumed) {
            batch.clear();
            size_t count = orderQueue.popBatch(batch, 10);

            if (count > 0) {
                std::cout << "[CONSUMER] Получен batch из " << count << " ордеров:\n";

                for (const auto& order : batch) {
                    engine.processOrder(order);
                    ordersConsumed++;

                    std::cout << "  └─ Order #" << order.id
                              << " | " << sideToString(order.side)
                              << " | " << orderTypeToString(order.type)
                              << " | $" << std::fixed << std::setprecision(2) << order.price
                              << " | Qty: " << order.quantity << "\n";
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }

        std::cout << "[CONSUMER] Завершил работу. Обработано ордеров: " << ordersConsumed << "\n";
    };

    auto startTime = std::chrono::high_resolution_clock::now();

    // Запускаем потоки
    std::thread producerThread(producer);
    std::thread consumerThread(consumer);

    // Ждем завершения
    producerThread.join();
    consumerThread.join();

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "\n📊 РЕЗУЛЬТАТЫ МНОГОПОТОЧНОЙ ОБРАБОТКИ\n";
    printSeparator();
    std::cout << "  Сгенерировано ордеров:    " << ordersProduced << "\n";
    std::cout << "  Обработано ордеров:       " << ordersConsumed << "\n";
    std::cout << "  Всего сделок:             " << engine.getOrderBook().getTrades().size() << "\n";
    std::cout << "  Время выполнения:         " << duration.count() << " мс\n";
    std::cout << "  Throughput:               "
              << std::fixed << std::setprecision(1)
              << (ordersConsumed.load() * 1000.0 / duration.count()) << " ордеров/сек\n";

    std::cout << "\nСостояние OrderBook после многопоточной обработки:\n";
    printSeparator();
    engine.getOrderBook().printBook();

    std::cout << "\nExecution Reports (последние 5):\n";
    printSeparator();
    const auto& reports = engine.getReports();
    size_t start = reports.size() > 5 ? reports.size() - 5 : 0;
    for (size_t i = start; i < reports.size(); ++i) {
        const auto& report = reports[i];
        std::cout << "  Order #" << std::setw(2) << report.id
                  << " | " << std::setw(16) << std::left << report.status << std::right
                  << " | $" << std::fixed << std::setprecision(2) << std::setw(7) << report.price
                  << " | Qty: " << report.quantity << "\n";
    }
}

// Главное меню

void showMenu() {
    std::cout << "\n";
    std::cout << "\n  Выберите сценарий:\n\n";
    std::cout << "    1. Простой матчинг лимитных ордеров\n";
    std::cout << "    2. Рыночные ордера (Market orders)\n";
    std::cout << "    3. Несколько ценовых уровней и FIFO\n";
    std::cout << "    4. Частичное исполнение ордеров\n";
    std::cout << "    5. MatchingEngine с execution reports\n";
    std::cout << "    6. Симуляция торговой сессии (с метриками)\n";
    std::cout << "    7. SPSC Queue + многопоточность (Producer/Consumer)\n";
    std::cout << "    0. Выход\n";
    std::cout << "\n  Ввод: ";
}

int main() {
    while (true) {
        showMenu();

        int choice;
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "\nНекорректный ввод! Введите число от 0 до 7.\n";
            continue;
        }

        switch (choice) {
            case 1:
                demo1_SimpleMatching();
                break;
            case 2:
                demo2_MarketOrders();
                break;
            case 3:
                demo3_MultipleLevelsAndFIFO();
                break;
            case 4:
                demo4_PartialFill();
                break;
            case 5:
                demo5_MatchingEngine();
                break;
            case 6:
                demo6_TradingSession();
                break;
            case 7:
                demo7_SPSCMultithreading();
                break;
            case 0:
                std::cout << "\nДо свидания!\n\n";
                return 0;
            default:
                std::cout << "\nНеверный выбор! Введите число от 0 до 7.\n";
        }

        std::cout << "\n\nНажмите Enter для продолжения...";
        std::cin.ignore(10000, '\n');
        std::cin.get();
    }

    return 0;
}
