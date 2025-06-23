#pragma once

#include "sink.h"

class LoggerClient {
public:
    LoggerClient();

    void log(const LogMsg& msg);
private:
    Sink sink_;
};