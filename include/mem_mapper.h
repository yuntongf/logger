#pragma once

#include <filesystem>
#include <cstddef>
#include <algorithm>
#include <cstring>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>


#include "util.h"

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

    MemMapper(int fd);

    MemMapper(const MemMapper& other) = delete;

    MemMapper(MemMapper&& other) = delete;

    /* Unmaps the file but does not close it */
    ~MemMapper();

    [[nodiscard]] inline bool empty() const {
        return header_->data_size == 0;
    }

    // size calculation does not include header
    inline double getRatio() const {
        std::size_t data_capacity = capacity_ - sizeof(Header);
        return (double) header_->data_size / (double) data_capacity;
    }

    void push(const void* data, std::size_t size);

    uint8_t* data() const;

    inline void clear();

    inline int getFd() const;

private:
    inline void* mmap_(void* ptr, const std::size_t size) {
        void* result = mmap(ptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);

        if (result == MAP_FAILED) {
            throw std::runtime_error("mmap failed: " + std::string(strerror(errno)));
        }

        return result;
    }

    inline void unmap_() {
        if (data_ && capacity_ > 0) {
            if (munmap(data_, capacity_) != 0) {
                throw std::runtime_error("munmap failed: " + std::string(strerror(errno)));
            }
            data_ = nullptr;
            header_ = nullptr;
            capacity_ = 0;
        }
    }

    inline std::size_t getPageSize_() const {
        return static_cast<std::size_t>(::getpagesize());
    }

    /* Reserve space, could potentially remap */
    void reserve_(const std::size_t size);

    void init_header();

private:
    Header* header_ = nullptr;
    uint8_t* data_ = nullptr;
    std::size_t capacity_ = 0; // size of the entire mmapped area including header
    fpath file_;
    int fd_;
};