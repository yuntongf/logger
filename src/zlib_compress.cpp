#include "zlib_compress.h"

size_t ZlibCompress::compressBound(size_t input_size) {
    if (!compress_stream_) {
        resetStream();
    }
    return static_cast<size_t>(::compressBound(input_size));
}

std::size_t ZlibCompress::compress(uint8_t* input_data, size_t input_size, uint8_t*& output_data, size_t output_size) {
    if (!compress_stream_ || !input_data || !output_data) {
        return 0;
    }
    compress_stream_->next_in = input_data;
    compress_stream_->avail_in = input_size;
    compress_stream_->next_out = output_data;
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

std::size_t ZlibCompress::decompress(uint8_t* input_data, size_t input_size, uint8_t*& output_data, size_t output_size) {
    if (!input_data || !output_data || input_size == 0 || output_size == 0) {
        return 0;
    }

    z_stream stream{};
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    stream.avail_in = input_size;
    stream.next_in = input_data;
    stream.avail_out = output_size;
    stream.next_out = output_data;

    if (inflateInit(&stream) != Z_OK) {
        return 0;
    }

    int ret = Z_OK;
    do {
        ret = inflate(&stream, Z_SYNC_FLUSH);
        if (ret != Z_OK && ret != Z_BUF_ERROR && ret != Z_STREAM_END) {
            inflateEnd(&stream);
            return 0;
        }
    } while (stream.avail_in > 0 && stream.avail_out > 0 && ret != Z_STREAM_END);

    std::size_t decompressed_size = output_size - stream.avail_out;
    inflateEnd(&stream);
    return decompressed_size;
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