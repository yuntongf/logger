#pragma once

#include <filesystem>
#include <string>
#include <unordered_set>
#include <string>

class FileManager {
    using fpath = std::filesystem::path;
public:
    FileManager() = delete;

    FileManager(fpath dir);

    ~FileManager();

    int getLogFileFd();
private:
    std::unordered_set<std::string> log_files;
};