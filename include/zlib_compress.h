#include "compression_interface.h"

#include <zlib.h>
#include <vector>
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

    size_t compress(std::vector<uint8_t>& input, std::vector<uint8_t>& output) override;

    // This helps with test
    static size_t decompress(std::vector<uint8_t>& input, std::vector<uint8_t>& output);

    void resetStream() override;
private:
    std::unique_ptr<z_stream, ZlibStreamDeflatorDeletor> compress_stream_;
};