#pragma once

#include <cstddef>
#include <cstdint>

class ICompression {
public:
    virtual ~ICompression() = default;

    virtual size_t compress(uint8_t* input_data, size_t input_size, uint8_t*& output_data, size_t output_size) = 0;

    virtual size_t compressBound(size_t input_size) = 0;

    virtual void resetStream() = 0;
};