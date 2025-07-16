#pragma once

#include <cstddef>
#include <cstdint>

class ICompression {
public:
    virtual ~ICompression() = default;

    virtual size_t compress(std::vector<uint8_t>& input, std::vector<uint8_t>& output) = 0;

    virtual size_t compressBound(size_t input_size) = 0;

    virtual void resetStream() = 0;
};