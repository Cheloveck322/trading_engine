#ifndef MPMCQUEUE_HPP
#define MPMCQUEUE_HPP

#include <cstddef>
#include <atomic>
#include <array>
#include <optional>
#include <thread>

template <typename T>
struct Cell
{
    std::atomic<std::size_t> sequence;
    T data;
};

template <typename T, std::size_t N>
class MPMCqueue
{
public:
    MPMCqueue();
    
    bool push(const T& item);
    std::optional<T> pop();
    
private:
    std::array<Cell<T>, N> _buffer{};
    std::atomic<std::size_t> _head{};
    std::atomic<std::size_t> _tail{};
};

#include "../src/mpmc_queue.cpp"

#endif