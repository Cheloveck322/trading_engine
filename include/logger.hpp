#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <string>
#include <mutex>
#include <chrono>

class Logger
{
public:
    Logger(const std::string& filename)
        : _file{ filename, std::ios::out | std::ios::app }
    {
    }

    template <typename... Args>
    void log(const std::string& fmt, Args&&... args)
    {
        std::scoped_lock lock(_mutex);
        auto now{ std::chrono::system_clock::from_time_t(std::chrono::steady_clock::now()) };
        _file << "[" << std::put_time(std::localtime(&now), "%H:%M:%S") << "] " << fmt << ' ';
        ((_file << args), ...);
        _file << "\n";
    }

private:
    std::ofstream _file{};
    std::mutex _mutex{};
};

#endif // LOGGER_HPP