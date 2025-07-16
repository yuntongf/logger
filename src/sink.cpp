
#include "sink.h"

#define CACHE_A_FILE_NAME "cache_a.log"
#define CACHE_B_FILE_NAME "cache_b.log"

#define LOG_ERR(msg) std::cerr << "[Logger error] " << msg << "\n";

constexpr SinkConfig DEFAULT_LOG_CONFIG = {"/log", LogLevel::INFO};

Sink::Sink() : Sink(DEFAULT_LOG_CONFIG) {}

Sink::Sink(const SinkConfig& config) {
    fpath log_dir(config.folder_str);

    executor_ = std::make_unique<Executor>();
    executor_->addRunner(ExecutorTag::FILE_WRITE_BACK);

    file_manager_ = std::make_unique<FileManager>(log_dir);
    formatter_ = std::make_unique<EffectiveFormatter>();
    compressor_ = std::make_unique<ZlibCompress>();
    encryptor_ = std::make_unique<Encryptor>();

    main_cache_fd_ = file_manager_->openCacheFile(CACHE_A_FILE_NAME);
    sub_cache_fd_ = file_manager_->openCacheFile(CACHE_B_FILE_NAME);

    mem_mapper_ = std::make_unique<MemMapper>(main_cache_fd_);
}

void Sink::log(LogMsg msg) {
    try {
        std::vector<uint8_t> serialized;
        formatter_->serialize(msg, serialized);

        int fd_to_flush = -1;
        {
            std::scoped_lock lck(mtx_);

            auto max_compress_size = compressor_->compressBound(serialized.size());
            std::vector<uint8_t> out;
            out.reserve(max_compress_size);
            auto output_size = compressor_->compress(serialized, out);
            encryptor_->encrypt(out);
            mem_mapper_->push(out);

            double usage_ratio = mem_mapper_->getRatio();
            if (usage_ratio > 0.8) {
                fd_to_flush = main_cache_fd_;
                std::swap(main_cache_fd_, sub_cache_fd_);
                mem_mapper_ = std::make_unique<MemMapper>(main_cache_fd_);
            }
        }

        if (fd_to_flush != -1) {
            executor_->postTask(ExecutorTag::FILE_WRITE_BACK, [this, fd_to_flush]{
                file_manager_->writeCacheToLogFile(fd_to_flush);
            });
        }
    } catch (const std::exception& e) {
        LOG_ERR(e.what());
    }
}