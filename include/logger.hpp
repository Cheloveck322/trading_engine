#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <string>
#include <mutex>
#include <chrono>
#include <filesystem>
#include <iostream>

class Logger
{
public:
    Logger(const std::string& filename)
    {
        std::filesystem::path logDir = std::filesystem::current_path().parent_path() / "logs";
        std::filesystem::create_directories(logDir);
        std::filesystem::path logFile = logDir / filename;

        _file.open(logFile, std::ios::out | std::ios::app);
        if (!_file.is_open())
            std::cerr << "[Logger Error] Cannot open: " << logFile << std::endl;
        else
            std::cout << "[Logger] Logging to: " << logFile << std::endl;
    }   

    template <typename... Args>
    void log(const std::string& fmt, Args&&... args)
    {
        std::scoped_lock lock(_mutex);
        auto now{ std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) };
        _file << "[" << std::put_time(std::localtime(&now), "%H:%M:%S") << "] " << fmt << ' ';
        ((_file << args), ...);
        _file << "\n";
    }

private:
    std::ofstream _file{};
    std::mutex _mutex{};
};

#endif // LOGGER_HPP