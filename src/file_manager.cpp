#include "file_manager.h"

FileManager::FileManager(fpath dir) : dir_(dir), log_file_fd_(openLogFile()) {}

FileManager::~FileManager() {
    for (auto fd : cache_file_fds_) {
        util::fs::close_file(fd);
    }
    util::fs::close_file(log_file_fd_);
}

int FileManager::openCacheFile(const char* filename) {
    try {
        int fd = util::fs::open_rw_file(dir_, filename);
        cache_file_fds_.push_back(fd);
        return fd;
    } catch (const std::runtime_error& e) {
        throw std::runtime_error(std::string("Cannot open cache file: ") + e.what());
    }
}

bool FileManager::checkLogFileFull() {
    return false;
}

std::string FileManager::getNextAvailableFilename() {
    auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();

    std::string base = "log_" + std::to_string(timestamp);
    std::string filename;

    try {
        int suffix = 0;

        // Check for existing files that start with the same base
        for (const auto& entry : fs::directory_iterator(dir_)) {
            if (entry.is_regular_file()) {
                std::string name = entry.path().filename().string();
                if (name.rfind(base, 0) == 0) {
                    ++suffix;
                }
            }
        }

        filename = base + (suffix > 0 ? "_" + std::to_string(suffix) : "") + ".log";
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << '\n';
        filename = base + ".log";
    }
    return filename;
}

int FileManager::openLogFile() {
    return util::fs::open_rw_file(dir_, getNextAvailableFilename());
}

// Don't need lock because we are running this on one strand
void FileManager::writeCacheToLogFile(int cache_file_fd) {
    if (checkLogFileFull()) {
        util::fs::close_file(log_file_fd_);
        log_file_fd_ = openLogFile();
    }
    util::fs::move_file_content(cache_file_fd, log_file_fd_);
}