#include "../include/mpmc_queue.hpp"

template <typename T, size_t N>
MPMCqueue<T, N>::MPMCqueue()
{
    for (size_t i = 0; i < N; ++i)
        _buffer[i].sequence.store(i, std::memory_order_relaxed);
}

template <typename T, size_t N>
bool MPMCqueue<T, N>::push(const T& item)
{
    size_t head;
    Cell<T>* cell;

    for (;;)
    {
        head = _head.load(std::memory_order_relaxed);
        cell = &_buffer[head % N];
        size_t seq = cell->sequence.load(std::memory_order_acquire);
        intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(head);

        if (diff == 0)
        {
            if (_head.compare_exchange_weak(
                    head, head + 1,
                    std::memory_order_acq_rel,
                    std::memory_order_relaxed))
                break;
        }
        else if (diff < 0)
        {
            return false; // full
        }
        else
        {
            std::this_thread::yield();
        }
    }

    cell->data = item;
    cell->sequence.store(head + 1, std::memory_order_release);
    return true;
}

template <typename T, size_t N>
std::optional<T> MPMCqueue<T, N>::pop()
{
    size_t tail;
    Cell<T>* cell;

    for (;;)
    {
        tail = _tail.load(std::memory_order_relaxed);
        cell = &_buffer[tail % N];
        size_t seq = cell->sequence.load(std::memory_order_acquire);
        intptr_t diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(tail + 1);

        if (diff == 0)
        {
            if (_tail.compare_exchange_weak(
                    tail, tail + 1,
                    std::memory_order_acq_rel,
                    std::memory_order_relaxed))
                break;
        }
        else if (diff < 0)
        {
            return std::nullopt; // empty
        }
        else
        {
            std::this_thread::yield();
        }
    }

    T value = std::move(cell->data);
    cell->sequence.store(tail + N, std::memory_order_release);
    return value;
}