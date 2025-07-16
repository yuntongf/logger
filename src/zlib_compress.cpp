#include "zlib_compress.h"

size_t ZlibCompress::compressBound(size_t input_size) {
    if (!compress_stream_) {
        resetStream();
    }
    return static_cast<size_t>(::compressBound(input_size));
}

std::size_t ZlibCompress::compress(std::vector<uint8_t>& input, std::vector<uint8_t>& output) {
    if (!compress_stream_ || input.empty()) {
        return 0;
    }
    compress_stream_->next_in = input.data();
    compress_stream_->avail_in = input.size();
    compress_stream_->next_out = output.data();
    compress_stream_->avail_out = output.capacity();

    int ret = Z_OK;
    do {
        ret = deflate(compress_stream_.get(), Z_SYNC_FLUSH);
        if (ret != Z_OK && ret != Z_BUF_ERROR && ret != Z_STREAM_END) {
            return 0;
        }
    } while (ret == Z_BUF_ERROR);

    return output.capacity() - compress_stream_->avail_out;
}

std::size_t ZlibCompress::decompress(std::vector<uint8_t>& input, std::vector<uint8_t>& output) {
    if (input.empty()) {
        return 0;
    }

    z_stream stream{};
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    stream.avail_in = input.size();
    stream.next_in = input.data();
    stream.avail_out = output.capacity();
    stream.next_out = output.data();

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

    std::size_t decompressed_size = output.capacity() - stream.avail_out;
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