
#include "sink.h"

#define CACHE_A_FILE_NAME "cache_a.log"
#define CACHE_B_FILE_NAME "cache_b.log"
#define LOG_ERR(msg) std::cout << msg << "\n";

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
    uint8_t* data = nullptr;   
    std::size_t size = 0;
    formatter_->serialize(msg, data, size);
    {
        std::unique_lock<std::mutex> lck(mtx_);

        auto estimate_size = compressor_->compressBound(size);
        uint8_t* output_data = static_cast<uint8_t*>(operator new(estimate_size));
        auto output_size = compressor_->compress(data, size, output_data, estimate_size);

        encryptor_->encrypt(output_data, output_size);

        mem_mapper_->push(output_data, output_size);

        double usage_ratio = mem_mapper_->getRatio();
        if (usage_ratio > 0.8) {
            std::swap(main_cache_fd_, sub_cache_fd_);
            executor_->postTask(ExecutorTag::FILE_WRITE_BACK, [this]{
                file_manager_->writeCacheToLogFile(sub_cache_fd_);
            });
        }
    }
}