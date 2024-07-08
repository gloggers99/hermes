#pragma once

#include <utility>
#include <vector>
#include <unordered_map>
#include <ostream>
#include <algorithm>
#include <functional>
#include <iostream>
#if defined(__MINGW32__)
#include <windows.h>
#endif

namespace Hermes {

enum class LogLevel {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
};

template<typename T = char>
class Log {
private:
    std::string logName;
    bool color;

    std::vector<std::reference_wrapper<std::basic_ostream<T>>> streams;
    std::unordered_map<LogLevel, std::string> colorMap = {
            {LogLevel::LOG_INFO, "\033[34m"},
            {LogLevel::LOG_WARN, "\033[33m"},
            {LogLevel::LOG_ERROR, "\033[31m"}
    };

    void checkColor() {
#if defined(__MINGW32__)
        DWORD consoleMode;
        if (std::getenv("WT_SESSION") != nullptr)
            this->color = true;
        else if (GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &consoleMode) != 0)
            this->color = true;
#else
        const char *term = std::getenv("TERM");
        if (term != nullptr && (std::string(term).find("color") != std::string::npos))
            this->color = true;
#endif
    }

public:
    void log(T *msg, LogLevel level = LogLevel::LOG_INFO) {
        std::for_each(this->streams.begin(),
                      this->streams.end(),
                      [&](std::reference_wrapper<std::basic_ostream<T>> stream) {
            stream.get()
            << (this->color ? this->colorMap[level] : "")
            << "["
            << this->logName << "] "
            << msg
            << (this->color ? "\033[0m" : "")
            << "\n";
        });
    }

    void addStream(std::basic_ostream<T> &stream) {
        this->streams.emplace_back(std::ref(stream));
    }

    void setName(const std::string &newLogName) {
        this->logName = newLogName;
    }

    void operator()(T *msg, LogLevel level = LogLevel::LOG_INFO) {
        this->log(msg);
    }

    Log() {
        this->checkColor();
    }

    Log(std::string logName, std::basic_ostream<T> &out = std::cout) : logName(std::move(logName)), streams({std::ref(out)}) {
        this->checkColor();
    }

    Log(std::string logName, const std::vector<std::basic_ostream<T>> &outStreams) : logName(std::move(logName)) {
        for (auto &stream : outStreams) {
            this->streams.emplace_back(std::ref(stream));
        }

        this->checkColor();
    }
    ~Log() = default;
};

} // namespace Hermes