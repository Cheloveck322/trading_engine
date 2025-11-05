#ifndef SPSCQUEUE_HPP
#define SPSCQUEUE_HPP

#include <atomic>
#include <cstddef>
#include <array>
#include <vector>

template <typename T, std::size_t N>
class SPSCQueue
{ 
public:
    SPSCQueue() = default;
    bool pop(T& item);
    bool push(const T& item);
    std::size_t popBatch(std::vector<T>& out, std::size_t maxSize);

private:    
    std::atomic<std::size_t> _head{};
    std::atomic<std::size_t> _tail{};
    std::array<T, N> _buffer{};
};

#include "../src/spscqueue.tpp"

#endif // SPSCQUEUE_HPP