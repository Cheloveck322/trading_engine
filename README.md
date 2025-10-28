# Trading Engine (C++)

## Описание проекта
**Trading Engine** — это учебный проект на C++, моделирующий работу биржевого движка. 
Он реализует основные функции книги заявок (Order Book) и алгоритма сопоставления ордеров (Matching Engine).  
Проект написан в однопоточном режиме и подходит для изучения принципов работы торговых систем и многопоточности в будущем.

---

## Цели проекта
- Реализовать базовый **OrderBook** с поддержкой лимитных и рыночных ордеров
- Обеспечить логику **частичного исполнения заявок**
- Сохранить принципы **FIFO (первым пришёл — первым исполнен)** внутри ценовых уровней
- Подготовить основу для расширения проекта (многопоточность, бенчмарки, ClickHouse и Python аналитика)

---

## Сборка и запуск

### 1 Требования
- CMake ≥ 3.15  
- Компилятор C++20 (g++, clang++, MSVC)  
- [GoogleTest](https://github.com/google/googletest)  
- [Google Benchmark](https://github.com/google/benchmark)  

### 2 Сборка всех целей

```bash
mkdir build && cd build
cmake ..
make -j
```

### 3 Запуск юнит-тестов

```bash
./test_orderbook
```

Вывода:
```
[==========] Running 2 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 2 tests from OrderBookTest
[ RUN      ] OrderBookTest.SimpleMatch
[       OK ] OrderBookTest.SimpleMatch (0 ms)
[ RUN      ] OrderBookTest.PartialFill
[       OK ] OrderBookTest.PartialFill (0 ms)
[----------] 2 tests from OrderBookTest (0 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 2 tests.
```

---

## Бенчмарки

### Запуск тестов производительности

```bash
./benchmark_queue
```

### Пример вывода

```
-----------------------------------------------------------
Benchmark                 Time             CPU   Iterations
-----------------------------------------------------------
BM_ProcessOrders    1235507 ns      1235428 ns          528
```
**Цель:** учебный проект для практики системного программирования и разработки низкоуровневых приложений на C++.