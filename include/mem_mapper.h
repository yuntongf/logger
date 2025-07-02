#pragma once

#include <filesystem>
#include <cstddef>
#include <algorithm>
#include <cstring>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

/* Wrapper around mmap */
class MemMapper {
    using fpath = std::filesystem::path;
    static constexpr std::size_t DEFAULT_SIZE = 16 * 1024;
    static constexpr uint32_t MAGIC = 0xcafef00d;

    struct Header {
        uint32_t magic;
        std::size_t data_size; // size of data excluding header
    };
public:
    MemMapper() = delete;

    MemMapper(fpath path);

    MemMapper(const MemMapper& other) = delete;

    MemMapper(MemMapper&& other) = delete;

    /* Unmaps the file but does not close it */
    ~MemMapper();

    [[nodiscard]] inline bool empty() const;

    // size calculation does not include header
    inline double getRatio() const;

    void push(const void* data, std::size_t size);

    uint8_t* data() const;

    inline void clear();

    inline int getFd() const;

private:
    void* mmap_(void* ptr, const std::size_t size);

    void unmap_();

    std::size_t getPageSize_() const;

    /* Reserve space, could potentially remap */
    void reserve_(const std::size_t size);

    void init_();

private:
    Header* header_ = nullptr;
    uint8_t* data_ = nullptr;
    std::size_t capacity_ = 0; // size of the entire mmapped area including header
    fpath file_;
    int fd_;
};