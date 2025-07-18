
#include "effective_msg.pb.h"
#include "effective_formatter.h"

void EffectiveFormatter::serialize(const LogMsg& msg, std::vector<uint8_t>& data) const {
    EffectiveMsg effective_msg;

    effective_msg.set_level(static_cast<int>(msg.level));
    effective_msg.set_timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
    effective_msg.set_pid(getpid());
    effective_msg.set_tid(gettid());
    effective_msg.set_line(msg.location.line());
    effective_msg.set_column(msg.location.column());
    effective_msg.set_file_name(msg.location.file_name());
    effective_msg.set_func_name(msg.location.function_name());
    effective_msg.set_msg(std::string(msg.msg).c_str());

    std::size_t size = effective_msg.ByteSizeLong();
    data.reserve(size);
    effective_msg.SerializeToArray(data.data(), size);
}