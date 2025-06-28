#pragma once

#include <cstddef>

class ICompression {
public:
    virtual ~ICompression() = default;

    virtual size_t compress(const void* input_data, size_t input_size, void* output_data, size_t output_size) = 0;

    virtual size_t compressBound(size_t input_size) = 0;

    virtual void resetStream() = 0;
};