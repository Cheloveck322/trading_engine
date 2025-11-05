#include "../include/spscqueue.hpp"
#include <iostream>

template <typename T, std::size_t N>
bool SPSCQueue<T, N>::push(const T& item)
{
    std::size_t head{ _head.load(std::memory_order_relaxed) };
    std::size_t next_head{ (head + 1) % N };

    if (next_head == _tail.load(std::memory_order_acquire))
        return false;

    _buffer[head] = item;
    _head.store(next_head, std::memory_order_release);
    return true;
}

template <typename T, std::size_t N>
bool SPSCQueue<T, N>::pop(T& item)
{
    std::size_t tail{ _tail.load(std::memory_order_relaxed) };
    std::size_t next_tail{ (tail + 1) % N };

    if (tail == _head.load(std::memory_order_acquire))
        return false;

    item = _buffer[tail];
    _tail.store(next_tail, std::memory_order_release);
    return true;
}

template <typename T, std::size_t N>
std::size_t SPSCQueue<T, N>::popBatch(std::vector<T>& out, std::size_t maxSize)
{
    std::size_t head{ _head.load(std::memory_order_acquire) };
    std::size_t tail{ _tail.load(std::memory_order_relaxed) };

    std::size_t available{ (head + N - tail) % N };
    std::size_t count{ std::min(maxSize, available)};

    for (std::size_t i{ 0 }; i < count; ++i)
    {
        out.push_back(_buffer[(tail + i) % N]);
    }

    _tail.store((tail + count) % N, std::memory_order_release);
    return count;
}