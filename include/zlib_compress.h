#include "compression_interface.h"

#include <zlib.h>
#include <memory>

struct ZlibStreamDeflatorDeletor {
    void operator()(z_stream* s) {
        if (s) {
            deflateEnd(s);
            delete s;
        }
    }
};

class ZlibCompress : public ICompression {
public:
    ~ZlibCompress() override = default;

    size_t compressBound(size_t input_size) override;

    size_t compress(uint8_t* input_data, size_t input_size, uint8_t*& output_data, size_t output_size) override;

    // This helps with test
    static size_t decompress(uint8_t* input_data, size_t input_size, uint8_t*& output_data, size_t output_size);

    void resetStream() override;
private:
    std::unique_ptr<z_stream, ZlibStreamDeflatorDeletor> compress_stream_;
};