#include "logger_client.h"

int main() {
    LoggerClient logger;
    logger.addFileSink("./log", LogLevel::INFO);

    logger.info("Hello world!");
    
    return 0;
}