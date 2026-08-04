cmake_minimum_required(VERSION 3.10.0)
project(NepBill VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Libs")

message("Building for: ${CMAKE_BUILD_TYPE}")

add_subdirectory(NepBill)