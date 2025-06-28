#include "compression_interface.h"

#include <zlib.h>
#include <memory>

struct ZlibStreamDeflatorDeletor {
    void operator()(z_stream* s) {
        if (s) {
            deflateEnd(s);
        }
    }
};

class ZlibCompress : public ICompression {
public:
    ~ZlibCompress() override = default;

    size_t compressBound(size_t input_size) override;

    size_t compress(const void* input_data, size_t input_size, void* output_data, size_t output_size) override;

    void resetStream() override;
private:
    std::unique_ptr<z_stream, ZlibStreamDeflatorDeletor> compress_stream_;
};