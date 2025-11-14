# Trading Engine

**Высокопроизводительный движок для симуляции биржевой торговли на C++17**

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)]()
[![Performance](https://img.shields.io/badge/throughput-118K_orders%2Fs-orange)]()
[![License](https://img.shields.io/badge/license-MIT-blue)]()

## Описание

Trading Engine — это высокопроизводительный симулятор биржевого движка с поддержкой лимитных и рыночных ордеров. Проект демонстрирует применение современных техник оптимизации C++ для достижения **микросекундной латентности** обработки ордеров.

### Ключевые особенности

- **Экстремальная производительность**: ~118,000 ордеров/сек (8.4 мкс на ордер)
- **O(1) доступ к ценовым уровням**: прямая индексация массива вместо `std::map`
- **Lock-free SPSC Queue**: неблокирующая очередь для многопоточности
- **Два типа ордеров**: `Limit` и `Market`
- **FIFO матчинг**: строгий порядок исполнения внутри ценового уровня
- **Trade callbacks**: события совершения сделок в реальном времени
- **Логирование**: запись всех сделок в файл
- **16 unit-тестов**: полное покрытие на Google Test

### Результаты оптимизации

**До** (с `std::map`): 87 секунд на 10,000 ордеров
**После** (с массивами): **84 миллисекунды** на 10,000 ордеров
**Ускорение: 1,041x**

## Архитектура

```
┌─────────────────────────────────────────────────────────────┐
│                   Matching Engine                           │
│  ┌──────────────────────────────────────────────────────┐   │
│  │                  Order Book                          │   │
│  │  ┌────────────────────┬────────────────────┐         │   │
│  │  │    BIDS (массив)   │   ASKS (массив)    │         │   │
│  │  │                    │                    │         │   │
│  │  │  Index  Price      │  Index  Price      │         │   │
│  │  │  ─────────────     │  ─────────────     │         │   │
│  │  │  [1002] $100.02 →  │  [1003] $100.03 →  │         │   │
│  │  │         deque<>    │         deque<>    │         │   │
│  │  │  [1001] $100.01 →  │  [1004] $100.04 →  │         │   │
│  │  │         deque<>    │         deque<>    │         │   │
│  │  │  [1000] $100.00 →  │  [1005] $100.05 →  │         │   │
│  │  │         deque<>    │         deque<>    │         │   │
│  │  │    ...             │    ...             │         │   │
│  │  └────────────────────┴────────────────────┘         │   │
│  │                                                      │   │
│  │  Прямая индексация: price → index за O(1)            │   │
│  │  index = (price - minPrice) / tickSize               │   │
│  └──────────────────────────────────────────────────────┘   │
│                                                             │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  Logger  →  logs/trades.log                          │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
                           ▲
                           │
                  ┌────────┴─────────┐
                  │  SPSC Queue      │
                  │  (lock-free)     │
                  └──────────────────┘
```

## Инновационная структура данных

### Проблема с `std::map`

Предыдущая моя реализация была через использование `std::map<double, deque<Order>>`:

```cpp
// Медленный подход
std::map<double, std::deque<Order>> _bids;

// Поиск цены: O(log n) - обход красно-черного дерева
auto it = _bids.find(100.50);
```

**Недостатки**:
- O(log n) для поиска, вставки, удаления
- Плохая кэш-локальность (узлы разбросаны по памяти)
- Аллокации для каждого узла дерева

### Решение: Array-based Order Book

Используем прямую индексацию массива:

```cpp
// Быстрый подход
std::vector<std::deque<std::shared_ptr<Order>>> _bids;  // Массив всех цен

inline size_t priceToIndex(double price) const noexcept {
    return static_cast<size_t>((price - _minPrice) / _tickSize);
}

// Поиск цены: O(1) - прямой доступ!
size_t idx = priceToIndex(100.50);
auto& orders = _bids[idx];
```

**Преимущества**:
- **O(1)** для всех операций с ценовым уровнем
- Отличная кэш-локальность
- Предсказуемая производительность
- Нет динамических аллокаций для индексации

**Компромиссы**:
- Требуется заранее знать диапазон цен
- Использует память для всех возможных уровней
- Пример: $90-$110 с шагом $0.01 = 2,001 уровень ≈ 320 KB

## Производительность

### Бенчмарк результаты

```bash
$ ./build/benchmark_orderbook
----------------------------------------------------------------
Benchmark                      Time             CPU   Iterations
----------------------------------------------------------------
BM_Process10000Orders   84272046 ns     84268433 ns            7
```

**Детальные метрики**:

| Метрика | Значение |
|---------|----------|
| **Throughput** | ~118,700 ордеров/сек |
| **Latency (avg)** | ~8.4 микросекунд/ордер |
| **Memory usage** | ~320 KB (для $90-$110) |
| **Iterations** | 7 (быстро для повторов) |

### Сравнение производительности

| Операция | `std::map` | Array-based | Speedup |
|----------|-----------|-------------|---------|
| Добавление ордера | O(log n) | **O(1)** | 10-100x |
| Поиск уровня | O(log n) | **O(1)** | 10-100x |
| Получение best bid/ask | O(1) | O(k)* | ~1x |
| Матчинг 10K ордеров | ~87,000 мс | **84 мс** | **1,041x** |

\* k = количество пустых уровней (обычно мало возле best prices)

## Сборка проекта

### Системные требования

- **Компилятор**: GCC 7+ / Clang 6+ / MSVC 2017+
- **CMake**: ≥ 3.10
- **C++ Standard**: C++17 или выше
- **Зависимости**:
  - Google Test (для тестов)
  - Google Benchmark (для бенчмарков)

### Установка зависимостей

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install cmake build-essential
sudo apt install libgtest-dev libbenchmark-dev
```

#### macOS
```bash
brew install cmake
brew install google-benchmark googletest
```

#### Arch Linux
```bash
sudo pacman -S cmake gcc benchmark gtest
```

### Компиляция

```bash
# Debug build (для разработки)
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# Release build (для бенчмарков, РЕКОМЕНДУЕТСЯ!)
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Release с максимальными оптимизациями
cmake -B build -S . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -flto"
cmake --build build
```

### Запуск

```bash
# Основное приложение
./build/trading_engine

# Unit-тесты
./build/test_orderbook    # 7 тестов для OrderBook
./build/test_engine       # 9 тестов для MatchingEngine

# Бенчмарки производительности
./build/benchmark_orderbook
```

## Интерактивная демонстрация

При запуске `./build/trading_engine` открывается интерактивное меню с **7 сценариями** использования:

```

  Выберите сценарий:

    1. Простой матчинг лимитных ордеров
    2. Рыночные ордера (Market orders)
    3. Несколько ценовых уровней и FIFO
    4. Частичное исполнение ордеров
    5. MatchingEngine с execution reports
    6. Симуляция торговой сессии (с метриками)
    7. SPSC Queue + многопоточность (Producer/Consumer)
    0. Выход

  Ввод:
```

### Описание сценариев

| № | Сценарий | Описание |
|---|----------|----------|
| **1** | Простой матчинг | Базовая демонстрация сопоставления buy/sell ордеров |
| **2** | Рыночные ордера | Market orders с мгновенным исполнением по лучшей цене |
| **3** | Множество уровней | FIFO матчинг с несколькими ценовыми уровнями |
| **4** | Частичное исполнение | Разделение больших ордеров на части |
| **5** | Execution Reports | Отчеты о статусе ордеров (accepted/filled/rejected) |
| **6** | Торговая сессия | Симуляция реальной сессии с метриками производительности |
| **7** | Многопоточность | Lock-free SPSC Queue с Producer/Consumer паттерном |

### Пример вывода

**Сценарий 1: Простой матчинг**

```
======================================================================
  СЦЕНАРИЙ 1: Простой матчинг лимитных ордеров
======================================================================

1. Продавец выставляет ордер: SELL 100 акций @ $100.00

2. Покупатель выставляет ордер: BUY 50 акций @ $100.00
  ✓ СДЕЛКА: Order #2 × Order #1 | 50 акций @ $100.00

3. Ещё один покупатель: BUY 30 акций @ $100.00
  ✓ СДЕЛКА: Order #3 × Order #1 | 30 акций @ $100.00

Состояние книги ордеров:
----------------------------------------------------------------------
--- Asks ---
100.00 (1 orders)
--- Bids ---

Итого сделок: 2
```

## Тестирование

Проект включает **16 unit-тестов** на Google Test с полным покрытием функциональности.

### OrderBook Tests (7 тестов)

```bash
$ ./build/test_orderbook

[==========] Running 7 tests from 1 test suite.
[----------] 7 tests from OrderBookTest
[ RUN      ] OrderBookTest.SimpleMatch
[       OK ] OrderBookTest.SimpleMatch (1050 ms)
[ RUN      ] OrderBookTest.PartialFill
[       OK ] OrderBookTest.PartialFill (924 ms)
[ RUN      ] OrderBookTest.FullFillRemovesOrders
[       OK ] OrderBookTest.FullFillRemovesOrders (940 ms)
[ RUN      ] OrderBookTest.NoMatchAddsToBook
[       OK ] OrderBookTest.NoMatchAddsToBook (907 ms)
[ RUN      ] OrderBookTest.FIFOWithinPriceLevel
[       OK ] OrderBookTest.FIFOWithinPriceLevel (876 ms)
[ RUN      ] OrderBookTest.MultiplePriceLevels
[       OK ] OrderBookTest.MultiplePriceLevels (717 ms)
[ RUN      ] OrderBookTest.AddRemainingAfterPartial
[       OK ] OrderBookTest.AddRemainingAfterPartial (726 ms)
[----------] 7 tests from OrderBookTest (6144 ms total)

[  PASSED  ] 7 tests.
```

**Покрытие**:
- Простое сопоставление buy/sell
- Частичное исполнение ордеров
- Полное исполнение и удаление из книги
- Отсутствие совпадений по цене
- FIFO порядок внутри ценового уровня
- Несколько ценовых уровней
- Добавление остатка после частичного исполнения

### MatchingEngine Tests (9 тестов)

```bash
$ ./build/test_engine

[==========] Running 9 tests from 1 test suite.
[  PASSED  ] 9 tests.
```

**Покрытие**:
- Полное исполнение лимитного ордера
- Частичное исполнение
- Принятие без матчинга
- Последовательные сделки
- Немедленное исполнение рыночного ордера
- Рыночный ордер без контрагента
- Комбинация limit + market ордеров
- Логирование отчетов об исполнении
- Многоуровневый market buy

## Roadmap

### Планируемые улучшения

- [ ] **Кэширование best bid/ask** — избежать O(k) поиска при каждом матчинге
- [ ] **Stop-loss ордера** — добавить условные ордера
- [ ] **Order cancellation** — отмена активных ордеров по ID
- [ ] **Depth of Market (DOM)** — агрегированная глубина рынка (L2 data)
- [ ] **Multiple symbols** — поддержка нескольких торговых инструментов
- [ ] **Market data feed** — стрим котировок через WebSocket
- [ ] **Persistence layer** — сохранение состояния OrderBook на диск
- [ ] **Time-in-Force** — поддержка GTC, IOC, FOK ордеров
- [ ] **Iceberg orders** — скрытые ордера большого объема
- [ ] **REST API** — HTTP интерфейс для внешних клиентов

### Альтернативные подходы к Order Book

| Подход | Сложность операций | Память | Use case |
|--------|-------------------|--------|----------|
| **Array-based** (текущий) | O(1) insert, O(k) best | Dense | Известный диапазон цен |
| **std::map** | O(log n) insert, O(1) best | Sparse | Произвольные цены |
| **Sorted vector** | O(n) insert, O(1) best | Compact | Редкие обновления |
| **Hybrid** (array + map) | O(1) hot, O(log n) cold | Medium | Оптимизация популярных уровней |

## Оптимизация для production

### Compile flags для максимальной скорости

```bash
cmake -B build -S . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -flto -DNDEBUG"
cmake --build build
```

**Объяснение флагов**:
- `-O3` — максимальные оптимизации компилятора
- `-march=native` — использовать инструкции процессора (AVX, SSE)
- `-flto` — Link-Time Optimization (межмодульная оптимизация)
- `-DNDEBUG` — отключить assert'ы

### Профилирование с perf (Linux)

```bash
# Записать профиль
perf record -g ./build/benchmark_orderbook

# Показать hotspots
perf report

# Анализ кэш-промахов
perf stat -e cache-references,cache-misses ./build/benchmark_orderbook
```

### Memory pooling для снижения аллокаций

```cpp
// Использовать кастомный аллокатор для deque
#include <boost/pool/pool_alloc.hpp>

std::deque<std::shared_ptr<Order>,
           boost::pool_allocator<std::shared_ptr<Order>>> orders;
```

## Лицензия

Этот проект распространяется под лицензией MIT. См. файл `LICENSE` для деталей.

## Автор

**Grigorev Vasily (cheloveck)**

- GitHub: [@cheloveck322](https://github.com/cheloveck322)
- Email: vasyagri2005@gmail.com

---