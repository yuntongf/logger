#include "mem_mapper.h"

MemMapper::MemMapper(int cache_file_fd) 
: fd_(cache_file_fd),
  capacity_(util::fs::get_file_size(cache_file_fd)) {
    void* ptr = mmap_(nullptr, capacity_);
    header_ = static_cast<Header*>(ptr);
    init_header();
    data_ = static_cast<uint8_t*>(ptr) + sizeof(Header);
};

MemMapper::~MemMapper() {
    unmap_();
}

void MemMapper::push(const std::vector<uint8_t>& data) {
    std::size_t new_size = header_->data_size + data.size();
    reserve_(new_size);
    memcpy(data_ + header_->data_size, data.data(), data.size());
    header_->data_size = new_size;
}

uint8_t* MemMapper::data() const {
    return static_cast<uint8_t*>(data_);
}

void MemMapper::clear() {
    header_->data_size = 0;
}

int MemMapper::getFd() const {
    return fd_;
}

void MemMapper::reserve_(const std::size_t size) {
    if (size <= capacity_) {
        return;
    }
    std::size_t page_size = getPageSize_();
    std::size_t new_capacity = ((size / page_size) + 1) * page_size;
    util::fs::truncate_file(fd_, new_capacity);

    unmap_();
    auto ptr = mmap_(nullptr, new_capacity);
    header_ = static_cast<Header*>(ptr);
    capacity_ = new_capacity;
    data_ = static_cast<uint8_t*>(ptr + sizeof(Header));
}

void MemMapper::init_header() {
    if (header_->magic == MemMapper::MAGIC) {
        return;
    } 
    header_->magic = MemMapper::MAGIC;
    header_->data_size = 0;
}