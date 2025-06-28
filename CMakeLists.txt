
set(CMAKE_CXX_STANDARD 20)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(logger ${SOURCES})
target_link_libraries(logger PRIVATE ZLIB::ZLIB OpenSSL::SSL OpenSSL::Crypto)