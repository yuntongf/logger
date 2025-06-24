#pragma once

#include <cstddef>

#include <filesystem>
#include <memory>

#include "mem_mapper.h"
#include "effective_formatter.h"

struct SinkConfig {
    std::filesystem::path dir;

};

class Sink {
public:
    Sink();

    explicit Sink(const SinkConfig&);

    std::size_t capacity() const;

    void log(LogMsg msg);

private:
    std::unique_ptr<MemMapper> mm_;
    std::unique_ptr<EffectiveFormatter> formatter_;
};