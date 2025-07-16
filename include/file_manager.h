#pragma once

#include <filesystem>
#include <string>
#include <unordered_set>
#include <string>
#include <fcntl.h>
#include <vector>

#include "util.h"


constexpr std::size_t DEFAULT_LOG_FILE_SIZE = 1024 * 4096; // 4MB
constexpr std::size_t DEFAULT_CACHE_FILE_SIZE = 4 * 4096; // 16KB

namespace fs = std::filesystem;

class FileManager {
    using fpath = fs::path;
public:
    FileManager() = delete;

    FileManager(fpath dir);

    ~FileManager();

    void writeCacheToLogFile(int cache_file_fd);

    int openCacheFile(const char* filename);
private:
    bool checkLogFileFull();

    int openLogFile();

    std::string getNextAvailableFilename();
private:
    fpath dir_;
    std::vector<int> cache_file_fds_;
    int log_file_fd_;
};