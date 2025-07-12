#include <gtest/gtest.h>
#include "zlib_compress.h"
#include <vector>
#include <cstring>
#include <string>

TEST(ZlibCompressTest, CompressDecompressRoundTrip) {
    ZlibCompress zlib;

    std::string input_str = "This is a test string for compression and decompression!";
    uint8_t* input_data = reinterpret_cast<uint8_t*>(input_str.data());
    std::size_t input_size = input_str.size();

    std::size_t max_compressed_size = zlib.compressBound(input_size);
    std::vector<uint8_t> compressed_buffer(max_compressed_size);
    uint8_t* compressed_data = compressed_buffer.data();

    std::size_t compressed_size = zlib.compress(input_data, input_size, compressed_data, max_compressed_size);
    ASSERT_GT(compressed_size, 0);

    std::vector<uint8_t> decompressed_buffer(input_size);
    uint8_t* decompressed_data = decompressed_buffer.data();

    std::size_t decompressed_size = zlib.decompress(compressed_data, compressed_size, decompressed_data, input_size);
    ASSERT_EQ(decompressed_size, input_size);

    std::string output_str(reinterpret_cast<char*>(decompressed_data), decompressed_size);
    EXPECT_EQ(output_str, input_str);
}
