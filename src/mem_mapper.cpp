#include "mem_mapper.h"

MemMapper::MemMapper(MemMapper::fpath path) : file_(std::move(path)) {
    std::size_t file_size = std::filesystem::file_size(file_);
    std::size_t size = std::max(file_size, DEFAULT_SIZE);
    fd_ = open(path.c_str(), O_RDWR);
    void* ptr = mmap_(nullptr, size);
    header_ = static_cast<Header*>(ptr);
    data_ = static_cast<uint8_t*>(ptr) + sizeof(Header);
    capacity_ = size;
    init_();
};

MemMapper::~MemMapper() {
    unmap_();
    close(fd_);
}

void MemMapper::push(const void* data, std::size_t size) {
    std::size_t new_size = header_->data_size + size;
    reserve_(new_size);
    memcpy(data_ + header_->data_size, data, size);
    header_->data_size = new_size;
}

bool MemMapper::empty() const {
    return header_->data_size == 0;
}

double MemMapper::getRatio() const {
    std::size_t data_capacity = capacity_ - sizeof(Header);
    return (double) header_->data_size / (double) data_capacity;
}

uint8_t* MemMapper::data() const {
    return static_cast<uint8_t*>(data_);
}

void MemMapper::clear() {
    header_->data_size = 0;
}

void MemMapper::reserve_(const std::size_t size) {
    if (size <= capacity_) {
        return;
    }
    std::size_t page_size = getPageSize_();
    std::size_t new_capacity = ((size / page_size) + 1) * page_size;
    unmap_();
    mmap_(header_, new_capacity);
    capacity_ = new_capacity;
}

void MemMapper::init_() {
    if (header_->magic == MemMapper::MAGIC) {
        return;
    } 
    header_->magic = MemMapper::MAGIC;
    header_->data_size = 0;
}