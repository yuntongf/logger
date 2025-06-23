#pragma once

#include <cstddef>

#include <filesystem>

#include "mem_mapper.h"
#include "formatter.h"

struct SinkConfig {
    std::filesystem::path dir;

};

class Sink {    
public:
    Sink();

    explicit Sink(const SinkConfig&);

    std::size_t capacity() const;

private:
    MemMapper mm_;
};