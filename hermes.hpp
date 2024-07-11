/*
 * Hermes logger by gloggers99
 */

#pragma once

#include <iomanip>
#include <vector>
#include <unordered_map>
#include <ostream>
#include <algorithm>
#include <functional>
#include <iostream>
#if defined(__MINGW32__)
#include <windows.h>
#endif
#include <fstream>
#include <sstream>
#include <variant>
#include <string>


namespace Hermes {

enum class LogLevel {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
};

template<typename T = char>
class Log {
private:
    bool enabled;
    std::string format;

    std::string logName;
    bool color;

    std::vector<std::reference_wrapper<std::basic_ostream<T>>> streams;
    std::unordered_map<LogLevel, std::string> colorMap = {
            {LogLevel::LOG_INFO, "\033[34m"},
            {LogLevel::LOG_WARN, "\033[33m"},
            {LogLevel::LOG_ERROR, "\033[31m"}
    };


    std::string getStringFromLogLevel(LogLevel level) {
        switch (level) {
            case LogLevel::LOG_INFO:
                return "INFO";
            case LogLevel::LOG_WARN:
                return "WARN";
            case LogLevel::LOG_ERROR:
                return "ERROR";
        }

        throw std::runtime_error("Invalid LogLevel.");
    }

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

    // i might just make a toml parser in the future for fun
    void reloadConfig() {
        std::unordered_map<std::string, std::variant<std::string*, bool*>> configMap = {
                {"enabled", &this->enabled},
                {"format", &this->format}
        };

        std::ifstream config("./hermes.conf");

        // no config
        if (!config.is_open())
            return;

        std::string line;
        std::istringstream ss;
        while (std::getline(config, line)) {
            ss.clear();
            ss.str(line);

            std::string token, delimiter;
            ss >> token >> delimiter;

            for (auto &pair : configMap) {
                if (pair.first == token) {
                    if (std::holds_alternative<bool*>(pair.second)) {
                        ss >> std::boolalpha >> *std::get<bool*>(pair.second);
                    } else if (std::holds_alternative<std::string*>(pair.second)) {
                        ss >> std::quoted(*std::get<std::string*>(pair.second));
                    }
                }
            }
        }
    }

public:
    void log(const std::basic_string<T> &msg, LogLevel level = LogLevel::LOG_INFO) {
        this->reloadConfig();

        if (!this->enabled)
            return;

        std::string colorString = this->color ? this->colorMap[level] : "";
        std::string resetString = this->color ? "\033[0m" : "";

        std::unordered_map<std::string, std::string> formatMap = {
                {"logname", this->logName},
                {"loglevel", this->getStringFromLogLevel(level)},
                {"logmessage", msg}
        };

        std::string tmpFormat = format;
        std::for_each(formatMap.begin(), formatMap.end(), [&](const std::pair<std::string, std::string>& pair) {
            std::string token = "{" + pair.first + "}";
            if (tmpFormat.find(token) != std::string::npos)
                tmpFormat.replace(tmpFormat.find(token), token.length(), pair.second);
        });

        tmpFormat = colorString + tmpFormat + resetString + "\n";

        std::for_each(this->streams.begin(),
                      this->streams.end(),
                      [&](std::reference_wrapper<std::basic_ostream<T>> stream) {
                stream.get() << tmpFormat;
        });

    }

    void addStream(std::basic_ostream<T> &stream) {
        this->streams.emplace_back(std::ref(stream));
    }

    void clearStreams() {
        this->streams.clear();
    }

    void disableColor() {
        this->color = false;
    }

    void enableColor() {
        this->color = true;
    }

    void setName(const std::string &newLogName) {
        this->logName = newLogName;
    }

    void operator()(const std::basic_string<T> msg, LogLevel level = LogLevel::LOG_INFO) {
        this->log(msg, level);
    }

    Log() : enabled(true) {
        this->checkColor();
    }

    explicit Log(std::string logName, std::basic_ostream<T> &out = std::cout) : enabled(true), logName(std::move(logName)), streams({std::ref(out)}) {
        this->checkColor();
    }

    Log(std::string logName, const std::vector<std::basic_ostream<T>> &outStreams) : enabled(true), logName(std::move(logName)) {
        for (auto &stream : outStreams) {
            this->streams.emplace_back(std::ref(stream));
        }

        this->checkColor();
    }
    ~Log() = default;
};


} // namespace Hermes
