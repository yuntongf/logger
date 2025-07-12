#include <gtest/gtest.h>
#include "effective_msg.pb.h"
#include "effective_formatter.h"

TEST(EffectiveFormatterTest, SerializePopulatesCorrectFields) {
    EffectiveFormatter formatter;

    auto make_dummy_log_msg = []() -> LogMsg {
        return LogMsg{
            std::source_location::current(),
            LogLevel::INFO,
            "Hello, test!"
        };
    };

    LogMsg msg = make_dummy_log_msg();

    uint8_t* serialized_data = nullptr;
    std::size_t serialized_size = 0;

    formatter.serialize(msg, serialized_data, serialized_size);

    EffectiveMsg parsed;
    ASSERT_TRUE(parsed.ParseFromArray(serialized_data, serialized_size));

    EXPECT_EQ(parsed.level(), static_cast<int>(msg.level));
    EXPECT_EQ(parsed.msg(), "Hello, test!");
    EXPECT_EQ(parsed.file_name(), msg.location.file_name());
    EXPECT_EQ(parsed.func_name(), msg.location.function_name());
    EXPECT_EQ(parsed.line(), msg.location.line());
    EXPECT_EQ(parsed.column(), msg.location.column());

    EXPECT_GT(parsed.timestamp(), 0);
    EXPECT_GT(parsed.pid(), 0);
    EXPECT_GT(parsed.tid(), 0);

    operator delete(serialized_data);
}
