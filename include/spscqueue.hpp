#ifndef SPSCQUEUE_HPP
#define SPSCQUEUE_HPP

#include <atomic>
#include <cstddef>
#include <array>

template <typename T, std::size_t N>
class SPSCQueue
{ 
public:
    SPSCQueue() = default;
    bool pop(T& item);
    bool push(const T& item);
    
private:    
    std::atomic<std::size_t> _head{};
    std::atomic<std::size_t> _tail{};
    std::array<T, N> _buffer{};
};

#include "../src/spscqueue.tpp"

#endif // SPSCQUEUE_HPP