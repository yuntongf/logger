#pragma once

#include <cstddef>
#include <filesystem>
#include <string_view>

struct SourceLocation {
    std::filesystem::path filepath;
    std::size_t thread_id;
    std::size_t line_num;
};

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

struct LogMsg {
    SourceLocation location;
    LogLevel level;
    std::string_view message;
};

/* given LogMsg, serialize it */
class Formatter {

    Formatter(const LogMsg& msg);
};