#pragma once

#include <cstddef>
#include <filesystem>
#include <string_view>
#include <source_location>

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

struct LogMsg {
    std::source_location location;
    LogLevel level;
    std::string_view msg;
};

/* given LogMsg, serialize it */
class IFormatter {
public:
    virtual void serialize(const LogMsg& msg, void* dest) const = 0;
};