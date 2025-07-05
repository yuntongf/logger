#pragma once

#include <filesystem>
#include <string>
#include <unordered_set>
#include <string>
#include <fcntl.h>
#include <vector>

#include "util.h"

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