#pragma once

#include <cstddef>

#include <filesystem>
#include <memory>
#include <mutex>

#include "mem_mapper.h"
#include "effective_formatter.h"
#include "encryptor.h"
#include "file_manager.h"
#include "zlib_compress.h"
#include "executor.h"

struct SinkConfig {
    std::string folder_str;
    LogLevel log_level;
};

class Sink {
    using fpath = std::filesystem::path;
public:
    Sink();

    explicit Sink(const SinkConfig&);

    void log(LogMsg msg);

    void flush();
private:

private:
    std::unique_ptr<Executor> executor_;
    std::unique_ptr<FileManager> file_manager_;
    std::unique_ptr<IFormatter> formatter_;
    std::unique_ptr<ICompression> compressor_;
    std::unique_ptr<Encryptor> encryptor_;
    std::unique_ptr<MemMapper> mem_mapper_;
    int main_cache_fd_;
    int sub_cache_fd_;
    std::mutex mtx_;
};