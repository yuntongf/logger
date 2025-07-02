
#include "sink.h"

constexpr SinkConfig DEFAULT_LOG_CONFIG = {"/log", LogLevel::INFO};
constexpr std::string CACHE_A_FILE = "cache_a";
constexpr std::string CACHE_B_FILE = "cache_b";
constexpr std::string SERVER_KEY_FILE = "../reader.pem";
constexpr std::string CLIENT_KEY_FILE = "../writer.pem";

#define LOG_ERR(msg) std::cout << msg << "\n";

Sink::Sink() : Sink(DEFAULT_LOG_CONFIG) {}

Sink::Sink(const SinkConfig& config) {
    fpath log_dir(config.folder_str);
    fpath main_cache_file = log_dir / CACHE_A_FILE;
    fpath sub_cache_file = log_dir / CACHE_B_FILE;
    fpath server_key_file_path(SERVER_KEY_FILE);
    fpath client_key_file_path(CLIENT_KEY_FILE);

    executor_ = std::make_unique<Executor>();
    executor_->addRunner(ExecutorTag::FILE_MANAGER);
    executor_->addRunner(ExecutorTag::FORMATTER);
    executor_->addRunner(ExecutorTag::COMPRESSOR);
    executor_->addRunner(ExecutorTag::ENCRYPTOR);
    executor_->addRunner(ExecutorTag::MEM_MAPPER);
    executor_->addRunner(ExecutorTag::FLUSHER);

    file_manager_ = std::make_unique<FileManager>(log_dir);
    formatter_ = std::make_unique<EffectiveFormatter>();
    compressor_ = std::make_unique<ZlibCompress>();
    encryptor_ = std::make_unique<Encryptor>(server_key_file_path, client_key_file_path);

    mem_mapper_ = std::make_unique<MemMapper>(main_cache_file);
    main_cache_fd_ = mem_mapper_->getFd();
    int fd = open(sub_cache_file.c_str(), O_RDWR | O_CREAT);
    if (fd == -1) {
        perror("failed to open sub cache file");
    }
    sub_cache_fd_ = fd;
}

void Sink::log(LogMsg msg) {
    uint8_t data;
    try {
        auto format_future = executor_->postTask(ExecutorTag::FORMATTER, [this, msg, &data]{
            formatter_->serialize(msg, &data);
        });
        format_future.wait();
    } catch (const std::runtime_error& e) {
        LOG_ERR(std::string("Formatter error: ") + e.what());
    }

    try {
        std::mutex mtx;
        auto f = executor_->postTask(ExecutorTag::COMPRESSOR, [this, ]{

        });
    }
}