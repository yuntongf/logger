
set(CMAKE_CXX_STANDARD 20)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

file(GLOB_RECURSE SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Protobuf REQUIRED)

add_executable(logger ${SOURCES})
target_link_libraries(logger PRIVATE
    ZLIB::ZLIB
    OpenSSL::SSL
    OpenSSL::Crypto
    protobuf::libprotobuf
)

enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/effective_formatter.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/zlib_compress.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/effective_msg.pb.cc"
)

add_executable(tests ${TEST_SOURCES})
target_include_directories(tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(tests PRIVATE
    gtest_main
    ZLIB::ZLIB
    OpenSSL::SSL
    OpenSSL::Crypto
    protobuf::libprotobuf
)

add_test(NAME AllTests COMMAND tests)
