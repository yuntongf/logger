#pragma once

#include <memory>
#include <filesystem>
#include <vector>
#include <utility>

#include "sink.h"

class LoggerClient {
public:
    inline void info(std::string_view msg) const {
        log(LogLevel::INFO, msg);
    }

    inline void warn(std::string_view msg) const {
        log(LogLevel::WARN, msg);
    }

    inline void debug(std::string_view msg) const {
        log(LogLevel::DEBUG, msg);
    }

    inline void error(std::string_view msg) const {
        log(LogLevel::ERROR, msg);
    }

    void addFileSink(const std::string& folder, LogLevel level) {
        SinkConfig config = {folder, level};
        sinks_.emplace_back(std::make_unique<Sink>(std::move(config)));
    }
    
private:
    void log(LogLevel level, std::string_view msg) const {
        LogMsg built_msg = {
            std::source_location::current(),
            level,
            msg
        };
        for (auto& sink : sinks_) {
            sink->log(std::move(built_msg));
        }
    }
private:
    std::vector<std::unique_ptr<Sink>> sinks_;
};