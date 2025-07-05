
set(CMAKE_CXX_STANDARD 20)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")

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