#include "zlib_compress.h"

size_t ZlibCompress::compressBound(size_t input_size) {
    if (!compress_stream_) {
        resetStream();
    }
    return static_cast<size_t>(::compressBound(input_size));
}

size_t ZlibCompress::compress(const void* input_data, size_t input_size, void* output_data, size_t output_size) {
    if (!compress_stream_ || !input_data || !output_data) {
        return 0;
    }
    compress_stream_->next_in = (Bytef*) input_data;
    compress_stream_->avail_in = input_size;
    compress_stream_->next_out = (Bytef*) output_data;
    compress_stream_->avail_out = output_size;

    int ret = Z_OK;
    do {
        ret = deflate(compress_stream_.get(), Z_SYNC_FLUSH);
        if (ret != Z_OK && ret != Z_BUF_ERROR && ret != Z_STREAM_END) {
            return 0;
        }
    } while (ret == Z_BUF_ERROR);
    return output_size - compress_stream_->avail_out;
}

void ZlibCompress::resetStream() {
    compress_stream_ = std::unique_ptr<z_stream, ZlibStreamDeflatorDeletor>(new z_stream());
    compress_stream_->zalloc = Z_NULL;
    compress_stream_->zfree = Z_NULL;
    compress_stream_->opaque = Z_NULL;

    auto ret = deflateInit(compress_stream_.get(), Z_BEST_COMPRESSION);
    if (ret != Z_OK) {
        compress_stream_.reset();
    }
}