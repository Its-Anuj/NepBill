cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Libs")

message("Building for: ${CMAKE_BUILD_TYPE}")

add_subdirectory(NepBill)